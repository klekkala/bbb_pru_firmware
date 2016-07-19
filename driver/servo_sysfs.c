#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/pwm.h>
#include <linux/remoteproc.h>

struct pwm_export {
	struct device child;
	struct pwm_device *pwm;
};



/**
 * pwm_config() - change a PWM device configuration
 * @pwm: PWM device
 * @duty_ns: "on" time (in nanoseconds)
 * @period_ns: duration (in nanoseconds) of one cycle
 *
 * Returns: 0 on success or a negative error code on failure.
 */
int pwm_config(struct pwm_device *pwm, int duty_ns, int period_ns)
{
	int err;

	if (!pwm || duty_ns < 0 || period_ns <= 0 || duty_ns > period_ns)
		return -EINVAL;

	err = pwm->chip->ops->config(pwm->chip, pwm, duty_ns, period_ns);
	if (err)
		return err;

	pwm->duty_cycle = duty_ns;
	pwm->period = period_ns;

	return 0;
}



static struct pwm_export *child_to_pwm_export(struct device *child)
{
	return container_of(child, struct pwm_export, child);
}

static struct pwm_device *child_to_pwm_device(struct device *child)
{
	struct pwm_export *export = child_to_pwm_export(child);

	return export->pwm;
}

static ssize_t period_show(struct device *child,
			   struct device_attribute *attr,
			   char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);

	return sprintf(buf, "%u\n", pwm_get_period(pwm));
}

static ssize_t period_store(struct device *child,
			    struct device_attribute *attr,
			    const char *buf, size_t size)
{
	struct pwm_device *pwm = child_to_pwm_device(child);
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

	ret = pwm_config(pwm, pwm_get_duty_cycle(pwm), val);

	return ret ? : size;
}



static DEVICE_ATTR_RW(period);
//static DEVICE_ATTR_RW(duty_cycle);
//static DEVICE_ATTR_RW(enable);
//static DEVICE_ATTR_RW(polarity);

static struct attribute *pwm_attrs[] = {
	&dev_attr_period.attr,
	//&dev_attr_duty_cycle.attr,
	//&dev_attr_enable.attr,
	//&dev_attr_polarity.attr,
	NULL
};
ATTRIBUTE_GROUPS(pwm);

static struct class pwm_class = {
	.name = "servo",
	.owner = THIS_MODULE,
	.dev_groups = pwm_chip_groups,
};

static int pwmchip_sysfs_match(struct device *parent, const void *data)
{
	return dev_get_drvdata(parent) == data;
}

void pwmchip_sysfs_export(struct pwm_chip *chip)
{
	struct device *parent;

	/*
	 * If device_create() fails the pwm_chip is still usable by
	 * the kernel its just not exported.
	 */
	parent = device_create(&pwm_class, chip->dev, MKDEV(0, 0), chip,
			       "pwmchip%d", chip->base);
	if (IS_ERR(parent)) {
		dev_warn(chip->dev,
			 "device_create failed for pwm_chip sysfs export\n");
	}
}

void pwmchip_sysfs_unexport(struct pwm_chip *chip)
{
	struct device *parent;

	parent = class_find_device(&pwm_class, NULL, chip,
				   pwmchip_sysfs_match);
	if (parent) {
		/* for class_find_device() */
		put_device(parent);
		device_unregister(parent);
	}
}

static int __init pwm_sysfs_init(void)
{
	 //Allocate memory for I/O.
    request_mem_region(0x4a310000, 32, "Data");
    
    //Ioremap returns a virtual address in Data_pointer.
    Data_pointer=ioremap(0x4a310000, 32);
    
    //Allocate memeory to *mosi
    pwm_chain=kmalloc(sizeof(8*uint16_t), GFP_KERNEL);

	return class_register(&pwm_class);
}
subsys_initcall(pwm_sysfs_init);