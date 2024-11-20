import cv2
import numpy as np
import threading
import datetime
import time
from multiprocessing import Value, Array, Process
import math

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
def test(camera_handle):
    while True:
        frame = read(camera_handle)
        crop =get_frame_cropped_for_green_line_trace(frame)
        lines = ctl_pic.detect_green_white_boundary(frame)
        line_v = ctl_pic.get_right_most_line(lines,frame)
        ctl_pic.draw_lines(frame,lines)
        ctl_pic.draw_line(line_v,frame, (255,0,0))
        ret , key = show_camera_and_get_key('crop', crop)
        ret , key = show_camera_and_get_key('frame', frame)

def get_frame_cropped_for_green_line_trace(frame):
        # 画像の高さと幅を取得
    height, width = frame.shape[:2]

    # 中央部分を1/4にトリミング
    start_x = width // 4
    start_y = height // 3
    end_x = width *3 // 4
    end_y = height *2// 3

    # フレームをトリミング
    cropped_frame = frame[start_y:end_y, start_x:end_x]
    return cropped_frame

def start(camera_handle, is_left_course):
    send("BEEP_ON()")
    # green_line_trace(camera_handle, 10)

    # frame = read(camera_handle)
    # show_camera_and_get_key('frame', frame)

    # while True:
    #     #test(camera_handle)
    #     green_line_trace(camera_handle, -10, False)


    # ★の所にあるのが、デブリブロックなら移動する
    # if isDebrisBlock(frame) is True:
    #     print("isDebrisBlock is True")
    #     send_wait("FW(10,60,60)")        # 前進10cm
    #     send_wait("BW(10,60,60)")        # バック10cm
    DEBUG = True

    # 2024/11/20　CSで変更した動作   
    if DEBUG is True:
        if is_left_course is True:
            send_wait("CW(40)")        # 右45度
            send_wait("FW(20,60,60)")  # 前進20cm
            send_wait("CCW(40)")       # 左45度

            send_wait("FW(90,60,60)")  # 前進90cm
            send("BEEP_ON()")
            
            send_wait("CW(75)")        # 右90度
            blue_object_trace(camera_handle, 10)
        else:
            send_wait("CCW(40)")        # 左45度
            send_wait("FW(20,60,60)")  # 前進20cm
            send_wait("CW(40)")       # 右45度

            send_wait("FW(90,60,60)")  # 前進90cm
            send("BEEP_ON()")
            
            send_wait("CCW(75)")        # 左90度
            blue_object_trace(camera_handle, 10)

    # 2024/11/19　社内で確認していた動作   
    else:
        if is_left_course is True:
            send_wait("CCW(40)")        # 右45度
            send_wait("FW(20,60,60)")  # 前進20cm
            send_wait("CW(40)")       # 左45度

            green_line_trace(camera_handle, 10)
            send("BEEP_ON()")
            
            send_wait("CW(75)")        # 右90度
            blue_object_trace(camera_handle, 10)
    # 2024/11/19　社内で確認していた動作   
        else:
            send_wait("CW(40)")        # 左45度
            send_wait("FW(20,60,60)")  # 前進20cm
            send_wait("CCW(40)")       # 右45度

            green_line_trace(camera_handle, -10, False)
            send("BEEP_ON()")
            
            send_wait("CCW(75)")        # 左90度
            blue_object_trace(camera_handle, 10)
    # 2024/11/19　社内で確認していた動作   
    

    
def green_line_trace(camera_handle, correct = 0, is_left_course=True):
    # 共有メモリを使ってモータコントローラスレッドにデータを送る
    left_motor = Value('i', 0)
    right_motor = Value('i', 0)
    is_exit = Value('i', 0)
    sleep_time = 0.01

    # 目標となる交点や直線が見つからない時はひとつ前の値を使う
    intersection_y = 0
    intersection_x = 320
    old_intersection_x = intersection_x
    old_intersection_y = intersection_y
    
    # モーターコントローラを裏で起動する
    thread = threading.Thread(target=motor_control_thread, args=(left_motor,right_motor,sleep_time,is_exit))
    thread.start()

    base_speed = 60

    start_time = time.time()
    while True:
        frame = read(camera_handle)

        # フレームを上3/4にする
        frame =  ctl_pic.get_crop_frame_3_4(frame)
        
        # 目標値の初期値は、画面中央真ん中
        intersection_y = old_intersection_y
        intersection_x = old_intersection_x

        # 緑の線を検出
        lines = ctl_pic.detect_green_white_boundary(frame)

        # レフトコースの場合
        if is_left_course is True:
            line_v = ctl_pic.get_right_most_line(lines,frame)
        # ライトコースの場合
        else:
            line_v = ctl_pic.get_left_most_line(lines,frame)

        _, left_line = detect_lines_in_angle_range(frame,0,30)


        line_h = ctl_pic.get_bottom_most_line(lines,frame)

        # 緑の横線を検出
        if line_h is not None:
            # 縦の線を検出
            if line_v is not None:
                #交点を検出
                intersection_x, intersection_y = ctl_pic.find_intersection(line_v, line_h)
                print(f"{intersection_x},{intersection_y}")
            # 縦の線がない
            else:
                intersection_y = ctl_pic.find_y_at_x(line_h, intersection_x)
                print(f"{intersection_x},{intersection_y}-")
                
        # 緑の横線がない
        else:
            # 縦線はある
            if line_v is not None:
                intersection_x = ctl_pic.find_x_at_y(line_v, intersection_y)
            # 縦線も横線もない
            else:
                pass
            # 直線と画面上端の交点を求める

        ctl_pic.draw_lines(frame,lines)

        if line_h is not None:
            ctl_pic.draw_line(line_h,frame,(0,255,0))
        if line_v is not None:
            ctl_pic.draw_line(line_v,frame,(0,255,0))


        # 黄色で交点を描画
        cv2.circle(frame, (int(intersection_x), int(intersection_y)), 5, (0, 255, 255), -1)  # 黄色で描画
        
        correct_intersection_x = intersection_x+correct
        # 黄色で交点を描画
        cv2.circle(frame, (int(correct_intersection_x), int(intersection_y)), 5, (255, 0, 0), -1)  # 黄色で描画
        
        target_line = 320, 0, 320, 480
        ctl_pic.draw_line(target_line,frame)

        # モーター制御
        power = (320 - correct_intersection_x)//20
        # print(power,intersection_x,intersection_y)

        left_motor.value = base_speed + int(power)
        right_motor.value = base_speed - int(power)

        if intersection_y > 130:  #50のとき145:
            is_exit.value = -1 # モータ制御を止める
            cv2.imwrite("green_line_trace_"+str(intersection_y)+".jpg",frame)

            send("MP(0,0)")
            print(f"intersection_y ={intersection_y}")
            break
                        

        end_time = time.time()
        diff_time = end_time - start_time
        print(diff_time)
        if (diff_time) > 4:
            send("MP(0,0)")
            is_exit.value = -1
            print("time up")
            break

        ret , key = show_camera_and_get_key('frame', frame)
        if ret is False:
            break

        old_intersection_x = intersection_x
        old_intersection_y = intersection_y
        time.sleep(0.1)
    is_exit.value = -1 # モータ制御を止める
    send("MP(0,0)")
    print("done")

def is_point_right_of_line(point, line):
    """
    点が線よりも右側にあるかどうかを判定する関数
    :param point: (x, y) 点の座標
    :param line: (x1, y1, x2, y2) 線の始点の座標、 線の終点の座標
    :return: True if point is left of the line, False otherwise
    """
    px, py = point
    x1, y1, x2, y2 = line

    # print(f"point = {point}")
    # print(f"line_start = ({x1}, {y1})")
    # print(f"line_end = ({x2}, {y2})")
    
    # 線の方程式を使って点の位置を判定
    position = (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1)
    
    return position < 0

def is_point_left_of_line(point, line):
    """
    点が線よりも左側にあるかどうかを判定する関数
    :param point: (x, y) 点の座標
    :param line: (x1, y1, x2, y2) 線の始点の座標、 線の終点の座標
    :return: True if point is right of the line, False otherwise
    """
    px, py = point
    x1, y1, x2, y2 = line
    
    # 線の方程式を使って点の位置を判定
    position = (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1)
    
    return position < 0

def calculate_angle(x1, y1, x2, y2):
    """
    2点 (x1, y1) と (x2, y2) の間の角度を計算する関数
    :param x1: 点1のx座標
    :param y1: 点1のy座標
    :param x2: 点2のx座標
    :param y2: 点2のy座標
    :return: 角度（度単位）
    """
    delta_x = x2 - x1
    delta_y = y2 - y1
    angle_rad = math.atan2(delta_y, delta_x)
    angle_deg = math.degrees(angle_rad)
    return angle_deg

def detect_lines_in_angle_range(frame, min_angle=30, max_angle=60):
    # グレースケールに変換
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # エッジ検出
    edges = cv2.Canny(gray, 50, 150, apertureSize=3)
    # Hough変換を使用して線を検出
    lines = cv2.HoughLines(edges, 1, np.pi / 180, 100)
    
    line_angle_min = max_angle
    line_angle_max = min_angle
    
    line_min = None
    line_max = None
    if lines is not None:
        for rho, theta in lines[:, 0]:
            # 角度を度に変換
            angle = math.degrees(theta)
            # 角度が指定範囲内にあるか確認
            if min_angle <= angle <= max_angle:
                # 線を描画
                a = np.cos(theta)
                b = np.sin(theta)
                x0 = a * rho
                y0 = b * rho
                x1 = int(x0 + 1000 * (-b))
                y1 = int(y0 + 1000 * (a))
                x2 = int(x0 - 1000 * (-b))
                y2 = int(y0 - 1000 * (a))

                # 最小角度更新
                if line_angle_min > angle:
                    angle = line_angle_min
                    line_min = x1, y1, x2, y2
                    
                # 最大角度更新
                if line_angle_max < angle:
                    angle = line_angle_max
                    line_max = x1, y1, x2, y2
                
    return line_max, line_min 

def convert_right_frame_full_frame_position(right_line, full_frame):
        height, width = full_frame.shape[:2]
        x1,y1,x2,y2 = right_line
        right_line = x1+width//2,y1,x2+width//2,y2
        return right_line

def is_out_of_lines(left_line, right_line, position):
    outside_of_left = False
    outside_of_right = False

    # 左ラインが見つかった
    if left_line is not None:
        # ラインより外側は無効
        outside_of_left = is_point_left_of_line(position,left_line)

    # 右ラインが見つかった
    if right_line is not None:
        # ラインより外側は無効
        outside_of_right =  is_point_right_of_line(position,right_line)

    return outside_of_left or outside_of_right
                    
def blue_object_trace_test(camera_handle, correct = 0):
    while True:
        frame = read(camera_handle)
        # フレームを上3/4にする
        frame =  ctl_pic.get_crop_frame_3_4(frame)
        height, width = frame.shape[:2]

        # フレームを左右半分に分割
        left_half = frame[:, :width//2]
        right_half = frame[:, width//2:]
        
        _, left_line = detect_lines_in_angle_range( left_half,30,60)
        right_line ,_ = detect_lines_in_angle_range(right_half,120,150 )

        # 座標変換
        if right_line is not None:
            right_line = convert_right_frame_full_frame_position(right_line,frame)
            

        # 青を全部
        blue_contours = ctl_pic.detect_all_blue_object(frame)

        ctl_pic.draw_line(left_line,frame,(0,255,255))
        ctl_pic.draw_line(right_line,frame,(0,0,255))


        # 線より外の重心は除外する
        inside_contours = []
        for contour in blue_contours:
            # 輪郭の面積を計算する
            area = cv2.contourArea(contour)
        
            # 面積が一定以上の場合にのみ処理を行う
            if area > 100:  # 面積の閾値は適宜調整してください
                x, y, w, h = cv2.boundingRect(contour)
                ctl_pic.draw_rectangle((x, y, w, h),frame, (255,255,0)) # 全部
                cv2.circle(frame,(x+w//2,y+h//2), 5, (255, 255, 0), -1)  # 色で描画
                    
                if(is_out_of_lines(left_line,right_line,(x+w//2,y+h//2))):
                    pass
                else:
                    inside_contours.append(contour)
                
        if inside_contours:
            # 最も横幅が大きい青色の物体を格納する変数を初期化
            widest_blue_object = None
            max_width = -1
    
            # 輪郭を反復処理して最も横幅が大きい青色の物体を検出
            for contour in inside_contours:
                x, y, w, h = cv2.boundingRect(contour)
                if w > max_width:
                    max_width = w
                    widest_blue_object = (x, y, w, h)

            x, y, w, h = widest_blue_object
            # 黄色で重心を描画
            # ctl_pic.draw_rectangle(new_blue_obj,frame, (255,0,0)) # 青
            #cv2.circle(frame, (int(intersection_x), int(intersection_y)), 5, (0, 255, 255), -1)  # 黄色で描画
            cv2.circle(frame,(x+w//2,y+h//2), 5, (255, 0, 0), -1)  # 色で描画
            ctl_pic.draw_rectangle((x, y, w, h),frame, (255,0,0)) # 青

        ret , key = show_camera_and_get_key('frame', frame, 1)
            #time.sleep(2)
        

# def blue_object_trace_test(camera_handle, correct = 0):
#     while True:
#         frame = read(camera_handle)
#         # フレームを上3/4にする
#         frame =  ctl_pic.get_crop_frame_3_4(frame)
#         height, width = frame.shape[:2]

#         # フレームを左右半分に分割
#         left_half = frame[:, :width//2]
#         right_half = frame[:, width//2:]
        
#         _, left_line = detect_lines_in_angle_range( left_half,30,60)
#         right_line ,_ = detect_lines_in_angle_range(right_half,120,150 )

#         blue_obj = ctl_pic.detect_most_wide_blue_object(frame)
#         x,y,w,h = blue_obj
#         new_blue_obj = x,y,w,h

#         # 座標変換
#         if right_line is not None:
#             x1,y1,x2,y2 = right_line
#             right_line = x1+width//2,y1,x2+width//2,y2

#         ctl_pic.draw_line(left_line,frame,(0,0,255))
#         ctl_pic.draw_line(right_line,frame,(0,255,255))

#         intersection_x = 320
#         intersection_y = 0
        

#         # print(f"--")
#         if blue_obj is not None:
#             # ctl_pic.draw_rectangle(blue_obj,frame, (0,0,255))
#             x, y , w, h =blue_obj
#             # 左ラインが見つかった
#             if left_line is not None:
#                 # 左側　
#                 if is_point_left_of_line((x+w//2,y+h//2),left_line) is True:
#                     new_blue_obj = None
#                     # print(f"1")
#                 else:
#                     # print(f"2")
#                     pass
#             else:
#                 # print(f"3")
#                 pass

#             # 右ラインが見つかった
#             if right_line is not None:
#                 # ラインより外側は無効
#                 if is_point_right_of_line((x+ w//2,y+ h//2),right_line) is True:
#                     new_blue_obj = None
#                     # print(f"4")
#                 else:
#                     # print(f"5")
#                     pass
#             # 右ラインが見つからない
#             else:
#                 # print(f"6")
#                 pass
                

#         if new_blue_obj is not None:
#             intersection_x = x + w//2
#             intersection_y = y + h//2

#             # 黄色で重心を描画
#             # ctl_pic.draw_rectangle(new_blue_obj,frame, (255,0,0)) # 青
#             #cv2.circle(frame, (int(intersection_x), int(intersection_y)), 5, (0, 255, 255), -1)  # 黄色で描画
#             cv2.circle(frame,(x+w//2,y+h//2), 5, (255, 0, 0), -1)  # 色で描画
#             ctl_pic.draw_rectangle(new_blue_obj,frame, (255,0,0)) # 青

#         ret , key = show_camera_and_get_key('frame', frame, 1)
#         #time.sleep(2)
    
def except_out_of_line(frame, blue_contours):

    if blue_contours is None:
        blue_contours
    
    # フレームを左右半分に分割
    height, width = frame.shape[:2]
    left_half = frame[:, :width//2]
    right_half = frame[:, width//2:]
    
    # 左右のフレームからラインを抽出
    _, left_line = detect_lines_in_angle_range(left_half,30,60)
    right_line ,_ = detect_lines_in_angle_range(right_half,120,150 )
    # 座標変換
    if right_line is not None:
        right_line = convert_right_frame_full_frame_position(right_line,frame)
    
    # ctl_pic.draw_line(left_line,frame,(0,255,255))
    # ctl_pic.draw_line(right_line,frame,(0,0,255))

    # 線より外の重心は除外する
    inside_contours = []
    for contour in blue_contours:
        # 輪郭の面積を計算する
        area = cv2.contourArea(contour)
    
        # 面積が一定以上の場合にのみ処理を行う
        if area > 100:  # 面積の閾値は適宜調整してください
            x, y, w, h = cv2.boundingRect(contour)
            # ctl_pic.draw_rectangle((x, y, w, h),frame, (255,255,0)) # 全部
            # cv2.circle(frame,(x+w//2,y+h//2), 5, (255, 255, 0), -1)  # 色で描画
                
            if(is_out_of_lines(left_line,right_line,(x+w//2,y+h//2))):
                pass
            else:
                inside_contours.append(contour)
            
    widest_blue_object = None
    if inside_contours:
        # 最も横幅が大きい青色の物体を格納する変数を初期化
        max_width = -1

        # 輪郭を反復処理して最も横幅が大きい青色の物体を検出
        for contour in inside_contours:
            x, y, w, h = cv2.boundingRect(contour)
            if w > max_width:
                max_width = w
                widest_blue_object = (x, y, w, h)

        x, y, w, h = widest_blue_object
        # 黄色で重心を描画
        # ctl_pic.draw_rectangle(new_blue_obj,frame, (255,0,0)) # 青
        #cv2.circle(frame, (int(intersection_x), int(intersection_y)), 5, (0, 255, 255), -1)  # 黄色で描画
        # cv2.circle(frame,(x+w//2,y+h//2), 5, (255, 0, 0), -1)  # 色で描画
        # ctl_pic.draw_rectangle((x, y, w, h),frame, (255,0,0)) # 青
            
    return widest_blue_object

def blue_object_trace(camera_handle, correct = 0):
    # 共有メモリを使ってモータコントローラスレッドにデータを送る
    left_motor = Value('i', 0)
    right_motor = Value('i', 0)
    is_exit = Value('i', 0)
    sleep_time = 0.01

    # モーターコントローラを裏で起動する
    thread = threading.Thread(target=motor_control_thread, args=(left_motor,right_motor,sleep_time,is_exit))
    thread.start()

    base_speed = 60

    start_time = time.time()
    while True:
        frame = read(camera_handle)
        # フレームを上3/4にする
        frame =  ctl_pic.get_crop_frame_3_4(frame)
        intersection_x = 320
        intersection_y = 0

        blue_contours = ctl_pic.detect_all_blue_object(frame)
        blue_contour = except_out_of_line(frame,blue_contours)
        
        if blue_contour is not None:
            x, y , w, h =blue_contour
            intersection_x = x + w//2
            intersection_y = y + h//2
        
        # 黄色で重心を描画
        cv2.circle(frame, (int(intersection_x), int(intersection_y)), 5, (0, 255, 255), -1)  # 黄色で描画

        intersection_x += correct
        power = (320 - intersection_x)//20

        left_motor.value = base_speed + power
        right_motor.value = base_speed - power

        print(intersection_y)
        if intersection_y > 170: #60の時180:
            cv2.imwrite("blue_line_trace_"+str(intersection_y)+".jpg",frame)
            is_exit.value = -1 # モータ制御を止める
            send("MP(0,0)")
            # print(f"intersection_y = {intersection_y}")
            break           

        end_time = time.time()
        diff_time = end_time - start_time
        # print(diff_time)
        if (diff_time) > 4:
            send("MP(0,0)")
            is_exit.value = -1
            print("time up")
            break

        ret , key = show_camera_and_get_key('frame', frame)
        if ret is False:
            break
        time.sleep(0.1)

    is_exit.value = -1 # モータ制御を止める
    send("MP(0,0)")
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
    # base_power = 40
    # pwr = abs(distance//40) 
    # if(angle == "left"):
    #     send_wait("CCW("+str(pwr)+")")
    # elif (angle == "right"):
    #     send_wait("CW("+str(pwr)+")")
    # else:
    #     pass #send_wait("MP(0,0)")

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
        

    show_camera_and_get_key("left_half", left_half)
    show_camera_and_get_key("right_half", right_half)
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
