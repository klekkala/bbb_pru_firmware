#
## Makefile for Invensense MPU6050 device.
#
#

obj-m += servo_sysfs.o

KDIR := /home/kiran/bb-kernel/KERNEL/
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
