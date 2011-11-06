obj-m:=multimmap.o
KDIR := /usr/src/linux-headers-$(shell uname -r)
PWD := $(shell pwd)

all:
        $(MAKE) -C $(KDIR) M=$(PWD) modules
