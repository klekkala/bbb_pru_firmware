##Firmware and Kernel module for PRU Servo

The PRU in the beagelbone blue is used to support 8 servo/ESC(s) and an additional encoder (apart from the 3 encoder which are used by the TI eQEP driver. The board consists fo 2 PRUs, PRU-0 and PRU-1 which are used for servo and encoder respectively. Currently, this repository consists of the source code for the servo PRU firmware and the kernel driver fo rthe servo. Support for the encoder will be made soon. Please ensure that your beaglebone runs with the debian distro with kernel 4.4.12+. Follow the steps to build and install:

###Servo firmware:
Before installing you need to setup your board with the TI PRU support package and compiler.
`mkdir /usr/share/ti/cgt-pru/bin`
`cd /usr/share/ti/cgt-pru/bin`
`ln -s /usr/bin/clpru clpru`
`echo -e "\nexport PRU_CGT=/usr/share/ti/cgt-pru\n" >> ~/.bashrc`
`. ~/.bashrc`

`git clone https://github.com/kiran4399/bbb_pru_firmware`
`cd bbb_pru_firmware/`
`make`
`sudo cp gen/servo_sysfs.out /lib/firmware/am335x-pru0-fw`

To load the firmware to the PRU, reboot the PRU core,
`echo "4a334000.pru0" > /sys/bus/platform/drivers/pru-rproc/unbind 2>/dev/null`
`echo "4a334000.pru0" > /sys/bus/platform/drivers/pru-rproc/bind`

If everything runs good, you need to see all positive messages when you do `dmesg`

###Kernel driver:
Install the linux headers for your board
`sudo apt-get update`
`apt-cache search linux-headers-$(uname -r)`
`sudo apt-get install linux-headers-$(uname -r)`

Assuming that you've already cloned the git repository:
`cd servo/`
`make`

If everything goes well, you'd get a servo_sysfs.ko file. Install it as a module in the kernel by running the following command as root:
`insmod servo_sysfs.ko`

If the module is installed properly, then you'd see a servo_sysfs init message when you do `dmesg`
