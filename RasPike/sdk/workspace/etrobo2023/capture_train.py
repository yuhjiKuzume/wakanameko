from picamera2.picamera2 import *
 
movie_file = "video.mp4"

# 動画を撮影
def capture():
  picam2 = Picamera2()

  picam2.configure(picam2.create_preview_configuration(main={"format": 'XRGB8888', "size": (1280, 960)}))

  picam2.start_and_record_video(movie_file, duration=7)
  picam2.close()

if __name__ == "__main__":
  capture()
