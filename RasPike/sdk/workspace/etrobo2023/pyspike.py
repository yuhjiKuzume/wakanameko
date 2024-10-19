import serial
import sys
import termios
import tty
import time
import serial
import threading
import cv2
import numpy
import datetime
import time

# カメラを初期化
cap = cv2.VideoCapture(0)


# シリアルポートの設定
ser = serial.Serial(
    port='/dev/ttyACM0',  # 使用するシリアルポート
    baudrate=9600,      # ボーレートの設定
    timeout=1           # タイムアウトの設定
)

def read_from_serial():
    """シリアルポートからのデータを別スレッドで受信し、表示する関数"""
    while True:
        if ser.in_waiting > 0:
            data = ser.readline().decode('utf-8').strip()
            if data:
                print(f"R:{data}")
        else:
            continue

# シリアルポートの設定
#ser = serial.Serial('/dev/ttyS0', baudrate=9600, timeout=1)

# キーボード入力の読み取り
def getch():
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    return ch

def read_from_keyboard():
    """からのデータを別スレッドで受信し、表示する関数"""
    try:
        while True:
            # ユーザーからの入力を取得
            # text = input("Enter text to send via serial: ")
            text = input()            
            if text == "exit":
                print("Exiting program.")
                break
                break
            if text == "reboot":
                ser.write(b'\x04')  # Control+Dを送信
                print("reboot")
                break
            # テキストをシリアルポートに送信
            ser.write(text.encode('utf-8'))
            ser.write(b'\r\n')
            
            #print(f"Sent: {text}")
    except KeyboardInterrupt:
        print("プログラムを終了します")
    finally:
        ser.close()


def main():
    # 別スレッドでシリアル受信を開始
    thread1 = threading.Thread(target=read_from_keyboard)
    thread1.daemon = True
    thread1.start()

    ser.write(b'\x03')  # Control+Cを送信

    thread2 = threading.Thread(target=read_from_serial)
    thread2.daemon = True
    thread2.start()


    while True:
        # フレームをキャプチャ
        ret, frame = cap.read()
        #if not ret:
        #    print("カメラからフレームを取得できませんでした")
        #    break
        cv2.imshow('mergeImg', frame)

        key = cv2.waitKey(1)
        # 's'キーが押されたらsave
        if key == ord('s'):
            dt_now = datetime.datetime.now()
            file_name = dt_now.strftime('%Y%m%d_%H%M%S')
            cv2.imwrite(file_name+".jpg",frame)
            
        # 'q'キーが押されたらループを終了
        if key == ord('q'):
            break

        if key == ord('t'):
            cap.set(cv2.CAP_PROP_SETTINGS,1)
            break


    # カメラとウィンドウを解放
    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
