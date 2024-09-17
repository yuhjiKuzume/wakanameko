import cv2

# 動画から指定のフレーム間隔で画像を抽出
def extract_frames(video_path, frame_interval):

    # 動画ファイルの読み込み
    cap = cv2.VideoCapture(video_path)

    # フレームレートを取得
    fps = cap.get(cv2.CAP_PROP_FPS)
    frame_interval_time = frame_interval / fps
    print(f"FPS: {fps}、{frame_interval}フレーム間の時間: {frame_interval_time} 秒")
    
    # 動画のフレームレートと総フレーム数を取得
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    
    # フレーム間隔を指定して、抜き出すフレームのインデックスを計算
    frames_to_capture = list(range(0, total_frames, frame_interval))
    
    current_frame = 0
    extract_image_list = []
    
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        # フレームが指定された間隔である場合に配列に格納
        if current_frame in frames_to_capture:
            extract_image_list.append(frame)

        current_frame += 1
    
    # リソースを解放
    cap.release()

    return extract_image_list
