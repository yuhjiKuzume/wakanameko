import cv2
import numpy as np
import threading
import datetime
import time

from device.camera_control import read, show_camera_and_get_key

import device.camera_control as ctl_cam
import device.serial_control as ctl_ser
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic


# カスタムスレッドクラス
class twe(threading.Thread):
    def __init__(self, group=None, target=None, name=None, args=(), kwargs={}):
        threading.Thread.__init__(self, group=group, target=target, name=name)
        self.args = args
        self.kwargs = kwargs
        return
    
    def run(self):
        self._target(*self.args, **self.kwargs)

    def get_id(self):
        if hasattr(self, '_thread_id'):
            return self._thread_id
        for id, thread in threading._active.items():
            if thread is self:
                return id

    # 強制終了させる関数
    def raise_exception(self):
        thread_id = self.get_id()
        resu = ctypes.pythonapi.PyThreadState_SetAsyncExc(ctypes.c_long(thread_id), ctypes.py_object(SystemExit))
        if resu > 1:
            ctypes.pythonapi.PyThreadState_SetAsyncExc(ctypes.c_long(thread_id), 0)
            print('Failure in raising exception')
            
def move_motor1(angle, distance):
    print(angle+" "+str(distance))

def move_motor(angle, distance):
    print(angle+":"+str(distance))
    base_power = 40
    pwr = abs(distance//10) 
    if(angle == "left"):
        ctl_ser.send_wait("CCW("+str(pwr)+")")
    elif (angle == "right"):
        ctl_ser.send_wait("CW("+str(pwr)+")")
    else:
        pass #ctl_ser.send_wait("MP(0,0)")

    #print(angle+" "+str(distance))

# クリックした点を保存するリスト
points = []

# マウスイベント時に作動する関数
def select_point(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        # 左クリックで選択した点をリストに追加
        points.append((x, y))
        print(points)
    elif event == cv2.EVENT_RBUTTONDOWN and len(points) > 0:
        # 右クリックでリストから削除
        points.pop()
        print(points)

def get_click_num():
    return len(points)


def orient_toward_red_bottle(camera_handle):
    while True:
        frame = camera_handle.capture_array()
        frame = cv2.resize(frame,(640,480))
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        backup_frame = frame.copy()
    
    
def start(camera_handle):
    cv2.namedWindow('smart')
    frame = camera_handle.capture_array()
    frame = cv2.resize(frame,(640,480))
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    cv2.setMouseCallback('smart', select_point, frame)
    while True:
        frame = camera_handle.capture_array()
        frame = cv2.resize(frame,(640,480))
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        backup_frame = frame.copy()
        cv2.polylines(frame, [np.array(points)], True, (255,0,0), 2)
        
        contours_red = ctl_pic.detect_all_red_object(frame)
        contour_bottle = None
        contours_big = []
        for contour in contours_red:
            # 輪郭の面積を計算する
            area = cv2.contourArea(contour)
        
            # 面積が一定以上の場合にのみ処理を行う
            if area > 1500:  # 面積の閾値は適宜調整してください
                contours_big.append(contour)

                # 輪郭を取得
                x, y, w, h = cv2.boundingRect(contour)
                if ctl_pic.isVertical([x, y, w, h]):
                    contour_bottle = contour

                #cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
                #text = "CIRCLE:" if ctl_pic.isHorizontal([x, y, w, h]) else "BOTTLE:"
                #cv2.putText(frame, text+str(area), (x, y+h+10), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)

        # ボトルが見つからない場合くっついている
        if contour_bottle is None :
            if len(contours_big):
                contour_bottle = contours_big[0]
                print("1つ")
            
        if (contour_bottle is not None):
            x, y, w, h = cv2.boundingRect(contour_bottle)
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 255), 2)
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 0), 2)
            frame_center = frame.shape[1] // 2
            object_center = x + w // 2
            if object_center < frame_center - 20:
                move_motor("left", frame_center - object_center)
            elif object_center > frame_center + 20:
                move_motor("right", frame_center - object_center)
            else:
                move_motor("stop", frame_center - object_center)
            ctl_pic.draw_center_of_gravity(contour_bottle,frame)
        ctl_pic.draw_scale(frame)    
        ret, key = show_camera_and_get_key('smart', frame)

        if ret == False:
            break

        time.sleep(0.5)        
        
    return True

def go_to_circle(camera_handle):
    #x = twe(name = 'Thread A', target=read_video, args=(camera_handle))
    # スレッド開始
    #x.start()
    ctl_ser.send_wait("CW(90)")     
    ctl_ser.send_wait("FW(90)")
    # raise_exceptionを呼び出すことでスレッドが終了
    #x.raise_exception()
    # 既に終了しているので処理を待機しないはず
    #x.join()
    
    #thread = start_thread(camera_handle)
    #ctl_ser.send_wait("CW(90)")     
    #ctl_ser.send_wait("FW(90)")
    # thread.raise_exception()
    