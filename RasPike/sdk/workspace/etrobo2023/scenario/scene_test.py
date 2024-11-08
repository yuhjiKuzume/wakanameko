import cv2
import numpy as np
import threading
import datetime
import time

import device.camera_control as ctl_cam
import device.serial_control as ctl_ser
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic

def move_motor(str):
    pass

def motor_ctrl():
    time.sleep(1)

def motor_ctrl_old():
    ctl_ser.send("motor_pair.pwm(100,-100)")
    time.sleep(1)
    ctl_ser.send("motor_pair.pwm(100,-50)")
    time.sleep(0.2)
    ctl_ser.send("motor_pair.pwm(100,-100)")
    time.sleep(1)
    ctl_ser.send("motor_pair.pwm(0,0)")


def isBlueCircleObject(rectangle, frame):
    x, y, w, h = rectangle
    # 中央の1ピクセルの色判定
    center_pixel = frame[y+h//2, x+w//2]
    is_white = np.all(center_pixel >= 190)  # RGB値が190以上なら「白」と判定

    b, g, r = center_pixel
#    text = f"RGB: ({r}, {g}, {b})"
#    cv2.putText(frame, text, (320, 400), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 0, 0), 2)        # フレームの右下にテキストを表示

    # 白でない場合（赤の場合、ボトルが乗っている）
    if(is_white is False):
        # 赤の閾値を設定 (ここでは単純にR > 150として判定)
        if r > 150 and g < 100 and b < 100:
            is_white = True

    return is_white

def isLandscape(rectangle):
    x, y, w, h = rectangle
    # ひらべったい場合はサークル
    if w > h:
        return True
    else:
        return False


# 赤のサークルか判定（真ん中が白の場合はサークル、真ん中が青の場合もサークル）
def isRedCircleObject(rectangle, frame):
    x, y, w, h = rectangle
    # 中央の1ピクセルの色判定
    center_pixel = frame[y+h//2, x+w//2]
    is_white = np.all(center_pixel >= 190)  # RGB値が190以上なら「白」と判定
    b, g, r = center_pixel
 #   text = f"RGB: ({r}, {g}, {b})"
 #   cv2.putText(frame, text, (0, 400), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 255), 2)        # フレームの右下にテキストを表示
    # 白でない場合（赤の場合、ボトルが乗っている）
    if(is_white is False):
        # RGB値を表示するテキストを準備
        # 青の閾値を設定 (ここでは単純にB > 150として判定)
        if b > 150 and g < 100 and r < 100:
            is_white = True


    return is_white

def start(camera_handle):
    thread = threading.Thread(target=motor_ctrl)
    thread.daemon = True
    thread.start()

    while True:
        frame = camera_handle.capture_array()
        frame = cv2.resize(frame,(640,480))
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            
        cropped_frame = ctl_pic.get_frame_cropped(frame)
        cv2.imshow('cropped', cropped_frame)

        # -------------------------
        # Get Infomation
        #lines = ctl_pic.detect_lines(frame)
        #green_line = ctl_pic.detece_green_white_boundary(frame)


        result_red = ctl_pic.detect_red_object(frame)
        if result_red:
            x, y, w, h = result_red
            #red_text = "CIRCLE" if isBlueCircleObject(result_blue, frame) else "[Danger]"
            red_text = "CIRCLE" if isLandscape(result_red) else "[Danger]"
        else:
            red_text = ""

        result_blue = ctl_pic.detect_blue_object(frame)
        if result_blue:
            #blue_text = "CIRCLE" if isBlueCircleObject(result_blue, frame) else "DEBRIS"
            blue_text = "CIRCLE" if isLandscape(result_blue) else "DEBRIS"
            frame_center = frame.shape[1] // 2
            object_center = x + w // 2
            if object_center < frame_center - 20:
                move_motor("left")
            elif object_center > frame_center + 20:
                move_motor("right")
            else:
                move_motor("stop")
        else:
            blue_text = ""
            move_motor("unrecognize")

        # -------------------------
        # Draw Infomation
        #ctl_pic.draw_lines(frame, lines)
        #ctl_pic.draw_green_white_boundary(frame)

        if result_blue:
            x, y, w, h = result_blue
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            cv2.putText(frame, blue_text, (x, y+h+10), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)

        if result_red:
            x, y, w, h = result_red
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 255), 2)
            cv2.putText(frame, red_text, (x, y+h+10), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)

        cv2.imshow('frame', frame)

        key = cv2.waitKey(1)
        # 's'キーが押されたらsave
        if key == ord('s'):
            dt_now = datetime.datetime.now()
            file_name = dt_now.strftime('%Y%m%d_%H%M%S')
            cv2.imwrite(file_name+".jpg",frame)
            
        # 'q'キーが押されたらループを終了
        if key == ord('q'):
            break