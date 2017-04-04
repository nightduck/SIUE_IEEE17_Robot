from Adafruit_I2C import Adafruit_I2C
from time import sleep

def init(addr):
	dev = Adafruit_I2C(addr)
        dev.write8(0x35, 0x02)	#Set range to 128cm
        return dev

def dist(dev):
	l = dev.readList(0x5E, 2)
	return (l[0]*16 + l[1])/64.0
