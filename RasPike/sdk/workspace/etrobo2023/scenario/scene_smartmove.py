import cv2
import numpy as np
import threading
import ctypes
import datetime
import time
from multiprocessing import Value, Array, Process

from device.camera_control import read, show_camera_and_get_key
from device.serial_control import send,send_wait
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic
from device.motor_control import  motor_control_thread

def move_motor(angle, distance):
    print(angle+":"+str(distance))
    base_power = 45
    pwr = abs(distance//30) 
    if(angle == "left"):
        send_wait("CCW("+str(pwr)+")")
    elif (angle == "right"):
        send_wait("CW("+str(pwr)+")")
    else:
        pass #send_wait("MP(0,0)")

    #print(angle+" "+str(distance))

# ボトルの方向を向く
def face_red_bottle(camera_handle):
    print("START-face_red_bottle")
    object_bottom_line_y = 480
    while True:
        frame = read(camera_handle)
        contours_red = ctl_pic.detect_all_red_object(frame)
        contour_bottle = None
        contours_big = []
        for contour in contours_red:
            # 輪郭の面積を計算する
            area = cv2.contourArea(contour)
        
            # 面積が一定以上の場合にのみ処理を行う
            if area > 1500:  # 面積の閾値は適宜調整してください
                contours_big.append(contour)

                # 輪郭を取得
                x, y, w, h = cv2.boundingRect(contour)
                if ctl_pic.isVertical([x, y, w, h]):
                    contour_bottle = contour

        # ボトルが見つからない場合くっついている
        if contour_bottle is None :
            if len(contours_big):
                contour_bottle = contours_big[0]
                print("1つ")
            
        if (contour_bottle is not None):
            x, y, w, h = cv2.boundingRect(contour_bottle)
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 255), 2)
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 0), 2)
            frame_center = frame.shape[1] // 2
            object_center = x + w // 2
            if object_center < frame_center - 20:
                move_motor("left", frame_center - object_center)
            elif object_center > frame_center + 20:
                move_motor("right", frame_center - object_center)
            else:
                object_bottom_line_y = y + h
                break
            ctl_pic.draw_center_of_gravity(contour_bottle,frame)
        ctl_pic.draw_scale(frame)    
        show_camera_and_get_key('smart', frame)

        time.sleep(0.5)
    return object_bottom_line_y

def get_frame_cropped(frame):
        # 画像の高さと幅を取得
    height, width = frame.shape[:2]

    # 高さ60トリミング
    start_x = 0
    start_y = 0
    end_x = width
    end_y = 100

    # フレームをトリミング
    cropped_frame = frame[start_y:end_y, start_x:end_x]
    return cropped_frame

def mask_upper_of_line(image,x1, y1, x2, y2):
    """
    緑と白の境界線 (y = ax + b) より上側を黒くする。
    a: 境界線の傾き
    b: 境界線の切片
    """
    slope = (y2 - y1) / (x2 - x1)
    intercept = y1 - slope * x1

    # 画像の高さと幅を取得
    height, width = image.shape[:2]
    
    # 空のマスクを作成
    mask = np.zeros((height, width), dtype=np.uint8)
    
    # 境界線の左側を塗りつぶす
#    for x in range(width - 1, -1, -1):
    for x in range(width - 1, -1, -1):
        y = int(slope * x + intercept)  # y座標を計算
        if 0 <= y < height:  # y座標が画像の範囲内にある場合
            mask[:y, x] = 255 # 境界線より上を白で塗りつぶす
    
    # マスクを反転させる（左側を黒くする）
    mask = 255 - mask
    
    # 元画像にマスクを適用
    result = cv2.bitwise_and(image, image, mask=mask)
    
    return result

# 青マーカーの方向を向く
def face_blue_marker(camera_handle, is_left_course):
    print("START-face_blue_marker")
    while True:
        frame = read(camera_handle)
        frame = get_frame_cropped(frame)
        
        # 緑色の境界線を検出する。
        lines = ctl_pic.detect_green_white_boundary(frame)
        if lines is None:
            continue
        line_h = ctl_pic.get_bottom_most_line(lines,frame)
        ctl_pic.draw_line(line_h,frame)
        
        if line_h is not None:
            x1, y1, x2, y2 = line_h
            frame = mask_upper_of_line(frame,x1, y1, x2, y2)
        
        contour_blue = ctl_pic.detect_blue_object(frame)
           
        if (contour_blue is not None):
            x, y, w, h = contour_blue
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 255), 2)
            # cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 0), 2)
            frame_center = frame.shape[1] // 2
            object_center = x + w // 2
            if object_center < frame_center - 20:
                move_motor("left", frame_center - object_center)
            elif object_center > frame_center + 20:
                move_motor("right", frame_center - object_center)
            else:
                break
        else:
            if is_left_course is True:
                move_motor("right", 10)
            else :
                move_motor("left", 10)

        show_camera_and_get_key('smart', frame)

        time.sleep(0.5)


def approach_red_bottle(camera_handle, distance= 80):
    send_wait("FW("+str(distance)+",100,100)")
    # frame = read(camera_handle)
    # x, y, w, h = ctl_pic.detect_red_object(frame)
    # distance = ctl_pic.get_distance(x,y)
    # send_wait("FW("+str(distance)+",100,100)") 


def approach_circle(camera_handle,threshold_y):
    # 緑ラインのy座標を求める基準となるx座標
    base_x = 320    # 画像の半分
    # 共有メモリを使ってモータコントローラスレッドにデータを送る
    left_motor = Value('i', 0)
    right_motor = Value('i', 0)
    is_exit = Value('i', 0)
    sleep_time = 0.01

    # モーターコントローラを裏で起動する
    thread = threading.Thread(target=motor_control_thread, args=(left_motor,right_motor,sleep_time,is_exit))
    thread.start()

    right_motor.value = 60
    left_motor.value = 61

    while True:
        # カメラから画像を取得する
        frame = read(camera_handle)

        # 緑色の境界線を検出する。
        lines = ctl_pic.detect_green_white_boundary(frame)
        if lines is None:
            continue
        line_h = ctl_pic.get_bottom_most_line(lines,frame)
        ctl_pic.draw_line(line_h,frame)

        x1, y1, x2, y2 = line_h
        # 斜め線の画像中央付近のＹ座標を検出する
        slope = (y2 - y1) / (x2 - x1)
        intercept = y1 - slope * x1
        target_green_line_y = slope * base_x  + intercept
        # print(target_green_line_y)
        cv2.circle(frame, (int(base_x), int(target_green_line_y)), 5, (0, 255, 255), -1)  # 赤い丸を描画
        show_camera_and_get_key('smart', frame)
        # 一定位置に来たら処理を抜ける。
        if target_green_line_y > threshold_y:
            break

    right_motor.value = 0
    left_motor.value = 0
    is_exit.value = -1 # モータ制御を止める
    send("MP(0,0)")
    time.sleep(0.5)
    # send("ARM_SHAKE()")

def go_to_circle(camera_handle):
    send_wait("FW(80,100,100)")

def approach_blue_mark(camera_handle):
    # 共有メモリを使ってモータコントローラスレッドにデータを送る
    left_motor = Value('i', 0)
    right_motor = Value('i', 0)
    is_exit = Value('i', 0)
    sleep_time = 0.01

    # モーターコントローラを裏で起動する
    thread = threading.Thread(target=motor_control_thread, args=(left_motor,right_motor,sleep_time,is_exit))
    thread.start()

    while True:
        # カメラを起動し青オブジェクトの位置を確認する。
        frame = read(camera_handle)
        ret =  ctl_pic.detect_blue_object(frame)
        if ret is not None:
            x, y, w, h = ret
            distance = ctl_pic.get_distance(x,y)
            if distance < 10:
                break

    is_exit.value = -1 # モータ制御を止める
    send_wait("MP(0,0)")

    
def start(camera_handle, is_left_course):
    # while True:
    #     face_blue_marker(camera_handle)

    send("BEEP_ON()")
    cv2.namedWindow('smart')
    bottle_y = face_red_bottle(camera_handle) # ボトルの方向を向く
    print(f"bottle={bottle_y}")
    if (bottle_y > 150):
        send_wait("BW(10,50,50)")  
        approach_red_bottle(camera_handle, 85) # ボトルまで近づく
    else:
        approach_red_bottle(camera_handle, 75) # ボトルまで近づく

    if is_left_course is True:
        send_wait("CW(60,50,80)")     
        face_blue_marker(camera_handle, is_left_course)
        approach_circle(camera_handle,155)
    else:
        send_wait("CCW(60,50,80)")     
        face_blue_marker(camera_handle, is_left_course)
        approach_circle(camera_handle,155)
    #go_to_circle(camera_handle)
    return True