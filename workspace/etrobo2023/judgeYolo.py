from ultralytics import YOLO
import cv2
import os
 
def detect_yolov8(file_path):
  # Load a model
  model = YOLO('yolov8n.pt')  # load an official model
 
  # 検出した結果の画像を保存するディレクトリ
  output_dir = "predictMinifig/"
 
  if not os.path.exists(output_dir):
    os.makedirs(output_dir)
 
  # img = cv2.imread(file_path)
  # results = model(img)
  # result_img = results.plot()
  # output_path = os.path.join(output_dir, "bestshot.jpg")
  # cv2.imwrite(output_path, result_img)
 
  # Predict with the model
  results = model.predict(file_path, project=output_dir, save=True, imgsz=320, conf=0.5)
 
  result_object = results[0]
  bounding_boxes = result_object.boxes.xyxy
  class_ids = result_object.boxes.cls
  class_names = result_object.names
 
  for box, class_id in zip(bounding_boxes, class_ids):
    class_name = class_names[int(class_id)]
    print(f"Box coordinates: {box}, Object: {class_name}")