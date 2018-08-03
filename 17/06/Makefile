target = nf_sockopte
obj-m:= $(target).o
KDIR :=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	insmod $(target).ko

uninstall:
	rmmod $(target).ko

test:
	$(CC) -o test.out test.c

clean:
	$(RM) *.ko *.o *.symvers *.out
	$(MAKE) -C $(KDIR) M=$(PWD) clean