import bluetooth
import serial
import time
import subprocess

#動作しない。作成中。
#https://qiita.com/azarashin/items/9fcd18174c199c62b679

SER_BLUE_DEBUG = False
ser_bluetooth = None
sock_bluetooth = None
alive_bluetooth = True

def init_bluetooth():
    # ダミー送信で接続する
    #command = "echo RICOH() > /dev/rfcomm0"
    #subprocess.run(['sudo', 'sh', '-c', command], check=True)    # コマンドをsudoで実行

    print("start_bluetooth_spike")
    while True:
        ret = initialize_port("FD:BC:30:DF:09:90", 1, '/dev/rfcomm0' )
        print(str(ret))

        if ret is True:
            thread = threading.Thread(target=read_from_bluetooth)
            thread.daemon = True
            thread.start()
            return thread
        else:
            time.sleep(1)

def initialize_port(mac_address, channel, com_port):
    global ser_bluetooth, sock_bluetooth, alive_bluetooth, SER_BLUE_DEBUG

    # Bluetoothデバイスのアドレスとポートを設定
    bd_addr = mac_address  # 接続したいデバイスのBluetoothアドレス
    port = channel  # 通常は1

    try:
        # Bluetoothソケットを作成して接続
        sock_bluetooth = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        sock_bluetooth.connect((bd_addr, port))

        # シリアル通信の設定
        ser_bluetooth = serial.Serial(com_port, baudrate=9600, timeout=1)

    except serial.SerialException as e:
        print(f"Serial exception occurred: {e}")

    except Exception as e:
        print(f"An error occurred: {e}")

    finally:
        if ser_bluetooth.is_open:
            return True
    return False
    

# 接続を閉じる
def close_bluetooth():
    global ser_bluetooth, sock_bluetooth, alive_bluetooth, SER_BLUE_DEBUG

    alive_bluetooth = False
    ser_bluetooth.close()
    sock_bluetooth.close()
    

# 受信バッファ
rec_bluetooth_buff =[]

# シリアル受信
def read_from_bluetooth():
    global ser_bluetooth, sock_bluetooth, alive_bluetooth, SER_BLUE_DEBUG
    try:
        """シリアルポートからのデータを別スレッドで受信し、表示する関数"""
        while True:
            if ser_bluetooth.in_waiting > 0:
                data = ser_bluetooth.readline().decode('utf-8').strip()
                if data:
                    rec_bluetooth_buff.append(data)
                    print(f"R:{data}") if SER_BLUE_DEBUG is True else None
            else:
                continue
    except KeyboardInterrupt:
        print("Bluetooth thread 終了します")

    finally:
        # 接続を閉じる
        close_bluetooth()


def send(str):
    global ser_bluetooth, sock_bluetooth, alive_bluetooth, SER_BLUE_DEBUG
    ser_bluetooth.write(str.encode('utf-8')+b'\r\n')
#    ser_bluetooth.write(b'\r\n')   
    print(str) if SER_BLUE_DEBUG is True else None

def send_wait(str):
    global ser_bluetooth, sock_bluetooth, alive_bluetooth, SER_BLUE_DEBUG
    ret = True
    rec_bluetooth_buff.clear()
    ser_bluetooth.write(str.encode('utf-8'))
    ser_bluetooth.write(b'\r\n')   
    print(str) if SER_BLUE_DEBUG is True else None

    #print(rec_bluetooth_buff)
    while True:
        time.sleep(0.1)
        print(rec_bluetooth_buff) if SER_BLUE_DEBUG is True else None
        if 'True' in rec_bluetooth_buff:
            break
        if 'False' in rec_bluetooth_buff:
            ret = False
            break
    return ret


# シリアルハンドラ取得
def get_serial_handle():
    global ser_bluetooth
    return ser_bluetooth

# 受信バッファ取得
def get_serial_buff():
    global rec_bluetooth_buff
    return rec_bluetooth_buff

# -------------
# SPIKEの全てのモータを停止
def spike_all_stop():
    send("stop_all()")

# -------------
# SPIKEのプログラムを停止
def spike_break():
    global ser_bluetooth
    ser_bluetooth.write(b'\x03')  # Control+Cを送信

# SPIKEのソフトウェアリブート
def spike_reboot():
    global ser_bluetooth
    ser_bluetooth.write(b'\x04')  # Control+Dを送信

