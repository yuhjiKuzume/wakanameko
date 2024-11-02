
import cv2
import camera_control as cam
from flask import Flask, render_template, Response

class Camera(object):
    def __init__(self):
        self.camera_handle = cam.init_camera()
    def __del__(self):
	    	    
        self.camera_handle.release()

    def get_frame(self):
        image = cam.read_camera(self.camera_handle)
        ret, frame = cv2.imencode('.jpg', image)
        return frame
app = Flask(__name__)


@app.route("/")
def index():
	return "Hello World!"

@app.route("/stream")
def stream():
	return render_template("stream.html")

def gen(camera):
	while True:
		frame = camera.get_frame()

		if frame is not None:
			yield (b"--frame\r\n"
				b"Content-Type: image/jpeg\r\n\r\n" + frame.tobytes() + b"\r\n")
		else:
			print("frame is none")

@app.route("/video_feed")
def video_feed():
	return Response(gen(Camera()),
			mimetype="multipart/x-mixed-replace; boundary=frame")


if __name__ == "__main__":
	app.debug = True
	app.run(host="0.0.0.0", port=5000)

