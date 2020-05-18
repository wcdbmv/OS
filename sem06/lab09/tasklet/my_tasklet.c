#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kerimov A. IU7-64b");
MODULE_DESCRIPTION("tasklet");

#define IRQ 1 /* keyboard irq */
static int dev_id;

char my_tasklet_data[] = "KEYBOARD INTERRUPT";

void my_tasklet_function(unsigned long data)
{
	int scancode = inb(0x60);
	if (scancode < 103) {
		printk(KERN_INFO "my_tasklet: %s: scancode: %d\n", (char *)data, scancode);
	}
}

DECLARE_TASKLET(my_tasklet, my_tasklet_function, (unsigned long) &my_tasklet_data);

irqreturn_t my_irq_handler(int irq, void *dev)
{
	if (irq == IRQ) {
		tasklet_schedule(&my_tasklet);
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

static int __init my_tasklet_init(void)
{
	int ret = request_irq(IRQ, my_irq_handler, IRQF_SHARED, "my_irq_handler", &dev_id);
	if (ret) {
		printk(KERN_ERR "my_tasklet: my_irq_handler wasn't register\n");
	} else {
		printk(KERN_INFO "my_tasklet: module loaded\n");
	}
	return ret;
}

static void __exit my_tasklet_exit(void)
{
	tasklet_kill(&my_tasklet);
	free_irq(IRQ, &dev_id);
	printk(KERN_DEBUG "my_tasklet: module unloaded\n");
}

module_init(my_tasklet_init)
module_exit(my_tasklet_exit)
