import cv2
import numpy as np


import cv2
import numpy as np
import threading
import datetime
from time import sleep

from device.camera_control import read, show_camera_and_get_key
from device.serial_control import send,send_wait,get_serial_buff,clear_serial_buff
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic

def draw_compass(degree):
    # 画像サイズ
    img_size = 200
    center = img_size // 2
    radius = center - 20
    degree -= 90

    # 空の画像を作成
    img = np.ones((img_size, img_size, 3), dtype=np.uint8) * 255

    # コンパスの円を描く
    cv2.circle(img, (center, center), radius, (0, 0, 0), 2)

    # 方角のラベルを描く
    directions = ['North', 'East', 'South', 'West']
    angles = [90, 180, -90, 0]  # 下が北になるように調整
    for direction, angle in zip(directions, angles):
        x = int(center + radius * np.cos(np.radians(angle)))
        y = int(center + radius * np.sin(np.radians(angle)))
        cv2.putText(img, direction, (x - 20, y + 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 1, cv2.LINE_AA)

    # 指定された度数の矢印を描く
    arrow_x = int(center + radius * np.cos(np.radians(degree)))
    arrow_y = int(center + radius * np.sin(np.radians(degree)))
    cv2.arrowedLine(img, (center, center), (arrow_x, arrow_y), (0, 0, 255), 2)

    # 画像として保存
    # while True:
    cv2.imshow("a",img)
    key = cv2.waitKeyEx(1) # waitKeyだとカーソル押下しても0が返ってくる
        # # 's'キーが押されたらsave
        # if key == ord('s'):
        #     dt_now = datetime.datetime.now()
        #     file_name = dt_now.strftime('%Y%m%d_%H%M%S_')
        #     cv2.imwrite(file_name+title+".jpg",frame)

        # # 'ESC'キーが押されたらループを終了
        # if key == 0x1B:
        #     break

# 使用例
degree = -180  # ここに度数を入力
# draw_compass(degree)

def start(camera_handle):
    while True:
        clear_serial_buff()
        send("getYou()")
        sleep(0.5)
        rec = get_serial_buff()
        print(rec)
        draw_compass(int(rec[1]))
