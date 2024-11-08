import threading

import device.serial_control as serial

# キーボード入力の読み取り
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
            if text == "spike_break":
                serial.spike_break()
                print("break")
                break
            # テキストをシリアルポートに送信
            serial.send(text)
            #print(f"Sent: {text}")
    except KeyboardInterrupt:
        serial.spike_break()
        serial.spike_all_stop()
        print("プログラムを終了します")
    finally:
        serial.close()

# キーボード受信スレッド開始
def start_thread():
    thread = threading.Thread(target=read_from_keyboard)
    #thread.daemon = True
    thread.start()
    return thread

def init_keyboard():
    tread = start_thread()
