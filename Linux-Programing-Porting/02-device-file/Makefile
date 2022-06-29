# Hello world kernel module

EXTRA_CFLAGS = -Wall
obj-m = hello.o

KDIR = /lib/modules/`uname -r`/build

all:
	gcc -o app app-memory-read-write.c
	make -C $(KDIR) M=`pwd` modules

clean:
	make -C $(KDIR) M=`pwd` clean
	rm -f app
