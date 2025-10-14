#ifndef MY_CHAR_DEVICE_H
#define MY_CHAR_DEVICE_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h> // For copy_to_user and copy_from_user
#include <linux/proc_fs.h>
#include <linux/sysfs.h>

#define DEVICE_NAME "my_char_device"
#define CLASS_NAME "my_char_device_class"

//ioctl command number
#define MY_IOCTL_NUMBER _IOWR(0x12,0,int)

struct my_char_device_data {
	int major_number;
	int minor_number;
	struct cdev my_cdev;
	struct device *my_device;
	char *buffer;
	size_t buffer_size;
	int count;
	struct proc_dir_entry *proc_entry;
	struct attribute *sysfs_attr;
	struct attribute_group sysfs_group;
};

extern struct my_char_device_data my_data;

int my_char_device_init(void);
void my_char_device_exit(void);

#endif
