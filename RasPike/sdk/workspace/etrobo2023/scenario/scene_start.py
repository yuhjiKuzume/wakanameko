import cv2
import numpy as np
import threading
import datetime
import time
import platform
from multiprocessing import Value, Array, Process

MODULE_TEST = True
#if MODULE_TEST is not True:
from device.camera_control import read, show_camera_and_get_key
import device.camera_control as ctl_cam 
from device.serial_control import send
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic
from device.motor_control import  motor_control_thread


# このプログラムの開始
def start(camera_handle):
    # 共有メモリを使ってモータコントローラスレッドにデータを送る
    left_motor = Value('i', 0)
    right_motor = Value('i', 0)
    is_exit = Value('i', 0)
    sleep_time = 0.01

    # モーターコントローラを裏で起動する
    thread = threading.Thread(target=motor_control_thread, args=(left_motor,right_motor,sleep_time,is_exit))
    thread.start()
    
    # カメラからの入力
    while True:
        if __name__ == "__main__": 
            ret ,frame = camera_handle.read()
        else:
            read()
#        if show_frame_and_get_key("frame",frame) == False:
#            print("False")
#            break

        KEY_ESC           = 0x1B
        KEY_CURSOR_LEFT   = 0x250000
        KEY_CURSOR_UP     = 0x260000
        KEY_CURSOR_RIGHT  = 0x270000
        KEY_CURSOR_DOWN   = 0x280000
        ret, key = show_camera_and_get_key("frame",frame)
        if key != -1:
            print(hex(key))

        elif key == KEY_CURSOR_UP:
            print("UP")
            left_motor.value +=10
            right_motor.value +=10

        elif key == KEY_CURSOR_RIGHT:
            print("RIGHT")
            right_motor.value +=10

        elif key == KEY_CURSOR_LEFT:
            print("LEFT")
            left_motor.value +=10

        elif key == KEY_CURSOR_DOWN:
            print("DOWN")
            left_motor.value -=10
            right_motor.value -=10

        # 'ESC'キーが押されたらループを終了
        elif key == KEY_ESC:
            print("Pushed ESC")
            break

if __name__ == "__main__":
    camera_handle = ctl_cam.init_camera()
    start(camera_handle)

    camera_handle.release()
    cv2.destroyAllWindows()
    print("END")

# move_tankは未実装
#motor_pair.move_tank(1, 'seconds', left_speed=-60, right_speed=-60)

#最高速度で90度曲がる。
#def MV():
#motor_pair.float()
#MP(100,100)
#time.sleep(0.5)
#MP(100,80)
#time.sleep(0.5)
#MP(100,100)
#time.sleep(0.5)
#MP(0,0)


