"""
====================================================
================ Import Packages ===================
====================================================
"""
import sys
import cv2
import scipy.misc
import numpy as np
import matplotlib.pyplot as plt
import colorsys


"""
====================================================
================== Definitions =====================
====================================================
"""
RED 	= (255,0,0)
GREEN 	= (0,255,0)
BLUE 	= (0,0,255)
YELLOW 	= (255,255,0)

# ROI[H range, W range]
ROI_H_LOW, ROI_H_HIGH = 690, 720
ROI_W_LOW, ROI_W_HIGH = 560, 720

# ROI[H range, W range]
ROI_H_LOW_2, ROI_H_HIGH_2 = 500, 600 
ROI_W_LOW_2, ROI_W_HIGH_2 = 600, 680

shadow = (68,78,90)

shadow_color = np.uint8([[[68, 78, 90]]])
shadow_HSV = cv2.cvtColor(shadow_color,cv2.COLOR_BGR2HSV)

shadow_H = shadow_HSV[0][0][0]		# Hue of sidewalk
shadow_S = shadow_HSV[0][0][1]		# Saturation of sidewalk
shadow_V = shadow_HSV[0][0][2]		# Value of sidewalk

shadow_lower = np.array([0, shadow_S-70, shadow_V-70])					# Determine lower bound
shadow_higher = np.array([shadow_H+70, shadow_S+70, shadow_V+70])		# Determine higher bound


"""
====================================================
=================== Functions ======================
====================================================
"""
# Image preprocessing with blurring
def blur_img(img):
	kernel = np.ones((100,100),np.float32)/10000	# Conv matrix for blurring the image

	img_blur = cv2.filter2D(img,-1,kernel)		# Convolve kernel with sidewalk image

	return img_blur

# Determine HSV values
def find_HSV(img, h_low, h_high, w_low, w_high):
	roi = img[h_low:h_high, w_low:w_high]				# Bottom middle of image

	average = roi.mean(axis=0).mean(axis=0)	# Average pixels [R G B]
	average = np.uint8([[average]])

	frame_HSV = cv2.cvtColor(average, cv2.COLOR_BGR2HSV)	# Convert color space to HSV for sidewalk section of img

	H = frame_HSV[0][0][0]		# Hue of sidewalk
	S = frame_HSV[0][0][1]		# Saturation of sidewalk
	V = frame_HSV[0][0][2]		# Value of sidewalk

	return H, S, V

# Threshold image to separate sidewalk from borders
def threshold_img(img, H, S, V):
	frame_HSV = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)		# Convert color space to HSV for whole img
	lower = np.array([H-50, S-50, V-50])					# Determine lower bound
	higher = np.array([H+50, S+50, V+50])					# Determine higher bound
	img_threshold = cv2.inRange(frame_HSV, lower, higher)	# Apply range of pixel values to image
	img_threshold_shadow = cv2.inRange(frame_HSV, shadow_lower, shadow_higher)
	# plt.imshow(img_threshold_shadow)
	# plt.show()

	mask = cv2.bitwise_or(img_threshold, img_threshold_shadow)

	# plt.imshow(mask)
	# plt.show()
	target = cv2.bitwise_and(img, img, mask=mask)
	target = cv2.cvtColor(target, cv2.COLOR_BGR2GRAY)

	# plt.imshow(target)
	# plt.show()

	return img_threshold

# Draw contour line to outline sidewalk
def draw_contour(img, img_blur, contour_color):
	ret, thresh = cv2.threshold(img, 127, 255, 0)
	im2, contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
	img_contour = cv2.drawContours(img_blur, contours, -1, contour_color, 3)

	# plt.imshow(img_contour)
	# plt.show()

	return img_contour

# Threshold contoured image
def threshold_contour_img(img):
	lower = np.array([245, 0, 0])						# Determine lower bound of red line
	higher = np.array([255, 0, 0])						# Determine higher bound of red line
	img_threshold = cv2.inRange(img, lower, higher)		# Apply range of pixel values to image

	# plt.imshow(im_threshold, cmap = 'gray')
	# plt.show()

	return img_threshold

def black_top_half(img):
	height, width = img.shape[:2]

	top = np.ones((int(height/2), width, 3), dtype=int)
	bottom = img[int(height/2):height, 0:width]

	concat = np.concatenate((top, bottom))

	return concat

def mask_imgs(img_og, img_thresh_1, img_thresh_2):
	mask = cv2.bitwise_or(img_thresh_1, img_thresh_2)
	# plt.imshow(mask)
	# plt.show()

	target = cv2.bitwise_and(img_og, img_og, mask=mask)
	target = cv2.cvtColor(target, cv2.COLOR_BGR2GRAY)

	return target

def draw_contour_main_realtime(img_og):
	img_blur = blur_img(img_og)		# Blur image by convolving with kernel

	# Threshold image
	H, S, V = find_HSV(img_blur, ROI_H_LOW, ROI_H_HIGH, ROI_W_LOW, ROI_W_HIGH)	# Find HSV values for sidewalk
	img_threshold_1 = threshold_img(img_blur, H, S, V)

	# Draw contour lines
	img_contour = draw_contour(img_threshold_1, img_blur, RED)

	# Threshold contoured image
	output_img = threshold_contour_img(img_contour)

	return img_og, output_img

def draw_contour_main(dir_path, in_fname):
	out_fname_bw = 'bw_contour_' + in_fname
	out_fname_color = 'color_contour_' + in_fname
	out_fname_thresh = 'thresh_' + in_fname

	# Image preprocessing
	img_og = cv2.imread(dir_path + in_fname)	# Load sidewalk image

	img_blur = blur_img(img_og)		# Blur image by convolving with kernel

	# Threshold image
	H, S, V = find_HSV(img_blur, ROI_H_LOW, ROI_H_HIGH, ROI_W_LOW, ROI_W_HIGH)	# Find HSV values for sidewalk
	img_threshold_1 = threshold_img(img_blur, H, S, V)

	# H, S, V = find_HSV(img_blur, ROI_H_LOW_2, ROI_H_HIGH_2, ROI_W_LOW_2, ROI_W_HIGH_2)
	# img_threshold_2 = threshold_img(img_blur, H, S, V)

	# H, S, V = find_HSV(img_blur, ROI_H_LOW_2+20, ROI_H_HIGH_2+20, ROI_W_LOW_2, ROI_W_HIGH_2)
	# img_threshold_3 = threshold_img(img_blur, H, S, V)

	# img_threshold_masked = mask_imgs(img_blur, img_threshold_1, img_threshold_2)
	# img_threshold_masked_2 = mask_imgs(img_blur, img_threshold_masked, img_threshold_3)
	# img_threshold_masked_3 = mask_imgs(img_blur, img_threshold_masked_2, img_threshold_4)
	# img_threshold_masked_4 = mask_imgs(img_blur, img_threshold_masked_3, img_threshold_5)

	# Draw contour lines
	img_contour = draw_contour(img_threshold_1, img_blur, RED)

	# Threshold contoured image
	output_img = threshold_contour_img(img_contour)

	# Save BW thresholded image and contoured blurred image
	# scipy.misc.imsave(out_fname_bw, output_img)
	# scipy.misc.imsave(out_fname_color, img_contour)
	# scipy.misc.imsave(out_fname_thresh, img_threshold)

	return img_og, output_img


def main():
	if len(sys.argv) != 3:
		print('usage: {} <in_filename>'.format(sys.argv[0]))
		sys.exit(0)
    
	dir_path = sys.argv[1]
	in_fname = sys.argv[2]

	draw_contour_main(dir_path, in_fname)


if __name__ == "__main__":
    main()