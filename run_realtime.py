import os
import cv2
import numpy as np
import matplotlib.pyplot as plt
from draw_contour import draw_contour_main_realtime
from img_pipeline import img_pipeline_main
from picamera.array import PiRGBArray
from picamera import PiCamera
import time

#~ def processing():
	#~ frame = 0
	#~ while frame < frames:
		#~ plt.imshow

		#~ yield 'img%02d.jpg' % frame
		#~ print ("saved frame%02d", frame)

		#~ cv2.imread('img%02d.jpg' % frame)
		#~ img_og, img_threshold = draw_contour_main_realtime(frame)
		#~ lane_img = img_pipeline_main(img_og, img_threshold)
	
		#~ out_fname = "realtime/processed_img/frame" + str(counter) + ".jpg"
		#~ cv2.imwrite(out_fname, lane_img)

		#~ frame += 1
			
	#~ return lane_img

#~ def main():
	#~ camera = PiCamera()
	#~ camera.resolution = (640, 368)
	#~ rawCapture = PiRGBArray(camera, size =(640, 368))

	#~ print ("Warming up camera...\n")
	#~ time.sleep(2)
	#~ print ("Setup complete...\n")
	
	#~ frames = 60
	#~ counter = 0
	
	#~ processed_buffer = []
	#~ start = time.time()
	#~ for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
		#~ print (counter)
		#~ image = np.array(frame.array)
		
		#~ rawCapture.seek(0)
		#~ rawCapture.truncate()
		
		#~ img_og, img_threshold = draw_contour_main_realtime(image)
		#~ lane_img = img_pipeline_main(img_og, img_threshold)
		
		#~ processed_buffer.append(lane_img)
		
		#~ if counter == 60:
			#~ break
			
		#~ counter += 1
	#~ print(time.time() - start)
	
	#~ cv2.VideoCapture(0).release()
	
	#~ dir_path = "video_frames"
	#~ dir_list = os.listdir(dir_path)
	#~ num_files = len(dir_list)

	#~ video = cv2.VideoWriter('video.avi', cv2.VideoWriter_fourcc(*'DIVX'), 20, (1280, 720))

	#~ for i in range(num_files):
		#~ print(i)
		#~ img_loaded = cv2.imread(dir_path + "/" + "frame" + str(i) + ".jpg")
		#~ video.write(img_loaded)

	#~ cv2.destroyAllWindows()
	#~ video.release()



def main():
	dir_path = "bleh/"
	dir_list = os.listdir(dir_path)
	num_files = len(dir_list)

	img_array = []

	start_all = time.time()
	for i in range(num_files):
		start_single_frame = time.time()
		print (i)

		# Draw contours on image
		img_og = cv2.imread(dir_path + "frame" + str(i) + ".jpg")
		start = time.time()
		img_threshold = draw_contour_main_realtime(img_og)
		print ("\tContouring Image: " + str(time.time() - start))

		start = time.time()
		lane_img = img_pipeline_main(img_og, img_threshold)
		print ("\tProcessing Image: " + str(time.time() - start))
		
		# plt.imshow(lane_img)
		# plt.show()

		img_array.append(lane_img)
		print ("\nSingle Frame Processing Time: " + str(time.time() - start_single_frame) + "\n")
	print ("\nTotal time: " + str(time.time() - start_all))

	img = cv2.imread(dir_path + "frame1.jpg")
	height, width = img.shape[:2]
	size = (width, height)
	video = cv2.VideoWriter('video.avi', cv2.VideoWriter_fourcc(*'DIVX'), 20, size)
	
	for i in range(len(img_array)):
		video.write(img_array[i])

	cv2.destroyAllWindows()
	video.release()





#~ def main():

#	cap = cv2.VideoCapture(0)
#~ #	cap.set(3, 1280)
#~ #	cap.set(4, 720)
	#~ counter = 0

	#~ while(True):
		#~ print (counter)
		#~ ret, frame = cap.read()
		#~ print(ret)

		#~ plt.imshow(frame)
		#~ plt.show()
		#~ out_fname = "realtime/og_img/frame" + str(counter) + ".jpg"
		#~ cv2.imwrite(out_fname, frame)

		#~ img_og, img_threshold = draw_contour_main_realtime(frame)
		#~ lane_img = img_pipeline_main(img_og, img_threshold)

		#~ out_fname = "realtime/processed_img/frame" + str(counter) + ".jpg"
		#~ cv2.imwrite(out_fname, lane_img)

		#~ plt.imshow(lane_img)
		#~ plt.show()

		#~ counter += 1

		#~ key = cv2.waitKey(1) & 0xFF

		#~ # if the `q` key was pressed, break from the loop
		#~ if key == ord("q"):
			#~ break

	#~ cv2.VideoCapture(0).release()
	
	#~ dir_path = "video_frames"
	#~ dir_list = os.listdir(dir_path)
	#~ num_files = len(dir_list)

	#~ video = cv2.VideoWriter('video.avi', cv2.VideoWriter_fourcc(*'DIVX'), 20, (1280, 720))

	#~ for i in range(num_files):
		#~ print(i)
		#~ img_loaded = cv2.imread(dir_path + "/" + "frame" + str(i) + ".jpg")
		#~ video.write(img_loaded)

	#~ cv2.destroyAllWindows()
	#~ video.release()
	
	
if __name__ == "__main__":
    main()
