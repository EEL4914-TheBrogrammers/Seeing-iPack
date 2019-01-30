from picamera import PiCamera
from time import sleep
import sys

mode = sys.argv[1]
output_name = sys.argv[2]

camera = PiCamera()

camera.start_preview()

if mode == 'picture':
	camera.capture('/home/pi/Desktop/Images/' + output_name)
	sleep(5)
elif mode == 'video':
	camera.start_recording('/home/pi/Desktop/Videos/' + output_name)
	sleep(10)
	camera.stop_recording()

camera.stop_preview()
