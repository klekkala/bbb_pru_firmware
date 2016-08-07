#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/pinctrl/consumer.h>

#define SERVO_PWM_PERIOD  20000000
#define SERVO_MAX_DUTY    2500000
#define SERVO_MIN_DUTY    500000
#define SERVO_DEGREE ((SERVO_MAX_DUTY - SERVO_MIN_DUTY) / 180)

struct pwm_servo_data {
	struct pwm_device *pwm;
	unsigned int angle;
	unsigned int angle_duty;
};

static ssize_t pwm_servo_show_angle(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct pwm_servo_data *servo =
		platform_get_drvdata(to_platform_device(dev));

	return sprintf(buf, "%u\n", servo->angle);
}

static ssize_t pwm_servo_store_angle(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long tmp;
	int error;
	struct pwm_servo_data *servo =
		platform_get_drvdata(to_platform_device(dev));

	error = kstrtoul(buf, 10, &tmp);

	if (error < 0)
		return error;

	if (tmp < 0 || tmp > 180)
		return -EINVAL;

	error = pwm_config(servo->pwm, SERVO_MIN_DUTY +
			SERVO_DEGREE * tmp, SERVO_PWM_PERIOD);
	if (error < 0)
		return error;

	servo->angle = tmp;
	servo->angle_duty = SERVO_MIN_DUTY + SERVO_DEGREE * tmp;

	return count;
}

static DEVICE_ATTR(angle, S_IWUSR | S_IRUGO, pwm_servo_show_angle,
		pwm_servo_store_angle);

static struct attribute *pwm_servo_attributes[] = {
	&dev_attr_angle.attr,
	NULL,
};

static const struct attribute_group pwm_servo_group = {
	.attrs = pwm_servo_attributes,
};





#ifdef CONFIG_OFdd
static const struct of_device_id pwm_servo_match[] = {
	{ .compatible = "pwm_servo", },
	{ },
};
MODULE_DEVICE_TABLE(of, pwm_servo_match);
#endif

static struct platform_driver pwm_servo_driver = {
	.probe	= pwm_servo_probe,
	.remove = pwm_servo_remove,
	.driver = {
		.name	= "pwm-servo",
#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(pwm_servo_match),
#endif
	},
};
module_platform_driver(pwm_servo_driver);

MODULE_AUTHOR("Adam Olek, Maciej Sobkowski <maciejjo@maciejjo.pl>");
MODULE_DESCRIPTION("PWM Servo driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:pwm-servo");
