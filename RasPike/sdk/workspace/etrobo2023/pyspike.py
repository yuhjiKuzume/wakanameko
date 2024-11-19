import os
import time
import cv2
import numpy as np

from device.camera_control import init_camera, close_camera
import device.camera_control as ctl_cam
from device.serial_control import init_serial
from device.keyboard_control import init_keyboard
import device.picture_control as ctl_pic
from device.file_control import wait_start, wait_start_carry
from device.time_control import set_start_time, get_left_time

import scenario.scene_goal as goal
import scenario.scene_moveblock as moveblock
import scenario.scene_smartmove as smartmove

IS_LEFT_COURSE = True

def main():
    wait_start_carry() # 開始待ち

    # deviceの初期化
    set_start_time()
    init_serial()              # シリアルの初期化
    init_keyboard()             # キーボードの初期化
    cam_handle = init_camera()  # カメラの初期化
    
# シナリオの実行
    moveblock.start(cam_handle, IS_LEFT_COURSE)
    smartmove.start(cam_handle, IS_LEFT_COURSE)
    goal.start(cam_handle, IS_LEFT_COURSE)
    # deviceの終了処理
    close_camera(cam_handle)
    while True:
        time.sleep(1)

if __name__ == "__main__":
    main()
