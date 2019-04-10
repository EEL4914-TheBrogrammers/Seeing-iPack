import os
import cv2
import sys
import time
import subprocess
import numpy as np
import RPi.GPIO as GPIO
import matplotlib.pyplot as plt

from picamera import PiCamera
from picamera.array import PiRGBArray

from spi_rpi import alert
from img_pipeline import img_pipeline_main
from draw_contour import draw_contour_main_realtime

processed_buffer = []
break_loop

def definition():
	global state
	global break_loop
	break_loop = 0
	state = 0

def setup_GPIO():
	print ("Setting up button...")
	GPIO.setwarnings(False)
	GPIO.setmode(GPIO.BCM)
	GPIO.setup(2, GPIO.IN, pull_up_down=GPIO.PUD_UP)
	print ("Setup complete...")

def button_callback(channel):
	global state
	if state == 1:
		print("Stopped")
		create_video()
		state = 0

def create_video():
	global break_loop
	cv2.VideoCapture(0).release()

	video = cv2.VideoWriter('video_real_test.avi', cv2.VideoWriter_fourcc(*'DIVX'), 60, (420, 420))

	for i in range(len(processed_buffer)):
		video.write(processed_buffer[i])

	cv2.destroyAllWindows()
	video.release()

	break_loop = 1

def main():
	alert("start")

	# Setting up GPIO interrupt
	print ("Setting up GPIO pin...\n")
	definition()
	setup_GPIO()
	GPIO.add_event_detect(2, GPIO.BOTH, callback=button_callback, bouncetime=200)
	print ("GPIO setup complete...\n")

	# Setting up camera
	print ("Warming up camera...\n")
	camera = PiCamera()
	camera.resolution = (420, 420)
	rawCapture = PiRGBArray(camera, size =(420, 420))
	time.sleep(2)
	print ("Camera setup complete...\n")
	
	frames = 60
	counter = 0
	
	start_all = time.time()

	for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
		start_single_frame = time.time()

		print (counter)
		image = np.array(frame.array)
		
		rawCapture.seek(0)
		rawCapture.truncate()
		
		# Draw contours on image
		start = time.time()
		img_threshold = draw_contour_main_realtime(image)
		print ("\tContouring Image: " + str(time.time() - start))

		# Process image and impose lane
		start = time.time()
		lane_img, alert_left, alert_right = img_pipeline_main(image, img_threshold)
		print ("\tProcessing Image: " + str(time.time() - start))
		
		processed_buffer.append(lane_img)

		print ("\nSingle Frame Processing Time: " + str(time.time() - start_single_frame) + "\n")

		if break_loop == 1:
			break

	print(time.time() - start_all)
	
	# cv2.VideoCapture(0).release()

	# video = cv2.VideoWriter('video_real_test.avi', cv2.VideoWriter_fourcc(*'DIVX'), 60, (640, 368))

	# for i in range(len(processed_buffer)):
	# 	video.write(processed_buffer[i])

	# cv2.destroyAllWindows()
	# video.release()

# global left_total
# global right_total

# def main():
# 	alert("start")
# 	dir_path = "bleh/"
# 	dir_list = os.listdir(dir_path)
# 	num_files = len(dir_list)

# 	img_array = []

# 	start_all = time.time()
# 	for i in range(num_files):
# 		start_single_frame = time.time()
		
# 		print (i)
# 		img_og = cv2.imread(dir_path + "frame" + str(i) + ".jpg")
		
# 		# Draw contours on image
# 		start = time.time()
# 		img_threshold = draw_contour_main_realtime(img_og)
# 		print ("\tContouring Image: " + str(time.time() - start))

# 		# Process image and impose lane
# 		start = time.time()
# 		lane_img, alert_left, alert_right = img_pipeline_main(img_og, img_threshold)
# 		print ("\tProcessing Image: " + str(time.time() - start))

# 		# Append processed image to create video later
# 		img_array.append(lane_img)
# 		print ("\nSingle Frame Processing Time: " + str(time.time() - start_single_frame) + "\n")
		
# 	print ("\nTotal time: " + str(time.time() - start_all))

# 	print ("\nCompiling video...")
# 	img = cv2.imread(dir_path + "frame1.jpg")
# 	height, width = img.shape[:2]
# 	size = (width, height)
# 	video = cv2.VideoWriter('video.avi', cv2.VideoWriter_fourcc(*'DIVX'), 20, size)
	
# 	for i in range(len(img_array)):
# 		video.write(img_array[i])

# 	cv2.destroyAllWindows()
# 	video.release()

# 	print ("\nCompilation complete")
	
if __name__ == "__main__":
    main()
