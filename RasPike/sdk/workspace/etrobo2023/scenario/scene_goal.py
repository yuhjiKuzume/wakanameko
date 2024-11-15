import cv2
import numpy as np
import threading
import ctypes
import datetime
import time
from multiprocessing import Value, Array, Process

from device.camera_control import read, show_camera_and_get_key
from device.serial_control import send, send_wait
import device.camera_control as ctl_cam
import device.serial_control as ctl_ser
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic


# ゴールの方向を向く
def face_blue_goal(camera_handle):
    print("START-face_blue_goal")
    while True:
        frame = read(camera_handle)
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
            ctl_pic.draw_center_of_gravity(contour_blue,frame)
        ctl_pic.draw_scale(frame)    
        show_camera_and_get_key('smart', frame)
        time.sleep(0.5)
    print("END-face_blue_goal")

# 回転
def move_motor1(angle, distance):
    print(angle+":"+str(distance))
        
def move_motor(angle, distance):
    print(angle+":"+str(distance))
    pwr = abs(distance//40) 
    if(angle == "left"):
        send_wait("CCW("+str(pwr)+")")
    elif (angle == "right"):
        send_wait("CW("+str(pwr)+")")
    else:
        pass

def go_to_goal1(camera_handle):
    send("LT(40)")
    while True: 
        frame = read(camera_handle)         # カメラから画像を取得
        result_blue = ctl_pic.detect_blue_object(frame)  # 青オブジェクトの検出
        if result_blue:
            x, y, w, h = result_blue
            distance = ctl_pic.get_distance(x,y)
        else:
            send_wait("MP(0,0)")
            break
        show_camera_and_get_key('smart', frame)
        

def read_video(camera_handle):
    while True:
        frame = ctl_cam.read(camera_handle)
        show_camera_and_get_key('smart', frame)

# ターン＆
def turn_and_go_to_line():
    send("BEEP_ON()")
    send("ARM_SHAKE(300,2)")
    send_wait("BW(15,60,60)")
    send_wait("CCW(115,60,60)")
    send_wait("FW_B(50)")
    send_wait("CCW(35,60,60,True)")


# 固定モーター制御(スレッド)
def go_to_goal(camera_handle):
    approach_blue_line(camera_handle)

def approach_blue_line(camera_handle):
    frame = read(camera_handle)
    x, y, w, h = ctl_pic.detect_blue_object(frame)
    distance = ctl_pic.get_distance(x,y)
    send_wait("FW("+str(distance)+")") 
    #thread.raise_exception()

def wait_motor_sequence(thread, camera_handle):
    thread.start()
    # モータ制御が終わるまでカメラは読み捨て
    while thread.is_alive():
        image = read(camera_handle)
        show_camera_and_get_key('smart', image)
        time.sleep(1)    

def start(camera_handle):
    # モーターコントローラを裏で起動する
    # ターンして黒い線まで行く
    thread = threading.Thread(target=turn_and_go_to_line)
    print("D")
    wait_motor_sequence(thread,camera_handle)
    print("E")

    # # ゴールの方向に向く
    face_blue_goal(camera_handle)

    # # ゴールまで移動
    go_to_goal(camera_handle)

    send("BEEP_ON()")
    send("ARM_SHAKE(300,2)")
    send("RICOH()")


    # # go_to_goal(camera_handle)
