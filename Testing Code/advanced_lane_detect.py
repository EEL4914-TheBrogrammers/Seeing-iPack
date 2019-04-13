""" =======================  Import dependencies ========================== """

import cv2
import math
import glob2
import numpy as np 
import matplotlib.pyplot as plt
import matplotlib.image as matimg
import scipy.misc
# from moviepy.editor import VideoFileClip

""" =======================  Calibration Functions ========================== """

# Calibrate camera
def camera_calib(directory, filename, x, y, img_size):
    objp = np.zeros((x*y,3), np.float32)
    objp[:,:2] = np.mgrid[0:x, 0:y].T.reshape(-1,2)

    objpoints = [] 
    imgpoints = []

    # Image List
    images = glob2.glob('./'+directory+'/'+filename+'*'+'.jpg')

    # Step through the list and search for chessboard corners
    for idx, fname in enumerate(images):
        img = cv2.imread(fname)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        # Find the chessboard corners
        ret, corners = cv2.findChessboardCorners(gray, (x, y), None)

        # If found, add object points, image points
        if ret == True:
            objpoints.append(objp)
            imgpoints.append(corners)
            
    if (len(objpoints) == 0 or len(imgpoints) == 0):
        raise Error("Calibration Failed")
    
    # Find camera parameters from several views of a calibration pattern
    ret, mtx, dst, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, img_size, None, None)
        
    return mtx, dst

# Remove image distortion
def undistort(img, cam_mtx, dst):
	# Transform image to compensate for lens distortion
	img = cv2.undistort(img, cam_mtx, dst, None, cam_mtx)
	
	return img

""" =======================  Image Thresholding ========================== """


""" =======================  Image Representation ========================== """

# Get image size
def get_img_size(img):
	height = np.size(img, 0)
	width = np.size(img, 1)

	return height, width

def print_imgs(img1, img2):
    f, ((ax1, ax2)) = plt.subplots(1, 2, figsize=(12, 18))
    ax1.imshow(img1)
    ax1.set_title('Original', fontsize=15)
    ax2.imshow(img2)
    ax2.set_title('Undistorted', fontsize=15)

    plt.show()

def main():
    """ ======================================= """
    """            Camera Calibration           """
    """ ======================================= """
    distorted_img_cal = matimg.imread("checkerboard_imgs/calibration2.jpg")
    directory = 'checkerboard_imgs'
    filename = 'calibration'
    x, y = 9, 6
    size = get_img_size(distorted_img_cal)

    # Camera calibration using 20 checker board images
    cam_mtx, dst = camera_calib(directory, filename, x, y, size)

    # Undistort calibration image
    undistorted_img_cal = undistort(distorted_img_cal, cam_mtx, dst)

    # Plot distorted image vs undistorted image
    print_imgs(distorted_img_cal, undistorted_img_cal)

    # Testing on Sidewalk image
    distorted_img_side = matimg.imread("sidewalk1.jpg")
    undistorted_img_side = undistort(distorted_img_side, cam_mtx, dst)
    scipy.misc.imsave("undist_sidewalk1.jpg", undistorted_img_side)

    # Plot distorted image vs undistorted image
    print_imgs(distorted_img_side, undistorted_img_side)

    # Testing on Sidewalk image
    distorted_img_side = matimg.imread("sidewalk2.jpg")
    undistorted_img_side = undistort(distorted_img_side, cam_mtx, dst)

    # Plot distorted image vs undistorted image
    print_imgs(distorted_img_side, undistorted_img_side)

    """ ======================================= """
    """               Thresholding              """
    """ ======================================= """



if __name__ == "__main__":
    main()
