import time
import device.camera_control as ctl_cam
import device.serial_control as ctl_ser
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic

import scenario.scene_goal as goal
import scenario.scene_moveblock as moveblock
import scenario.scene_smartmove as smartmove
import scenario.scene_smartmove as smartmove
import scenario.scene_test as test

def main():
    ser_thread = ctl_ser.start_thread() # シリアル受信スレッド起動
    key_thread = ctl_key.start_thread() # キーボード受信スレッド起動
    ctl_ser.spike_break()

    cam_handle = ctl_cam.init_camera()
#    test.start(cam_handle)

#    chase_red.start(cam_handle)
#    moveblock.start(cam_handle)
#    smartmove.start(cam_handle)
#    goal.start(cam_handle)
    while True:
        time.sleep(1)



if __name__ == "__main__":
    main()
