import cv2
import numpy as np
import threading
import ctypes
import datetime
import time

from device.camera_control import read, show_camera_and_get_key
from device.serial_control import send,send_wait
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic

def move_motor(angle, distance):
    print(angle+":"+str(distance))
    base_power = 40
    pwr = abs(distance//10) 
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

def read_video(camera_handle):
    while True:
        frame = read(camera_handle)
        show_camera_and_get_key('smart', frame)
        
def approach_red_bottle(camera_handle):
    frame = read(camera_handle)
    x, y, w, h = ctl_pic.detect_red_object(frame)
    distance = ctl_pic.get_distance(x,y)
    thread = start_thread(camera_handle)
    send_wait("FW("+distance+")") 
    #thread.raise_exception()

def start_thread(camera_handle):
    thread = threading.Thread(target=read_video,args=camera_handle)
    thread.daemon = True
    thread.start()
    return thread    

def go_to_circle(camera_handle):
    #x = twe(name = 'Thread A', target=read_video, args=(camera_handle))
    # スレッド開始
    #x.start()
    send_wait("CW(90)")     
    send_wait("FW(90)")
    # raise_exceptionを呼び出すことでスレッドが終了
    #x.raise_exception()
    # 既に終了しているので処理を待機しないはず
    #x.join()
    
    #thread = start_thread(camera_handle)
    #send_wait("CW(90)")     
    #send_wait("FW(90)")
    # thread.raise_exception()
    
def start(camera_handle):
    cv2.namedWindow('smart')
    face_red_bottle(camera_handle) # ボトルの方向を向く
    approach_red_bottle(camera_handle) # ボトルまで近づく
    go_to_circle(camera_handle)
        
    return True