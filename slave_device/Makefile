obj-m += slave_device.o

all:
	cp ../ksocket/Module.symvers ./Module.symvers
	make -j8 -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
