from picamera2.picamera2 import *
import time
import subprocess
 
h264_file = "./video.h264"
mp4_file = "./video.mp4"

width = 1980
length = 1080
# 動画を撮影
def capture():
  picam2 = Picamera2()

  fullReso = picam2.camera_properties['PixelArraySize']  # センサー解像度
  aspectRatio = fullReso[ 0 ] / fullReso[ 1 ]            # アスペクト比(W/H)
  previewReso = ( int( 1000 * aspectRatio ), 1000 )        # プレビュー表示解像度
  camera_config = picam2.create_video_configuration(main={"size": previewReso}, raw={"size": fullReso})
  picam2.framerate = 30 # フレームレート設定
  encoder = H264Encoder(bitrate=10000000) # ビットレート設定
  
  # 設定を適用
  picam2.configure(camera_config)
  # カメラを起動
  picam2.start_recording(encoder, h264_file)
  time.sleep(20) # 10秒間録画
  picam2.stop_recording()
 
  # H264をMP4に変換するコマンド
  convert_command = f"ffmpeg -i {h264_file} -c:v copy {mp4_file}"

  # ffmpegコマンドを実行してMP4に変換
  subprocess.run(convert_command, shell=True)
