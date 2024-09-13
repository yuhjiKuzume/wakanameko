from picamera2.picamera2 import *
import time
import os
 
def picam_save():
  picam2 = Picamera2()
 
  save_dir = "minifig"
  if not os.path.exists(save_dir):
    os.makedirs(save_dir)
 
  timestamp = time.strftime("%Y%m%d_%H%M%S")
  filename = f"minifig_{timestamp}.jpg"
  file_path = os.path.join(save_dir, filename)
 
  picam2.start_preview(Preview.QTGL)
  picam2.start()
  time.sleep(2)
 
  picam2.capture_file(file_path)
  picam2.close()
 
  return file_path