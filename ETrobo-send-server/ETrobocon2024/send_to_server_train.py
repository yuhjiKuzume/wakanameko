import os
import time
import shutil

# 監視するフォルダのパス
folder_to_watch = "C:\\Users\\r23600316\\Desktop\\raspi\\"

# フォルダを監視し指定したファイルが見つかれば画像判定を呼び出す
def watch_folder(folder):
    file_to_watch_train = "bestshot_train.jpg"

    print(f"Monitoring folder: {folder}")
    while True:
      # フォルダ内のファイルリストを取得
      files_in_folder = os.listdir(folder)

      # 目的のファイルが存在するか確認
      if file_to_watch_train in files_in_folder:
        dest_folder = "C:\\Users\\r23600316\\ETrobocon2024\\"
        file_Name = "bestshot_train.jpeg"
        dest_file = os.path.join(dest_folder, file_Name)
        target_file_path = folder_to_watch + file_to_watch_train
        shutil.copy(target_file_path, dest_file)

        os.system("UploadTrain.bat")
        print("プラレール画像を競技サーバへ送信完了")
    
        # ファイルを見つけた後に処理を終了する場合
        break
        
        # 1秒待つ
      time.sleep(1)

if __name__ == "__main__":
  watch_folder(folder_to_watch)
