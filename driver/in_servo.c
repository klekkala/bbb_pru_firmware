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
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/remoteproc.h> 


static struct kobject *pru_kobject;
static int pru_servo;

uint16_t *data;

static void * data_pointer; 

//struct dev_t stores the major and minor numbers

static dev_t in_servo1, in_servo2, in_servo3, in_servo4, in_servo5, in_servo6, in_servo7, in_servo8;

//To register as a character device in the kernel
static struct cdev pru_servo;

//udev daemon uses this to create device files
static struct class *cl_servo;

static int openchardevice(struct inode *i, struct file *f)  
{
    printk(KERN_INFO "Device has succesfully been opened\n");
    return 0;
}
static int closechardevice(struct inode *i, struct file *f) 
{
    printk(KERN_INFO "Device has successfully been closed\n");
    return 0;
}

static ssize_t pru_servo_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos)
{
  copy_from_user(mosi,buf,count);
  uint8_t mosi_transfer=*mosi;
  iowrite8(count_servo,Data_pointer);
}


static struct file_operations file_ops =      
{
    .owner = THIS_MODULE,
    .open = openchardevice,
    .release = closechardevice,
    .write=pru_servo_write,
};


static int __init pruservo_module_init (void)
{
  
  struct device* dev_ret1, dev_ret2, dev_ret3, dev_ret4, dev_ret5, dev_ret6, dev_ret7, dev_ret8;
  //Allocate Major and Minor number to the device file
  if ( alloc_chrdev_region(&device1, 0, 1, "pru_servo") < 0)
    {
        printk(KERN_INFO "No major and minor numbers were assigned");
        return -1;
    }
  //Initialize the cdev structure with file operation structure
    
    cdev_init(&pru_servo, &file_ops);
  //Tell the kernel about the char device structure
    
    if (cdev_add(&pru_servo, in_servo1, 1) || cdev_add(&pru_servo, in_servo2, 1) || cdev_add(&pru_servo, in_servo3, 1) || cdev_add(&pru_servo, in_servo4, 1) || cdev_add(&pru_servo, in_servo5, 1) || cdev_add(&pru_servo, in_servo6, 1) || cdev_add(&pru_servo, in_servo7, 1) || cdev_add(&pru_servo, in_servo8, 1) < 0)
    {
        printk(KERN_INFO "Device has not been succesfully registered in the kernel");
        return -1;
    }

    //Creates class cl that will be populated by Kernel 
    cl_servo = class_create(THIS_MODULE, "char1");

    //cl is populated for udev daemon to create the device file
    dev_ret1 = device_create(cl_servo, NULL, in_servo1, NULL, "servopru_device");
    dev_ret2 = device_create(cl_servo, NULL, in_servo2, NULL, "servopru_device");
    dev_ret3 = device_create(cl_servo, NULL, in_servo3, NULL, "servopru_device");
    dev_ret4 = device_create(cl_servo, NULL, in_servo4, NULL, "servopru_device");
    dev_ret5 = device_create(cl_servo, NULL, in_servo5, NULL, "servopru_device");
    dev_ret6 = device_create(cl_servo, NULL, in_servo6, NULL, "servopru_device");
    dev_ret7 = device_create(cl_servo, NULL, in_servo7, NULL, "servopru_device");
    dev_ret8 = device_create(cl_servo, NULL, in_servo8, NULL, "servopru_device");
    
    //Allocate memory for I/O.
    request_mem_region(0x4a310000, 36, "Data");
    
    //Ioremap returns a virtual address in Data_pointer.
    Data_pointer=ioremap(0x4a310000, 36);
    
    //Allocate memeory to *mosi
    mosi=kmalloc(sizeof(uint8_t), GFP_KERNEL);
  return 0;

}

static void __exit pruservo_module_exit (void)
{
        pr_debug ("Module un initialized successfully \n");
        kobject_put(pru_kobject);
        /* let's shut it down now */
        rproc_shutdown(rproc_pru);
}

static void __exit pruservo_exit(void)
{
    device_destroy(cl, device1);
    device_destroy(cl, device2);
    device_destroy(cl, device3);
    device_destroy(cl, device4);
    device_destroy(cl, device5);
    device_destroy(cl, device6);
    device_destroy(cl, device7);
    device_destroy(cl, device8);

    class_destroy(cl_servo);
    cdev_del(&spi_pru);
    unregister_chrdev_region(device1, 1);
    release_mem_region(0x4a310000, 36);
    iounmap(Data_pointer);
    Data_pointer=NULL;
    mosi=NULL;
    miso=NULL;
}


module_init(pruservo_module_init);
module_exit(pruservo_module_exit);


MODULE_AUTHOR("Kiran Kumar Lekkala <kiran4399@gmail.com");
MODULE_DESCRIPTION("Driver for PRU Servo and 4th Encoder(eQep)");
MODULE_LICENSE("GPL v2");