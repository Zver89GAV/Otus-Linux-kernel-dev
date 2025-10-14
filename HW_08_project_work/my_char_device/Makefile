obj-m += my_char_device.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	rm -f *.o *.ko *.mod.c .*.cmd Module.symvers modules.order
