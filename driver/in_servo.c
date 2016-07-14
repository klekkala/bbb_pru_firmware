/*
 * Driver for Bosch Sensortec BMP180 and BMP280 digital pressure sensor.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h> 
#include <linux/printk.h> 
#include <linux/kobject.h> 
#include <linux/sysfs.h> 
#include <linux/init.h> 
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/remoteproc.h> 


static struct kobject *pru_kobject;
static int pru_servo;


static ssize_t pru_servo_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos)
{
  copy_from_user(mosi,buf,count);
  uint8_t mosi_transfer=*mosi;
  iowrite8(count_servo,Data_pointer);
}


static struct kobj_attribute pru_servo_attribute =__ATTR(pru_servo, 0660, pru_servo_show,
                                                   pru_servo_store);

static int __init pruservo_module_init (void)
{
       struct device *dev_ret;
  //Allot Major and Minor number to the device file
  if ( alloc_chrdev_region(&device1, 0, 1, "spi_pru") < 0)
    {
        printk(KERN_INFO "No major and minor numbers were assigned");
        return -1;
    }
  //Initialize the cdev structure with file operation structure
    cdev_init(&spi_pru, &file_ops);
  //Tell the kernel about the char device structure
    if (cdev_add(&spi_pru, device1, 1) < 0)
    {
        printk(KERN_INFO "Device has not been succesfully registered in the kernel");
        return -1;
    }
    //Creates class cl that will be populated by Kernel 
    cl = class_create(THIS_MODULE, "char1"); 
    //cl is populated for udev daemon to create the device file
    dev_ret = device_create(cl, NULL, device1, NULL, "spi_pru_device");
    //Allocate memory for I/O.
    request_mem_region(0x4a310000, 8,"Data");
    //Ioremap returns a virtual address in Data_pointer.
    Data_pointer=ioremap(0x4a310000, 8);
    //Allocate memeory to *mosi
    mosi=kmalloc(sizeof(uint8_t), GFP_KERNEL);
  return 0;

}

static void __exit pruservo_module_exit (void)
{
        pr_debug ("Module un initialized successfully \n");
        kobject_put(pru_kobject);
}



module_init(pruservo_module_init);
module_exit(pruservo_module_exit);


MODULE_AUTHOR("Kiran Kumar Lekkala <kiran4399@gmail.com");
MODULE_DESCRIPTION("Driver for PRU Servo and 4th Encoder(eQep)");
MODULE_LICENSE("GPL v2");