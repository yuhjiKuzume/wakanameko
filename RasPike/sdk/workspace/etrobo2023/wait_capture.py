import time
import os
import capture_minifig
import test

# 監視するフォルダのパス
folder_to_watch = "./flagFolder"
start_time = 0

# フォルダを監視する関数
def watch_folder(folder, capture_count):
  # ミニフィグ撮影の実行を命じるフラグ
  # 指定したファイルが作成されたらミニフィグ撮影を実行
  file_to_watch = f"capture_flag_{count}.txt"

  print(f"{capture_count + 1}回目撮影")
  print(f"Monitoring folder: {folder}")

  while True:
    # フォルダ内のファイルリストを取得
    files_in_folder = os.listdir(folder)

    # 目的のファイルが存在するか確認
    if file_to_watch in files_in_folder:
      print(f"{file_to_watch} found! Executing curl command...")

      # ミニフィグ撮影
      capture_minifig.capture(capture_count)

      # 撮影が終了したら処理を終了する
      break
        
  # 1秒待つ
  time.sleep(1)

if __name__ == "__main__":
  count = 0
  while (count < 6):
    test.test()
    # watch_folder(folder_to_watch, count)
    count += 1