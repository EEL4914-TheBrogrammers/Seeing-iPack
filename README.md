# Seeing-iPack
Backpack for the visually impaired. Through the use of ultrasonic sensors, LiDAR sensor, and camera, we will create a backpack that guides the user similar to a seeing eye dog or walking stick.

# Due
- [x] 1/28/18: Presentation 
- [x] 2/4/18: Preliminary Design Report

# TO-DO
- [x] Interface RaspberryPi with MSP432 via SPI
- [x] Get picture from RaspberryPi and camera
- [x] Get video from RaspberryPi and camera
- [x] Get distance readings from ultrasonic sensor
	- [x] Get time reading from sensor
	- [x] Calculate distance from time and speed of sound
- [ ] Get distance and signal strength from LiDAR sensor
- [x] Start computer vision
- [ ] Start guiding algorithm
- [x] Start designing PCB

# Research Topics
- [x] Canny Edge Detection
- [ ] Use for neural networks
- [ ] Lane detection for vehicles
- [x] Spec sheets for each component

# 2/8/19 Weekly Meeting
## Tasks
- **Completed:** Ordered ultrasonic sensors (HC-SR04), Raspberry Pi 3B+, and Raspberry Pi Camera V2.
- **Completed:** Interfacing RaspberryPi with MSP432 via SPI.
- **Completed:** Interfacing ultrasonic sensor with MSP432 via GPIO and Timer modules.
- **Completed:** Interfacing RaspberryPi 3B+ with RaspberryPi Camera V2. Camera can take images and video and store them to files.
- **Completed:** Breakout board PCB for Ultrasonic sensors.
- **Completed:** Testing images using cv2 Canny function.
- **In progress:** Order vibration motors, rechargeable battery pack parts, and backpack materials.
- **In progress:** Microcontroller PCB
- **In progress:** Interfacing LiDAR with MSP432 via UART.
- **In progress:** Applying Canny Edge Detection on a video.

## Questions
- **Design Opinion:** LiDAR size
- MSP432 debugging circuit
- Soldering MSP432 chip onto PCB
