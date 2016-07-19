am/*
 * Driver for PRU Servo operation.
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

static dev_t in_servo;
int error_ret;

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

static ssize_t pru_servo_write(struct kobject *kobj, struct kobj_attribute, char *buf)
{
  copy_from_user(mosi,buf,4);
  uint8_t mosi_transfer=*mosi;
  iowrite8(count_servo,Data_pointer);
}


struct attribute_group = {
    .name = "in_servo",
};


static struct kobj_attribute foo_attribute =__ATTR(foo, 0660, foo_show,
                                                   foo_store);

static struct file_operations file_ops =      
{
    .owner = THIS_MODULE,
    .open = openchardevice,
    .release = closechardevice,
    .write=pru_servo_write,
};


static int __init pruservo_module_init (void)
{
  
  struct device *dev_ret;
  //Allocate Major and Minor number to the device file
  if ( alloc_chrdev_region(&device, 0, 1, "pru_servo") < 0)
    {
        printk(KERN_INFO "No major and minor numbers were assigned");
        return -1;
    }
  //Initialize the cdev structure with file operation structure
    
    cdev_init(&pru_servo, &file_ops);
  //Tell the kernel about the char device structure
    
    if (cdev_add(&pru_servo, in_servo, 1) < 0)
    {
        printk(KERN_INFO "Device has not been succesfully registered in the kernel");
        return -1;
    }

    //Creates class cl that will be populated by Kernel 
    cl_servo = class_create(THIS_MODULE, "char1");

    //cl is populated for udev daemon to create the device file
    error_ret = sysfs_create_group(cl_servo, NULL, in_servo, NULL, "servopru_device");

    if(error_ret){
        pr_debug("failed to create the foo file in /sys/kernel/kobject_example \n");
        return error;
    }
    
    //Allocate memory for I/O.
    request_mem_region(0x4a310000, 32, "Data");
    
    //Ioremap returns a virtual address in Data_pointer.
    Data_pointer=ioremap(0x4a310000, 32);
    
    //Allocate memeory to *mosi
    pwm_chain=kmalloc(sizeof(8*uint16_t), GFP_KERNEL);
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
    device_destroy(cl_servo, in_servo);

    class_destroy(cl_servo);
    cdev_del(&pru_servo);
    unregister_chrdev_region(in_servo, 1);
    release_mem_region(0x4a310000, 36);
    iounmap(Data_pointer);
    Data_pointer=NULL;
    mosi=NULL;
}


module_init(pruservo_module_init);
module_exit(pruservo_module_exit);


MODULE_AUTHOR("Kiran Kumar Lekkala <kiran4399@gmail.com>");
MODULE_DESCRIPTION("Driver for PRU Servo");
MODULE_LICENSE("GPL v2");