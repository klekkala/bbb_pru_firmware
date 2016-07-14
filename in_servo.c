/*
 * Copyright (c) Kiran Kumar Lekkala(kiran4399@gmail.com)
 *
 * Driver for Bosch Sensortec BMP180 and BMP280 digital pressure sensor.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Datasheet:
 * https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMP180-DS000-121.pdf
 * https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BMP280-DS001-12.pdf
 */

#include <linux/module.h> 
#include <linux/printk.h> 
#include <linux/kobject.h> 
#include <linux/sysfs.h> 
#include <linux/init.h> 
#include <linux/fs.h>
#include <linux/string.h> 
 
static struct kobject *pru_kobject;
static int pru_servo;

static ssize_t pru_servo_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
        return sprintf(buf, "%d\n", pru_servo);
}

static ssize_t pru_servo_store(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf, size_t count)
{
        sscanf(buf, "%du", &pru_servo);
        return count;
}


static struct kobj_attribute pru_servo_attribute =__ATTR(pru_servo, 0660, pru_servo_show,
                                                   pru_servo_store);

static int __init pruservo_module_init (void)
{
        int error = 0;

        pr_debug("Module initialized successfully \n");

        pru_kobject = kobject_create_and_add("kobject_example",
                                                 kernel_kobj);
        if(!pru_kobject)
                return -ENOMEM;

        error = sysfs_create_file(pru_kobject, &pru_servo_attribute.attr);
        if (error) {
                pr_debug("failed to create the pru_servo file in /sys/kernel/kobject_example \n");
        }

        return error;
}

static void __exit pruservo_module_exit (void)
{
        pr_debug ("Module un initialized successfully \n");
        kobject_put(pru_kobject);
}



MODULE_AUTHOR("Kiran Kumar Lekkala <kiran4399@gmail.com");
MODULE_DESCRIPTION("Driver for PRU Servo and 4th Encoder(eQep)");
MODULE_LICENSE("GPL v2");