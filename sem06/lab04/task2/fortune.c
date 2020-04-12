#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kerimov A. IU7-64b");
MODULE_DESCRIPTION("LKM fortune");

#define MAX_COOKIE_LENGTH PAGE_SIZE

#define FORTUNE_DIRNAME  "fortune_dir"
#define FORTUNE_FILENAME "fortune"
#define FORTUNE_SYMLINK  "fortune"
#define FORTUNE_FILEPATH FORTUNE_DIRNAME "/" FORTUNE_FILENAME

static struct proc_dir_entry *fortune_dir = NULL;
static struct proc_dir_entry *fortune_file = NULL;
static struct proc_dir_entry *fortune_symlink = NULL;

static char *cookie_pot;  // Хранилище наших фортунок
static int cookie_index;  // Индекс для добавления печеньки
static int next_fortune;  // Индекс для чтения печеньки

ssize_t fortune_read(struct file *filep, char __user *buf, size_t count, loff_t *offp)
{
	int len;

	if (*offp > 0 || next_fortune == 0) {
		return 0;
	}

	if (next_fortune >= cookie_index) {
		next_fortune = 0;
	}

	len = strlen(&cookie_pot[next_fortune]);
	if (copy_to_user(buf, &cookie_pot[next_fortune], len)) {
		printk(KERN_ERR "=== fortune: copy_to_user error\n");
		return -EFAULT;
	}
	next_fortune += len;

	return len;
}

ssize_t fortune_write(struct file *file, const char __user *buf, size_t len, loff_t *offp)
{
	if (len > MAX_COOKIE_LENGTH - cookie_index + 1) {
		printk(KERN_ERR "=== fortune: cookie_pot overflow\n");
		return -ENOSPC;
	}

	if (copy_from_user(&cookie_pot[cookie_index], buf, len)) {
		printk(KERN_ERR "=== fortune: copy_to_user error\n");
		return -EFAULT;
	}

	cookie_index += len;
	cookie_pot[cookie_index - 1] = '\0';

	return len;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = fortune_read,
	.write= fortune_write
};

static int __init fortune_init(void)
{
	if (!(cookie_pot = vmalloc(MAX_COOKIE_LENGTH))) {
		printk(KERN_ERR "=== fortune: vmalloc error\n");
		return -ENOMEM;
	}
	memset(cookie_pot, 0, MAX_COOKIE_LENGTH);

	if (!(fortune_dir = proc_mkdir(FORTUNE_DIRNAME, NULL))) {
		vfree(cookie_pot);
		printk(KERN_ERR "=== fortune: proc_create error\n");
		return -ENOMEM;
	}

	if (!(fortune_file = proc_create(FORTUNE_FILENAME, 0666, fortune_dir, &fops))) {
		remove_proc_entry(FORTUNE_DIRNAME, NULL);
		vfree(cookie_pot);
		printk(KERN_ERR "=== fortune: proc_create error\n");
		return -ENOMEM;
	}

	if (!(fortune_symlink = proc_symlink(FORTUNE_SYMLINK, NULL, FORTUNE_FILEPATH))) {
		remove_proc_entry(FORTUNE_FILENAME, NULL);
		remove_proc_entry(FORTUNE_DIRNAME, NULL);
		vfree(cookie_pot);
		printk(KERN_ERR "=== fortune: proc_symlink error\n");
		return -ENOMEM;
	}

	cookie_index = 0;
	next_fortune = 0;

	printk(KERN_INFO "=== fortune: module loaded\n");
	return 0;
}

static void __exit fortune_exit(void)
{
	remove_proc_entry(FORTUNE_SYMLINK, NULL);
	remove_proc_entry(FORTUNE_FILENAME, NULL);
	remove_proc_entry(FORTUNE_DIRNAME, NULL);
	vfree(cookie_pot);
	printk(KERN_INFO "=== fortune: module unloaded\n");
}

module_init(fortune_init)
module_exit(fortune_exit)
