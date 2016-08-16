#include <linux/kernel.h> /* printk() */
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/remoteproc.h>
#include <asm/uaccess.h> //for copy_to_user and related functions
#include <linux/ioport.h> //for allocating memory
#include <asm/io.h> //for ioremap
#include "servo_hw.h"

uint16_t *set_val;
uint8_t *get_val;

static void * Data_pointer; 

/***********************************    MACROS   ******************************************/
#define _BIT(X) (1<<X)
#define _BIT_EMPTY 0




//#define PWM_PRU 		PRU_ID_PRU0
#define PWM_CHANNELS			7
#define PWM_STEPS				8 /* always 8 */
#define PWM_OUTPUT_PER_STEP 	((PWM_CHANNELS+PWM_STEPS-1)/PWM_STEPS)

/***********************************   DATA DEF  ******************************************/

struct rproc *rproc_alloc(struct device *dev, const char *name,
				const struct rproc_ops *ops,
				const char *firmware, int len);

typedef struct
{
	unsigned int gpio_clear_set_reg;
	unsigned int bit_mask;
}pwm_output_t;

typedef struct
{
	pwm_output_t pwm_output[PWM_OUTPUT_PER_STEP];
	unsigned char pwm_pos[PWM_OUTPUT_PER_STEP];
}pwm_step_t;

typedef struct
{
	unsigned char servo_per_step;
	unsigned char pad[3];
	pwm_step_t pwm_step[PWM_STEPS];
}pwm_pru_mem_desc_t;


/***********************************  VARIABLES  ******************************************/
static pwm_pru_mem_desc_t * pru_servo_desc = NULL;
/***********************************     CODE    ******************************************/


int is_valid_signal(int num)
{
	if(num <0 || num>=PWM_CHANNELS)
		return 0;

	return 1;
}



int hw_init_pwm_device(void)
{
	
	printk(KERN_INFO "Pru initialization started\r\n");

	int ret;

	/* let's power on and boot our remote processor */
	/*ret = rproc_boot(my_rproc);
	if (ret) {

		printk(KERN_ERR "Can not upload pru software\r\n");
        return -1;
	}

   	if(pru_run(PWM_PRU) != PRU_ERROR_NO_ERROR)
	{
   		printk(KERN_ERR "Unable to start pru\r\n");
		return -1;
	}*/

	//Allocate memory for I/O.
	request_mem_region(0x4a310000, 14, "Data");
	//Ioremap returns a virtual address in Data_pointer.
	Data_pointer=ioremap(0x4a310000, 14);
	//Allocate memeory to *mosi
	set_val=kmalloc(sizeof(uint16_t), GFP_KERNEL);

	return 0;
}

int hw_enable_bank(int pwm_bank)
{	
	return 0;
}

int hw_disable_bank(int pwm_bank)
{
	return 0;
}

int hw_set_pos(int pwm_num, unsigned char pos)
{
	if(pwm_num >= PWM_CHANNELS || pru_servo_desc == NULL)
	{
		return -1;
	}
	pru_servo_desc->pwm_step[pwm_num/PWM_OUTPUT_PER_STEP].pwm_pos[pwm_num%PWM_OUTPUT_PER_STEP] = pos;
	return 0;
}	

unsigned char hw_get_position(int pwm_num)
{
	if(pwm_num >= PWM_CHANNELS || pru_servo_desc == NULL)
	{
		return 0;
	}
	return pru_servo_desc->pwm_step[pwm_num/PWM_OUTPUT_PER_STEP].pwm_pos[pwm_num%PWM_OUTPUT_PER_STEP];
}

void hw_close_pwm_device(void)
{
	//rproc_shutdown(my_rproc);
}


