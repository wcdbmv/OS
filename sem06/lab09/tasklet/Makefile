obj-m += my_tasklet.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

all default: modules
install: modules_install

modules modules_install help clean:
	make -C $(KERNELDIR) M=$(shell pwd) $@
