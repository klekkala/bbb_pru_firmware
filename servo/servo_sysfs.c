

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/remoteproc.h>
#include <asm/uaccess.h> //for copy_to_user and related functions
#include <linux/ioport.h> //for allocating memory
#include <asm/io.h> //for ioremap

uint16_t *set_val;
uint16_t *get_val;
static void * Data_pointer;

#define ARRAY_LEN(X) (sizeof(X)/sizeof(X[0]))


#define MINOR_SHIFT 2

#define SERVO_NUM 	7
#define DEV_NAME 	"servo_drv/servo%d"
#define SERVO_MIN	30
#define SERVO_MAX	250
#define SERVO_DEF	150

#define DEF_STR 256
#define DEF_DEV_NAME 50



int pwm_open(struct inode *inode, struct file *file);
int pwm_release(struct inode *inode, struct file *file);
ssize_t pwm_read (struct file *file,const char __user *buf, size_t count,loff_t *f_pos);
ssize_t pwm_write(struct file *file,const char __user *buf, size_t count,loff_t *f_pos);



typedef struct
{
	char 				name[DEF_DEV_NAME];
	unsigned char 		position;
	unsigned char 		id;
	struct miscdevice 	dev;	
}dev_channel_t;



/* Major number */
const int pwm_major = 125;


/* Structure that declares the usual file */
/* access functions */
static struct file_operations pwm_fops = {
  owner: THIS_MODULE,
  read: pwm_read,
  write: pwm_write,
  open: pwm_open,
  release: pwm_release
};


dev_channel_t * servo[SERVO_NUM];


dev_channel_t * pwm_create_device(int id)
{
	int ret;
	dev_channel_t * device;

	device=kmalloc(sizeof(dev_channel_t), GFP_KERNEL);
	memset(device, 0, sizeof(dev_channel_t));

	snprintf(device->name, ARRAY_LEN(device->name), DEV_NAME, id);
	device->dev.minor 	= id + MINOR_SHIFT;
	device->dev.name 	= device->name;
	device->dev.fops 	= &pwm_fops;
	device->position	= SERVO_DEF;
	device->id			= id;

	ret = misc_register(&(device->dev));
    if (ret)
	{
	    printk(KERN_ERR "Unable to register pwm driver misc device %s error %d\n", device->name, ret);
		kfree(device);
		device = NULL;
	}
	return device;
}



int pwm_init(void)
{
	int i;
	printk(KERN_INFO "PWM for servo motors driver init\r\n");

	/* Allocating memory for the buffer */
	for(i=0; i<SERVO_NUM; i++)
	{
		servo[i] = pwm_create_device(i);
	}

	//Allocate memory for I/O.
	request_mem_region(0x4a310000, 14, "Data");
	//Ioremap returns a virtual address in Data_pointer.
	Data_pointer=ioremap(0x4a310000, 14);
	//Allocate memeory to *mosi
	set_val=kmalloc(sizeof(uint16_t), GFP_KERNEL);
	get_val=kmalloc(sizeof(uint16_t), GFP_KERNEL);

	return 0;
}



void pwm_exit(void)
{
	int i;
	for(i=0; i<SERVO_NUM; i++)
	{
		if(servo[i] != NULL)
		{
			misc_deregister(&(servo[i]->dev));
			kfree(servo[i]);
			servo[i] = NULL;
		}
	}

	printk(KERN_INFO "PWM for servo motors driver close\r\n");
}

module_init(pwm_init);
module_exit(pwm_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kiran Kumar Lekkala");
MODULE_DESCRIPTION("Driver for PRU Servo");


int pwm_open(struct inode *inode, struct file *file)
{
	int id = MINOR(inode->i_rdev) - MINOR_SHIFT;

	if(id >= SERVO_NUM)
	{
		printk(KERN_ERR "incorrect id: %d \r\n", id);
		return -1;
	}
	
	file->private_data = servo[id];
	return 0;
}

int pwm_release(struct inode *inode, struct file *file)
{
	return 0;
}

ssize_t pwm_read(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
	char tmpbuf[6];
	dev_channel_t * device;
	int j, len;


	device = (dev_channel_t*)(file->private_data);
	if(device == NULL)
	{
		printk(KERN_ERR "device not initialized\r\n");
		return -1;
	}
	//uint16_t get_val_transfer=ioread16(Data_pointer);
	//int len = sizeof(get_val_transfer);
	//snprintf(tmpbuf, 6, "%03d\n", device->position);
	uint8_t get_val_transfer=ioread8(Data_pointer+int(device->id));
	int len = sizeof(get_val_transfer);
	copy_to_user(buf,get_val_transfer,len);
	}
	return j;
}

ssize_t pwm_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
	//char tmpbuf[6];
	//int err;
	uint8_t set_val_transfer=0;
	dev_channel_t * device;

	if(count>5)
		return -1;

	copy_from_user(set_val,buf,count);
	//tmpbuf[count]='\0';
	
	if(res >= SERVO_MIN && res <= SERVO_MAX && err == 0)
	{
		device = (dev_channel_t*)(file->private_data);
		//device->position = res;

		set_val_transfer = &set_val;
		iowrite8(set_val_transfer, Data_pointer);
    		int len =sizeof(mosi_transfer);
		return count;
	}
	printk(KERN_WARNING "incorrect number %s\r\n", tmpbuf);
	return -1;
}
