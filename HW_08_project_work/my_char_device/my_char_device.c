#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h> // For copy_to_user and copy_from_user
#include <linux/proc_fs.h>
#include <linux/sysfs.h>
#include <linux/seq_file.h>

#include "my_char_device.h"

//Глобальная структура данных для драйвера
struct my_char_device_data my_data;

//ioctl function
static long my_char_device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	int value;

	switch(ioctl_num)
	{
		case MY_IOCTL_NUMBER:
			//Get the current count
			value = my_data.count;
			if(copy_to_user((void __user *)ioctl_param, &value, sizeof(int)))
			{
				return EFAULT;
			}
			printk(KERN_INFO "%s: ioctl called, count = %d\n",DEVICE_NAME,my_data.count);
			break;
		default:
			return -ENOTTY;
	}
	return 0;
}

//Операция open
static int my_char_device_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "%s: Device opened\n",DEVICE_NAME);
	return 0;
}

//Операция release
static int my_char_device_release(struct inode *inode, struct file * file)
{
	printk(KERN_INFO "%s: Device released\n",DEVICE_NAME);
	return 0;
}

//Операция read
static ssize_t my_char_device_read(struct file *file, char __user *user_buffer, size_t length, loff_t *offset)
{
	int bytes_read = 0;

	if(*offset >= my_data.count)
	{
		return 0; //End of file
	}

	if(length > my_data.count - *offset)
	{
		length = my_data.count - *offset;
	}

	if(copy_to_user(user_buffer, my_data.buffer + *offset, length))
	{
		return -EFAULT; //Bad address
	}

	*offset += length;
	bytes_read = length;

	printk(KERN_INFO "%s: Read %d bytes\n",DEVICE_NAME, bytes_read);
	return bytes_read;
}

//Операция write
static ssize_t my_char_device_write(struct file * file, const char __user *user_buffer, size_t length, loff_t *offset)
{
	int bytes_written = 0;

	if(length > my_data.buffer_size - my_data.count)
	{
		length = my_data.buffer_size - my_data.count;
	}

	if(copy_from_user(my_data.buffer + my_data.count, user_buffer, length))
	{
		return EFAULT; //Bad address
	}

	my_data.count += length;

	bytes_written = length;

	printk(KERN_INFO "%s: Wrote %d bytes\n", DEVICE_NAME, bytes_written);
	return bytes_written;
}

//File operations structure
static struct file_operations fops = 
{
	.owner = THIS_MODULE,
	.open = my_char_device_open,
	.release = my_char_device_release,
	.read = my_char_device_read,
	.write = my_char_device_write,
	.unlocked_ioctl = my_char_device_ioctl //Use unlocked ioctl
};

//Proc file read function
static int proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "Current count: %d\n", my_data.count);
	return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
	return single_open(file,proc_show, NULL);
}

static const struct proc_ops proc_ops = 
{
	.proc_open = proc_open,
	.proc_read = proc_show
};

//Sysfs attribute show function
static ssize_t sysfs_attr_show(struct attribute *attr, char *buf)
{
	return sprintf(buf,"%d\n",my_data.count);
}

//Sysfs attribute store function
static ssize_t sysfs_attr_store(struct attribute *attr, const char *buf, size_t count)
{
	int new_count;
	if(sscanf(buf,"%d",&new_count) == 1)
	{
		my_data.count = new_count;
		printk(KERN_INFO "%s: Sysfs count set to %d\n",DEVICE_NAME,my_data.count);
		return count;
	}
	return -EINVAL;
}

static struct attribute sysfs_attr = {
	.name = "count",
	.mode = S_IRUGO | S_IWUGO, //Read and write for all users
	.show = sysfs_attr_show,
	.store = sysfs_attr_store,
};

static struct attribute_group sysfs_group = {
	.attrs = &sysfs_attr,
	.ngattrs = 1,
};

//Inititalization function
int my_char_device_init(void)
{
	int ret;
	struct device *device;

	//Allocate a character device number
	ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
	if(ret < 0)
	{
		printk(KERN_ALERT "%s: Failed to allocate character device region\n",DEVICE_NAME);
		return ret;
	}

	my_data.major_number = MAJOR(dev);
	my_data.minor_number = MINOR(dev);
	printk(KERN_INFO "%s: Major number = %d, Minor number = %d\n", DEVICE_NAME, my_data.major_number, my_data.minor_number);

	//Initialize the character device
	cdev_init(&my_data.my_cdev,&fops);
	my_data.my_cdev.owner = THIS_MODULE;
	my_data.my_cdev.dev = dev;

	ret = cdev_add(&my_data.my_cdev,my_data.major_number,1);
	if(ret < 0)
	{
		printk(KERN_ALERT "%s: Failed to add character device\n",DEVICE_NAME);
		unregister_chrdev_region(dev,1);
	}

	//Create device class for udev
	device = device_create(&my_char_device_class, NULL, my_data.major_number, NULL, DEVICE_NAME);
	if(IS_ERR(device))
	{
		printk(KERN_ALERT "%s: Failed to create device\n", DEVICE_NAME);
		cdev_del(&my_data.my_cdev);
		unregister_chrdev_region(dev,1);
		return PTR_ERR(device);
	}

	//Create /proc entry
	my_data.proc_entry = proc_create(DEVICE_NAME,0666,NULL, &proc_ops);
	if(!my_data.proc_entry)
	{
		printk(KERN_ALERT "%s: Failed to create /proc entry \n", DEVICE_NAME);
	}

	//Create /sys entry
	ret = sysfs_create_group(&device->kobj,&sysfs_group);
	if(ret)
	{
		printk(KERN_ALERT "%s: Failed to create /sys entry\n",DEVICE_NAME);
	}

	
	my_data.buffer = kmalloc(1024,GFP_KERNEL);
	if(!my_data.buffer)
	{
		printk(KERN_ALERT "%s: Failed to allocate buffer\n", DEVICE_NAME);
		sysfs_remove_group(&device->kobj,&sysfs_group);
		proc_remove(my_data.proc_entry);
		device_destroy(my_char_device_class, MAJOR(dev));
		cdev_del(&my_data.my_cdev);
		unregister_chrdev_region(dev,1);
		return -ENOMEM;
	}

	my_data.count = 0;

	printk(KERN_INFO "%s: Device initialized successfully\n",DEVICE_NAME);
	return 0;
}

//Exit function
void my_char_device_exit(void)
{
	printk(KERN_INFO "%s: Device exiting\n",DEVICE_NAME);
	kfree(my_data.buffer);

	sysfs_remove_group(&device->kobj,&sysfs_group);
	proc_remove(my_data.proc_entry);
	device_destroy(my_char_device_class, MAJOR(dev));

	cdev_del(&my_data.my_cdev);
	unregister_chrdev_region(dev,1);
}

module_init(my_char_device_init);
module_exit(my_char_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexandr Gattsuk");
MODULE_DESCRIPTION("A simple character device driver");
