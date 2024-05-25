#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

static int __init hello_world_init(void)
{
	printk("hello world from my first driver.\n");
	return 0;
}

static void __exit hello_world_exit(void)
{
	printk("exiting my first driver now.\n");
	return;
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DDL");
MODULE_DESCRIPTION("a simple hello world driver for RASPI");
MODULE_VERSION("1.0");


