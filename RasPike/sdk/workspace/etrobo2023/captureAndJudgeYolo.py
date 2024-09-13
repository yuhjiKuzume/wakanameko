import captureMinifig
import judgeYolo

if __name__ == "__main__":
    filePath = captureMinifig.picam_save()
    judgeYolo.detect_yolov8(filePath)