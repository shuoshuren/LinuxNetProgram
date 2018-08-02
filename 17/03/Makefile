target = hello_world_module
obj-m:= $(target).o
KDIR :=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	insmod $(target).ko

uninstall:
	rmmod $(target).ko

clean:
	$(RM) *.ko *.o *.symvers 
	$(MAKE) -C $(KDIR) M=$(PWD) clean