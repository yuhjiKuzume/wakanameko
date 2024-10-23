import sys
import tty
import termios
import threading

import device.serial_control as serial

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
            if text == "reboot":
                serial.spike_reboot()
                print("reboot")
                break
            if text == "break":
                serial.spike_break()
                print("break")
                break
            # テキストをシリアルポートに送信
            serial.send(text)
         
            #print(f"Sent: {text}")
    except KeyboardInterrupt:
        print("プログラムを終了します")
    finally:
        serial.close()

# キーボード受信スレッド開始
def start_thread():
    thread = threading.Thread(target=read_from_keyboard)
    thread.daemon = True
    thread.start()
    return thread
