import cv2
import numpy as np
import threading
import datetime
import platform

# どのデバッグ環境でもモジュールテストできるようにする
if platform.system() != 'Windows':
    USE_PICAMERA2 = True
    if USE_PICAMERA2 is True:
        from picamera2 import Picamera2
else:
    USE_PICAMERA2 = False
    
# opencv waitkey code
KEY_PAGEUP        = 0x210000
KEY_PAGEDOWN      = 0x220000
KEY_END           = 0x230000
KEY_HOME          = 0x240000
KEY_CURSOR_LEFT   = 0x250000
KEY_CURSOR_UP     = 0x260000
KEY_CURSOR_RIGHT  = 0x270000
KEY_CURSOR_DOWN   = 0x280000
KEY_ENTER         = 0xd
KEY_ESC           = 0x1B
KEY_SPACE         = 0x20
KEY_TAB           = 0x9
KEY_BACKSPACE     = 0x8
KEY_F1            = 0x700000
KEY_F2            = 0x710000
KEY_F3            = 0x720000
KEY_F4            = 7536640,
KEY_F5            = 7602176,
KEY_F6            = 7667712,
KEY_F7            = 7733248,
KEY_F8            = 7798784,
KEY_F9            = 7864320,
KEY_F10           = 7929856,
KEY_F11           = 7995392,
KEY_DELETE        = 3014656,
KEY_INSERT        = 2949120,
KEY_CTRL_Z        = 0x1A
KEY_CTRL_Y        = 0x19

if USE_PICAMERA2 is True:
    def init_camera():
        # カメラを初期化
        camera_handle = Picamera2()
        mode = camera_handle.sensor_modes[3] 
        config = camera_handle.create_preview_configuration({"size":(1640,1232)}) # for V2 camera
        camera_handle.configure(config)
        camera_handle.start()
        return camera_handle

    def read_camera(camera_handle):
        frame = camera_handle.capture_array()
        frame = cv2.resize(frame,(640,480))
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        return frame
else:
    def init_camera():
        if platform.system() == 'Windows':
            camera_handle = cv2.VideoCapture(0,cv2.CAP_DSHOW)
        else:
            camera_handle = cv2.VideoCapture(0)
        return camera_handle

    def read_camera(camera_handle):
        _, frame = camera_handle.read()
        return frame

def close_camera(camera_handle):
    camera_handle.release()
    cv2.destroyAllWindows()

def show_camera_and_get_key(title, frame):
    cv2.imshow(title,frame)
    key = cv2.waitKeyEx(1) # waitKeyだとカーソル押下しても0が返ってくる
    # 's'キーが押されたらsave
    if key == ord('s'):
        dt_now = datetime.datetime.now()
        file_name = dt_now.strftime('%Y%m%d_%H%M%S_')
        cv2.imwrite(file_name+title+".jpg",frame)

    # 'ESC'キーが押されたらループを終了
    if key == KEY_ESC:
        return False, key
    return True, key

if __name__ == "__main__":
    cam_handle = init_camera()
    print("USE_PICAMERA2") if USE_PICAMERA2 is True else None

    try:
        while True:
            camera_frame = read_camera(cam_handle)
            ret, key = show_camera_and_get_key('camera_test_image', camera_frame)
            if ret is False:
                break
                
    except cv2.error:
        print("プログラムを終了します")
    finally:
        close_camera(cam_handle)
