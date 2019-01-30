from picamera import PiCamera
from time import sleep

camera = PiCamera()

camera.start_preview()
camera.capture('/home/pi/Desktop/Images/test_img.jpg')
sleep(5)
camera.stop_preview()
