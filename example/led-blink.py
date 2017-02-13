from nightWiring import io
from time import sleep
from array import array

# Demo: Raspberry Pi with ICA HAT
ledMap = array('i', [27, 23, 22, 24])

io.setup()
io.setupGPIO(ledMap, 4)

for i in range(0, 4):
    io.pinMode(i, io.OUTPUT)

while True:
    for i in range(0, 4):
        for j in range(0, 4):
            io.digitalWrite(j, io.HIGH)
        io.digitalWrite(i, io.LOW)
        sleep(0.5)
