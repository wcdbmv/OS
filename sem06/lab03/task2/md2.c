#include <linux/init.h>
#include <linux/module.h>
#include "md.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kerimov A.");
MODULE_DESCRIPTION("sem03/lab03/task2/md2");

static int __init md2_init(void)
{
	printk(KERN_INFO "=== md2: INIT\n");

	printk(KERN_INFO "=== md2: md1_string is \"%s\"\n", md1_string);
	printk(KERN_INFO "=== md2: md1_number is %d\n", md1_number);

	printk(KERN_INFO "=== md2: 12321 is %s\n", md1_palindrome(12321));
	printk(KERN_INFO "=== md2: 12345 is %s\n", md1_palindrome(12345));
	printk(KERN_INFO "=== md2: reverse of 12321 is %d\n", md1_reverse(12321));
	printk(KERN_INFO "=== md2: reverse of 12345 is %d\n", md1_reverse(12345));

	return 0;
}

static void __exit md2_exit(void)
{
	printk(KERN_INFO "=== md2: EXIT\n");
}

module_init(md2_init);
module_exit(md2_exit);
