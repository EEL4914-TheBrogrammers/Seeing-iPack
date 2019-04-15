import spidev
import time
import sys

spi = spidev.SpiDev()
spi.open(0,0)
spi.max_speed_hz = 500000

send_l = []
send_r = []

def alert(side):
	if side == "left_motor":
		spi.xfer([0x30])
		spi.xfer([0x0a])
	elif side == "right_motor":
		spi.xfer([0x31])
		spi.xfer([0x0a])
	elif side == "no_motor":
		spi.xfer([0x32])
		spi.xfer([0x0a])
	elif side == "stop":
		spi.xfer([0x33])
		spi.xfer([0x0a])
	elif side == "start":
		spi.xfer([0x34])
		spi.xfer([0x0a])
	elif side == "gpio_config_stop":
		spi.xfer([0x35])
		spi.xfer([0x0a])
		time.sleep(1)
		alert("dud")
	elif side == "cam_config_start":
		spi.xfer([0x36])
		spi.xfer([0x0a])
		time.sleep(1)
		alert("dud")
	elif side == "cam_config_stop":
		spi.xfer([0x37])
		spi.xfer([0x0a])
		time.sleep(1)
		alert("dud")
	elif side == "pause":
		spi.xfer([0x38])
		spi.xfer([0x0a])
		time.sleep(1)
		alert("dud")
	elif side == "cam_compile_stop":
		spi.xfer([0x39])
		spi.xfer([0x0a])
		time.sleep(1)
		alert("dud")
	elif side == "dud":
		spi.xfer([0x3A])
		spi.xfer([0x0a])

def main():
	if len(sys.argv) != 2:
		print('usage: {} <side>'.format(sys.argv[0]))
		sys.exit(0)

	side = sys.argv[1]

	print ("Triggering " + side + " side...")
	alert(side)
	alert(side)

if __name__ == "__main__":
	main()
