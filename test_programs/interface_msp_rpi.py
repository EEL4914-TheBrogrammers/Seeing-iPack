import time
import spidev	# Linux kernel driver for SPI

bus = 0		# SPI bus 0
device = 0	# 

# Enable SPI
spi = spidev.SpiDev()

# Open a connection
spi.open(bus, device)

# Set SPI speed and mode
spi.max_speed_hz = 800000

while True:
	resp = spi.xfer2([0x0A])
	time.sleep(0.1)
