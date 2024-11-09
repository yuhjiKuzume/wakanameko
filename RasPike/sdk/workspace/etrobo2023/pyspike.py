import time
from device.camera_control import init_camera, close_camera
from device.serial_control import init_serial
from device.keyboard_control import init_keyboard
import device.picture_control as ctl_pic
from device.file_control import wait_start, wait_start_carry
from device.time_control import set_start_time, get_left_time

import scenario.scene_goal as goal
import scenario.scene_moveblock as moveblock
import scenario.scene_smartmove as smartmove
import scenario.scene_smartmove_test as smartmove_t
import scenario.scene_test as test
import scenario.scene_angle_test as angle_test
import os

FULL_MODE = False
def main():
    if FULL_MODE is True:
      wait_start() # 開始待ち
    else:
      wait_start_carry() # 開始待ち

    # deviceの初期化
    set_start_time()
    init_serial()              # シリアルの初期化
    #init_bluetooth()            # シリアルの初期化
    init_keyboard()             # キーボードの初期化
#    cam_handle = init_camera()  # カメラの初期化
    cam_handle = None
    angle_test.start(cam_handle)

# シナリオの実行
#    test.start(cam_handle)
#    chase_red.start(cam_handle)
    # moveblock.start(cam_handle)
    # smartmove.start(cam_handle)
    #goal.start(cam_handle)
    
    # deviceの終了処理
    close_camera(cam_handle)
    while True:
        time.sleep(1)

if __name__ == "__main__":
    main()
