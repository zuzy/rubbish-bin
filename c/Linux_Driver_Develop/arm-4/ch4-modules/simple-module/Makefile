KVERS = $(shell uname -r)

obj-m += hello.o
modulename-objs := hello.c

EXTRA_CFLAGS = -g -O0

build: kernel_modules
kernel_modules:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules
clean:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean