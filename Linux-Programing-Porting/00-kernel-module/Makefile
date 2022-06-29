# Hello world kernel module

EXTRA_CFLAGS = -Wall
obj-m = hello.o

BBB_KERNEL := /home/hung/Desktop/Working_Space/beaglebone-black/kernelbuildscripts/KERNEL/
KDIR = /lib/modules/`uname -r`/build

TOOLCHAIN := /home/hung/Desktop/Working_Space/beaglebone-black/gcc-11.1.0-nolibc/arm-linux-gnueabi/bin

PREFIX := arm-linux-gnueabi-
CC := gcc
LD := ld

all :
	make ARCH=arm CROSS_COMPILE=$(TOOLCHAIN)/$(PREFIX) -C $(BBB_KERNEL) M=$(shell pwd) modules

clean:
	make -C $(BBB_KERNEL) M=`pwd` clean
