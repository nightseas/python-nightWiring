from distutils.core import setup
from distutils.core import Extension
from distutils.command.build_ext import build_ext as _build_ext

import sys

modules = [
	Extension('nightWiring.io', 
		sources=[
			'nightWiring/nightWiringWrap.c',
			'src/nightWiring/nightWiring.c',
			'src/nightWiring/nightWiringPriority.c',
			'src/nightWiring/nightWiringThread.c',
			'src/nightWiring/nightWiringGPIO.c',
			'src/nightWiring/nightWiringShift.c',
			'src/nightWiring/nightWiringSoftPwm.c',
			'src/nightWiring/nightWiringSoftTone.c',
			'src/nightWiring/nightWiringSPI.c',
			'src/nightWiring/nightWiringI2C.c',
			'src/nightWiring/nightWiringSerial.c'
		], 
		include_dirs=['src/nightWiring/'],
		extra_compile_args=['-Wformat=2']
	),
]

setup(
    name='nightWiring',
    version='0.1',
    author='Xiaohai Li',
    author_email='haixiaolee@gmail.com',
    url='https://github.com/nightseas',
    license='MPLv2',
    packages=['nightWiring'],
    description='Cross-platform hardware interface',
    classifiers=['Environment :: Console',
                 'Intended Audience :: Developers',
                 'Intended Audience :: Education',
                 'License :: OSI Approved :: GPL-3.0 License',
                 'Operating System :: POSIX :: Linux',
                 'Programming Language :: Python',
                 'Topic :: Software Development :: Embedded Systems'
    ],
    ext_modules=modules
)
