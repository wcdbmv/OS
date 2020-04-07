obj-m += md1.o md2.o md3.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

all default: modules
install: modules_install

modules modules_install help clean:
	make -C $(KERNELDIR) M=$(shell pwd) $@

# lsmod
# sudo insmod md.ko
# lsmod | grep md
# dmesg | tail -350
# modinfo md.ko
# sudo rmmod md
