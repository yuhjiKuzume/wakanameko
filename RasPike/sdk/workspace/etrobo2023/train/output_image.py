import cv2
import os

def output_images(image_list,output_path):
     
    # 出力ディレクトリが存在しない場合は作成
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    frame_number = 0
    for image in image_list:
        output_filename = os.path.join(output_path, f"{frame_number:02}.jpg")
        cv2.imwrite(output_filename, image)
        frame_number += 1
