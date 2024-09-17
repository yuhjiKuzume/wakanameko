import cv2
import numpy as np

"""
-------画像のモーメントを計算--------
"""
def calculate_moment(image):

    image_gray = cv2.bitwise_not(image)

    moment_x = []
    moment_y = []

    # 画像表示用に入力画像をカラーデータに変換する
    img_disp = cv2.cvtColor(image_gray, cv2.COLOR_GRAY2BGR)

    # モーメントの計算
    m = cv2.moments(image_gray, False)
    # 重心の計算
    x,y= m['m10']/m['m00'] , m['m01']/m['m00']
    #print(f"Weight Center = ({x}, {y})")

    moment_x.append(x)
    moment_y.append(y)

    # 座標を四捨五入
    x, y = round(x), round(y)

    # 重心位置に x印を書く
    cv2.line(img_disp, (x-5,y-5), (x+5,y+5), (0, 0, 255), 2)
    cv2.line(img_disp, (x+5,y-5), (x-5,y+5), (0, 0, 255), 2)

    return x,y,img_disp

"""
-------2画像の差分を取ってからモーメントを計算--------
"""
def image_moment(target_image,base_image):

    moment_x = []
    moment_y = []
    moment_image_list = []

    for img in target_image:
        img_diff = cv2.absdiff(img,base_image)

        # 画像表示用に入力画像をカラーデータに変換する
        img_disp = cv2.cvtColor(img_diff, cv2.COLOR_GRAY2BGR)

        # モーメントの計算
        m = cv2.moments(img_diff, False)
        # 重心の計算
        x,y= m['m10']/m['m00'] , m['m01']/m['m00']
        # print(f"Weight Center = ({x}, {y})")

        moment_x.append(x)
        moment_y.append(y)

        # 座標を四捨五入
        x, y = round(x), round(y)

        # 重心位置に x印を書く
        cv2.line(img_disp, (x-5,y-5), (x+5,y+5), (0, 0, 255), 2)
        cv2.line(img_disp, (x+5,y-5), (x-5,y+5), (0, 0, 255), 2)

        moment_image_list.append(img_disp)

    return moment_x,moment_y,moment_image_list

"""
-------2点間の距離を計算--------
"""
def calculate_euclidean_distance(point1_x,point1_y,point2_x,point2_y):
    point1 = np.array([point1_x,point1_y])
    point2 = np.array([point2_x,point2_y])

    distance = np.linalg.norm(point1-point2)
    # print(distance)

    return distance