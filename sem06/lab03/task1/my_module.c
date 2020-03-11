#include <linux/init.h>         // __init, __exit
#include <linux/kernel.h>       // printk
#include <linux/module.h>       // module_init, module_exit
#include <linux/sched.h>        // struct task_struct
#include <linux/sched/signal.h> // next_task
#include <linux/sched/task.h>   // init_task

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("sem03/lab03");
MODULE_AUTHOR("Kerimov A.");

static int __init my_module_init(void)
{
	printk(KERN_INFO "MY MODULE: INIT\n");

	struct task_struct *task = &init_task;
	do {
		printk(KERN_INFO "MY MODULE: s%s - %d, parent: %s - %d\n",
			task->comm, task->pid, task->parent->comm, task->parent->pid);
	} while ((task = next_task(task)) != &init_task);
	return 0;
}

static void __exit my_module_exit(void)
{
	printk(KERN_INFO "MY MODULE: EXIT\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
