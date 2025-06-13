import os
import time
import shutil
from ultralytics import YOLO

# 監視するフォルダのパス
folder_to_watch = "C:\\Users\\r23600316\\Desktop\\raspi\\"

# フォルダを監視し指定したファイルが見つかれば画像判定を呼び出す
def watch_folder(folder, capture_count):
    file_to_watch = f"resize_minifig_{capture_count}.jpg"

    print(f"{capture_count + 1}回目画像取得")
    print(f"Monitoring folder: {folder}")
    while True:
        # フォルダ内のファイルリストを取得
        files_in_folder = os.listdir(folder)

        # 目的のファイルが存在するか確認
        if file_to_watch in files_in_folder:
            print(f"{file_to_watch} found! Executing curl command...")

            target_file_path = folder_to_watch + file_to_watch
            result = predict_minifig(target_file_path)

            # バウンディングボックスの座標を取得
            bounding_boxes = result.boxes.xyxy
            # クラスIDを取得
            class_ids = result.boxes.cls
            # クラス名の辞書を取得
            class_names_dict = result.names

            for box, class_id in zip(bounding_boxes, class_ids):
                class_name = class_names_dict[int(class_id)]
                print(f"Box coordinates: {box}, Object: {class_name}")
                if (class_name == "bestshot"):
                    dest_folder = "C:\\Users\\r23600316\\ETrobocon2024\\"
                    if not os.path.exists(dest_folder):
                        os.makedirs(dest_folder)

                    filename = "bestshot_minifig.jpeg"
                    dest_file = os.path.join(dest_folder, filename)
                    shutil.copy(target_file_path, dest_file)
                    os.system("UploadMinifig.bat")
                    print("ミニフィグ画像を競技サーバへ送信完了")
                    return True
                
            if (capture_count == 5):
                dest_folder = "C:\\Users\\r23600316\\ETrobocon2024\\"
                if not os.path.exists(dest_folder):
                    os.makedirs(dest_folder)

                filename = "bestshot_minifig.jpeg"
                dest_file = os.path.join(dest_folder, filename)
                shutil.copy(target_file_path, dest_file)
                os.system("UploadMinifig.bat")
                print("ミニフィグ画像を競技サーバへ送信完了")
                return True
                

            # ファイルを見つけた後に処理を終了する場合
            break
        
        # 1秒待つ
        time.sleep(1)
    return False

# YOLOv8で画像判定
def predict_minifig(target_file_path):
    model = YOLO("C:\\Users\\r23600316\\ETrobocon2024\\Minifig_frontAndback_train\\weights\\best.pt")

    results = model.predict(target_file_path, save=True, conf=0.1)

    return results[0]

if __name__ == "__main__":
    count = 0
    sent_to_server = False
    while (count < 6):
        sent_to_server = watch_folder(folder_to_watch, count)
        if (sent_to_server):
            break
        else:
            count += 1
