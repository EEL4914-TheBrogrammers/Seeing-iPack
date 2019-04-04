import RPi.GPIO as GPIO
import time
import sys

print("LED script called")

def main(state):
	GPIO.setmode(GPIO.BCM)
	GPIO.setwarnings(False)
	GPIO.setup(3, GPIO.OUT)
	if state == "0":
		print ("LED on\n")
		GPIO.output(3, GPIO.HIGH)
	elif state == "1":
		print ("LED off\n")
		GPIO.output(3, GPIO.LOW)

if __name__ == "__main__":
	state = sys.argv[2]

	main(state)
