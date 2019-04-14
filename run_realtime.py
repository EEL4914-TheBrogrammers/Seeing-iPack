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

def definition():
	global state
	global pressed
	pressed = 0
	state = 0

def setup_GPIO():
	print ("Setting up button...")
	GPIO.setwarnings(False)
	GPIO.setmode(GPIO.BCM)
	GPIO.setup(2, GPIO.IN, pull_up_down=GPIO.PUD_UP)
	print ("Setup complete...")

def button_callback(channel):
	global state
	global pressed
	print ("\n\n\n\nIN BUTTON CALLBACK")
	print ("Button: ", state)
	if state == 0:
		print ("Starting")
		alert("start")
		state = 1
#		pressed = 1
		time.sleep(0.2)
	elif state == 1:
#		cv2.VideoCapture(0).release()
		print ("Stopping")
		#alert("stop")
		state = 0
#		pressed = 1
		time.sleep(0.2)

def create_video():
	video = cv2.VideoWriter('video_real_test.avi', cv2.VideoWriter_fourcc(*'DIVX'), 10, (420, 420))

	for i in range(len(processed_buffer)):
		video.write(processed_buffer[i])

	cv2.destroyAllWindows()
	video.release()

def camera_setup():
	print("Warming up camera...\n")
	camera = PiCamera()
	camera.resolution = (420, 420)
	rawCapture = PiRGBArray(camera, size =(420, 420))
	time.sleep(2)

	print ("Camera setup complete...\n")

	return camera, rawCapture

def main():
	global start_all
	#alert("stop")

	# Setting up GPIO interrupt
	print ("Setting up GPIO pin...\n")
	definition()
	setup_GPIO()
	GPIO.add_event_detect(2, GPIO.BOTH, callback=button_callback, bouncetime=300)
	print ("GPIO setup complete...\n")

	frames = 60
	counter = 0
	global state
	global pressed

	state = 0
	blah = 0

	while(1):
		blah = 0
		state = 0

		while(state == 0):
			time.sleep(0.2)

		# Set up camera
		camera, rawCapture = camera_setup()

		processed_buffer = []

		for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
			mer = 0
			print (counter)
			start_single_frame = time.time()
#			if state == 0:
#				break
#			if (state == 0 and pressed == 1):
#				print ("IN STOP LOGIC")
#				alert("stop")
#				pressed = 0
#				break
#			elif (state == 1 and pressed == 1):
#				print ("IN START LOGIC")
#				alert("start")
#				pressed = 0

			image = np.array(frame.array)

			rawCapture.seek(0)
			rawCapture.truncate()

			# Draw contours on image
			start = time.time()
			img_threshold = draw_contour_main_realtime(image)
			print ("\tContouring Image: " + str(time.time() - start))

			# Process image and impose lane
			start = time.time()
			if state == 0:
				alert("stop")
				mer = 1

			lane_img, alert_left, alert_right, temp = img_pipeline_main(image, img_threshold)
			print ("\tProcessing Image: " + str(time.time() - start))
			processed_buffer.append(lane_img)

			print ("\nSingle Frame Processing Time: " + str(time.time() - start_single_frame) + "\n")

			if state == 0 and mer == 1:
				break

			counter += 1

		# Close camera
		camera.close()

#		print ("\nCompiling video...")
#		video = cv2.VideoWriter('video_real_time.avi', cv2.VideoWriter_fourcc(*'DIVX'), 10, (420, 420))

#		for i in range(len(processed_buffer)):
#			video.write(processed_buffer[i])

		# Free memory
#		processed_buffer = None

#		cv2.destroyAllWindows()
#		video.release()

#		print ("\nFinished compiling video...")
#		print ("\nIdling...")


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
