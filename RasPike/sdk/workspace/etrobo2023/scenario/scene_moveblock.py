import cv2
import numpy as np
import threading
import datetime
import time

from device.camera_control import read,show_camera_and_get_key  
from device.serial_control import send,send_wait
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic




# Rコース 以下のコースを通る
#  ○-----○-----○-----○
#  |     |     |     |
#  |  ●●●●●●●●●●●●●●●●>>
#  |  ●  |     |     | 35cm
#  ○--●--○-----○-----○
#  |  ●  |     |     |
#  |  ●  |     |     | 35cm 
#  |  ●  |     |     |
#  ○--●--○-----○-----○
#  |  ●  |     |     |
#  |  ●  |     |     | 35cm
#  |  ●  |     |     |
#  ★--●--○-----○-----○
#  | ●
#  ●

def start(camera_handle):
    send("BEEP_ON()")

    frame = read(camera_handle)
    show_camera_and_get_key('frame', frame)

    # ★の所にあるのが、デブリブロックなら移動する
    if isDebrisBlock(frame) is True:
        send_wait("FW(10)")        # 前進10cm
        send_wait("BW(10)")        # バック10cm

    send_wait("CW(45)")        # 右45度
    send_wait("FW(20,40,40)")  # 前進20cm
    send_wait("CCW(45)")       # 左45度
    send_wait("FW(90,40,40)")  # 前進90cm
    send_wait("CW(90)")        # 右90度
    send_wait("FW(90,40,40)")  # 前進90cm

def isDangerBlock(frame):

    # 赤オブジェクトの検出
    result_red = ctl_pic.detect_red_object(frame)
    if result_red:
        # 縦長である
        if ctl_pic.isVertical(result_red):
            x, y, w, h = result_red
            frame_center_x = frame.shape[1] // 2
            object_center_x = x + w // 2
            frame_center_y = frame.shape[0] // 2
            object_center_y = y + h // 2

            # 画面左右の中央、上下の真ん中より下にある
            if(abs(frame_center_x-object_center_x)<20) and (object_center_y > 200):
                return True
    return False

def isDebrisBlock(frame):

    # 青オブジェクトの検出
    result_blue = ctl_pic.detect_blue_object(frame)
    if result_blue:
        # 縦長である
        if ctl_pic.isVertical(result_blue):
            x, y, w, h = result_blue
            frame_center_x = frame.shape[1] // 2
            object_center_x = x + w // 2
            frame_center_y = frame.shape[0] // 2
            object_center_y = y + h // 2

            # 画面左右の中央、上下の真ん中より下にある
            if(abs(frame_center_x-object_center_x)<20) and (object_center_y > 200):
                return True
    return False


