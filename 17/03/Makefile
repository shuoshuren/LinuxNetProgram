obj-m:=hello_world_module.o
KDIR :=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(RM) *.ko *.o *.symvers 
	$(MAKE) -C $(KDIR) M=$(PWD) clean