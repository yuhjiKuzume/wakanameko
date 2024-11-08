import cv2
import numpy as np
import threading
import ctypes
import datetime
import time
import device.camera_control as ctl_cam
import device.serial_control as ctl_ser
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic


# ゴールの方向を向く
def face_blue_goal(camera_handle):
    print("START-face_blue_goal")
    while True:
        frame = ctl_cam.read(camera_handle)
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
        cv2.imshow('smart', frame)

        cv2.waitKey(1)
        time.sleep(0.5)

# 回転
def move_motor1(angle, distance):
    print(angle+":"+str(distance))
        
def move_motor(angle, distance):
    print(angle+":"+str(distance))
    pwr = abs(distance//10) 
    if(angle == "left"):
        ctl_ser.send_wait("CCW("+str(pwr)+")")
    elif (angle == "right"):
        ctl_ser.send_wait("CW("+str(pwr)+")")
    else:
        pass

def go_to_goal(camera_handle):
    ctl_ser.send("LT(40)")
    while True: 
        frame = ctl_cam.read(camera_handle)         # カメラから画像を取得
        result_blue = ctl_pic.detect_blue_object(frame)  # 青オブジェクトの検出
        if result_blue:
            x, y, w, h = result_blue
            distance = ctl_pic.get_distance(x,y)
        else:
            ctl_ser.send_wait("MP(0,0)")
            break
        cv2.imshow('smart', frame)
        cv2.waitKey(1)        
        

def read_video(camera_handle):
    while True:
        frame = ctl_cam.read(camera_handle)
        cv2.imshow('smart', frame)
        cv2.waitKey(1)
            
def start(camera_handle):
    ctl_ser.send("BEEP_ON()")
    ctl_ser.send("ARM_SHAKE(300,2)")
    ctl_ser.send_wait("BWA(15,50,50)")
    ctl_ser.send_wait("CCW(115)")
    ctl_ser.send_wait("FW_B(40)")
    ctl_ser.send_wait("CCW(35,50,50,True)")
    #read_video(camera_handle)
    #face_blue_goal(camera_handle)
    go_to_goal(camera_handle)
