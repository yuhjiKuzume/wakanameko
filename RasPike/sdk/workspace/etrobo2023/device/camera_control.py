import cv2
import numpy as np
import threading
import datetime

from picamera2 import Picamera2

camera_handle = None
camera_frame = None
CAM_DEBUG = True

def init_camera():
    global camera_handle
    # カメラを初期化
    camera_handle = Picamera2()
    mode = camera_handle.sensor_modes[3] 
    config = camera_handle.create_preview_configuration({"size":(1640,1232)}) 
    camera_handle.configure(config)
    camera_handle.start()
    return camera_handle

def close_camera():
    global camera_handle
    camera_handle.close()
    cv2.destroyAllWindows()

def read(camera_handle):
    frame = camera_handle.capture_array()
    frame = cv2.resize(frame,(640,480))
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    return frame

if __name__ == "__main__":
    cam_handle = init_camera()

    try:
        while True:
            camera_frame = camera_handle.capture_array()
            camera_frame = cv2.resize(camera_frame,(640,480))
            camera_frame = cv2.cvtColor(camera_frame, cv2.COLOR_BGR2RGB)
            if CAM_DEBUG is True:
                cv2.imshow('raw_image', camera_frame)

            key = cv2.waitKey(1)
            # 's'キーが押されたらsave
            if key == ord('s'):
                dt_now = datetime.datetime.now()
                file_name = dt_now.strftime('%Y%m%d_%H%M%S')
                cv2.imwrite(file_name+".jpg",camera_frame)
                
            # 'q'キーが押されたらループを終了
            if key == ord('q'):
                break

    except cv2.error:
        print("プログラムを終了します")
    finally:
        close_camera()
