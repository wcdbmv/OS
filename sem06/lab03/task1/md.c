#include <linux/init.h>
#include <linux/module.h>
#include <linux/init_task.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kerimov A.");
MODULE_DESCRIPTION("sem03/lab03/task1");

static int __init md_init(void)
{
	printk(KERN_INFO "=== md: INIT\n");

	struct task_struct *task = &init_task;
	do {
		printk(KERN_INFO "=== md: %s - %d, parent: %s - %d\n",
			task->comm, task->pid, task->parent->comm, task->parent->pid);
	} while ((task = next_task(task)) != &init_task);

	return 0;
}

static void __exit md_exit(void)
{
	printk(KERN_INFO "=== md: EXIT\n");
}

module_init(md_init);
module_exit(md_exit);
