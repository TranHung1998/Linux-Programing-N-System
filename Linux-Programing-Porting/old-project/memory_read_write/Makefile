obj-m := prr_exercise.o
KERN_DIR = /data1/hungthetran/gz1/build/temp/sysroots/cortexa7hf-neon-vfpv4-poky-linux-gnueabi/usr/src/kernel/

all:
    gcc -o app app-memory-read-write.c
    make -C $(KERN_DIR) M=$(PWD) modules
clean:
    make -C $(KERN_DIR) M=$(PWD) clean
    rm -f app
help:
    make -C $(KERN_DIR) M=$(PWD) helps