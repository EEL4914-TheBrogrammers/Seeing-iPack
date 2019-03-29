import cv2
import sys
import matplotlib.pyplot as plt

vidcap = cv2.VideoCapture(sys.argv[1])
success, image = vidcap.read()
count = 0

while success:
	image = cv2.resize(image, (1280, 720))
	cv2.imwrite("bleh/frame%d.jpg" % count, image)     # save frame as JPEG file      
	success, image = vidcap.read()
	print('Read a new frame: ', success)
	count += 1