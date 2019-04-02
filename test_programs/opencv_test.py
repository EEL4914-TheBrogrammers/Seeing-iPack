import matplotlib
matplotlib.use('Agg')

import cv2
import numpy as np
from matplotlib import pyplot as plt

img = cv2.imread('../Images/test_img.jpg', 0)

plt.imshow(img, cmap = 'gray')
