#include <linux/init.h>
#include <linux/module.h>
#include "md.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kerimov A.");
MODULE_DESCRIPTION("sem03/lab03/task2/md1");

static int __init md1_init(void)
{
	printk(KERN_INFO "=== md1: INIT\n");
	return 0;
}

static void __exit md1_exit(void)
{
	printk(KERN_INFO "=== md1: EXIT\n");
}

module_init(md1_init);
module_exit(md1_exit);


char *md1_string = "Hello, World!";
int   md1_number = 1024;

EXPORT_SYMBOL(md1_string);
EXPORT_SYMBOL(md1_number);

extern char *md1_palindrome(int n)
{
	printk(KERN_INFO "=== md1: PALINDROME");

	return n == md1_reverse(n) ? "palindrome" : "not palindrome";
}

extern int md1_reverse(int n)
{
	int r, t;
	printk(KERN_INFO "=== md1: REVERSE");

	r = 0;
	for (t = n; t; t /= 10) {
		r = 10 * r + t % 10;
	}

	return r;
}

EXPORT_SYMBOL(md1_palindrome);
EXPORT_SYMBOL(md1_reverse);
