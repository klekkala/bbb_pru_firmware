###Firmware and Kernel module for PRU Servo

The PRU in the beagelbone blue is used to support 8 servo/ESC(s) and an additional encoder (apart from the 3 encoder which are used by the TI eQEP driver. The board consists fo 2 PRUs, PRU-0 and PRU-1 which are used for servo and encoder respectively. Currently, this repository consists of the source code for the servo PRU firmware and the kernel driver fo rthe servo. Support for the encoder will be made soon. 

To build the firmware please follow the following steps:


...........................................................
