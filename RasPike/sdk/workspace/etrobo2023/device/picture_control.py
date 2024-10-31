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
    lower_green = np.array([35, 100, 100])
    upper_green = np.array([85, 255, 255])
    mask_green = cv2.inRange(hsv, lower_green, upper_green)
    edges = cv2.Canny(mask_green, 50, 150)
    lines = cv2.HoughLines(edges, 1, np.pi / 180, 100)
    return lines

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
    if w > h*1.1:
        return True
    else:
        return False

def isVertical(rectangle):
    x, y, w, h = rectangle
    # 縦長の場合はボトル
    if w <= h*1.1:
        return True
    else:
        return False

def get_distance(x,y):
    if y <= 60:
        return "60"
    elif y <= 184:
        return "50"
    elif y <= 122:
        return "30"
    elif y <= 273:
        return "20"
    elif y <= 425:
        return "10"
    else:
        return "0"
    
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