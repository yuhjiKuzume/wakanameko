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
import scenario.scene_smartmove_test as smartmove_t
import scenario.scene_test as test
import scenario.scene_angle_test as angle_test

FULL_MODE = False
def main():
    # if FULL_MODE is True:
    #   wait_start() # 開始待ち
    # else:
    #   wait_start_carry() # 開始待ち

    # deviceの初期化
    set_start_time()
    init_serial()              # シリアルの初期化
    #init_bluetooth()            # シリアルの初期化
    #init_keyboard()             # キーボードの初期化
    cam_handle = init_camera()  # カメラの初期化
    #angle_test.start(cam_handle)
    
    # while True:
    #     image = ctl_cam.read(cam_handle)
    #     # 画像をHSV色空間に変換
    #     hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    #     # # 赤色の範囲を定義
    #     # lower_red1 = np.array([0, 120, 70])
    #     # upper_red1 = np.array([10, 255, 255])
    #     # lower_red2 = np.array([170, 120, 70])
    #     # upper_red2 = np.array([180, 255, 255])

    #     # # 赤色のマスクを作成
    #     # mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
    #     # mask2 = cv2.inRange(hsv, lower_red2, upper_red2)
    #     # mask = mask1 + mask2

    #     # 黄色のHSVの範囲を定義
    #     lower_yellow = np.array([20, 100, 100])
    #     upper_yellow = np.array([30, 255, 255])

    #     # 黄色のマスクを作成
    #     mask = cv2.inRange(hsv, lower_yellow, upper_yellow)

    #     # 赤いオブジェクトの輪郭を検出
    #     contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    #     # 赤いオブジェクトの中心を格納するリスト
    #     centers = []

    #     for contour in contours:
    #         # 輪郭のモーメントを計算
    #         M = cv2.moments(contour)
    #         if M['m00'] != 0:
    #             # 輪郭の中心を計算
    #             cx = int(M['m10'] / M['m00'])
    #             cy = int(M['m01'] / M['m00'])
    #             centers.append((cx, cy))
    #             # 各赤いオブジェクトの中心に緑の丸を描く
    #             cv2.circle(image, (cx, cy), 5, (0, 255, 0), -1)

    #     if len(centers) == 4:
    #         # 4つの中心の中間点を計算
    #         mid_x = int(sum([c[0] for c in centers]) / 4)
    #         mid_y = int(sum([c[1] for c in centers]) / 4)
    #         # 中間点に青い丸を描く
    #         cv2.circle(image, (mid_x, mid_y), 10, (255, 0, 0), -1)
    
    #     ctl_cam.show_camera_and_get_key('smart', image)
    #     time.sleep(1)    
    #cam_handle = None
    #angle_test.start(cam_handle)

# シナリオの実行
#    test.start(cam_handle)
#    chase_red.start(cam_handle)
    moveblock.start(cam_handle)
    smartmove.start(cam_handle)
    goal.start(cam_handle)
    
    # deviceの終了処理
    close_camera(cam_handle)
    while True:
        time.sleep(1)

if __name__ == "__main__":
    main()
