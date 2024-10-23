import device.camera_control as ctl_cam
import device.serial_control as ctl_ser
import device.keyboard_control as ctl_key

import scenario.scene_goal as goal
import scenario.scene_moveblock as moveblock
import scenario.scene_smartmove as smartmove
import scenario.scene_smartmove as smartmove
import scenario.scene_chase_red_object as chase_red

def main():

    cam_thread = ctl_cam.start_thread()
    ser_thread = ctl_ser.start_thread()
    key_thread = ctl_key.start_thread()
    ctl_ser.spike_break()

#    chase_red.start()
#    moveblock.start()
#    smartmove.start()
    goal.start()


if __name__ == "__main__":
    main()
