# python-nightWiring
Python wrap of nightWiring lib.

## Idea

Refer to nightWiring (https://github.com/nightseas/nightWiring).

Advantage:

 - Python is an easier and more flexible way for end user to control peripherals.
 - To be a pythoner could protect you in the jungle. lol

Same as nightWiring, you need to create a GPIO map table if you want to use GPIO functions. For interfaces such as SPI or serial port, only device file name (bus number) is needed.

## Installation

```sh
sudo python setup.py
```

## Programming

```python
# Import nightWiring.io (NOT nightWiring itself, and don't ask me why)
from nightWiring import io

from time import sleep

# Needed for GPIO map
from array import array

# A demo GPIO map for Raspberry Pi with ICA HAT, 'i' means integer.
ledMap = array('i', [27, 23, 22, 24])

# Setup nightWiring core function and GPIO function with our LED GPIO map.
io.setup()
io.setupGPIO(ledMap, 4)

# LED0 blink
io.pinMode(0, io.OUTPUT)

while True:
  io.digitalWrite(0, io.HIGH)
  sleep(0.5)
  io.digitalWrite(0, io.LOW)
  sleep(0.5)
```

## Execute your program

```sh
python code_file_name.py
```

Refer to examples for details.
