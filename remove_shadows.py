import cv2
import numpy as np
import math
import statistics
import matplotlib.pyplot as plt

img = cv2.imread('frame185.jpg') #path to the image

cvtcolor_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

plt.imshow(cvtcolor_img)
plt.show()

cvtcolor_img = cv2.cvtColor(cvtcolor_img,cv2.COLOR_GRAY2BGR)


plt.imshow(cvtcolor_img)
plt.show()