import time
import os
import capture_train

# 監視するフォルダのパス
folder_to_watch = "./flagFolder"
start_time = 0

# フォルダを監視する関数
def watch_folder(folder):
  # ミニフィグ撮影の実行を命じるフラグ
  # 指定したファイルが作成されたらミニフィグ撮影を実行
  file_to_watch = f"movie_flag.txt"

  print(f"Monitoring folder: {folder}")

  while True:
    # フォルダ内のファイルリストを取得
    files_in_folder = os.listdir(folder)

    # 目的のファイルが存在するか確認
    if file_to_watch in files_in_folder:
      print(f"{file_to_watch} found! Executing curl command...")

      # プラレール動画撮影
      capture_train.capture()
      print("撮影完了")
      time.sleep(5)
      # ベストショット判定
      os.system("python ./train/train_bestshot.py")

      # 撮影が終了したら処理を終了する
      break
        
  # 1秒待つ
  time.sleep(1)

if __name__ == "__main__":
  watch_folder(folder_to_watch)
