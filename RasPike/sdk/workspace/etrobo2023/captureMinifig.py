from picamera2.picamera2 import *
import time
import os
 
# 画像を撮影
def picam_save(count):
  picam2 = Picamera2()
 
  # 撮影した画像ファイルを保存するディレクトリ
  save_dir = "minifig_dir"
  if not os.path.exists(save_dir):
    os.makedirs(save_dir)
 
  # 撮影した画像の名前
  filename = f"minifig_{count}.jpg"
  file_path = os.path.join(save_dir, filename)
 
  # カメラ画角指定
  picam2.configure(picam2.create_preview_configuration(main={"format": 'XRGB8888', "size": (2304, 1296)}))
  # picam2.start_preview(Preview.QTGL)
  picam2.start()
  time.sleep(2)
 
  picam2.capture_file(file_path)
  picam2.close()
 
  # 撮影した画像の保存先パスを返す
  return file_path

if __name__ == "__main__":
  arg = int(sys.argv[1])
  filePath = picam_save(arg)

  # PCの共有フォルダに画像を送信
  mount_dir = "/mnt/share"
  os.system(f"sudo cp {filePath} {mount_dir}")