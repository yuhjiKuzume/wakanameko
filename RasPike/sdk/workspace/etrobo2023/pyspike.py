import time
from device.camera_control import init_camera, close_camera
from device.serial_control import init_serial
from device.keyboard_control import init_keyboard
import device.picture_control as ctl_pic

import scenario.scene_goal as goal
import scenario.scene_moveblock as moveblock
import scenario.scene_smartmove as smartmove
import scenario.scene_smartmove_test as smartmove_t
import scenario.scene_test as test
import os

folder_to_watch = "./flagFolder"

# フォルダを監視する関数
def watch_folder(folder):
  # ミニフィグ撮影の実行を命じるフラグ
  # 指定したファイルが作成されたらミニフィグ撮影を実行
  file_to_watch = f"startSmartCarry.txt"
        # char fileName[] = "etrobo2023/flagFolder/startSmartCarry.txt";

  print(f"Monitoring folder: {folder}")

  while True:
    # フォルダ内のファイルリストを取得
    files_in_folder = os.listdir(folder)

    # 目的のファイルが存在するか確認
    if file_to_watch in files_in_folder:
      print(f"{file_to_watch} found! Executing curl command...")
      break
        
  # 1秒待つ
  time.sleep(1)

def main():

    watch_folder(folder_to_watch)

    # deviceの初期化
    init_serial()              # シリアルの初期化
    #init_bluetooth()            # シリアルの初期化
    init_keyboard()             # キーボードの初期化
    cam_handle = init_camera()  # カメラの初期化

    # シナリオの実行
#    test.start(cam_handle)
#    chase_red.start(cam_handle)
    moveblock.start(cam_handle)
    smartmove.start(cam_handle)
    goal.start(cam_handle)
    
    # deviceの終了処理
    close_camera()
    while True:
        time.sleep(1)

if __name__ == "__main__":
    main()
