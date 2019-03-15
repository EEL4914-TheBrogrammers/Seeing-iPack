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
- [x] Get distance and signal strength from LiDAR sensor
- [x] Start computer vision
- [x] Start guiding algorithm
- [x] Start designing PCB

# Research Topics
- [x] Canny Edge Detection
- [x] Use for neural networks
- [x] Lane detection for vehicles
- [x] Spec sheets for each component

<!-- # 2/8/19 Weekly Meeting
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
- **In progress:** Applying Canny Edge Detection on a video. -->

# 2/15/19 Weekly Meeting
## Tasks
- **Completed:** Interfacing RaspberryPi with MSP432 via SPI.
- **Completed:** Interfacing ultrasonic sensor with MSP432 via GPIO and Timer modules.
- **Completed:** Interfacing RaspberryPi 3B+ with RaspberryPi Camera V2. Camera can take images and video and store them to files.
- **Completed:** Breakout board PCB for Ultrasonic sensors v1.
- **Completed:** Testing images using cv2 Canny function.
- **Completed:** Measuring ultrasonic sensor range (height, width, and length)
- **In progress:** Order vibration motors, battery charger IC, and backpack materials.
- **In progress:** Microcontroller PCB v1.
- **In progress:** Interfacing LiDAR with MSP432 via I2C.
- **In progress:** Applying Canny Edge Detection on a video.
- **In progress:** Meetings with Rich to discuss project ideas and feedback.

# 2/20/19 Weekly Meeting
## Tasks
- **Completed:** Setting up Raspberry Pi Touchscreen LCD.
- **Completed:** Set up SSH on Raspberry Pi to allow for remote access.
- **Completed:** Microcontroller PCB v1 (Ultrasonic header, Raspberry Pi header, LiDAR header, microcontroller, IC conversion from 5V to 3.3V).
- **In progress:** Setting up Emic2 TTS. Able to run TTS successfully using UART on DAD. Debugging UART code for MSP432.
- **In progress:** Setting up Garmin LiDAR Lite v3. Debugging I2C code for MSP432.

# 3/15/19 Weekly Meeting
## Tasks
- **Completed:** Interfacing Emic2 TTS with MSP432 via UART.
- **Completed:** Interfacing LiDAR Lite V3 with MSP432 via I2C.
- **Completed:** Having 3 ultrasonic sensors run in succession.
- **Completed:** Measuring LiDAR range (length).
- **Completed:** Ordered PCBs for both the MSP432 circuit and Ultrasonic breakout board.
- **In progress:** Combining ultrasonic sensor, LiDAR, and TTS with reduced lag between every sweep over all devices.
- **In progress:** Sidewalk lane detection algorithm on Raspberry Pi.
