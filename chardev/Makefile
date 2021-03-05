PWD := $(shell pwd)

obj-m += chardev.o

KERNEL=/STORAGE/naveen/testing-bundles/linux-imx

all:
	make -C $(KERNEL) M=$(PWD) modules
clean:
	make -C $(KERNEL) M=$(PWD) clean
