import device.serial_control as ctl_ser
import time

def start():
    #ctl_ser.send_wait("motor_A.run_for_time(300, -100, 100, 1, 1, 1)")  # 上げる
    #ctl_ser.send_wait("motor_A.run_for_time(300, 100, 100, 1, 1, 1)")  # 下げる
    ctl_ser.send_wait("FW(10,40,40)")
    ctl_ser.send_wait("BW(10,40,40)")

    ctl_ser.send_wait("CW(45)")
    ctl_ser.send_wait("FW(20,40,40)")
    ctl_ser.send_wait("CCW(45)")
    ctl_ser.send_wait("FW(90,40,40)")
    ctl_ser.send_wait("CW(90)")
    ctl_ser.send_wait("FW(90,40,40)")

#    time.sleep(1)
#    ctl_ser.send("motor_pair.pwm(0,0)")
#    ctl_ser.send("motor_pair.pwm(40, -40)")
#    ctl_ser.send_wait("motor_pair.pwm(0,0)")
#    time.sleep(1)
    while True:
        pass
