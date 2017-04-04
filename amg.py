from Adafruit_I2C import Adafruit_I2C
from time import sleep

def init():
	dev = Adafruit_I2C(0x29)
        dev.write8(0x3D,0)  #Configuration mode
        sleep(0.1)
        dev.writeList(0x55,[0x0C,0,0xF9,0xFF,0xF5,0xFF,0xFB,0xFE,0x09,0x00,0xA8,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xE8,0x03,0xB4,0x02])
        dev.write8(0x3D,0x08)  #IMU
        #dev.write8(0x3D,0x09)  #Compass
        #dev.write8(0x3D,0x0A)  #M4G
        #dev.write8(0x3D,0x0B)  #NDOF_FMC_OFF
        #dev.write8(0x3D,0x0C)  #NDOF
        sleep(0.1)
        dev.write8(0x3B,0x81)  #Change configuration units
        return dev

def data(dev):
	data = dev.readList(0x08,0x2D)
        print data

def calibData(dev):
        data = dev.readList(0x55,0x16)
        print data

def magData(dev):
	list = dev.readList(0xe, 6)
        x = list[0] + list[1]*256
        if (x > 32767): x -= 65536
        y = list[2] + list[3]*256
        if (y > 32767): y -= 65536
        z = list[4] + list[5]*256
        if (z > 32767): z -= 65536

        print "x: " + str(x)
	print "y: " + str(y)
	print "z: " + str(z)

def accData(dev):
	list = dev.readList(0x8, 6)
        x = list[0] + list[1]*256
        if (x > 32767): x -= 65536
        y = list[2] + list[3]*256
        if (y > 32767): y -= 65536
        z = list[4] + list[5]*256
        if (z > 32767): z -= 65536

        print "x: " + str(x)
	print "y: " + str(y)
	print "z: " + str(z)
