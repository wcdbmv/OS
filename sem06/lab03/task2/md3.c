#include <linux/init.h>
#include <linux/module.h>
#include "md.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kerimov A.");
MODULE_DESCRIPTION("sem03/lab03/task2/md3");

static int __init md3_init(void)
{
	printk(KERN_INFO "=== md3: INIT\n");

	printk(KERN_INFO "=== md3: md1_string is \"%s\"\n", md1_string);
	printk(KERN_INFO "=== md3: md1_number is %d\n", md1_number);

	printk(KERN_INFO "=== md3: 12321 is %s\n", md1_palindrome(12321));
	printk(KERN_INFO "=== md3: 12345 is %s\n", md1_palindrome(12345));
	printk(KERN_INFO "=== md3: reverse of 12321 is %d\n", md1_reverse(12321));
	printk(KERN_INFO "=== md3: reverse of 12345 is %d\n", md1_reverse(12345));

	return -1;
}

module_init(md3_init);
