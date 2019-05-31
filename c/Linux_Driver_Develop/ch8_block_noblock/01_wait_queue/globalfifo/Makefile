ifneq ($(KERNELRELEASE),)
# obj-m := driver.o
obj-m := driver_concurrent.o
else
KDIR := /lib/modules/$(shell uname -r)/build
all:
	make -C $(KDIR) M=$(PWD) modules 
clean:
	@rm -rf *.ko *.o *.mod.o *.mod.c *.symvers .driver* modules.order .cache* .tmp*
endif
