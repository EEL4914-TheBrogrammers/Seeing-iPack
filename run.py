import sys
import os
import cv2
import scipy.misc
import numpy as np
import matplotlib.pyplot as plt
from draw_contour import draw_contour_main
from img_pipeline import img_pipeline_main

def black_top_half(img):
	height, width = img.shape[:2]

	top = np.ones((int(height/2), width), dtype=int)
	bottom = img[int(height/2):height, 0:width]
	print (bottom.shape)

	concat = np.concatenate((top, bottom))

	return concat

def main():
	if len(sys.argv) != 3:
		print('usage: {} <in_filename>'.format(sys.argv[0]))
		sys.exit(0)
    
	dir_path = sys.argv[1]
	in_fname = sys.argv[2]

	dir_list = os.listdir(dir_path)
	num_files = len(dir_list)

	img_array = []

	for i in range(num_files):
		print (i)

		# Draw contours on image
		img_og, img_threshold = draw_contour_main(dir_path, in_fname + str(i) + ".jpg")
	
		lane_img = img_pipeline_main(img_og, img_threshold)

		# plt.imshow(lane_img)
		# plt.show()

		height, width, layers = lane_img.shape
		size = (width, height)

		img_array.append(lane_img)

	video = cv2.VideoWriter('video.avi', cv2.VideoWriter_fourcc(*'DIVX'), 20, size)
	
	for i in range(len(img_array)):
		video.write(img_array[i])

	cv2.destroyAllWindows()
	video.release()

if __name__ == "__main__":
    main()