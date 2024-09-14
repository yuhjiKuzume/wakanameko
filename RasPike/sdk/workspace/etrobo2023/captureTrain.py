from picamera2.picamera2 import *
import time
import subprocess
 
h264_file = "./video.h264"
mp4_file = "./video.mp4"

# 動画を撮影
def capture_train():
  picam2 = Picamera2()

  picam2.resolution = (1280, 720) # 解像度設定
  picam2.framerate = 30 # フレームレート設定
  encoder = H264Encoder(bitrate=10000000) # ビットレート設定
  picam2.start_recording(encoder, h264_file)
  time.sleep(10) # 10秒間録画
  picam2.stop_recording()
 
  # H264をMP4に変換するコマンド
  convert_command = f"ffmpeg -i {h264_file} -c:v copy {mp4_file}"

  # ffmpegコマンドを実行してMP4に変換
  subprocess.run(convert_command, shell=True)

if __name__ == "__main__":
  capture_train()
