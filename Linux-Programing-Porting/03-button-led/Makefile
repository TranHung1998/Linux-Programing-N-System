BBB_KERNEL := /home/hung/Desktop/Working_Space/beaglebone-black/kernelbuildscripts/KERNEL/
KDIR = /lib/modules/`uname -r`/build

TOOLCHAIN := /home/hung/Desktop/Working_Space/beaglebone-black/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin
TOOLCHAIN2 := /home/hung/Desktop/Working_Space/beaglebone-black/kernelbuildscripts/dl/gcc-10.3.0-nolibc/arm-linux-gnueabi/bin

PREFIX := arm-linux-gnueabi-
CC := $(TOOLCHAIN)/arm-linux-gnueabihf-gcc
LD := ld

all :
	$(CC) --version
	make ARCH=arm CROSS_COMPILE=$(TOOLCHAIN2)/$(PREFIX) -C $(BBB_KERNEL) M=$(shell pwd) modules
	$(CC) -o app app-memory-read-write.c
clean :
	make -C $(BBB_KERNEL) M=$(shell pwd) clean
	rm -f app