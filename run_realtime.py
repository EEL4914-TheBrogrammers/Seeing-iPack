import os
import cv2
import numpy as np
import matplotlib.pyplot as plt
from draw_contour import draw_contour_main_realtime
from img_pipeline import img_pipeline_main
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
from spi_rpi import alert

def main():
	alert("start")
	camera = PiCamera()
	camera.resolution = (420, 420)
	rawCapture = PiRGBArray(camera, size =(420, 420))

	print ("Warming up camera...\n")
	time.sleep(2)
	print ("Setup complete...\n")
	
	frames = 60
	counter = 0
	
	processed_buffer = []
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
		
		if counter == 100:
			break
			
		counter += 1

	print(time.time() - start_all)
	
	cv2.VideoCapture(0).release()

	video = cv2.VideoWriter('video_real_test.avi', cv2.VideoWriter_fourcc(*'DIVX'), 60, (640, 368))

	for i in range(len(processed_buffer)):
		video.write(processed_buffer[i])

	cv2.destroyAllWindows()
	video.release()

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
