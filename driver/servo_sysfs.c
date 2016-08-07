#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>

#include <linux/poll.h>

#define MAJORNUM 42

struct class *hello_class;

static int hello_open(struct inode *inode, struct file *file);
static int hello_release(struct inode *inode, struct file *file);
void register_device(void);
void unregister_device(void);

#define HELLO_VERSION	"0.1"
static int hello_major = 0;

struct file_operations my_fops = {
	.open =	hello_open,
	.release = hello_release,
	.write = hello_write,
};


static ssize_t hello_write(struct file *filp,const char __user *buf, size_t count,loff_t *f_pos)
{
	copy_from_user(mosi,buf,count);
	uint8_t mosi_transfer=*mosi;
	iowrite8(mosi_transfer,Data_pointer);
    int len =sizeof(mosi_transfer);
    return len;
}


int hello_init(void)
{
	printk(KERN_INFO "Module says hi\n");
	register_device();
	//Allocate memory for I/O.
	request_mem_region(0x4a310000, 8,"Data");
	//Ioremap returns a virtual address in Data_pointer.
	Data_pointer=ioremap(0x4a310000, 8);
	//Allocate memeory to *mosi
	mosi=kmalloc(sizeof(uint8_t), GFP_KERNEL);
	return 0;
}

void hello_exit(void)
{
	printk(KERN_INFO "Module goes bye\n");
	unregister_device();
	 release_mem_region(0x4a310000, 8);
    iounmap(Data_pointer);
    Data_pointer=NULL;
    mosi=NULL;
}

void register_device() {
	struct cdev *my_dev = cdev_alloc();
	hello_class = class_create(THIS_MODULE, "hello");
	
	if (my_dev != NULL) {
		my_dev->ops = &my_fops;
		my_dev->owner = THIS_MODULE;
	}
	else
		goto failed;
	
	hello_major = register_chrdev(0, "hello", my_dev->ops);
	if (hello_major < 0)
		goto failed;
	
	if (IS_ERR(device_create(hello_class, NULL, MKDEV(hello_major, 0), NULL, "hello")))
		printk(KERN_INFO "can't create sysfs entry for /dev/hello-0\n");
	else
		printk(KERN_INFO "Registered dev file /dev/hello-0 MAJORNUM %d minor 0\n", hello_major);
		       
failed:
	return;
}

void unregister_device() {
	device_destroy(hello_class, MKDEV(hello_major, 0));
	class_unregister(hello_class);
	class_destroy(hello_class);
	
	unregister_chrdev(hello_major, "hello");
	
}

static int hello_open(struct inode *inode, struct file *file)
{
	return -1;
}

static int hello_release(struct inode *inode, struct file *file)
{
	return -1;
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_VERSION(HELLO_VERSION);
MODULE_AUTHOR("Nick Glynn (exosyst@gmail.com)");
MODULE_DESCRIPTION("Dummy Driver");
MODULE_LICENSE("GPL v2");
