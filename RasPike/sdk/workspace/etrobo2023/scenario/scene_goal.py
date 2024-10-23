import device.serial_control as ctl_ser
import time

def start():
    ctl_ser.send_wait("FW(10)")
    ctl_ser.send_wait("BW(10)")
#    time.sleep(1)
#    ctl_ser.send("motor_pair.pwm(0,0)")
#    ctl_ser.send("motor_pair.pwm(40, -40)")
#    ctl_ser.send_wait("motor_pair.pwm(0,0)")
#    time.sleep(1)
    while True:
        pass
