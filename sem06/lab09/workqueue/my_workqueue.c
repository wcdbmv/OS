#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kerimov A. IU7-64b");
MODULE_DESCRIPTION("workqueue");

#define IRQ 1 /* keyboard interrupt */
static int dev_id;

static struct workqueue_struct *my_wq;

void my_workqueue_function(struct work_struct *work)
{
	const int scancode = inb(0x60);
	if (scancode < 103) {
		printk(KERN_INFO "my_workqueue: KEYBOARD INTERRUPT: scancode: %d\n", scancode);
	}
}

DECLARE_WORK(workname, my_workqueue_function);

irqreturn_t my_irq_handler(int irq, void *dev)
{
	if (irq == IRQ) {
		queue_work(my_wq, &workname);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

static int __init my_workqueue_init(void)
{
	int ret = request_irq(IRQ, my_irq_handler, IRQF_SHARED, "my_irq_handler", &dev_id);
	if (ret) {
		printk(KERN_ERR "my_tasklet: my_irq_handler wasn't registered\n");
		return ret;
	}

	if (!(my_wq = create_workqueue("my_queue"))) {
		free_irq(IRQ, &dev_id);
		printk(KERN_INFO "my_workqueue: workqueue wasn't created");
		return -ENOMEM;
	}

	printk(KERN_INFO "my_workqueue: module loaded\n");
	return 0;
}

static void __exit my_workqueue_exit(void)
{
	flush_workqueue(my_wq);
	destroy_workqueue(my_wq);
	free_irq(IRQ, &dev_id);
	printk(KERN_INFO "my_workqueue: module unloaded\n");
}

module_init(my_workqueue_init)
module_exit(my_workqueue_exit)
