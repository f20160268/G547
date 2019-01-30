/*
	AUTHOR:		Parth Kalgaonkar
	ID Number:	2016A3PS0268P
	E-mail: 	f2016268@pilani.bits-pilani.ac.in

	ABOUT:
		This driver creates 3 dev files namely
			-adxl_x
			-adxl_y
			-adxl_z
		for each of the three axes of the accelerometer.

		Each device file has it's own unique file operations so as to allow extension for actual devices.

		For random number generation, kernel helper function 'get_random_bytes' is used.
		get_random_bytes uses /dev/urandom device for generating true random numbers.
		/dev/urandom gets the random numbers from the kernel entropy pool.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <linux/random.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Parth Kalgaonkar <parthkalgaonkar@gmail.com>");

static dev_t adxl;
static struct class *cls;
static struct cdev x_dev;
static struct cdev y_dev;
static struct cdev z_dev;
dev_t x,y,z;

uint16_t val;

/*
	Callback functions for 'open'
*/
static int xopen(struct inode *i, struct file *f){
	printk(KERN_INFO "ADXL X: open\n");
	return 0;
}

static int yopen(struct inode *i, struct file *f){
	printk(KERN_INFO "ADXL Y: open\n");
	return 0;
}

static int zopen(struct inode *i, struct file *f){
	printk(KERN_INFO "ADXL Z: open\n");
	return 0;
}

/*
	Function to generate Random numbers.
	get_random_bytes generates specified number of bytes of random numbers in the given buffer.
	get_random_bytes uses /dev/urandom which is the sysfs interface for the kernel entropy pool,
*/
uint16_t getrand(void){
	uint16_t retval;
	get_random_bytes(&retval, 2);
	return retval;
}

/*
	Callback functions for 'read'
*/
static ssize_t xread(struct file *f, char __user *buf, size_t len, loff_t *off){
	printk(KERN_INFO "ADXL X: read\n");
	val=getrand()&0x3ff;
	copy_to_user(buf, &val, 2);
	return 0;
}

static ssize_t yread(struct file *f, char __user *buf, size_t len, loff_t *off){
	printk(KERN_INFO "ADXL Y: read\n");
	val=getrand()&0x3ff;
	copy_to_user(buf, &val, 2);
	return 0;
}

static ssize_t zread(struct file *f, char __user *buf, size_t len, loff_t *off){
	printk(KERN_INFO "ADXL Z: read\n");
	val=getrand()&0x3ff;
	copy_to_user(buf, &val, 2);
	return 0;
}

/*
	Callback functions for 'close'
*/
static int xclose(struct inode *i, struct file *f){
	printk(KERN_INFO "ADXL X: close\n");
	return 0;
}

static int yclose(struct inode *i, struct file *f){
	printk(KERN_INFO "ADXL Y: close\n");
	return 0;
}

static int zclose(struct inode *i, struct file *f){
	printk(KERN_INFO "ADXL Z: close\n");
	return 0;
}

/*
	FOPS sturcture for each axis
*/
static struct file_operations xops={
	.owner = THIS_MODULE,
	.open = xopen,
	.release = xclose,
	.read = xread
};

static struct file_operations yops={
	.owner = THIS_MODULE,
	.open = yopen,
	.release = yclose,
	.read = yread
};

static struct file_operations zops={
	.owner = THIS_MODULE,
	.open = zopen,
	.release = zclose,
	.read = zread
};

int init_module(void)
{
	printk(KERN_INFO "ADXL: Hello world\n");

	if (alloc_chrdev_region(&adxl, 0, 3, "ADXL") < 0){
		return -1;
	}

	//generating different dev_t number for each axis
	x=MKDEV(MAJOR(adxl), MINOR(adxl)+0);
	y=MKDEV(MAJOR(adxl), MINOR(adxl)+1);
	z=MKDEV(MAJOR(adxl), MINOR(adxl)+2);

	//registering single class for all axes
	if ((cls=class_create(THIS_MODULE, "chrdrv"))==NULL){
		unregister_chrdev_region(adxl, 3);
		return -2;
	}

	//initialising each c_dev struct, creating device and making the device file live
	cdev_init(&x_dev, &xops);
	device_create(cls, NULL, x, NULL, "adxl_x");
	if(cdev_add(&x_dev, x, 1) < 0){
		device_destroy(cls, x);
		class_destroy(cls);
		unregister_chrdev_region(adxl, 3);
		return -4;
	}

	cdev_init(&y_dev, &yops);
	device_create(cls, NULL, y, NULL, "adxl_z");
	if(cdev_add(&y_dev, y, 1) < 0){
		cdev_del(&x_dev);
		device_destroy(cls, y);
		device_destroy(cls, x);
		class_destroy(cls);
		unregister_chrdev_region(adxl, 3);
		return -4;
	}

	cdev_init(&z_dev, &zops);
	device_create(cls, NULL, z, NULL, "adxl_y");
	if(cdev_add(&z_dev, z, 1) < 0){
		cdev_del(&y_dev);
		cdev_del(&x_dev);
		device_destroy(cls, z);
		device_destroy(cls, y);
		device_destroy(cls, x);
		class_destroy(cls);
		unregister_chrdev_region(adxl, 3);
		return -4;
	}

	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

void cleanup_module(void){
	cdev_del(&z_dev);
	cdev_del(&y_dev);
	cdev_del(&x_dev);
	device_destroy(cls, z);
	device_destroy(cls, y);
	device_destroy(cls, x);
	class_destroy(cls);
	unregister_chrdev_region(adxl, 3);

	printk(KERN_INFO "ADXL: Goodbye world\n");
}