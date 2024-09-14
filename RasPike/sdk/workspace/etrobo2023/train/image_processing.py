import cv2
import numpy as np

def binarize_image(image_list):
    """指定されたリストの画像を二値化
        Args:
            image_list: 処理対象ファイルリスト
    """
    binarided_image_list = []           # 処理結果画像のリスト

    for image in image_list:
        # 画像をグレースケールで読み込み
        image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        
        # 二値化処理 (閾値は127で固定)
        _, binarized_image = cv2.threshold(image, 127, 255, cv2.THRESH_BINARY)

        # 白黒反転
        # binarized_image = cv2.bitwise_not(binarized_image)

        binarided_image_list.append(binarized_image)

    return binarided_image_list

def extract_blue_parts(image_list):
    """画像の青のみを抽出
        Args:
            image_list: 処理対象のファイルリスト
    """
    extract_blue_image_list = []        # 処理結果画像のリスト

    # 青色の範囲を指定 (HSV色空間での青色の範囲)
    lower_blue = np.array([100, 50, 50])
    upper_blue = np.array([140, 255, 255])

    for image in image_list:
        
        # 画像をHSV色空間に変換
        hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        
        # 青色部分をマスクとして抽出
        blue_mask = cv2.inRange(hsv_image, lower_blue, upper_blue)
        
        # マスクを使って青色部分を抽出
        blue_parts = cv2.bitwise_and(image, image, mask=blue_mask)
        
        # 青色部分の画像を保存
        extract_blue_image_list.append(blue_parts)

    return extract_blue_image_list

def calculate_between_image_at_interval(image_list,skip_frame,operation,inverse):
    """2値化画像のリストに対して、任意の画像間隔で画像間演算する
        Args:
            image_list: 処理対象のファイル
            skip_frame: 演算する画像のフレーム間隔
            operation: 演算子（0:and 1:or）
            inverse: 白黒を反転して演算するか
    """
    skip_frame += 1
    image_file_count = len(image_list)
    result_image_list = []
    print(f"image_file_count:{image_file_count}")

    for index in range(image_file_count):
        if index + skip_frame < image_file_count:
            result_image_list.append(calculate_between_image(image_list[index],image_list[index + skip_frame],operation,inverse))
        elif index + skip_frame >= image_file_count:
            result_image_list.append(calculate_between_image(image_list[index],image_list[index + skip_frame - image_file_count],operation,inverse))
    
    return result_image_list

def calculate_between_image(image1, image2, operation=0, inverse=False):
    """2つの二値化画像を、画像間演算する
        Args:
            image1, image2: 演算対象のファイル
            operation: 演算子（0:and 1:or）
            inverse: 白黒を反転して演算するか
    """
    # 画像サイズを確認
    if image1.shape != image2.shape:
        raise ValueError("画像のサイズが一致しません。")

    if inverse :
        # 白黒を反転
        image1 = cv2.bitwise_not(image1)
        image2 = cv2.bitwise_not(image2)

    if operation == 0:
        common_pixels_image = cv2.bitwise_and(image1, image2)
    elif operation == 1:
        common_pixels_image = cv2.bitwise_or(image1, image2)

    if inverse :    
        # 白黒を戻す
        common_pixels_image = cv2.bitwise_not(common_pixels_image)
    
    return common_pixels_image

def calculate_image(image_list,operation=0):
    """全ての画像を演算する
        Args:
            image_list: 演算対象のファイル
            operation: 演算子（0:and 1:or）
    """
    accumulated_image = image_list[0]
    counter = 0
    for image in image_list[1:]:
        counter += 1
        if image.shape != accumulated_image.shape:
            raise ValueError("画像のサイズが一致しません。")

        # 画像を加算
        if operation == 0:
            accumulated_image = cv2.bitwise_and(accumulated_image, image)
        elif operation == 1:
            cv2.bitwise_not(image)
            accumulated_image = cv2.bitwise_or(accumulated_image, image)
            cv2.bitwise_not(accumulated_image)
    return accumulated_image

def binarize_blue(image_list):
    """画像の青は黒、それ以外は白になるよう二値化
        Args:
            image_list: 処理対象のファイルリスト
    """
    result_list=[]
    for image in image_list:
        # BGRからHSVに変換
        hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
        
        # 青色の範囲を定義 (HSVでの範囲)
        lower_blue = np.array([100, 150, 0])
        upper_blue = np.array([140, 255, 255])
        
        # 青色のマスクを作成
        mask = cv2.inRange(hsv_image, lower_blue, upper_blue)
        
        # マスクを適用して、青色を白 (255)、それ以外を黒 (0) に二値化
        binary_image = cv2.threshold(mask, 127, 255, cv2.THRESH_BINARY)[1]
        binary_image = cv2.bitwise_not(binary_image)

        result_list.append(binary_image)
    
    return result_list

def extract_common_pixels(image1, image2):
    """共通画素を抽出
        Args:
            image1, image2: 演算対象のファイル
    """

    image1 = cv2.bitwise_not(image1)
    image2 = cv2.bitwise_not(image2)

    # 論理積をとる (AND演算)
    common_pixels = cv2.bitwise_and(image1, image2)
    common_pixels = cv2.bitwise_not(common_pixels)

    return common_pixels

def unsharp_mask(image_list):
    """アンシャープマスクを適用
        Args:
            image_list: 処理対象のファイルリスト
    """

    unsharp_mask_image = []

    for image in image_list:
        blurred = cv2.GaussianBlur(image, (5, 5), 0)
        unsharp_image = cv2.addWeighted(image, 1.5, blurred, -0.5, 0)
        unsharp_mask_image.append(unsharp_image)
    
    return unsharp_mask_image
