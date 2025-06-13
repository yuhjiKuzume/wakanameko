import os
from ultralytics import YOLO

# YOLOv8で画像判定
def predict_minifig(target_file_path):
    model = YOLO("C:\\Users\\r23600316\\ETrobocon2024\\Minifig_frontAndback_train\\weights\\best.pt")

    results = model.predict(target_file_path,project="minifig", save=True, conf=0.1)

    return results[0]

if __name__ == "__main__":
    predict_minifig("./IMG_2379.jpg")
