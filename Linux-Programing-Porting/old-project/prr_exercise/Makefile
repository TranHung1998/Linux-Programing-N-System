obj-m := prr_exercise.o
KERN_DIR = /data1/hungthetran/gz1/build/temp/sysroots/cortexa7hf-neon-vfpv4-poky-linux-gnueabi/usr/src/kernel/

all:
    make -C $(KERN_DIR) M=$(PWD) modules
clean:
    make -C $(KERN_DIR) M=$(PWD) clean
help:
    make -C $(KERN_DIR) M=$(PWD) helps