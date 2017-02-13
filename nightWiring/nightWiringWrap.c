#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <sys/wait.h>
#include <pthread.h>

#include "Python.h"

#include "nightWiring.h"
#include "nightWiringGPIO.h"
#include "nightWiringShift.h"
#include "nightWiringSoftPwm.h"
#include "nightWiringSoftTone.h"
#include "nightWiringI2C.h"
#include "nightWiringSPI.h"
#include "nightWiringSerial.h"


static PyObject *module = NULL;

static PyObject *py_digitalWrite(PyObject *self, PyObject *args) {
    int pin = 0;
    int value = LOW;

    if(!PyArg_ParseTuple(args, "ii", &pin, &value)) {
        return NULL;
    }

    digitalWrite(pin, value);

    Py_RETURN_NONE;
}

static PyObject *py_digitalRead(PyObject *self, PyObject *args) {
    int pin = 0, ret = 0;

    if(!PyArg_ParseTuple(args, "i", &pin)) {
        return NULL;
    }

    ret = digitalRead(pin);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    return Py_BuildValue("i", ret);
}

static PyObject *py_pinMode(PyObject *self, PyObject *args) {
    int pin = 0;
    int value = INPUT;

    if(!PyArg_ParseTuple(args, "ii", &pin, &value)) {
        return NULL;
    }

    pinMode(pin, value);

    Py_RETURN_NONE;
}

static PyObject *py_setupGPIO(PyObject *self, PyObject *args) {
    Py_buffer pinMapBuff;
    int pinNum = 0, ret = 0;
    
    if(!PyArg_ParseTuple(args, "s*i", &pinMapBuff, &pinNum)) {
        return NULL;
    }
    
    ret = nightWiringGpioSetup((int*)pinMapBuff.buf, pinNum);
    PyBuffer_Release(&pinMapBuff);

    return Py_BuildValue("i", ret);
}

static PyObject *py_setup(PyObject *self, PyObject *args) {
    int ret = 0;
    
    ret = nightWiringSetup();
    
    return Py_BuildValue("i", ret);
}

static PyObject *py_shiftIn(PyObject *self, PyObject *args) {
    int dPin = 0, cPin = 0, order = 0, ret = 0;

    if(!PyArg_ParseTuple(args, "iii", &dPin, &cPin, &order)) {
        return NULL;
    }

    ret = shiftIn(dPin, cPin, order);

    return Py_BuildValue("i", ret);
}

static PyObject *py_shiftOut(PyObject *self, PyObject *args) {
    int dPin = 0, cPin = 0, order = 0, value = 0;

    if(!PyArg_ParseTuple(args, "iiii", &dPin, &cPin, &order, &value)) {
        return NULL;
    }

    shiftOut(dPin, cPin, order, value);

    Py_RETURN_NONE;
}

static PyObject *py_softPwmCreate(PyObject *self, PyObject *args) {
    int pin = 0, initialValue = 0, pwmRange = 0, ret = 0;

    if(!PyArg_ParseTuple(args, "iii", &pin, &initialValue, &pwmRange)) {
        return NULL;
    }

    ret = softPwmCreate(pin, initialValue, pwmRange);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    return Py_BuildValue("i", ret);
}

static PyObject *py_softPwmWrite(PyObject *self, PyObject *args) {
    int pin = 0, value = 0;

    if(!PyArg_ParseTuple(args, "ii", &pin, &value)) {
        return NULL;
    }

    softPwmWrite(pin, value);
    
    Py_RETURN_NONE;
}

static PyObject *py_softPwmStop(PyObject *self, PyObject *args) {
    int pin = 0;

    if(!PyArg_ParseTuple(args, "i", &pin)) {
        return NULL;
    }

    softPwmStop(pin);
    
    Py_RETURN_NONE;
}

static PyObject *py_softToneCreate(PyObject *self, PyObject *args) {
    int pin = 0, ret = 0;

    if(!PyArg_ParseTuple(args, "i", &pin)) {
        return NULL;
    }

    ret = softToneCreate(pin);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    return Py_BuildValue("i", ret);
}

static PyObject *py_softToneWrite(PyObject *self, PyObject *args) {
    int pin = 0, freq = 0;

    if(!PyArg_ParseTuple(args, "ii", &pin, &freq)) {
        return NULL;
    }

    softToneWrite(pin, freq);
    
    Py_RETURN_NONE;
}

static PyObject *py_softToneStop(PyObject *self, PyObject *args) {
    int pin = 0;

    if(!PyArg_ParseTuple(args, "i", &pin)) {
        return NULL;
    }

    softToneStop(pin);
    
    Py_RETURN_NONE;
}

static PyObject *py_serialOpen(PyObject *self, PyObject *args) {
    int baud = 0, ret = 0;
    Py_buffer deviceName;

    if(!PyArg_ParseTuple(args, "s*i", &deviceName, &baud)) {
        return NULL;
    }

    ret = serialOpen((const char *)deviceName.buf, (const int)baud);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    PyBuffer_Release(&deviceName);
    return Py_BuildValue("i", ret);
}

static PyObject *py_serialFlush(PyObject *self, PyObject *args) {
    int fd = 0;

    if(!PyArg_ParseTuple(args, "i", &fd)) {
        return NULL;
    }

    serialFlush((const int)fd);

    Py_RETURN_NONE;
}

static PyObject *py_serialClose(PyObject *self, PyObject *args) {
    int fd = 0;

    if(!PyArg_ParseTuple(args, "i", &fd)) {
        return NULL;
    }

    serialClose((const int)fd);

    Py_RETURN_NONE;
}

static PyObject *py_serialPutchar(PyObject *self, PyObject *args) {
    int fd = 0, chr = 0;

    if(!PyArg_ParseTuple(args, "ii", &fd, &chr)) {
        return NULL;
    }

    serialPutchar((const int)fd, (const unsigned char)(chr & 0x0FF));

    Py_RETURN_NONE;
}

static PyObject *py_serialPuts(PyObject *self, PyObject *args) {
    int fd = 0;
    Py_buffer dataBuff;

    if(!PyArg_ParseTuple(args, "is*", &fd, &dataBuff)) {
        return NULL;
    }

    serialPuts((const int)fd, (const char *)dataBuff.buf);

    PyBuffer_Release(&dataBuff);
    Py_RETURN_NONE;
}

static PyObject *py_serialPrintf(PyObject *self, PyObject *args) {
    int fd = 0;
    Py_buffer dataBuff;

    if(!PyArg_ParseTuple(args, "is*", &fd, &dataBuff)) {
        return NULL;
    }

    serialPrintf((const int)fd, (const char *)dataBuff.buf);
    
    PyBuffer_Release(&dataBuff);
    Py_RETURN_NONE;
}

static PyObject *py_serialDataAvail(PyObject *self, PyObject *args) {
    int fd = 0, ret = 0;

    if(!PyArg_ParseTuple(args, "i", &fd)) {
        return NULL;
    }

    ret = serialDataAvail((const int)fd);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    return Py_BuildValue("i", ret);
}

static PyObject *py_serialGetchar(PyObject *self, PyObject *args) {
    int fd = 0, ret = 0;

    if(!PyArg_ParseTuple(args, "i", &fd)) {
        return NULL;
    }

    ret = serialGetchar((const int)fd);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    return Py_BuildValue("i", ret);
}

static PyObject *py_i2cSetup(PyObject *self, PyObject *args) {
    int devId = 0, ret = 0;
    Py_buffer deviceName;

    if(!PyArg_ParseTuple(args, "s*i", &deviceName, &devId)) {
        return NULL;
    }

    ret = i2cSetup((const char *)deviceName.buf, (const int)devId);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    PyBuffer_Release(&deviceName);
    return Py_BuildValue("i", ret);
}

static PyObject *py_i2cRead(PyObject *self, PyObject *args) {
    int fd = 0, ret = 0;

    if(!PyArg_ParseTuple(args, "i", &fd)) {
        return NULL;
    }

    ret = i2cRead(fd);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    return Py_BuildValue("i", ret);
}

static PyObject *py_i2cReadReg8(PyObject *self, PyObject *args) {
    int fd = 0, reg = 0, ret = 0;

    if(!PyArg_ParseTuple(args, "ii", &fd, &reg)) {
        return NULL;
    }

    ret = i2cReadReg8(fd, reg);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    return Py_BuildValue("i", ret);
}

static PyObject *py_i2cReadReg16(PyObject *self, PyObject *args) {
    int fd = 0, reg = 0, ret = 0;

    if(!PyArg_ParseTuple(args, "ii", &fd, &reg)) {
        return NULL;
    }

    ret = i2cReadReg16(fd, reg);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    return Py_BuildValue("i", ret);
}

static PyObject *py_i2cWrite(PyObject *self, PyObject *args) {
    int fd = 0, data = 0, ret = 0;

    if(!PyArg_ParseTuple(args, "ii", &fd, &data)) {
        return NULL;
    }

    ret = i2cWrite(fd, data);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    return Py_BuildValue("i", ret);
}

static PyObject *py_i2cWriteReg8(PyObject *self, PyObject *args) {
    int fd = 0, reg = 0, data = 0, ret = 0;

    if(!PyArg_ParseTuple(args, "iii", &fd, &reg, &data)) {
        return NULL;
    }

    ret = i2cWriteReg8(fd, reg, data);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    return Py_BuildValue("i", ret);
}

static PyObject *py_i2cWriteReg16(PyObject *self, PyObject *args) {
    int fd = 0, reg = 0, data = 0, ret = 0;

    if(!PyArg_ParseTuple(args, "iii", &fd, &reg, &data)) {
        return NULL;
    }

    ret = i2cWriteReg16(fd, reg, data);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    return Py_BuildValue("i", ret);
}


static PyObject *py_spiSetup(PyObject *self, PyObject *args) {
    int speed = 0, mode = 0, ret = 0;
    Py_buffer deviceName;

    if(!PyArg_ParseTuple(args, "s*ii", &deviceName, &speed, &mode)) {
        return NULL;
    }

    ret = spiSetupMode((const char *)deviceName.buf, speed, mode);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    PyBuffer_Release(&deviceName);
    return Py_BuildValue("i", ret);
}

static PyObject *py_spiDataTransfer(PyObject *self, PyObject *args) {
    int fd = 0, len = 0, ret = 0;
    Py_buffer dataBuff;

    if(!PyArg_ParseTuple(args, "is*i", &fd, &dataBuff, &len)) {
        return NULL;
    }

    ret = spiDataTransfer(fd, (unsigned char *)dataBuff.buf, len);
    if(ret < 0) {
        return PyErr_SetFromErrno(PyExc_IOError);
    }

    PyObject *out = Py_BuildValue("s", dataBuff.buf);
    PyBuffer_Release(&dataBuff);
    
    return out;
}

/* Define module methods */
static PyMethodDef module_methods[] = {
    {"setup", py_setup, METH_VARARGS, "Initialize module"},
    {"setupGPIO", py_setupGPIO, METH_VARARGS, "Initialize GPIO"},
    {"pinMode", py_pinMode, METH_VARARGS, "Set pin mode"},
    {"digitalWrite", py_digitalWrite, METH_VARARGS, "Set an output bit"},
    {"digitalRead", py_digitalRead, METH_VARARGS, "Read the value of a given Pin"},
    {"shiftOut", py_shiftOut, METH_VARARGS, "Shift data out to a clocked source"},
    {"shiftIn", py_shiftIn, METH_VARARGS, "Shift data in from a clocked source"},
    {"softPwmCreate", py_softPwmCreate, METH_VARARGS, "Create a new softPWM thread"},
    {"softPwmWrite", py_softPwmWrite, METH_VARARGS, "Write a PWM value to the given pin"},
    {"softPwmStop", py_softPwmStop, METH_VARARGS, "Stop an existing softPWM thread"},
    {"softToneCreate", py_softToneCreate, METH_VARARGS, "Create a new tone thread"},
    {"softToneWrite", py_softToneWrite, METH_VARARGS, "Write a frequency value to the given pin"},
    {"softToneStop", py_softToneStop, METH_VARARGS, "Stop an existing softTone thread"},
    {"serialOpen", py_serialOpen, METH_VARARGS, "Open and initialise the serial port"},
    {"serialFlush", py_serialFlush, METH_VARARGS, "Flush the serial buffers (both output & input)"},
    {"serialClose", py_serialClose, METH_VARARGS, "Release the serial port"},
    {"serialPutchar", py_serialPutchar, METH_VARARGS, "Send a single character to the serial port"},
    {"serialPuts", py_serialPuts, METH_VARARGS, "Send a string to the serial port"},
    {"serialPrintf", py_serialPrintf, METH_VARARGS, "Printf over serial, no difference to serialPuts in Python"},
    {"serialDataAvail", py_serialDataAvail, METH_VARARGS, "Return the number of bytes of data avalable to be read in the serial port"},
    {"serialGetchar", py_serialGetchar, METH_VARARGS, "Get a single character from the serial device"},
    {"i2cSetup", py_i2cSetup, METH_VARARGS, "Open the I2C device, and regsiter the target device"},
    {"i2cRead", py_i2cRead, METH_VARARGS, "Simple device read"},
    {"i2cReadReg8", py_i2cReadReg8, METH_VARARGS, "Read an 8-bit value from a regsiter on the device"},
    {"i2cReadReg16", py_i2cReadReg16, METH_VARARGS, "Read a 16-bit value from a regsiter on the device"},
    {"i2cWrite", py_i2cWrite, METH_VARARGS, "Simple device write"},
    {"i2cWriteReg8", py_i2cWriteReg8, METH_VARARGS, "Write an 8-bit value to the given register"},
    {"i2cWriteReg16", py_i2cWriteReg16, METH_VARARGS, "Write a 16-bit value to the given register"},
    {"spiSetup", py_spiSetup, METH_VARARGS, "Open the SPI device and set it up"},
    {"spiDataTransfer", py_spiDataTransfer, METH_VARARGS, "Write and Read a set of data over the SPI bus"},
    {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "io",
    NULL,
    -1,
    module_methods
};
#endif

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit_io(void) {
    module = PyModule_Create(&module_def);
#else
PyMODINIT_FUNC initio(void) {
    module = Py_InitModule("io", module_methods);
#endif

    if(module == NULL) {
#if PY_MAJOR_VERSION >= 3
        return NULL;
#else
        return;
#endif
    }

    PyModule_AddObject(module, "HIGH", Py_BuildValue("i", HIGH));
    PyModule_AddObject(module, "LOW", Py_BuildValue("i", LOW));
    PyModule_AddObject(module, "INPUT", Py_BuildValue("i", INPUT));
    PyModule_AddObject(module, "OUTPUT", Py_BuildValue("i", OUTPUT));
    PyModule_AddObject(module, "INT_EDGE_NONE", Py_BuildValue("i", INT_EDGE_NONE));
    PyModule_AddObject(module, "INT_EDGE_FALLING", Py_BuildValue("i", INT_EDGE_FALLING));
    PyModule_AddObject(module, "INT_EDGE_RISING", Py_BuildValue("i", INT_EDGE_RISING));
    PyModule_AddObject(module, "INT_EDGE_BOTH", Py_BuildValue("i", INT_EDGE_BOTH));
    PyModule_AddObject(module, "PIN_MAX_NUM", Py_BuildValue("i", NW_PIN_MAX));
    PyModule_AddObject(module, "LSBFIRST", Py_BuildValue("i", LSBFIRST));
    PyModule_AddObject(module, "MSBFIRST", Py_BuildValue("i", MSBFIRST));
    

#if PY_MAJOR_VERSION >= 3
    return module;
#else
    return;
#endif
}
