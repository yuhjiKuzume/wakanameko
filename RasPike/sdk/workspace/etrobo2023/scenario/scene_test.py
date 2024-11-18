import cv2
import numpy as np
import threading
import datetime
import time

from device.camera_control import read, show_camera_and_get_key
from device.serial_control import send, send_wait

import device.camera_control as ctl_cam
import device.serial_control as ctl_ser
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic

def move_motor(str):
    pass

def motor_ctrl():
    time.sleep(1)

def motor_ctrl_old():
    ctl_ser.send("motor_pair.pwm(100,-100)")
    time.sleep(1)
    ctl_ser.send("motor_pair.pwm(100,-50)")
    time.sleep(0.2)
    ctl_ser.send("motor_pair.pwm(100,-100)")
    time.sleep(1)
    ctl_ser.send("motor_pair.pwm(0,0)")


def isBlueCircleObject(rectangle, frame):
    x, y, w, h = rectangle
    # 中央の1ピクセルの色判定
    center_pixel = frame[y+h//2, x+w//2]
    is_white = np.all(center_pixel >= 190)  # RGB値が190以上なら「白」と判定

    b, g, r = center_pixel
#    text = f"RGB: ({r}, {g}, {b})"
#    cv2.putText(frame, text, (320, 400), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 0, 0), 2)        # フレームの右下にテキストを表示

    # 白でない場合（赤の場合、ボトルが乗っている）
    if(is_white is False):
        # 赤の閾値を設定 (ここでは単純にR > 150として判定)
        if r > 150 and g < 100 and b < 100:
            is_white = True

    return is_white

def isLandscape(rectangle):
    x, y, w, h = rectangle
    # ひらべったい場合はサークル
    if w > h*1.2:
        return True
    else:
        return False


# 赤のサークルか判定（真ん中が白の場合はサークル、真ん中が青の場合もサークル）
def isRedCircleObject(rectangle, frame):
    x, y, w, h = rectangle
    # 中央の1ピクセルの色判定
    center_pixel = frame[y+h//2, x+w//2]
    is_white = np.all(center_pixel >= 190)  # RGB値が190以上なら「白」と判定
    b, g, r = center_pixel
 #   text = f"RGB: ({r}, {g}, {b})"
 #   cv2.putText(frame, text, (0, 400), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 255), 2)        # フレームの右下にテキストを表示
    # 白でない場合（赤の場合、ボトルが乗っている）
    if(is_white is False):
        # RGB値を表示するテキストを準備
        # 青の閾値を設定 (ここでは単純にB > 150として判定)
        if b > 150 and g < 100 and r < 100:
            is_white = True


    return is_white

def start2(camera_handle):
    thread = threading.Thread(target=motor_ctrl)
    thread.daemon = True
    thread.start()

    while True:
        frame = camera_handle.capture_array()
        frame = cv2.resize(frame,(640,480))
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        
        contours_red = ctl_pic.detect_all_red_object(frame)
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

            
        result_red = ctl_pic.detect_red_object(frame)
        if result_red:
            x, y, w, h = result_red
            #red_text = "CIRCLE" if isBlueCircleObject(result_blue, frame) else "[Danger]"
            red_text = "CIRCLE" if isLandscape(result_red) else "[Danger]"
        else:
            red_text = ""

        result_blue = ctl_pic.detect_blue_object(frame)
        if result_blue:
            #blue_text = "CIRCLE" if isBlueCircleObject(result_blue, frame) else "DEBRIS"
            blue_text = "CIRCLE" if isLandscape(result_blue) else "DEBRIS"
            frame_center = frame.shape[1] // 2
            object_center = x + w // 2
            if object_center < frame_center - 20:
                move_motor("left")
            elif object_center > frame_center + 20:
                move_motor("right")
            else:
                move_motor("stop")
        else:
            blue_text = ""
            move_motor("unrecognize")

        # -------------------------
        # Draw Infomation
        #ctl_pic.draw_lines(frame, lines)
        #ctl_pic.draw_green_white_boundary(frame)

        if result_blue:
            x, y, w, h = result_blue
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            cv2.putText(frame, blue_text, (x, y+h+10), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)

        if result_red:
            x, y, w, h = result_red
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 255), 2)
            cv2.putText(frame, red_text, (x, y+h+10), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)

        ret, _ = show_camera_and_get_key('frame', frame)
        if ret is False:
            break
        

def get_frame_cropped(frame):
        # 画像の高さと幅を取得
    height, width = frame.shape[:2]

    # 画面下半分にトリミング
    start_x = 0
    start_y = height // 2
    end_x = start_x + width
    end_y = height * 3 // 4

    # フレームをトリミング
    cropped_frame = frame[start_y:end_y, start_x:end_x]
    return cropped_frame
        
def start_get_target_by_arm_direction(camera_handle):
    while True:
        frame = read(camera_handle)
        cropped = get_frame_cropped(frame)
        if show_camera_and_get_key('frame', frame) is False:
            break
        if show_camera_and_get_key('cropped', cropped) is False:
            break

def find_midpoint(x1, y1, x2, y2):
    mid_x = (x1 + x2) / 2
    mid_y = (y1 + y2) / 2
    return mid_x, mid_y

def start(camera_handle):
    while True:
        frame = read(camera_handle)
        detect_yellow_objects(frame)
        if show_camera_and_get_key('frame', frame) is False:
            break
        
def detect_yellow_objects(frame):
    # 画像を読み込む
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # 黄色の範囲を定義
    lower_yellow = np.array([20, 100, 100])
    upper_yellow = np.array([30, 255, 255])

    # 黄色のマスクを作成
    mask = cv2.inRange(hsv, lower_yellow, upper_yellow)

    # 輪郭を検出
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    # 輪郭を面積でソートして大きいものからnum_objects個を抽出
    contours = sorted(contours, key=cv2.contourArea, reverse=True)[:2]

    # 黄色い物体の中点を計算
    centers = []
    for contour in contours:
        M = cv2.moments(contour)
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
            centers.append((cX, cY))

    # オブジェクト描画
    for contour in contours:
        x, y, w, h = cv2.boundingRect(contour)
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

    # 中点を画像に描画
    for center in centers:
        cv2.circle(frame, center, 5, (0, 255, 0), -1)

    if len(centers) == 2:

        cv2.circle(frame, find_midpoint(centers[0],centers[1]), 5, (0, 255, 0), -1)

