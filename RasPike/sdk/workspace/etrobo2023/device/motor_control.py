import cv2
import numpy as np
import threading
import datetime
import time
import platform
from multiprocessing import Value, Array, Process

MODULE_TEST = True
#if MODULE_TEST is not True:
import device.camera_control as ctl_cam 
from device.serial_control import send
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic



# モーターを動かす
# SPIKEに MP(XX,XX)を送る
# Power は-100(バック)～0（停止)～100(前進)
def move_motor(left_power, right_power):
    send_string = "MP("+str(left_power)+","+str(right_power)+")"
    if MODULE_TEST is True:
        print(send_string)
    else:
        send(send_string)

# モーターコントローラ(スレッド)
# 　共有メモリを監視し、モータを動かす
def motor_control_thread(left_power, right_power, sleep_time, is_exit):
    # 初期値の設定
    prev_left_power = left_power.value
    prev_right_power = right_power.value

    while True:
        # プロセスの終了処理(0以外が渡された時)
        if is_exit.value != 0 : break
        
        # 前回送信したデータと違いがあれば送信する
        if left_power.value != prev_left_power or right_power.value != prev_right_power:
            move_motor(left_power.value, right_power.value)
            prev_left_power = left_power.value
            prev_right_power = right_power.value

        # 監視間隔
        time.sleep(sleep_time)

