#
## Makefile for Invensense MPU6050 device.
#
#

obj-m := servo_sysfs.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
