from picamera2.picamera2 import *
import cv2
import time
import os
 
# 撮影した画像ファイルを保存するディレクトリ
save_dir = "minifig_photo"

# 画像を撮影
def capture(count):
  picam2 = Picamera2()
 
  if not os.path.exists(save_dir):
    os.makedirs(save_dir)
 
  # 撮影した画像の名前
  filename = f"minifig_{count}.jpg"
  file_path = os.path.join(save_dir, filename)
 
  # カメラ画角指定
  picam2.configure(picam2.create_preview_configuration(main={"format": 'XRGB8888', "size": (1280, 960)}))
  
  # 撮影
  picam2.start_and_capture_files(file_path, num_files=3, delay=0.5)

  target_file_path = f"./{save_dir}/resize_minifig_{count}.jpg"
  img = cv2.imread(file_path)
  resize_file = cv2.resize(img, (800, 600))
  cv2.imwrite(target_file_path, resize_file)

  # PCの共有フォルダに画像を送信
  mount_dir = "/mnt/share"
  os.system(f"sudo cp {target_file_path} {mount_dir}")
 
  # 撮影した画像の保存先パスを返す
  # return file_path

# if __name__ == "__main__":
#   arg = int(sys.argv[1])
#   filePath = capture(arg)

  # target_file_path = f"./{save_dir}/resize_minifig_{arg}.jpg"
  # img = cv2.imread(filePath)
  # resize_file = cv2.resize(img, (800, 600))
  # cv2.imwrite(target_file_path, resize_file)

#   # PCの共有フォルダに画像を送信
#   mount_dir = "/mnt/share"
#   os.system(f"sudo cp {target_file_path} {mount_dir}")