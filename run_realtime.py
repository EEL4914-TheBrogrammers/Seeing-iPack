import sys
import os
import cv2
import scipy.misc
import numpy as np
import matplotlib.pyplot as plt
from draw_contour import draw_contour_main
from img_pipeline import img_pipeline_main

def main():

	cap = cv2.VideoCapture(0)
#	cap.set(3, 1280)
#	cap.set(4, 720)
	counter = 0

	while(True):
		print (counter)
		ret, frame = cap.read()
		print(ret)

		plt.imshow(frame)
		plt.show()
		out_fname = "realtime/og_img/frame" + str(counter) + ".jpg"
		cv2.imwrite(out_fname, frame)

		img_og, img_threshold = draw_contour_main_realtime(frame)
		lane_img = img_pipeline_main(img_og, img_threshold)

		out_fname = "realtime/processed_img/frame" + str(counter) + ".jpg"
		cv2.imwrite(out_fname, lane_img)

		plt.imshow(lane_img)
		plt.show()

		counter += 1

		key = cv2.waitKey(1) & 0xFF

		# if the `q` key was pressed, break from the loop
		if key == ord("q"):
			break

	cv2.VideoCapture(0).release()
	
	dir_path = "video_frames"
	dir_list = os.listdir(dir_path)
	num_files = len(dir_list)

	video = cv2.VideoWriter('video.avi', cv2.VideoWriter_fourcc(*'DIVX'), 20, (1280, 720))

	for i in range(num_files):
		print(i)
		img_loaded = cv2.imread(dir_path + "/" + "frame" + str(i) + ".jpg")
		video.write(img_loaded)

	cv2.destroyAllWindows()
	video.release()

if __name__ == "__main__":
    main()
