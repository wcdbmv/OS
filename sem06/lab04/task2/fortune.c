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
#define FORTUNE_FILENAME "fortune_file"
#define FORTUNE_SYMLINK  "fortune"
#define FORTUNE_FILEPATH FORTUNE_DIRNAME "/" FORTUNE_FILENAME

static struct proc_dir_entry *fortune_dir = NULL;
static struct proc_dir_entry *fortune_file = NULL;
static struct proc_dir_entry *fortune_symlink = NULL;

static char *cookie_pot;  // Хранилище наших фортунок
static int cookie_index;  // Индекс для добавления печеньки
static int next_fortune;  // Индекс для чтения печеньки

static char tmp[MAX_COOKIE_LENGTH];

ssize_t fortune_read(struct file *filep, char __user *buf, size_t count, loff_t *offp)
{
	int len;

	if (*offp > 0 || cookie_index == 0)
		return 0;

	if (next_fortune >= cookie_index)
		next_fortune = 0;

	len = snprintf(tmp, MAX_COOKIE_LENGTH, "%s\n", &cookie_pot[next_fortune]);
	if (copy_to_user(buf, tmp, len)) {
		printk(KERN_ERR "=== fortune: copy_to_user error\n");
		return -EFAULT;
	}
	next_fortune += len;
	*offp += len;

	return len;
}

ssize_t fortune_write(struct file *file, const char __user *buf, size_t len, loff_t *offp)
{
	if (len > MAX_COOKIE_LENGTH - cookie_index + 1) {
		printk(KERN_ERR "=== fortune: cookie_pot overflow error\n");
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
	owner: THIS_MODULE,
	read:  fortune_read,
	write: fortune_write
};

static void cleanup(void)
{
	if (fortune_symlink) remove_proc_entry(FORTUNE_SYMLINK, NULL);
	if (fortune_file)    remove_proc_entry(FORTUNE_FILENAME, fortune_dir);
	if (fortune_dir)     remove_proc_entry(FORTUNE_DIRNAME, NULL);
	if (cookie_pot)      vfree(cookie_pot);
}

static int shutdown_enomem(const char *s)
{
	cleanup();
	printk(KERN_ERR "=== fortune: %s error\n", s);
	return -ENOMEM;
}

static int __init fortune_init(void)
{
	if (!(cookie_pot = vmalloc(MAX_COOKIE_LENGTH)))
		return shutdown_enomem("vmalloc");
	memset(cookie_pot, 0, MAX_COOKIE_LENGTH);

	if (!(fortune_dir = proc_mkdir(FORTUNE_DIRNAME, NULL)))
		return shutdown_enomem("proc_create");
	if (!(fortune_file = proc_create(FORTUNE_FILENAME, 0666, fortune_dir, &fops)))
		return shutdown_enomem("proc_create");
	if (!(fortune_symlink = proc_symlink(FORTUNE_SYMLINK, NULL, FORTUNE_FILEPATH)))
		return shutdown_enomem("proc_symlink");

	cookie_index = 0;
	next_fortune = 0;

	printk(KERN_INFO "=== fortune: module loaded\n");
	return 0;
}

static void __exit fortune_exit(void)
{
	cleanup();
	printk(KERN_INFO "=== fortune: module unloaded\n");
}

module_init(fortune_init)
module_exit(fortune_exit)
