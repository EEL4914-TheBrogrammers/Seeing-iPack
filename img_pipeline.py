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
import matplotlib.image as matimg
import time
import os
import fnmatch
import shutil
from spi_rpi import alert

"""
====================================================
=================== Functions ======================
====================================================
"""
# Define perspective points
def define_perspective_points(img):
	w, h = img.shape[1], img.shape[0]
	# src_pts = np.float32([[(w/2)-390, (h/2)],[(w/8), h],[(w*7/8), h],[(w/2)+390, (h/2)]])
	# dst_pts = np.float32([[w/4, 0],[w/4, h],[w*3/4, h],[w*3/4, 0]])

#    src_pts = np.float32([[(w*2/8), (h/2)],[10, h],[1270, h],[(w*6/8), (h/2)]])
	src_pts = np.float32([[(w*2/8), (h/2)],[5, h],[w - 5, h],[(w*6/8), (h/2)]])
	dst_pts = np.float32([[w/4, 0],[w/4, h],[w*3/4, h],[w*3/4, 0]])

	return src_pts, dst_pts

# Warp image to birds eye view to show 2D surface
def perspective_transformation(img, src_pts, dst_pts, img_size):
	matrix = cv2.getPerspectiveTransform(src_pts, dst_pts)
	img_warp = cv2.warpPerspective(img, matrix, dsize=img_size, flags = cv2.INTER_LINEAR)

	return img_warp

def radius_curvature(ploty, left_fitx, right_fitx, shape):
	# ym_per_pix = 30/720 # meters per pixel in y dimension
	# xm_per_pix = 3.7/700 # meters per pixel in x dimension
	ym_per_pix = 7/720 # meters per pixel in y dimension
	xm_per_pix = 1/1500 # meters per pixel in x dimension
	y_eval = np.max(ploty)
	
	# Fit new polynomials to x,y in world space
	left_fit_cr = np.polyfit(ploty*ym_per_pix, left_fitx*xm_per_pix, 2)
	right_fit_cr = np.polyfit(ploty*ym_per_pix, right_fitx*xm_per_pix, 2)

	# Calculate the new radii of curvature
	left_curverad = ((1 + (2*left_fit_cr[0]*y_eval*ym_per_pix + left_fit_cr[1])**2)**1.5) / np.absolute(2*left_fit_cr[0])
	right_curverad = ((1 + (2*right_fit_cr[0]*y_eval*ym_per_pix + right_fit_cr[1])**2)**1.5) / np.absolute(2*right_fit_cr[0])
	mean_curverad = round(np.mean([left_curverad, right_curverad]), 2)
	
	# Find vehicle position
	frame_center_pixels = shape[1]/2
	camera_position_pixels = ((left_fitx[-1]+right_fitx[-1])/2)
	center_offset_meters = (camera_position_pixels - frame_center_pixels)*xm_per_pix

	return mean_curverad, center_offset_meters

def find_lines(img_warp):
	# Add up pixel values for every column, 2 highest peaks is the location for base of lane lines
	# White pixels = (255, 255, 255), Black pixels = (0, 0, 0)
	histogram = np.sum(img_warp[img_warp.shape[0]//2:,:], axis=0)

	# plt.plot(histogram)
	# plt.show()

	# Create an output image to draw on and  visualize the result
	out_img = np.dstack((img_warp, img_warp, img_warp))*255
	window_img = np.zeros_like(out_img)

	# Find the peak of the left and right halves of the histogram
	# These will be the starting point for the left and right lines
	midpoint = np.int(histogram.shape[0]/2)
	leftx_base = np.argmax(histogram[:midpoint]+200)				# Max of left half
	rightx_base = np.argmax(histogram[midpoint:]) + midpoint	# Max of right half

	# Choose the number of sliding windows
	nwindows = 30

	# Set height of windows
	window_height = np.int(img_warp.shape[0]/nwindows)
	
	# Identify the x and y positions of all nonzero pixels in the image
	nonzero = img_warp.nonzero()
	nonzeroy = np.array(nonzero[0])
	nonzerox = np.array(nonzero[1])
	
	# Current positions to be updated for each window
	leftx_current = leftx_base
	rightx_current = rightx_base
	
	# Set the width of the windows +/- margin
	margin = 5
	# Set minimum number of pixels found to recenter window
	minpix = 25
	# Create empty lists to receive left and right lane pixel indices
	left_lane_inds = []
	right_lane_inds = []

	# Draw window by window until the whole lines are encompassed by windows
	for window in range(nwindows):
		# Identify window boundaries in x and y (and right and left)
		win_y_low = img_warp.shape[0] - (window+1)*window_height
		win_y_high = img_warp.shape[0] - window*window_height
		win_xleft_low = leftx_current - margin
		win_xleft_high = leftx_current + margin
		win_xright_low = rightx_current - margin
		win_xright_high = rightx_current + margin

		# Draw the windows on the visualization image
		#cv2.rectangle(out_img,(win_xleft_low,win_y_low),(win_xleft_high,win_y_high),(255,255,0), 2)		# Left yellow
		#cv2.rectangle(out_img,(win_xright_low,win_y_low),(win_xright_high,win_y_high),(0,255,255), 2)	# Right blue 

		# plt.imshow(out_img)
		# plt.show()

		# Identify the nonzero pixels in x and y within the window
		good_left_inds = ((nonzeroy >= win_y_low) & (nonzeroy < win_y_high) & (nonzerox >= win_xleft_low) &  (nonzerox < win_xleft_high)).nonzero()[0]
		good_right_inds = ((nonzeroy >= win_y_low) & (nonzeroy < win_y_high) & (nonzerox >= win_xright_low) &  (nonzerox < win_xright_high)).nonzero()[0]
		
		# Append these indices to the lists
		left_lane_inds.append(good_left_inds)
		right_lane_inds.append(good_right_inds)
		
		# If you found > minpix pixels, recenter next window on their mean position
		if len(good_left_inds) > minpix:
			leftx_current = np.int(np.mean(nonzerox[good_left_inds]))
		if len(good_right_inds) > minpix:        
			rightx_current = np.int(np.mean(nonzerox[good_right_inds]))

	# Concatenate the arrays of indices
	left_lane_inds = np.concatenate(left_lane_inds)
	right_lane_inds = np.concatenate(right_lane_inds)

	# Extract left and right line pixel positions
	leftx = nonzerox[left_lane_inds]
	lefty = nonzeroy[left_lane_inds] 
	rightx = nonzerox[right_lane_inds]
	righty = nonzeroy[right_lane_inds] 

	# Fit a second order polynomial to each ( f(y) = Ay^2+By+C )
	left_fit = np.polyfit(lefty, leftx, 2)
	right_fit = np.polyfit(righty, rightx, 2)

	#Visualization
	# Generate x and y values for plotting
	ploty = np.linspace(0, img_warp.shape[0]-1, img_warp.shape[0] )
	left_fitx = left_fit[0]*ploty**2 + left_fit[1]*ploty + left_fit[2]
	right_fitx = right_fit[0]*ploty**2 + right_fit[1]*ploty + right_fit[2]
	
	# plt.plot(left_fitx)
	# plt.plot(right_fitx)
	# plt.show()
	
	# Create an image to draw on and an image to show the selection window
	out_img = np.dstack((img_warp, img_warp, img_warp))*255
	window_img = np.zeros_like(out_img)

	# Color in left and right line pixels
	out_img[nonzeroy[left_lane_inds], nonzerox[left_lane_inds]] = [255, 0, 0]
	out_img[nonzeroy[right_lane_inds], nonzerox[right_lane_inds]] = [0, 0, 255]

	# plt.imshow(out_img)
	# plt.show()

	# Generate a polygon to illustrate the search window area
	# And recast the x and y points into usable format for cv2.fillPoly()
	left_line_window1 = np.array([np.transpose(np.vstack([left_fitx-margin, ploty]))])
	left_line_window2 = np.array([np.flipud(np.transpose(np.vstack([left_fitx+margin, 
								  ploty])))])
	left_line_pts = np.hstack((left_line_window1, left_line_window2))
	right_line_window1 = np.array([np.transpose(np.vstack([right_fitx-margin, ploty]))])
	right_line_window2 = np.array([np.flipud(np.transpose(np.vstack([right_fitx+margin, 
								  ploty])))])
	right_line_pts = np.hstack((right_line_window1, right_line_window2))

	# Draw the lane onto the warped blank image
	#cv2.fillPoly(window_img, np.int_([left_line_pts]), (0,255, 0))
	#cv2.fillPoly(window_img, np.int_([right_line_pts]), (0,255, 0))

	result = cv2.addWeighted(out_img, 1, window_img, 0.3, 0)

	out_img[nonzeroy[left_lane_inds], nonzerox[left_lane_inds]] = [255, 0, 0]
	out_img[nonzeroy[right_lane_inds], nonzerox[right_lane_inds]] = [0, 0, 255]

	# Get curvature values and text to superimpose on frame
	mean_curverad, position =  radius_curvature(ploty, left_fitx, right_fitx, window_img.shape)
	
	result = cv2.addWeighted(out_img, 1, window_img, 0.3, 0)
	return left_fit, right_fit, result, mean_curverad, position

def invert_perspective(img, src_pts, dst_pts, img_size):
	inv_matrix = cv2.getPerspectiveTransform(dst_pts, src_pts)
	result = cv2.warpPerspective(img, inv_matrix, dsize=img_size, flags = cv2.INTER_LINEAR)
	return inv_matrix, result

def superimpose_lane_area(img, warp_img, l_fit, r_fit, inv_matrix, mean_curverad, position):    
	height,width = img.shape[0],img.shape[1]

	ploty = np.linspace(0, height-1, num=height)
	left_fitx = l_fit[0]*ploty**2 + l_fit[1]*ploty + l_fit[2]
	right_fitx = r_fit[0]*ploty**2 + r_fit[1]*ploty + r_fit[2]

	# Prepare x, y points into cv2.fillPoly() format
	pts_left = np.array([np.transpose(np.vstack([left_fitx, ploty]))])
	pts_right = np.array([np.flipud(np.transpose(np.vstack([right_fitx, ploty])))])
	pts = np.hstack((pts_left, pts_right))

	# Fill the lane region on lane_area
	lane_area =  np.zeros_like(img).astype(np.uint8)

	if position < (-0.03) or position > (0.03):
		# cv2.fillPoly(lane_area, np.int_([pts]), (0,0,255))
		if position < (-0.03):
			alert_left = 0
			alert_right += 1
			if alert_right == 1:
				print ("\n\nALERTING RIGHT\n\n")
				alert("right")
		if position > (0.03):
			alert_right = 0
			alert_left += 1
			if alert_right == 1:
				print ("\n\nALERTING LEFT\n\n")
				alert("left")
	else:
		alert_left = 0
		alert_right = 0
		alert("start")
		# cv2.fillPoly(lane_area, np.int_([pts]), (0,200,0))

	cv2.polylines(lane_area, np.int32([pts_left]), isClosed=False, color=(255,20,147), thickness=5)
	cv2.polylines(lane_area, np.int32([pts_right]), isClosed=False, color=(255,20,147), thickness=5)
		
	# Warp the filled lane back to original image space using inverse perspective matrix
	new_warp = cv2.warpPerspective(lane_area, inv_matrix, (width, height))
	
	# Superimpose on the original image
	result = cv2.addWeighted(img, 1, new_warp, 0.3, 0)
	# center_text = 'Position: ' + '{:6.2f}'.format(position) + ' m'
	# if position < (-0.35) or position > (0.35):
	# 	cv2.putText(result, center_text,(30, 180), cv2.FONT_HERSHEY_TRIPLEX, 1.7, (0, 0, 255), 3)
	# else:
	# 	cv2.putText(result, center_text,(30, 180), cv2.FONT_HERSHEY_TRIPLEX, 1.7, (255, 255, 255), 3)

	return result

def img_pipeline_main(img_og, img_threshold):
	img_size = (img_threshold.shape[1], img_threshold.shape[0])

	start = time.time()
	src_pts, dst_pts = define_perspective_points(img_og)
	print ("Defining Perspective: " + str(time.time() - start))

	# Warp image to transform perspective
	start = time.time()
	warp_img = perspective_transformation(img_threshold, src_pts, dst_pts, img_size)
	print ("Warp Image: " + str(time.time() - start))
	# plt.imshow(warp_img)
	# plt.show()

	# Find lines
	start = time.time()
	left_fit, right_fit, lines_img, mean_curverad, position = find_lines(warp_img)
	print ("Find Lines: " + str(time.time() - start))

	# Unwarp transformed perspective image
	start = time.time()
	inv_matrix, unwarp_img = invert_perspective(warp_img, src_pts, dst_pts, img_size)
	print ("Unwarp Image: " + str(time.time() - start))
	# plt.imshow(unwarp_img)
	# plt.show()

	start = time.time()
	lane_img = superimpose_lane_area(img_og, warp_img, left_fit, right_fit, inv_matrix, mean_curverad, position)
	print ("Superimpose Lane Area: " + str(time.time() - start))
	# plt.imshow(lane_img)
	# plt.show()

	return lane_img

def main():
	if len(sys.argv) != 2:
		print('usage: {} <in_filename>'.format(sys.argv[0]))
		sys.exit(0)
	
	in_fname = sys.argv[1]
	in_fname_threshold = "bw_contour_" + in_fname

	# Image preprocessing
	img_og = cv2.imread(in_fname)					# Load sidewalk image
	img_threshold = cv2.imread(in_fname_threshold)	# Load sidewalk image
	# img_threshold = cv2.cvtColor(img_threshold, cv2.COLOR_BGR2GRAY)

	img_pipeline_main(img_og, img_threshold)


if __name__ == "__main__":
	main()
