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
- Completed interfacing RaspberryPi with MSP432 via SPI. Both the RaspberryPi and MSP432 can receive and transfer data to and from each other. This link will help as we start on our guiding algorithm.
- Completed interfacing ultrasonic sensor with MSP432 via GPIO and Timer modules. Based on the time between the rising edge and the falling edge from the Echo pin, the distance can be calculated using the formula *Distance = (time in uSec)/58*.
- Completed interfacing RaspberryPi 3B+ with RaspberryPi Camera V2. Camera can take images and video and store them to specified files.
- Completed breakout board PCB for Ultrasonic sensors.
- In progress: Microcontroller PCB
- In progress: Interfacing LiDAR with MSP432 via UART.
- In progress: Applying Canny Edge Detection on a video.

## Questions
- Design Opinion: LiDAR size
- MSP432 debugging circuit