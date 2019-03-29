import cv2
import numpy as np 
import matplotlib.pyplot as plt 


img = cv2.imread('frame0.jpg')


height, width = img.shape[:2]

top = np.zeros((int(height/2), width, 3), dtype=int)

bottom = img[int(height/2):height, 0:width]

concat = np.concatenate((top, bottom))



plt.imshow(img[0:int(height/2), 0:width])
plt.show()

plt.imshow(top)
plt.show()

plt.imshow(bottom)
plt.show()

plt.imshow(concat)
plt.show()