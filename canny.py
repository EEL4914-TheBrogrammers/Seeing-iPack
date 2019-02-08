import cv2
import numpy as np
from matplotlib import pyplot as plt

img1 = '/home/pi/Desktop/Images/sidewalk1.jpg'
img2 = '/home/pi/Desktop/Images/sidewalk2.jpg'
img3 = '/home/pi/Desktop/Images/sidewalk3.jpg'

img = cv2.imread(img2, 0)
edges = cv2.Canny(img, 100, 200)

plt.subplot(131),plt.imshow(img)
plt.title('Original Image'), plt.xticks([]), plt.yticks([])
plt.subplot(132),plt.imshow(img,cmap = 'gray')
plt.title('Original Image in Grayscale'), plt.xticks([]), plt.yticks([])
plt.subplot(133),plt.imshow(edges,cmap = 'gray')
plt.title('Edge Image'), plt.xticks([]), plt.yticks([])

plt.show()
