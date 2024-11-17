import cv2
import numpy as np
import threading
import datetime

def detect_blue_object(frame):
    # HSV色空間に変換
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    # 青色の範囲を定義
    lower_blue = np.array([100, 150, 0])
    upper_blue = np.array([140, 255, 255])
    # 青色のマスクを作成
    mask = cv2.inRange(hsv, lower_blue, upper_blue)
    # 輪郭を検出
    contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    if contours:
        largest_contour = max(contours, key=cv2.contourArea)
        x, y, w, h = cv2.boundingRect(largest_contour)
        return x ,y ,w, h
    return None

def detect_all_blue_object(frame):
    # HSV色空間に変換
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    # 青色の範囲を定義
    lower_blue = np.array([100, 150, 0])
    upper_blue = np.array([140, 255, 255])
    # 青色のマスクを作成
    mask = cv2.inRange(hsv, lower_blue, upper_blue)
    # 輪郭を検出
    contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    return contours

def detect_yellow_object(frame):
    # 画像を読み込む
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # 黄色の範囲を定義
    lower_yellow = np.array([20, 100, 100])
    upper_yellow = np.array([30, 255, 255])

    # 黄色のマスクを作成
    mask = cv2.inRange(hsv, lower_yellow, upper_yellow)

    # 一番大きい輪郭を検出
    contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    if contours:
        largest_contour = max(contours, key=cv2.contourArea)
        x, y, w, h = cv2.boundingRect(largest_contour)
        return x ,y ,w, h
    return None

def detect_all_blue_object(frame):
    # HSV色空間に変換
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    # 黄色の範囲を定義
    lower_yellow = np.array([20, 100, 100])
    upper_yellow = np.array([30, 255, 255])
    # 黄色のマスクを作成
    mask = cv2.inRange(hsv, lower_yellow, upper_yellow)
    # 輪郭を検出
    contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    return contours




def detect_red_object(frame):
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    lower_red = np.array([0, 120, 70])
    upper_red = np.array([10, 255, 255])
    mask1 = cv2.inRange(hsv, lower_red, upper_red)

    lower_red = np.array([170, 120, 70])
    upper_red = np.array([180, 255, 255])
    mask2 = cv2.inRange(hsv, lower_red, upper_red)

    mask = mask1 + mask2
    contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    if contours:
        largest_contour = max(contours, key=cv2.contourArea)
        x, y, w, h = cv2.boundingRect(largest_contour)
        return x ,y ,w, h
    return None

def detect_all_red_object(frame):
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    lower_red = np.array([0, 120, 70])
    upper_red = np.array([10, 255, 255])
    mask1 = cv2.inRange(hsv, lower_red, upper_red)

    lower_red = np.array([170, 120, 70])
    upper_red = np.array([180, 255, 255])
    mask2 = cv2.inRange(hsv, lower_red, upper_red)

    mask = mask1 + mask2
    contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    return contours

def detect_lines(frame):
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    _, binary = cv2.threshold(gray, 50, 255, cv2.THRESH_BINARY_INV)
    edges = cv2.Canny(binary, 50, 150, apertureSize=3)
    lines = cv2.HoughLines(edges, 1, np.pi / 180, 100)
    return lines

def detect_circles(frame):
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    blurred = cv2.medianBlur(gray, 5)
    circles = cv2.HoughCircles(blurred, cv2.HOUGH_GRADIENT, dp=1.2, minDist=30, param1=50, param2=30, minRadius=15, maxRadius=100)
    return circles

def calculate_angle(line):
    rho, theta = line[0]
    angle = np.degrees(theta)
    return angle

def draw_lines(frame, lines):
    if lines is not None:
        leftmost_line = min(lines, key=lambda line: line[0][0])
        rightmost_line = max(lines, key=lambda line: line[0][0])
        
        for line in lines:
            rho, theta = line[0]
            a = np.cos(theta)
            b = np.sin(theta)
            x0 = a * rho
            y0 = b * rho
            x1 = int(x0 + 1000 * (-b))
            y1 = int(y0 + 1000 * (a))
            x2 = int(x0 - 1000 * (-b))
            y2 = int(y0 - 1000 * (a))
            
            if line is leftmost_line:
                color = (0, 255, 255)  # Yellow
            elif line is rightmost_line:
                color = (255, 0, 0)  # Blue
            else:
                color = (0, 0, 255)  # Red
            
            cv2.line(frame, (x1, y1), (x2, y2), color, 2)
            angle = calculate_angle(line)
            cv2.putText(frame, f"{angle:.2f} deg", (x1, y1), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1, cv2.LINE_AA)

def draw_circles(frame, circles):
    if circles is not None:
        circles = np.uint16(np.around(circles))
        for i in circles[0, :]:
            center = (i[0], i[1])
            radius = i[2]
            cv2.circle(frame, center, radius, (0, 255, 0), 2)
            cv2.circle(frame, center, 2, (0, 0, 255), 3)

def detect_green_white_boundary(frame):
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    # lower_green = np.array([35, 100, 100])
    # upper_green = np.array([85, 255, 255])
    lower_green = np.array([40, 40, 40])
    upper_green = np.array([80, 255, 255])

    mask_green = cv2.inRange(hsv, lower_green, upper_green)
    edges = cv2.Canny(mask_green, 50, 150)
    lines = cv2.HoughLines(edges, 1, np.pi / 180, 100)
    return lines

def get_bottom_most_line(lines, frame):
    # 一番下側の横のラインを保存する変数を初期化
    bottom_y = 0
    bottom_line = None

    if lines is not None:
        for rho, theta in lines[:, 0]:
            # 線の角度が垂直に近いかどうかを確認
            #if -0.3 < theta < 0.3 or np.pi - 0.3 < theta < np.pi + 0.3:  # 垂直に近い線の許容範囲を広げる
            # if -0.1 < theta < 0.1 or np.pi - 0.1 < theta < np.pi + 0.1:  # 垂直に近い線の許容範囲を設定
                
            if np.pi / 2 - 0.3 < theta < np.pi / 2 + 0.3:  # 水平に近い線の許容範囲を広げるために閾値を調整
            # if np.pi / 2 - 0.1 < theta < np.pi / 2 + 0.1:  # 水平に近い線の許容範囲を設定
                # rhoとthetaを使って線の端点を計算
                a = np.cos(theta)
                b = np.sin(theta)
                x0 = a * rho
                y0 = b * rho
                x1 = int(x0 + 1000 * (-b))
                y1 = int(y0 + 1000 * (a))
                x2 = int(x0 - 1000 * (-b))
                y2 = int(y0 - 1000 * (a))
                
                # この線が下側にある場合、下端の線を更新
                if y0 > bottom_y:
                    bottom_y = y0
                    bottom_line = (x1, y1, x2, y2)
                
    return bottom_line

def get_right_most_line(lines, frame ):
    # 一番右側の縦のラインを保存する変数を初期化
    rightmost_x = -1
    rightmost_line = None

    if lines is not None:
        for rho, theta in lines[:, 0]:
            # 線の角度が垂直に近いかどうかを確認
            if -0.3 < theta < 0.3 or np.pi - 0.3 < theta < np.pi + 0.3:  # 垂直に近い線の許容範囲を広げる
            # if -0.1 < theta < 0.1 or np.pi - 0.1 < theta < np.pi + 0.1:  # 垂直に近い線の許容範囲を設定
                
            # if np.pi / 2 - 0.3 < theta < np.pi / 2 + 0.3:  # 水平に近い線の許容範囲を広げるために閾値を調整
            # if np.pi / 2 - 0.1 < theta < np.pi / 2 + 0.1:  # 水平に近い線の許容範囲を設定
                # rhoとthetaを使って線の端点を計算
                a = np.cos(theta)
                b = np.sin(theta)
                x0 = a * rho
                y0 = b * rho
                x1 = int(x0 + 1000 * (-b))
                y1 = int(y0 + 1000 * (a))
                x2 = int(x0 - 1000 * (-b))
                y2 = int(y0 - 1000 * (a))
                
                # この線が右側にある場合、右端の線を更新
                if x0 > rightmost_x:
                    rightmost_x = x0
                    rightmost_line = (x1, y1, x2, y2)
                
    return rightmost_line

def get_and_draw_cross_line_right_bottom(lines, frame):
    # 一番下のラインを保存する変数を初期化
    lowest_y = float('inf')
    lowest_line = None

    # 一番右側の縦のラインを保存する変数を初期化
    rightmost_x = -1
    rightmost_line = None
    
    # 検出された線を繰り返し処理
    if lines is not None:
        for rho, theta in lines[:, 0]:
            # 線の角度が水平に近いかどうかを確認
            if np.pi / 2 - 0.3 < theta < np.pi / 2 + 0.3:  # 垂直に近い線の許容範囲を広げる
                # rhoとthetaを使って線の端点を計算
                a = np.cos(theta)
                b = np.sin(theta)
                x0 = a * rho
                y0 = b * rho
                x1 = int(x0 + 1000 * (-b))
                y1 = int(y0 + 1000 * (a))
                x2 = int(x0 - 1000 * (-b))
                y2 = int(y0 - 1000 * (a))
                
                # この線が下側にある場合、最下端の線を更新
                if y0 > lowest_y:
                    lowest_y = y0
                    lowest_line = (x1, y1, x2, y2)
            
            # 線の角度が垂直に近いかどうかを確認
            if -0.3 < theta < 0.3 or np.pi - 0.3 < theta < np.pi + 0.3:  # 水平に近い線の許容範囲を広げる
                # rhoとthetaを使って線の端点を計算
                a = np.cos(theta)
                b = np.sin(theta)
                x0 = a * rho
                y0 = b * rho
                x1 = int(x0 + 1000 * (-b))
                y1 = int(y0 + 1000 * (a))
                x2 = int(x0 - 1000 * (-b))
                y2 = int(y0 - 1000 * (a))
                
                # この線が右側にある場合、右端の線を更新
                if x0 > rightmost_x:
                    rightmost_x = x0
                    rightmost_line = (x1, y1, x2, y2)

    # 元の画像に一番下のラインと一番右側の縦のラインを描画
    if lowest_line is not None:
        x1, y1, x2, y2 = lowest_line
        cv2.line(frame, (x1, y1), (x2, y2), (0, 0, 255), 3)  # 赤色で描画

    if rightmost_line is not None:
        x1, y1, x2, y2 = rightmost_line
        cv2.line(frame, (x1, y1), (x2, y2), (255, 0, 0), 3)  # 青色で描画

    # 一番下のラインと一番右側の縦のラインの交点を計算して黄色で描画
    if lowest_line is not None and rightmost_line is not None:
        # 一番下のラインの方程式: y = m1*x + c1
        m1 = (lowest_line[3] - lowest_line[1]) / (lowest_line[2] - lowest_line[0])
        c1 = lowest_line[1] - m1 * lowest_line[0]
        
        # 一番右側の縦のラインの方程式: x = c2 (垂直なので傾きは無限大)
        c2 = rightmost_line[0]
        
        # 一番下のラインと一番右側の縦のラインの交点: (c2, m1*c2 + c1)
        intersection_x = int(c2)
        intersection_y = int(m1 * c2 + c1)
        
        # 黄色で交点を描画
        cv2.circle(frame, (intersection_x, intersection_y), 5, (0, 255, 255), -1)  # 黄色で描画

def find_intersection(line1, line2):
    x1, y1, x2, y2 = line1
    x3, y3, x4, y4 = line2
    
    # 行列式を計算
    det = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)
    
    if det == 0:
        return None  # ラインが平行または一致している場合
    
    # 交点を計算
    det_x = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)
    det_y = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)
    
    x = det_x // det
    y = det_y // det
    
    return x, y

# 使用例
# line1 = (1, 2, 3, 4)
# line2 = (5, 6, 7, 8)

# intersection = find_intersection(line1, line2)
# if intersection:
#     print(f"交点は: {intersection}")
# else:
#     print("ラインは平行または一致しており、交点がありません。")

                

def draw_line(line, frame):
    if line is not None:
        x1, y1, x2, y2 = line
        cv2.line(frame, (x1, y1), (x2, y2), (0, 0, 255), 3)  # 赤色で描画
    

def get_point(line, y_position):
    # Y=10の位置に赤い丸を描画
    x1, y1, x2, y2 = line
    x_position = int((y_position - y1) * (x2 - x1) / (y2 - y1) + x1)
    return x_position

def draw_dot(x,y,frame):
    cv2.circle(frame, (x, y), 5, (0, 0, 255), -1)  # 赤い丸を描画


def draw_green_white_boundary(lines, frame):
    if lines is not None:
        for line in lines:
            rho, theta = line[0]
            a = np.cos(theta)
            b = np.sin(theta)
            x0 = a * rho
            y0 = b * rho
            x1 = int(x0 + 1000 * (-b))
            y1 = int(y0 + 1000 * (a))
            x2 = int(x0 - 1000 * (-b))
            y2 = int(y0 - 1000 * (a))
    cv2.line(frame, (x1, y1), (x2, y2), (255, 0, 255), 2)  # Pink

def get_frame_cropped(frame):
        # 画像の高さと幅を取得
    height, width = frame.shape[:2]

    # 中央部分を1/4にトリミング
    start_x = width // 4
    start_y = height // 4
    end_x = start_x + width // 2
    end_y = start_y + height // 2

    # フレームをトリミング
    cropped_frame = frame[start_y:end_y, start_x:end_x]
    return cropped_frame

def isHorizontal(rectangle):
    x, y, w, h = rectangle
    # ひらべったい場合はサークル
    if w > h*1.2:
        return True
    else:
        return False

def isVertical(rectangle):
    x, y, w, h = rectangle
    # 縦長の場合はボトル
    if w <= h*1.2:
        return True
    else:
        return False

def get_distance(x,y):
    if y <= 60:
        return 60
    elif y <= 184:
        return 50
    elif y <= 122:
        return 30
    elif y <= 273:
        return 20
    elif y <= 425:
        return 10
    else:
        return 0
    
def draw_scale(frame):
    # 走行体の外幅
    cv2.line(frame, (288,60), (411,60), (0, 255, 0), 2) # 50cm
    cv2.line(frame, (258,122), (427,122), (0, 255, 0), 2) # 30cm
    cv2.line(frame, (235,184), (441,184), (0, 255, 0), 2) # 20cm
    cv2.line(frame, (192,273), (469,273), (0, 255, 0), 2) # 10cm
    cv2.line(frame, (124,425), (512,425), (0, 255, 0), 2) # 0cm

    cv2.line(frame, (124,425), (288,60), (0, 255, 0), 2) # 左
    cv2.line(frame, (512,425), (411,60), (0, 255, 0), 2) # 右

    # 走行体のアーム幅
    cv2.line(frame, (236,421), (331,61), (255,0,0), 2) # 左
    cv2.line(frame, (396,421), (373,63), (255,0,0), 2) # 右

def draw_center_of_gravity(contour, frame):
    # 重心を計算
    M = cv2.moments(contour)
    if M['m00'] != 0:
        cx = int(M['m10'] / M['m00'])
        cy = int(M['m01'] / M['m00'])
        cv2.circle(frame, (cx, cy), 5, (0, 0, 255), -1)
        
def find_x_at_y0(line):
    x1, y1, x2, y2 =line
    # 直線の傾き (m) を計算
    m = (y2 - y1) / (x2 - x1)
    
    # 直線のy切片 (b) を計算
    b = y1 - m * x1
    
    # y = 0 のときの x 座標を計算
    x_at_y0 = -b / m
    
    return x_at_y0

def erase_green_left(frame):
    height, width = frame.shape[:2]

    # 緑の斜め線を検出するためにHSV色空間に変換
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    lower_green = np.array([35, 100, 100])
    upper_green = np.array([85, 255, 255])
    mask = cv2.inRange(hsv, lower_green, upper_green)

    # 緑の斜め線の輪郭を検出
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    if len(contours) == 0:
        print("緑の斜め線が見つかりませんでした。")
        return frame

    # 最も大きな輪郭を選択
    largest_contour = max(contours, key=cv2.contourArea)

    # 緑の斜め線の左側を白にするためのマスクを作成
    mask = np.zeros_like(frame)
    cv2.drawContours(mask, [largest_contour], -1, (255, 255, 255), thickness=cv2.FILLED)

    # 緑の斜め線より左側を白にする
    for y in range(height):
        for x in range(width):
            if mask[y, x][0] == 255:
                frame[y, :x] = [255, 255, 255]
                break
    return frame    