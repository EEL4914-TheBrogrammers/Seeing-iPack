from picamera import PiCamera
from time import sleep
import sys

output_name = sys.argv[1]

with picamera.PiCamera() as camera:
	camera.start_recording('/home/pi/Desktop/Videos/' + output_name)

	key = cv2.waitkey(1) & 0xFF
	if key == ord("q"):
		break

	camera.stop_recording()