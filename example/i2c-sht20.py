from nightWiring import io
from time import sleep


def shtInit():
    #RPi I2C bus: I2C1, SHT20 address: 0x40
    _i2c = io.i2cSetup('/dev/i2c-1', 0x40)
    return _i2c

def shtReadTemp(_i2c):
    io.i2cWrite(_i2c, 0xF3)
    sleep(0.1)
    ret = io.i2cRead(_i2c)
    ret = (ret<<8) + io.i2cRead(_i2c)
    ret = ret * 175.72 / 65536.0 - 46.85
    #print("%3.1f" %ret)
    return ret

def shtReadRh(_i2c):
    io.i2cWrite(_i2c, 0xF5)
    sleep(0.1)
    ret = io.i2cRead(_i2c)
    ret = (ret<<8) + io.i2cRead(_i2c)
    ret = ret * 125.0 / 65536.0 - 6.0
    #print("%3.1f" %ret)
    return ret


io.setup()
i2cFd = shtInit()
while True:
    print("[STH20] Temp: %.1fC, Rh: %.1f%%\n\n" %(shtReadTemp(i2cFd), shtReadRh(i2cFd)))
    sleep(1)
