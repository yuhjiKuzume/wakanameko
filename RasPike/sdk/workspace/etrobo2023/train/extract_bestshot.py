import cv2
import os

from calculate_base_moment import *
from image_processing import *
from extract_frame import extract_frames   
from output_image import output_images

def extract_bestshot_image(video_path,output_root_folder,frame_interval=20,calculate_interval=4,debug_mode=False):
    """プラレールのベストショット画像を抽出する
        Args:
            video_path: 処理対象の動画ファイルへのパス
            output_root_folder: 処理の中間画像の出力先
            frame_interval: 動画からフレームを抽出する間隔
            calculate_interval: フレーム間演算時の間隔
            debug_mode: 中間ファイルを出力するかどうか
    """

    """
    -------前処理--------
    """
    # 動画からフレームに分割
    extract_image = extract_frames(video_path, frame_interval)
    # extract_image = unsharp_mask(extract_image)                 # アンシャープマスクを適用
    # 分割したフレームを全て2値化画像に変換
    binarided_original_image = binarize_image(extract_image)
    # フレームから青(レール)だけを抽出
    extract_bleu_image = extract_blue_parts(extract_image)


    # プラレールを除く背景画像を抽出
    keep_common_pixcels_image = calculate_between_image_at_interval(binarided_original_image,calculate_interval,0,True)
    background_image = calculate_image(keep_common_pixcels_image) 

    """
    -------背景からレール(青)以外の画素を取り除く--------
    """
    # 青→黒、背景→白になるように二値化
    binarized_blue_part = binarize_blue(extract_bleu_image)
    # 任意の間隔をあけて、共通画素のみ抽出
    keep_common_pixcels_image_of_blue = calculate_between_image_at_interval(binarized_blue_part,calculate_interval,0,True)
    # 全ての画像の共通画素のみ抽出
    train_rails = calculate_image(keep_common_pixcels_image_of_blue,0)
    # 背景とレール画像の共通画素のみ抽出
    unwanted_background_removed_image = extract_common_pixels(train_rails,background_image)

    """
    -------背景画像のモーメントを計算--------
    """
    # フレームと背景の差分を取り、モーメントを計算
    moment_x,moment_y,moment_image_list = image_moment(binarided_original_image,background_image)
    # 背景画像のモーメントを計算
    x,y,base_moment = calculate_moment(unwanted_background_removed_image)
    # x,y,base_moment = calculate_moment(background_image)
    print(f"(x,y)=({x,y})")

    """
    -------各画像のモーメントを計算--------
    """
    # yの最大値5つを抽出
    max_point = 6
    top_y_with_index = [index for index, y in sorted(enumerate(moment_y), key=lambda x: x[1], reverse=True)[:max_point]]
    print(top_y_with_index)

    # 背景と差分画像の重心間の距離を計算
    distance=[]
    for index in top_y_with_index:
        dis = calculate_euclidean_distance(x,y,moment_x[index],moment_y[index])
        # print(f"{index}：{dis}")     # 背景と撮影画像との差分画像の重心の距離を出力
        distance.append(dis)

    # 背景の重心のx座標に近いものにする場合
    # extract_x_in_max_y = [moment_x[i] for i in top_y_with_index]
    # print(extract_x_in_max_y)
    # nearest_x_index = min(range(len(extract_x_in_max_y)), key=lambda i: abs(extract_x_in_max_y[i] - x))
    # print(top_y_with_index[nearest_x_index])            # ベストショットのインデックス

    # ベストショットのフレーム番号を取得
    min_distance_index=top_y_with_index[distance.index(min(distance))]
    print(f"ベストショット：{min_distance_index:02}")


    """
    -------デバッグ用に画像を出力--------
    """
    # 動画から分割した画像を出力
    if debug_mode:
        # 出力フォルダが無ければ作成
        if not os.path.exists(output_root_folder):
            os.makedirs(output_root_folder)

        # 出力フォルダ
        extract_frames_folder= output_root_folder + "extract_frame"                                             # 分割フレーム
        binarized_frames_folder = output_root_folder + "binarized_frames"                                       # 2値化フレーム
        extract_blue_images_folder = output_root_folder + "extrace_blue_frames"                                 # 青抽出フレーム
        common_pixcel_images_folder = output_root_folder + "frame_diff_for_binariy"                             # 2画像の共通画素
        background_image_path = output_root_folder + "background_image.jpg"                                     # プラレールを除く背景
        train_rails_image_path = output_root_folder + "train_rails_blue.jpg"                                        
        binarized_blue_part_image_path = output_root_folder + "binarized_blue_part"
        keep_common_pixcels_image_of_blue_path = output_root_folder + "common_blue_part"
        unwanted_background_removed_image_path = output_root_folder + "unwanted_background_removed_image.jpg"
        moment_image_path = output_root_folder + "moment"
        baese_moment_path = os.path.join(output_root_folder, "moment//base_moment.jpg") 

        output_images(extract_image,extract_frames_folder)
        output_images(binarided_original_image,binarized_frames_folder)
        output_images(extract_bleu_image,extract_blue_images_folder)
        output_images(keep_common_pixcels_image,common_pixcel_images_folder)
        cv2.imwrite(background_image_path, background_image)
        output_images(binarized_blue_part,binarized_blue_part_image_path)
        output_images(keep_common_pixcels_image_of_blue,keep_common_pixcels_image_of_blue_path)
        cv2.imwrite(train_rails_image_path,train_rails)
        cv2.imwrite(unwanted_background_removed_image_path,unwanted_background_removed_image)
        output_images(moment_image_list,moment_image_path)
        cv2.imwrite(baese_moment_path,base_moment)

    return extract_image[min_distance_index]
