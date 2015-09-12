#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "driver.h"

MODULE_LICENSE("GPL");

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define DEVICE_NAME "hd44780"
#define CLASS_NAME "hd44780"

static int Major;
static struct class *hd44780_class = NULL;
static struct device *hd44780_device = NULL;

static int Device_Open = 0;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static char msg[256];
static size_t msg_length;

int init_module(void) {
	printk(KERN_INFO "HD44780: Module is loading...");

	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) {
		printk(KERN_ALERT "Registring device failed with %d\n", Major);
		return Major;
	}

	hd44780_class = class_create(THIS_MODULE, CLASS_NAME);
	hd44780_device = device_create(hd44780_class, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);

	driver_init();

	printk(KERN_INFO "HD44780: Module loaded! Major: %d", Major);
	return 0;
}

void cleanup_module(void) {
	printk(KERN_INFO "HD44780: Removing module...");

	driver_cleanup();

	device_destroy(hd44780_class, MKDEV(Major, 0));
	class_unregister(hd44780_class);
	class_destroy(hd44780_class);
	unregister_chrdev(Major, DEVICE_NAME);

	printk(KERN_INFO "HD44780: Module removed!");
}

static int device_open(struct inode *inode, struct file *file) {

	if (Device_Open) {
		return -EBUSY;
	}

	Device_Open++;

	try_module_get(THIS_MODULE);

	return 0;
}

static int device_release(struct inode *inode, struct file *file) {
	Device_Open--;

	module_put(THIS_MODULE);

	return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset) {

	int bytes_read = 0;

	copy_to_user(buffer, msg, msg_length);

	return msg_length;
}

static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t *offset) {

	sprintf(msg, buffer);
	msg_length = length;
	msg[msg_length] = 0;

	driver_print(msg);

	printk(KERN_INFO "%s(%d)", msg, length);

	return length;
}
