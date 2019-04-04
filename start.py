import RPi.GPIO as GPIO
import time
import sys
import subprocess

def definition():
	global state
	state = 0

def setup_GPIO():
	print ("Setting up button...")
	GPIO.setwarnings(False)
	GPIO.setmode(GPIO.BCM)
	GPIO.setup(2, GPIO.IN, pull_up_down=GPIO.PUD_UP)
	print ("Setup complete...")

def button_callback(channel):
	global state
	if state == 0:
		print("On")
		subprocess.Popen(["python","led.py","state","0"], shell=False)
		state = 1
	elif state == 1:
		print("Off")
		subprocess.Popen(["python","led.py","state","1"], shell=False)
		state = 0



if __name__ == '__main__':
	definition()
	setup_GPIO()

	GPIO.add_event_detect(2, GPIO.BOTH, callback=button_callback, bouncetime=200)

	while True:
		time.sleep(1e6)


# Polling button
#while True:
#	input_state = GPIO.input(2)
#	if input_state == False:
#		print('Button pressed')
#		time.sleep(0.2)

#GPIO.cleanup()
