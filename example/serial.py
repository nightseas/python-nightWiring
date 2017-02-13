from nightWiring import io
from time import sleep

io.setup()
#RPi serial port: ttyAMA0
serialFd = io.serialOpen('/dev/ttyAMA0', 115200)

io.serialPuts(serialFd, 'Hello ' + 'nightWiring %d\n' % (115200))

while True:
    if(io.serialDataAvail(serialFd)>0):
        print('%c' %(io.serialGetchar(serialFd)))
