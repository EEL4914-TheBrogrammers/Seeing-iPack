import spidev
import time
import sys

spi = spidev.SpiDev()
spi.open(0,0)
spi.max_speed_hz = 500000

send_l = []
send_r = []

def alert(side):
	if side == "left":
		spi.xfer([0x30])
		spi.xfer([0x0a])
	elif side == "right":
		spi.xfer([0x31])
		spi.xfer([0x0a])
	elif side == "stop":
		spi.xfer([0x32])
		spi.xfer([0x0a])
	elif side == "start":
		spi.xfer([0x33])
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
