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
    base_power = 50
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
                break
            ctl_pic.draw_center_of_gravity(contour_bottle,frame)
        ctl_pic.draw_scale(frame)    
        show_camera_and_get_key('smart', frame)

        time.sleep(0.5)

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

# 青マーカーの方向を向く
def face_blue_marker(camera_handle):
    print("START-face_red_bottle")
    while True:
        frame = read(camera_handle)
        frame = get_frame_cropped(frame)
        contour_blue = ctl_pic.detect_blue_object(frame)
           
        if (contour_blue is not None):
            x, y, w, h = contour_blue
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 255), 2)
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 0), 2)
            frame_center = frame.shape[1] // 2
            object_center = x + w // 2
            if object_center < frame_center - 20:
                move_motor("left", frame_center - object_center)
            elif object_center > frame_center + 20:
                move_motor("right", frame_center - object_center)
            else:
                break
        else:
            move_motor("right", 10)

        show_camera_and_get_key('smart', frame)

        time.sleep(0.5)


def approach_red_bottle(camera_handle):
    send_wait("FW(60,100,100)")
    # frame = read(camera_handle)
    # x, y, w, h = ctl_pic.detect_red_object(frame)
    # distance = ctl_pic.get_distance(x,y)
    # send_wait("FW("+str(distance)+",100,100)") 


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

    
def start(camera_handle):
    while True:
        face_blue_marker(camera_handle)

    # send("BEEP_ON()")
    # cv2.namedWindow('smart')
    # face_red_bottle(camera_handle) # ボトルの方向を向く
    # approach_red_bottle(camera_handle) # ボトルまで近づく
    # send_wait("CW(30,50,80)")     
    # face_blue_marker(camera_handle)
    # go_to_circle(camera_handle)
    return True



