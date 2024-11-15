# LEGO type:standard slot:2 autostart
import time
import hub
import uasyncio
from uasyncio import Event
import gc
from hub import Image, display

DONT_CARE = -1
UNDEFINED = -2

print("Start")

# Port Map
port_map = {
    "motor_A": "A",
    "motor_B": "B",
    "motor_C": "E",
    "color_sensor": "C",
    "ultrasonic_sensor": "F",
    "spike_serial_port": "D",
}


# Device objects

hub.display.show(hub.Image.ALL_CLOCKS,delay=400,clear=True,wait=False,loop=True,fade=0)

# reset gyro
hub.motion.align_to_model(hub.FRONT,hub.TOP)
hub.motion.yaw_pitch_roll(0)
time.sleep(1)

while True:
    motor_A = getattr(hub.port, port_map["motor_A"]).motor
    motor_B = getattr(hub.port, port_map["motor_B"]).motor
    motor_C = getattr(hub.port, port_map["motor_C"]).motor
    color_sensor = getattr(hub.port, port_map["color_sensor"]).device
    ultrasonic_sensor = getattr(hub.port, port_map["ultrasonic_sensor"]).device
    motor_rot_A = getattr(hub.port, port_map["motor_A"]).device
    motor_rot_B = getattr(hub.port, port_map["motor_B"]).device
    motor_rot_C = getattr(hub.port, port_map["motor_C"]).device
    touch_sensor = hub.button.connect

    if (
        motor_A is None
        or motor_B is None
        or motor_C is None
        or color_sensor is None
        or ultrasonic_sensor is None
    ):
        continue
    break

# モーターの回転を逆にして指定したい場合、以下に-1を設定してください
invert_A = 1
invert_B = 1
invert_C = -1

color_sensor_mode = 0
ultrasonic_sensor_mode = 0

#最終の有効な通信を受け取った時刻
last_command_time = 0
#通信断絶を検知する時間
detect_com_fail_period = 5000000

motor_rot_A.mode(2)
motor_rot_B.mode(2)
motor_rot_C.mode(2)

# シリアルポートの設定
spike_serial_port = port_map["spike_serial_port"]

# Start of added Modules -----------------------------------------
spike_angle = 0       #spike_ヨー角
spike_color = None  #SPIKE RGB

# モーターペア化
motor_pair = motor_B.pair(motor_C)
def motor_rotation_B():
    return motor_rot_B.get()[0]

def motor_rotation_C():
    return motor_rot_C.get()[0] * -1

def wait_motor_stop():
    while True:
        before_b = motor_rotation_B()
        before_c = motor_rotation_C()
        time.sleep(0.1)
        after_b = motor_rotation_B()
        after_c = motor_rotation_C()
        # print(str(before_b)+","+str(after_b)+","+str(before_c)+","+str(after_c))
        if before_b != 0 and after_b != 0 and before_c != 0 and after_c != 0:
            if before_b == after_b and before_c == after_c:
                break

def WMS():
    wait_motor_stop()
    print("True")

# 左右モータ同時駆動
def MP(RPwr=40,LPwr=40):
    motor_pair.pwm(RPwr,-LPwr)

# 左右モータ同時駆動
def MP_B(RPwr=40,LPwr=40):
    motor_pair.pwm(RPwr,-LPwr)

# アーム制御
def ARM_UP(wait_msec=300):
    motor_A.run_for_time(wait_msec, -100, 100, 1, 1, 1)

def ARM_DOWN(wait_msec=300):
    motor_A.run_for_time(wait_msec, 100, 100, 1, 1, 1)

def ARM_SHAKE(wait_msec=300,count=2):
    wait_s = 0.001 * wait_msec
    for i in range(count):
        motor_A.run_for_time(wait_msec, -100, 100, 1, 1, 1)
        time.sleep(wait_s)
        motor_A.run_for_time(wait_msec, 100, 100, 1, 1, 1)
        time.sleep(wait_s)

def BEEP_ON(frq=1300):
    hub.sound.beep(frq,20)
    time.sleep(0.1)
    hub.sound.beep(frq,20)


def enable_cw(start_angle, current_angle, move_step  ):
    # -180～180座標系を0～360座標系に変換　
    start_angle += 180
    current_angle += 180
    target_angle = start_angle + move_step

    if(target_angle >= current_angle >= start_angle):
        return True
    elif (target_angle>=360):
        if (current_angle<=target_angle-360):
            return True
    return False


def enable_ccw(start_angle, current_angle, move_step ):
    # -180～180座標系を0～360座標系に変換　
    start_angle += 180
    current_angle += 180
    target_angle = start_angle - move_step

    if(target_angle <= current_angle <= start_angle):
        return True
    elif (target_angle <= 0):
        if (current_angle>=target_angle+360):
            return True
    return False

# 走行体右回転（CW方向）
def CW(deg,RPwr=50,LPwr=50,is_stop_black=False):
    global spike_angle
    start_angle = spike_angle = hub.motion.yaw_pitch_roll()[0]
    print("Y:"+str(spike_angle))
    motor_pair.pwm(-RPwr,-LPwr)
    while True:
        time.sleep(0.1)
        current_angle = spike_angle = hub.motion.yaw_pitch_roll()[0]
        print("Y:"+str(spike_angle))
        if (enable_cw(start_angle, current_angle, deg) is False):
            motor_pair.pwm(0,0)
            break
        if is_stop_black is True:
            r,g,b,_ = getRGB()
            sum_color = (r + g + b)//3
            print("r,g,b=%d,%d,%d" %(r,g,b))
            if sum_color < ((WHITE_BRIGHTNESS + BLACK_BRIGHTNESS) // 2):
                print("sum_color < ((WHITE_BRIGHTNESS + BLACK_BRIGHTNESS) // 2)")
                #print("Time Over command=%d fail=%d" % (num_command, num_fail))
                motor_pair.pwm(0,0)
                break

    wait_motor_stop()
    print("True")

# 走行体左回転（CCW方向）
def CCW(deg,RPwr=50,LPwr=50,is_stop_black=False):
    global spike_angle
    start_angle = spike_angle = hub.motion.yaw_pitch_roll()[0]
    print("Y:"+str(spike_angle))
    motor_pair.pwm(RPwr,LPwr)
    while True:
        time.sleep(0.1)
        current_angle = spike_angle = hub.motion.yaw_pitch_roll()[0]
        print("Y:"+str(spike_angle))
        if (enable_ccw(start_angle, current_angle, deg) is False):
            motor_pair.pwm(0,0)
            break
        if is_stop_black is True:
            r,g,b,_ = getRGB()
            sum_color = (r + g + b)//3
            print("r,g,b=%d,%d,%d" %(r,g,b))
            if sum_color < ((WHITE_BRIGHTNESS + BLACK_BRIGHTNESS) // 2):
                print("sum_color < ((WHITE_BRIGHTNESS + BLACK_BRIGHTNESS) // 2)")
                motor_pair.pwm(0,0)
                break

    wait_motor_stop()
    print("True")

# 走行体前進
def FW(cm,speed=50,power=50):
    pi = 3.14
    r = 5
    degree = cm/(2*pi*r)*360
    motor_pair.preset(0,0)
    #motor_pair.run_to_position(degree,-degree)
    motor_pair.run_to_position(degree,-degree,speed, power,1000,1000,0)
    wait_motor_stop()
    print("True")

def convert_rgb_to_hsv(rgb_val):
    r, g, b, _ = rgb_val
    #print(f"RGB values print >> R:{r:.2f}  G:{g:.2f}  B:{b:.2f}")

    # RGBのうち、最大・最小を取得
    max_val = max(r, g, b)
    min_val = min(r, g, b)
    delta = max_val - min_val

    # 色相(h)を求める
    if delta == 0:
        h = 0.0
    elif max_val == r:
        h = 60 * ((g - b) / delta)
    elif max_val == g:
        h = 60 * ((b - r) / delta) + 120
    else:
        h = 60 * ((r - g) / delta) + 240

    if h < 0:
        h += 360

    # 彩度(s)を求める
    if max_val == 0:
        s = 0.0
    else:
        s = (delta / max_val) * 100

    # 明度(v)を求める
    v = max_val

    return h ,s, v

# 定数の定義
LINE_TRACER_PERIOD = 1000  # 例として1000を設定
DELTA_T = LINE_TRACER_PERIOD / 1000.0 / 1000.0  # 処理周期

# グローバル変数の初期化
integral = 0.0  # 偏差の積分
diff_before = 0.0  # 1つ前の偏差
diff_after = 0.0  # 現在の偏差

# PIDパラメータの定義
KP_SOTO = 0.047
KI_SOTO = 0.007
KD_SOTO = 0.017

KP_UTI = 0.043
KI_UTI = 0.06
KD_UTI = 0.015

# PIDのクラス定義
class TargetPID:
    def __init__(self, KP, KI, KD):
        self.KP = KP
        self.KI = KI
        self.KD = KD


# PID制御による操作量の計算
def steering_amount_pid_calculation(rgb_val, target_color, selected_pid_parameter):
    global diff_before, diff_after, integral

    # 各色のPID閾値テーブル定義
    target_pid = [
        TargetPID(0.07, 0.024, 0.019),  # 直線用PID
        TargetPID(0.13, 0.035, 0.0055)  # 楕円用PID
    ]

    r,g,b,_ = rgb_val
    # RGBの合計値を計算
    sum_color = r + g + b

    # 目標輝度値とカラーセンサ値の差分を計算
    diff_before = diff_after
    diff_after = float(target_color - sum_color)
    integral += (diff_after + diff_before) / 2.0 * DELTA_T

    p = target_pid[selected_pid_parameter].KP * diff_after
    i = target_pid[selected_pid_parameter].KI * integral
    d = (target_pid[selected_pid_parameter].KD * (diff_after - diff_before)) / DELTA_T

    # 操作量を返す
    return p + i + d


WHITE_BRIGHTNESS = 170
BLACK_BRIGHTNESS = 10
STEERING_COEF = 0.2
def steering_amount_pid_calculation_p(rgb_val):
    target_brightness = (WHITE_BRIGHTNESS + BLACK_BRIGHTNESS) / 2
    # RGBの合計値を計算
    r,g,b,_ = rgb_val
    sum_color = (r + g + b)//3
    diff_brightness = (target_brightness - sum_color)
    #ステアリング操舵量を計算
    steering_amount = (diff_brightness * STEERING_COEF)
    return steering_amount


def motor_drive_control(steering_amount, trace_edge, dynamic_base_speed):
    # 左右モータ設定パワーの計算
    left_motor_power = int(dynamic_base_speed + (steering_amount * trace_edge))
    right_motor_power = int(dynamic_base_speed - (steering_amount * trace_edge))

    # モータパワーが100を超えた場合の調整
    if left_motor_power > 100 or right_motor_power > 100:
        print("motor_powerが100超えたので調節")
        if left_motor_power > right_motor_power:
            right_motor_power = int(100 * (right_motor_power / left_motor_power))
            left_motor_power = 100
        elif left_motor_power < right_motor_power:
            left_motor_power = int(100 * (left_motor_power / right_motor_power))
            right_motor_power = 100
        else:
            left_motor_power = 100
            right_motor_power = 100

    # 左右モータ駆動パワーの設定
    motor_pair.pwm(left_motor_power, -right_motor_power)
    #print('left_motor_power=%d,right_motor_power=%d' %(left_motor_power,right_motor_power))

# LineTrace
RIGHT_EDGE = (-1)
LEFT_EDGE = (1)
def LT(Pwr=50, blue_color_check = True):
    setRGB()
    time.sleep(0.1)
    while True:
        amount = steering_amount_pid_calculation_p(getRGB())
        motor_drive_control(amount,RIGHT_EDGE,Pwr)
        if blue_color_check is True:
            r,g,b,_= getRGB()
            if ((b) > (r+g)):
                break
        time.sleep(0.1)
    motor_pair.pwm(0, 0)


    print("True")

def FW_B(Pwr=50):
    setRGB()
    time.sleep(0.1)
    while True:
        r,g,b,_ = getRGB()
        sum_color = (r + g + b)//3
        if sum_color < ((WHITE_BRIGHTNESS + BLACK_BRIGHTNESS) // 2):
            break
        motor_pair.pwm(Pwr, -Pwr)
        time.sleep(0.1)
    motor_pair.pwm(0, 0)
    print("True")

def to_goal():
    FW_B()
    LT()

# 走行体後退
def BW(cm,speed=50,power=50):
    pi = 3.14
    r = 5
    degree = cm/(2*pi*r)*360
    motor_pair.preset(0,0)
    motor_pair.run_to_position(-degree,degree,speed, power,1000,1000,0)
    wait_motor_stop()
    print("True")

def getRot():
    pi = 3.14
    r = 5
    degB = motor_rotation_B()
    degC = motor_rotation_C()
    cm_b = degB*(2*pi*r)/360
    cm_c = degC*(2*pi*r)/360

    print("motor_B="+str(degB)+":"+str(cm_b)+"cm")
    print("motor_C="+str(degC)+":"+str(cm_c)+"cm")

# ヨー角
bActiveAngle = False
def setAngle(angle=0):
    hub.motion.yaw_pitch_roll(angle)

def getAngle():
    global spike_angle
    spike_angle = hub.motion.yaw_pitch_roll()[0]
    return spike_angle

def getYou():
    print("Y:"+str(getAngle()))

async def getAngles(wait_time):
    global spike_angle, bActiveAngle
    bActiveAngle = True
    while True:
        print("Y:"+str(getAngle()))
        time.sleep(wait_time)
        if bActiveAngle is False:
            break

def logAngle(wait_time=1):
    uasyncio.run(getAngles(wait_time))

def stopAngle():
    bActiveAngle = False

# RGBセンサー
def setRGB():
    color_sensor.mode(5)

def getRGB():
    global spike_color
    numbers = color_sensor.get()
    if len(numbers) != 4:
        setRGB()
    else:
        # 4で割った結果を新しいリストに格納
        spike_color = [num // 4 for num in numbers]
        #print(spike_color)
    return spike_color

bActiveRGB = False

async def getRGBs(wait_time):
    global spike_color, bActiveRGB
    bActiveRGB = True
    while True:
        print(getRGB())
        time.sleep(wait_time)
        if bActiveRGB is False:
            break

def logRGB(wait_time=1):
    uasyncio.run(getRGBs(wait_time))

def stopRGB():
    bActiveRGB = False

# Display
def RICOH():
    #display.show("RICOH IT SOLUTIONS")
    display.show("RICOH")
    image = Image("99909:90909:99909:99009:90909")
    display.show(image)

# End of added Modules -----------------------------------------

def detect_com_fail():
    hub.display.show(hub.Image.SAD,delay=400,clear=True,wait=False,loop=False,fade=0)
    hub.led(9)
    stop_all()


def wait_serial_ready():
    while True:
        reply = ser.read(1000)
        print(reply)
        if reply == b"":
            break


async def wait_read(ser, size):
    global last_command_time
    while True:
        buf = ser.read(size)
        if buf == b"" or buf is None:
            # 最終の有効コマンドを受け取ってから5秒経ったら、通信断絶として、画面表示を変える
            if last_command_time > 0 and (last_command_time + detect_com_fail_period ) < time.ticks_us():
                detect_com_fail()
            # readの間はタスクスイッチされないので、ここで制御を渡す
            await uasyncio.sleep_ms(0)
            continue
        #        print("Val=%d" %(int.from_bytes(buf,'big')))
        return buf


num_command = 0
num_fail = 0
prev_time = time.ticks_us()
count = 0
sum_time = 0
motor_reset_A =0
motor_reset_B =0
motor_reset_C =0
motor_stop_A = 0
motor_stop_B = 0
motor_stop_C = 0
color_sensor_change = 0
gyro_reset =0
other_command = 0
ultrasonic_sensor_change = 0
gyro_angle = 0
gyro_sensor_mode_change = 0

async def receiver():
    global num_command, num_fail, count, sum_time, prev_time
    global motor_reset_A,motor_reset_B,motor_reset_C,color_sensor_change,gyro_reset,other_command,ultrasonic_sensor_change,gyro_sensor_mode
    global gyro_angle,gyro_sensor_mode_change,last_command_time
    global motor_stop_A,motor_stop_B,motor_stop_C

    print(" -- start")
    value = 0
    cmd_id = 0

    global color_sensor_mode,ultrasonic_sensor_mode
    if True:
        while True:
            await uasyncio.sleep_ms(0)

            # ヘッダバイト(1st bitが1)であれば読み直す
            cmd = 0
            while True:
                cmd = int.from_bytes(await wait_read(ser, 1), "big")
                if cmd & 0x80:
                    # Found Header
                    #                print("Header Found")
                    break

                # Get ID
            while True:
                data1 = int.from_bytes(await wait_read(ser, 1), "big")
                num_command = num_command + 1
                if data1 & 0x80:
                    cmd = data1
                    num_fail = num_fail + 1
                    print ("data1 broken")
                    continue
                idx = cmd & 0x7F

                data2 = int.from_bytes(await wait_read(ser, 1), "big")
                if data2 & 0x80:
                    cmd = data2
                    num_fail = num_fail + 1
                    print ("data2 broken")
                    continue

                cmd_id = idx
                value = ((data1 & 0x1F) << 7) | data2
                # check +/-
                if data1 & 0x20:
                    value = value * (-1)

                break

            #print('cmd=%d,value=%d' %(cmd_id,value))
            if value < -2048 or value > 2048:
                #                print("Value is invalid")
                num_fail = num_fail + 1
                continue

            # 有効なコマンドを受け取った最後の時間
            last_command_time = time.ticks_us()

            # 高速化のために、motorスピードを優先して判定する
            if cmd_id == 1:
                motor_A.pwm(invert_A * value)
            elif cmd_id == 2:
                motor_B.pwm(invert_B * value)
            elif cmd_id == 3:
                motor_C.pwm(invert_C * value)
            #    count = count + 1
            #    now = time.ticks_us()
            #    sum_time = sum_time + (now - prev_time)
            #    prev_time = now
            elif cmd_id == 5:
                if value == 1:
                    motor_A.brake()
                else:
                    motor_A.float()
                motor_stop_A = 1
            elif cmd_id == 6:
                if value == 1:
                    motor_B.brake()
                else:
                    motor_B.float()
                motor_stop_B = 1
            elif cmd_id == 7:
                if value == 1:
                    motor_C.brake()
                else:
                    motor_C.float()
                motor_stop_C = 1
            elif cmd_id == 9:
                motor_A.preset(0)
                motor_reset_A = 1
            elif cmd_id == 10:
                motor_B.preset(0)
                motor_reset_B = 1
            elif cmd_id == 11:
                motor_C.preset(0)
                motor_reset_C = 1
            elif cmd_id == 61:
                # Port2 Color Sensor
                # Color Sensor Mode
                # 切り替えの間カラーセンサーの取得がされると不安定になるため、modeは一時的に0にする
                color_sensor_mode = 0
                if value == 1:
                    # Ambient
                    color_sensor.mode(2)
                elif value == 2:
                    # Color
                    color_sensor.mode(0)
                elif value == 3:
                    # Reflect
                    color_sensor.mode(1)
                elif value == 4:
                    # RGB
                    #print("Set RGB")
                    color_sensor.mode(5)
                #ダミーリード
                cv = color_sensor.get()
                color_sensor_mode = value
                color_sensor_change = 1
            elif cmd_id == 62:
                # Port3 Ultra Sonic Sensor
                led = b''+chr(9)+chr(9)+chr(9)+chr(9)
                ultrasonic_sensor.mode(5,led)
                if value == 1:
                    print("Ultrasonic Sensor")
                    ultrasonic_sensor.mode(0)
                elif value == 2:
                    print("Ultrasonic Sensor:Listen")
                    ultrasonic_sensor.mode(3)
                # 設定のまでのWait
                ultrasonic_sensor_mode = value
                ultrasonic_sensor_change = 1
            elif cmd_id == 13:
                #Port4 Gyro Sensor
                gyro_sensor_mode = 0
                if value == 1:
                    hub.motion.yaw_pitch_roll(0)
                gyro_reset = 1
            else:
                other_command = cmd_id



async def send_data(cmd, val):
    sendData = "@{:0=4}:{:0=6}".format(cmd, int(val))
    #print(sendData)
    ser.write(sendData)
    #高速で送るとパケットが落ちるため、0.5msec休ませる
    await uasyncio.sleep(0.0005)

async def send_ack(cmd):
    sendData = "<{:0=4}:000000".format(cmd)
    #print(sendData)
    ser.write(sendData)
    #高速で送るとパケットが落ちるため、0.5msec休ませる
    await uasyncio.sleep(0.0005)

async def notifySensorValues():
    print("Start Sensors")
    global ser
    global motor_reset_A,motor_reset_B,motor_reset_C,color_sensor_change,gyro_reset,other_command,ultrasonic_sensor_change
    global gyro_angle,gyro_sensor_mode_change
    global motor_stop_A,motor_stop_B,motor_stop_C
    global spike_angle

    touch_sensor_value = -1
    prev_button_command = 0
    long_period = 0
    long_period_count = 0
    #100msec周期
    long_period_time_us = 100000
    while True:
        # 次の更新タイミング  ここでは10msec
        next_time = time.ticks_us() + 10 * 1000





        # カラーセンサーの切り替えがあった場合、タイミングによってはget()がNoneになったり、
        # RGBではない値が取れたりするので、その場合は次の周期で通知する
        if color_sensor_mode == 1:
            color_val = color_sensor.get()
            if color_val is not None:
            # ambient
                await send_data(1, color_val[0])
        elif color_sensor_mode == 2:
            color_val = color_sensor.get()
            if color_val is not None:
                # color
                # TODO:Convert to EV3 Value
                await send_data(2, color_val[0])
        elif color_sensor_mode == 3:
            color_val = color_sensor.get()
            if color_val is not None:
            # Reflect
                await send_data(3, color_val[0])
        elif color_sensor_mode == 4:
            color_val = color_sensor.get()
            if color_val[0] is not None and len(color_val) == 4 and color_val[2] is not None:
                r = color_val[0]
                g = color_val[1]
                b = color_val[2]
                await send_data(4, r / 4)
                await send_data(5, g / 4)
                await send_data(6, b / 4)

        # 超音波センサー
        if ultrasonic_sensor_mode == 1:
            val = ultrasonic_sensor.get()[0]
            if val is None:
                val = -1
#            print("Val=%d" %(int(val)))
            await send_data(22, val)
        elif ultrasonic_sensor_mode == 2:
           await send_data(23, ultrasonic_sensor.get())

        # モーター出力
        await send_data(64, motor_rot_A.get()[0] * invert_A)
        await send_data(65, motor_rot_B.get()[0] * invert_B)
        await send_data(66, motor_rot_C.get()[0] * invert_C)

        # Gyro
        gyro = hub.motion.gyroscope()
        #10msec周期なので、その分を加算する
        await send_data(8,int(gyro[2]))
        spike_angle = hub.motion.yaw_pitch_roll()[0]
        # print("YOU="+str(spike_angle))
        await send_data(7,int(spike_angle))


        #タッチセンサー
        val = touch_sensor.is_pressed()
        if touch_sensor_value != val :
            touch_sensor_value = val
            sendVal = 0
            if touch_sensor_value:
                # Touchセンサーは加圧のアナログ値で、2048以上をタッチとして扱うため2048とする
                sendVal = 2048
            await send_data(28,sendVal)

        #Ackの処理
        #モーターリセット
        if motor_reset_A == 1:
            motor_reset_A = 0
            await send_ack(9)
        if motor_reset_B == 1:
            motor_reset_B = 0
            await send_ack(10)
        if motor_reset_C == 1:
            motor_reset_C = 0
            await send_ack(11)
        if color_sensor_change == 1:
            color_sensor_change = 0
            await send_ack(61)
        if ultrasonic_sensor_change == 1:
            ultrasonic_sensor_change = 0
            await send_ack(62)
        if gyro_reset == 1:
            gyro_reset = 0
            await send_ack(13)
        if gyro_sensor_mode_change == 1:
            gyro_sensor_mode_change = 0
            await send_ack(63)
        if motor_stop_A == 1:
            motor_stop_A = 0
            await send_ack(5)
        if motor_stop_B == 1:
            motor_stop_B = 0
            await send_ack(6)
        if motor_stop_C == 1:
            motor_stop_C = 0
            await send_ack(7)

        if other_command != 0:
            await send_ack(other_command)
            other_command =0


        #100msec周期で取る項目
        cur = time.ticks_us()
        if long_period < cur:
            #電流/電圧は1sec毎に送るが、位相はずらす
            if ( (long_period_count+5) % 10 ) == 0:
                await send_data(29,hub.battery.current())
            if ( (long_period_count) % 10 ) == 0:
                await send_data(30,hub.battery.voltage())

            #本体ボタン(100msec毎)
            button_command = 0
            if hub.button.left.is_pressed():
                button_command = 1
            if hub.button.right.is_pressed():
                button_command = button_command + 2
            if hub.button.center.is_pressed():
                button_command = button_command + 16
            if button_command != prev_button_command:
                await send_data(0,button_command)
                prev_button_command = button_command

            long_period = cur + long_period_time_us
            long_period_count = long_period_count + 1

        time_diff = next_time - time.ticks_us()
        #        print("timediff={}".format(time_diff))
        if time_diff < 0:
            time_diff = 0
        await uasyncio.sleep_ms(int(time_diff / 1000))


def stop_all():
    motor_A.pwm(0)
    motor_B.pwm(0)
    motor_C.pwm(0)


async def main_task():
    gc.collect()
    uasyncio.create_task(notifySensorValues())
    uasyncio.create_task(receiver())
    await uasyncio.sleep(10*60)
    global num_command, num_fail, count, sum_time, count
    stop_all()
    while True:
        print("Time Over command=%d fail=%d" % (num_command, num_fail))
        time.sleep(1)


#    print ("period = %dmsec num=%d" %((sVum_time/count)/1000,count))


if True:

    image = Image("99999:90090:99090:90090:99090")
    display.show(image)

    print(" -- serial init -- ")
    ser = getattr(hub.port, spike_serial_port)

    ser.mode(hub.port.MODE_FULL_DUPLEX)
    time.sleep(1)
    ser.baud(115200)

    wait_serial_ready()

    uasyncio.run(main_task())
