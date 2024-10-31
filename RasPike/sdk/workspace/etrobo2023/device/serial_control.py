import serial
import threading
import time

SER_DEBUG = False
ser = None

def init():
    global ser
    try:
        # シリアルポートの設定
        ser = serial.Serial(
            port='/dev/ttyACM0',  # 使用するシリアルポート
            #port='/dev/USB_SPIKE',  # 使用するシリアルポート
            baudrate=9600,      # ボーレートの設定
            timeout=1           # タイムアウトの設定
        )
    except serial.SerialException as e:
        print(f"Serial exception occurred: {e}")

    except Exception as e:
        print(f"An error occurred: {e}")

    finally:
        if ser.is_open:
            return True
    return False


# 受信バッファ
rec_buff =[]

# シリアル受信
def read_from_serial():
    global ser
    """シリアルポートからのデータを別スレッドで受信し、表示する関数"""
    while True:
        if ser.in_waiting > 0:
            data = ser.readline().decode('utf-8').strip()
            if data:
                rec_buff.append(data)
                if SER_DEBUG is True:
                    print(f"R:{data}")
        else:
            continue

def send(str):
    global ser
    ser.write(str.encode('utf-8')+b'\r\n')
#    ser.write(b'\r\n')   
    if SER_DEBUG is True:
        print(str)

def send_wait(str):
    global ser
    rec_buff.clear()
    ser.write(str.encode('utf-8'))
    ser.write(b'\r\n')   
    if SER_DEBUG is True:
        print(str)
    #print(rec_buff)
    while True:
        time.sleep(0.1)
        if SER_DEBUG is True:
            print(rec_buff)
        if 'True' in rec_buff:
            break



# シリアルハンドラ取得
def get_serial_handle():
    global ser
    return ser

# 受信バッファ取得
def get_serial_buff():
    global rec_buff
    return rec_buff

# 受信スレッド開始
def start_thread():
    print("start_thread")

    while True:
        ret = init()
        print(str(ret))
        if ret is True:
            thread = threading.Thread(target=read_from_serial)
            thread.daemon = True
            thread.start()
            return thread
        else:
            time.sleep(1)


# -------------
# SPIKEのプログラムを停止
def spike_all_stop():
    send("motor_pair.pwm(0,0)")

# -------------
# SPIKEのプログラムを停止
def spike_break():
    global ser
    ser.write(b'\x03')  # Control+Cを送信

# SPIKEのソフトウェアリブート
def spike_reboot():
    global ser
    ser.write(b'\x04')  # Control+Dを送信

