#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>



#define SAMPLE_BUF_SIZE 1024
static char sample_buf[SAMPLE_BUF_SIZE];


ssize_t ads1015_read(struct file *file, char __user *user, size_t size, loff_t *off)
{
	unsigned long rc = 0;

	rc = copy_to_user(user, "Hello.\n", 7);
	if (rc > 0) {
		printk("copy_to_user failed to write %ld bytes\n", rc);
	}
	return 7;
}
EXPORT_SYMBOL(ads1015_read);


ssize_t ads1015_write(struct file *file, const char __user *user, size_t size, loff_t *off)
{
	unsigned long rc = 0;
	memset(sample_buf, 0x00, sizeof(sample_buf));
	if (size > SAMPLE_BUF_SIZE) {
		size = SAMPLE_BUF_SIZE;
	}
	rc = copy_from_user(sample_buf, user, size);
	if (rc > 0) {
		printk("copy_from_user failed to read %ld bytes\n", rc);
	}
	printk("you said '%s'\n", sample_buf);
	return size;
}
EXPORT_SYMBOL(ads1015_write);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("DDL");
MODULE_DESCRIPTION("Driver for the ADS1015 daughtercard by AdaFruit\n");
MODULE_VERSION("1.0");

