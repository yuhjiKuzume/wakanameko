import cv2
import numpy as np
import threading
import datetime
import time
from multiprocessing import Value, Array, Process

from device.camera_control import read,read_camera,show_camera_and_get_key  
from device.serial_control import send,send_wait
import device.keyboard_control as ctl_key
import device.picture_control as ctl_pic
from device.motor_control import  motor_control_thread



# Rコース 以下のコースを通る
#  ○-----○-----○-----○
#  |     |     |     |
#  |  ●●●●●●●●●●●●●●●●>>
#  |  ●  |     |     | 35cm
#  ○--●--○-----○-----○
#  |  ●  |     |     |
#  |  ●  |     |     | 35cm 
#  |  ●  |     |     |
#  ○--●--○-----○-----○
#  |  ●  |     |     |
#  |  ●  |     |     | 35cm
#  |  ●  |     |     |
#  ★--●--○-----○-----○
#  | ●
#  ●

def start(camera_handle):
    # send("BEEP_ON()")
    frame = read(camera_handle)
    show_camera_and_get_key('frame', frame)

    # ★の所にあるのが、デブリブロックなら移動する
    # if isDebrisBlock(frame) is True:
    #     print("isDebrisBlock is True")
    #     send_wait("FW(10,60,60)")        # 前進10cm
    #     send_wait("BW(10,60,60)")        # バック10cm

    send_wait("CCW(40)")        # 右45度
    send_wait("FW(20,60,60)")  # 前進20cm
    send_wait("CW(40)")       # 左45度

    green_line_trace(camera_handle)
    
    send_wait("CW(75)")        # 右90度
    correct_angle(camera_handle) # ２つのラインの真ん中に向くように走行体を補正
    send_wait("FW(115,60,60)")  # 前進115cm

    # thread = threading.Thread(target=turn_and_face_1)
    # print("A")
    # wait_motor_sequence(thread,camera_handle)
    # print("B")
    # correct_angle(camera_handle) # ２つのラインの真ん中に向くように走行体を補正

    # thread = threading.Thread(target=turn_and_face_2)
    # print("C")
    # wait_motor_sequence(thread,camera_handle)
    # print("D")
    # correct_angle(camera_handle) # ２つのラインの真ん中に向くように走行体を補正

    # thread = threading.Thread(target=turn_and_face_3)
    # print("E")
    # wait_motor_sequence(thread,camera_handle)
    # print("F")
    # correct_angle(camera_handle) # ２つのラインの真ん中に向くように走行体を補正
    

    
def green_line_trace(camera_handle):
    # 共有メモリを使ってモータコントローラスレッドにデータを送る
    left_motor = Value('i', 0)
    right_motor = Value('i', 0)
    is_exit = Value('i', 0)
    sleep_time = 0.01

    # モーターコントローラを裏で起動する
    thread = threading.Thread(target=motor_control_thread, args=(left_motor,right_motor,sleep_time,is_exit))
    thread.start()

    base_speed = 50

    start_time = time.time()
    while True:
        frame = read(camera_handle)
        lines = ctl_pic.detect_green_white_boundary(frame)

        line_v = ctl_pic.get_right_most_line(lines,frame)
        if line_v is not None:
            ctl_pic.draw_line(line_v,frame)
            line_h = ctl_pic.get_bottom_most_line(lines,frame)

            ctl_pic.draw_line(line_h,frame)
            if line_h is not None:
                intersection_x, intersection_y= ctl_pic.find_intersection(line_v, line_h)
            
                if line_h is None:
                    intersection_y = 0
                    intersection_x = ctl_pic.find_x_at_y0(line_v)
                     
                else:
                    # 交点を計算
                    intersection_x, intersection_y = ctl_pic.find_intersection(line_v, line_h)
                
                    # 黄色で交点を描画
                    cv2.circle(frame, (intersection_x, intersection_y), 5, (0, 255, 255), -1)  # 黄色で描画

                    power = (320 - intersection_x)//20
                    # print(power,intersection_x,intersection_y)

                left_motor.value = base_speed + power
                right_motor.value = base_speed - power

                print(intersection_y)
                if intersection_y > 145:
                    is_exit.value = -1 # モータ制御を止める
                    send("MP(0,0)")
                    print("intersection_y > 400")
                    break;            

        end_time = time.time()
        diff_time = end_time - start_time
        # print(diff_time)
        # if (diff_time) > 4:
        #     send("MP(0,0)")
        #     is_exit.value = -1
        #     print("time up")
        #     break

        ret , key = show_camera_and_get_key('frame', frame)
        if ret is False:
            break
        time.sleep(0.1)
    print("done")



 
def wait_motor_sequence(thread, camera_handle):
    thread.start()
    # モータ制御が終わるまでカメラは読み捨て
    while thread.is_alive():
        image = read(camera_handle)
        show_camera_and_get_key('smart', image)
        time.sleep(1)    

def turn_and_face_1():
    send_wait("CW(45,40,40)")        # 右45度
    send_wait("FW(20,60,60)")  # 前進20cm
    send_wait("CCW(45,40,40)")       # 左45度
    

def turn_and_face_2():
    send_wait("FW(90,40,40)")  # 前進90cm
    send_wait("CW(80,60,60)")        # 右90度

def turn_and_face_3():
    send_wait("FW(90,60,60)")  # 前進90cm
    
    
def isDangerBlock(frame):

    # 赤オブジェクトの検出
    result_red = ctl_pic.detect_red_object(frame)
    if result_red:
        # 縦長である
        if ctl_pic.isVertical(result_red):
            x, y, w, h = result_red
            frame_center_x = frame.shape[1] // 2
            object_center_x = x + w // 2
            frame_center_y = frame.shape[0] // 2
            object_center_y = y + h // 2

            # 画面左右の中央、上下の真ん中より下にある
            if(abs(frame_center_x-object_center_x)<20) and (object_center_y > 200):
                return True
    return False

def isDebrisBlock(frame):

    # 青オブジェクトの検出
    result_blue = ctl_pic.detect_blue_object(frame)
    if result_blue:
        # 縦長である
        if ctl_pic.isVertical(result_blue):
            x, y, w, h = result_blue
            frame_center_x = frame.shape[1] // 2
            object_center_x = x + w // 2
            frame_center_y = frame.shape[0] // 2
            object_center_y = y + h // 2

            # 画面左右の中央、上下の真ん中より下にある
            if(abs(frame_center_x-object_center_x)<20) and (object_center_y > 200):
                return True
    return False

def move_motor(angle, distance):
    print(angle+":"+str(distance))
    base_power = 40
    pwr = abs(distance//40) 
    if(angle == "left"):
        send_wait("CCW("+str(pwr)+")")
    elif (angle == "right"):
        send_wait("CW("+str(pwr)+")")
    else:
        pass #send_wait("MP(0,0)")

def get_frame_cropped(frame):
        # 画像の高さと幅を取得
    height, width = frame.shape[:2]

    # 中央部分を1/4にトリミング
    start_x = 0 # width // 4
    start_y = height // 4
    end_x = start_x + width # // 2
    end_y = start_y + height // 2

    # フレームをトリミング
    cropped_frame = frame[start_y:end_y, start_x:end_x]
    return cropped_frame

def correct_angle(camera_handle):
    print("correct_angle - START")
    while True:
        frame = read_camera(camera_handle)
        cropped_frame = get_frame_cropped(frame)
        x, y = get_center_from_two_lines(cropped_frame)
        if x is not None:
            cv2.circle(frame, (x, y), 10, (255, 0, 0), -1)    
            frame_center = frame.shape[1] // 2
            object_center = x
            
            if object_center < frame_center - 20:
                move_motor("left", frame_center - object_center)
            elif object_center > frame_center + 20:
                move_motor("right", frame_center - object_center)
            else:
                move_motor("stop", frame_center - object_center)
                break
        # time.sleep(1)
        show_camera_and_get_key("title", frame)
    print("correct_angle - END")

def get_center_from_two_lines(frame):
    x = None
    y = None

    height, width = frame.shape[:2]

    # フレームを左右半分に分割
    left_half = frame[:, :width//2]
    right_half = frame[:, width//2:]
    
    # それぞれで直線を取得
    left_lines = get_lines_frame(left_half)
    right_lines = get_lines_frame(right_half)

    # 一番真ん中に近い線を抽出
    left_most_line_right_half = get_left_most_line(right_lines, width, frame)
    right_most_line_left_half = get_right_most_line(left_lines, width, frame)

    # print(right_most_line_left_half,left_most_line_right_half)

    if ((left_most_line_right_half is not None) and (right_most_line_left_half is not None)):
        x = (left_most_line_right_half[0] + right_most_line_left_half[2])//2
        y = (left_most_line_right_half[3] + right_most_line_left_half[1])//2
    # elif (left_most_line_right_half is not None):
    #     x = 0
    #     y = 0
    # elif (right_most_line_left_half is not None):
    #     x = width
    #     y = 0
        

    # show_camera_and_get_key("left_half", left_half)
    # show_camera_and_get_key("right_half", right_half)
    return x, y

def get_lines_frame(frame_half):
    # グレースケールに変換
    gray = cv2.cvtColor(frame_half, cv2.COLOR_BGR2GRAY)
    
    # Cannyエッジ検出器を使用してエッジを検出
    edges = cv2.Canny(gray, 50, 150)

    # HoughLinesPを使用して直線を検出
    lines = cv2.HoughLinesP(edges, 1, np.pi/180, threshold=50, minLineLength=100, maxLineGap=10)
    return lines

def get_left_most_line(lines, width, frame):
    left_most_line = None
    # 右半分で一番左の直線を検出（左上から右下）
    if lines is not None:
        min_x1 = width  # 大きな値で初期化
        for line in lines:
            x1, y1, x2, y2 = line[0]
            if x1 < min_x1 and x1 < x2:
                min_x1 = x1
                left_most_line = (x1 + width//2, y1, x2 + width//2, y2)
        cv2.line(frame, (x1+width//2, y1), (x2+width//2, y2), (0, 0, 255), 3)
    return left_most_line

def get_right_most_line(lines, width, frame):
    right_most_line = None
    # 左半分で一番右の直線を検出（右上から左下）
    if lines is not None:
        max_x1 = 0  # 小さな値で初期化
        for line in lines:
            x1, y1, x2, y2 = line[0]
            # if x1 > max_x1 and x1 > x2:
            if x1 > max_x1 and x1 > x2:
                max_x1 = x1
                right_most_line = (x1, y1, x2, y2)
            elif x2 > max_x1 and x2 > x1:
                max_x1 = x2
                right_most_line = (x1, y1, x2, y2)
        cv2.line(frame, (x1, y1), (x2, y2), (0, 255, 255), 3)
    return right_most_line
