obj-m:=multimmap.o
#KDIR := /lib/modules/$(shell uname -r)/build
KDIR := /usr/src/linux-headers-2.6.26-2-amd64
PWD := $(shell pwd)

all:
        $(MAKE) -C $(KDIR) M=$(PWD) modules
