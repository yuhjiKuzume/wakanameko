import os
import cv2
import shutil
import time

from extract_bestshot import *

if __name__ == "__main__":

    """
    -------入出力設定--------
    """
    video_path = "/home/waka/wakanameko/RasPike/sdk/workspace/etrobo2023/video.mp4"
    output_root_folder = "../train_movie/"
    frame_interval = 20         # 動画から画像抽出時のフレーム間隔
    calculate_interval = 5      # 【重要】画像間演算時のフレーム間隔。抽出フレームでプラレールが重ならない間隔を指定
    debug_mode = False
    bestshot_folder_Path = "/home/waka/wakanameko/RasPike/sdk/workspace/etrobo2023/train_ditect"
    bestshot_filename = "bestshot_train.jpg"

    print(os.path.exists(video_path))

    """
    -------前処理--------
    """
    # 処理時間計測用
    start_time = time.time()
    # 作業フォルダがあれば削除
    if os.path.exists(output_root_folder):
        shutil.rmtree(output_root_folder)
        print("作業フォルダを削除")
    # ベストショット出力フォルダが無ければ作成
    if not os.path.exists(bestshot_folder_Path):
        os.makedirs(bestshot_folder_Path)

    # """
    # -------撮影完了を検知--------
    # """
    # print("動画受信待ち")
    # while True:
    #     if os.path.exists(video_path):
    #         break
    # print("動画受信")

    """
    -------ベストショット抽出--------
    """
    bestshot_image = extract_bestshot_image(video_path,output_root_folder,frame_interval,calculate_interval,debug_mode)
    bestshot_image = cv2.resize(bestshot_image,(800,600))       # サイズを800×600で出力
    resize_image = os.path.join(bestshot_folder_Path, bestshot_filename)
    cv2.imwrite(resize_image,bestshot_image)
    # PCの共有フォルダに画像を送信
    mount_dir = "/mnt/share"
    os.system(f"sudo cp {resize_image} {mount_dir}")

    """
    -------開発用--------
    """
    # # 処理時間計測用
    # end_time = time.time()
    # print(f"処理時間: {end_time - start_time:.4f} 秒")

    # # 画面に出力
    # cv2.imshow('Image', bestshot_image)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()