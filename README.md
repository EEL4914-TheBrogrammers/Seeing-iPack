# Seeing-iPack
Backpack for the visually impaired. Through the use of ultrasonic sensors, LiDAR sensor, and camera, we will create a backpack that guides the user similar to a seeing eye dog or walking stick.

# Due
- [x] 1/28/18: Presentation 
- [ ] 2/4/18: Preliminary Design Report

# TO-DO
- [x] Interface RaspberryPi with MSP432 via SPI
- [x] Get picture from RaspberryPi and camera
- [x] Get video from RaspberryPi and camera
- [x] Get distance readings from ultrasonic sensor
	- [x] Get time reading from sensor
	- [x] Calculate distance from time and speed of sound
- [ ] Get distance and signal strength from LiDAR sensor
- [ ] Start machine learning
- [ ] Start designing PCB

# Research Topics
- [ ] Canny Edge Detection
- [ ] Use for neural networks
- [ ] Lane detection for vehicles
- [x] Spec sheets for each component

# 2/8/19 Weekly Meeting
- Completed interfacing RaspberryPi with MSP432 via SPI. Both the RaspberryPi and MSP432 can receive and transfer data to and from each other. This link will help as we start on our guiding algorithm.
