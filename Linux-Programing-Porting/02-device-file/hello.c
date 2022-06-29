#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h> 
#include <linux/device.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>

#define FIRSTMINOR          0
#define CNT                 1
#define PRR_PORT_BASE       0xFF000044
#define PRR_PORT_LEN        4
#define PRR_PORT_NAME       (uvcs-prr) 
#define DEFAULT_ZERO        0
#define DRIVER_CLASS_NAME   "iomodule_class"
#define DRIVER_FILE_NAME    "iomodule"

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

#define DATA_TRANSMIT   9
#define REG_MAX_SIZE    4
#define DEV_MAX_SIZE    12

struct _memory_dev
{
    unsigned long *input_addr;
    unsigned long *input_val;
    unsigned int output_val;
};

struct _iomodule_drv 
{
    dev_t dev_num;
    struct class* dev_class;
    struct device* dev_create;
    void __iomem* dev_ioaddr;
    struct cdev *cdev_addr;
    int count;
    struct _memory_dev *mem_addr;
} iomodule_drv;


static int iomodule_open (struct inode *inode, struct file *filp);
static int iomodule_close (struct inode *inode, struct file *filp);
static ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
static ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
static int iomodule_cpu_core (unsigned int prr_register);


struct _iomodule_prr
{
    unsigned int cpu_core;
} iomodule_prr;

static struct file_operations file = 
{
    .open = iomodule_open,
    .release = iomodule_close,
    .owner = THIS_MODULE,
    .read = pcd_read,
    .write = pcd_write,
};

static int iomodule_open (struct inode *inode, struct file *filp)
{
    //do something
    iomodule_drv.count ++;
    pr_info("You have open iomodule %d times\n", iomodule_drv.count);
    return 0;
}

static int iomodule_close (struct inode *inode, struct file *filp)
{
    //do something
    iomodule_drv.count --;
    pr_info("You have open iomodule %d times\n", iomodule_drv.count);
    return 0;
}

static ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    char *kernel_buff = NULL;
    kernel_buff = kzalloc(count,GFP_KERNEL);
    if(!kernel_buff)
    {
        return -ENOMEM;
    }
    //pr_info("Read request of %zu bytes.\n Current file position: %lld\n",count,*f_pos);

    /* adjust the count */
    if((*f_pos + count) > DATA_TRANSMIT)
    {
        count = DATA_TRANSMIT - *f_pos;
    }

    if(!count)
    {
        return -ENOMEM;
    }
    
    /* copy data from user_buf to kernel_buf*/
    if (!*(iomodule_drv.mem_addr->input_addr) )
    {
        *(iomodule_drv.mem_addr->input_addr) = PRR_PORT_BASE;
    }
    /* get Hardware data */
    pr_info ("input_addr: %X\n", *(iomodule_drv.mem_addr->input_addr));
    /* yeu cau kernel cho phep su dung port region*/
    iomodule_drv.dev_ioaddr = ioremap(*(iomodule_drv.mem_addr->input_addr), PRR_PORT_LEN);
    iomodule_drv.mem_addr->output_val = ioread32(iomodule_drv.dev_ioaddr);
    //iomodule_cpu_core(iomodule_drv.mem_addr->output_val);
    //pr_info ("output_val: %x\n", iomodule_drv.mem_addr->output_val);

    /* copy data to kernel_buf (data la mot so)*/
    int len = sprintf(kernel_buff,"%u",iomodule_drv.mem_addr->output_val);
    kernel_buff[len+1] = '0';
    //pr_info ("Kernel_buff: %s\n", kernel_buff);
    
    /* copy to user */
    if(copy_to_user(buff,kernel_buff,count))
    {
        return -EFAULT;
    }

    /* update current file position */
    *f_pos = count;

    //pr_info("Number of bytes successfully read = %zu\n",count);
    //pr_info("Updated file position = %lld\n",*f_pos);

    /* Free memory allocate for kernel buff */
    kfree(kernel_buff);
    return 0;
}

static ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    char *kernel_buff = NULL;
    kernel_buff = kzalloc(count,GFP_KERNEL);
    int ret = DEFAULT_ZERO;
    if(!kernel_buff)
    {
        return -ENOMEM;
    }
    //pr_info("Write request of %zu bytes. Current file position: %lld\n",count,*f_pos);

    /* adjust the count */
    if((*f_pos + count) > DATA_TRANSMIT)
    {
        count = DATA_TRANSMIT - *f_pos;
    }

    if(!count)
    {
        kfree(kernel_buff);
        return -ENOMEM;
    }

    /* get Hardware data */
    /* copy to user */
    if( copy_from_user(kernel_buff,buff,count) )
    {
        kfree(kernel_buff);
        return -EFAULT;
    }

    ret = kstrtoul(kernel_buff, 16, iomodule_drv.mem_addr->input_addr);
    pr_info ("input_addr: %X\n", *(iomodule_drv.mem_addr->input_addr));

    if(ret < 0){
        pr_info("Failed to cover string to unsigned long. Fault: %d\n",ret);
        return -ret;
    }

    /* yeu cau kernel cho phep su dung port region*/
    iomodule_drv.dev_ioaddr = ioremap(*(iomodule_drv.mem_addr->input_addr), PRR_PORT_LEN);
    if(!iomodule_drv.dev_ioaddr)
    {
        pr_info("Failed to remap memory\n");
        kfree(kernel_buff);
        return -EFAULT;
    }

    //(ioread32(iomodule_drv.dev_ioaddr));
    iomodule_drv.mem_addr->output_val = ioread32(iomodule_drv.dev_ioaddr);
    //pr_info ("output_val: %u\n", iomodule_drv.mem_addr->output_val);

    /* update current file position */
    *f_pos = count;

    //pr_info("Number of bytes successfully read = %zu\n",count);
    //pr_info("Updated file position = %lld\n",*f_pos);

    /* Free memory allocate for kernel buff */
    kfree(kernel_buff);

    return 0;
}

static int iomodule_cpu_core (unsigned int prr_register)
{
    iomodule_prr.cpu_core = prr_register & (1 << 26);
    pr_info("PRR value: %x\n",prr_register);
    if (iomodule_prr.cpu_core)
    {
        pr_info("The product does not have two Cortex-A7 CPUs.\n");
    }
    else 
    {
        pr_info("The product has two Cortex-A7 CPUs.\n");
    }
    return 0;
}

static int __init read_write_IOmodule_init(void)
{
    int ret = DEFAULT_ZERO;
    /* Cap phat Device number */
    iomodule_drv.dev_num = DEFAULT_ZERO;
    ret = alloc_chrdev_region(&iomodule_drv.dev_num, FIRSTMINOR, CNT, "IOmodule device number");
    if (ret < 0)
    {
        pr_info("Failed registed device number dynamically\n");
        goto failed_registed_devnum;
    }
    pr_info("Device Number: (%d,%d)\n", MAJOR(iomodule_drv.dev_num), MINOR(iomodule_drv.dev_num));
    
    /* Tao Device file */
    iomodule_drv.dev_class = class_create(THIS_MODULE, DRIVER_CLASS_NAME);
    if(iomodule_drv.dev_class == NULL)
    {
        pr_info("Failed create class\n");
        goto failed_registed_devclass;
    }
    iomodule_drv.dev_create = device_create(iomodule_drv.dev_class, NULL, iomodule_drv.dev_num, NULL, DRIVER_FILE_NAME);
    if (IS_ERR(iomodule_drv.dev_create))
    {
        pr_info("Failed create device");
        goto failed_registed_device;
    }

    /* Khoi tao Character Device (cdev) */
    /* Cap phat bo nho cho cdev do cdev la bien struct "dac biet" nen can dung ham nay khoi tao */
    iomodule_drv.cdev_addr = cdev_alloc();
    /* Khoi tao cac truong cho cau truc cdev (cdev->ops = fops)*/
    cdev_init(iomodule_drv.cdev_addr, &file);
    /* Dang ky cdev voi Linux Kernel*/
    ret = cdev_add(iomodule_drv.cdev_addr, iomodule_drv.dev_num, 1);
    if (ret<0)
    {
        pr_info("Failed Registed cdev device");
        goto failed_registed_cdev;
    }

    /* Khoi tao bo nho cho memory */
    iomodule_drv.mem_addr = NULL;
    iomodule_drv.mem_addr = kzalloc(sizeof(struct _memory_dev),GFP_KERNEL);
    if(!iomodule_drv.mem_addr)
    {
        pr_info("Failed to allocate memory struct _memory_dev\n");
        ret = -ENOMEM;
        goto failed_allocate_struct_memory;
    }

    /* Phan chia dia chi o nho memory */
    iomodule_drv.mem_addr->input_addr = kzalloc(DEV_MAX_SIZE,GFP_KERNEL);
    if(!iomodule_drv.mem_addr->input_addr)
    {
        pr_info("Failed to allocate memory for device\n");
        ret = -ENOMEM;
        goto failed_allocate_memory;
    }
    iomodule_drv.mem_addr->input_val  = iomodule_drv.mem_addr->input_addr + REG_MAX_SIZE;

    pr_info("Struct addr: %p\n", iomodule_drv.mem_addr);
    pr_info("Memory addr: %p\n", iomodule_drv.mem_addr->input_addr);
    /* yeu cau kernel cho phep su dung port region*/
    iomodule_drv.dev_ioaddr = ioremap(PRR_PORT_BASE, PRR_PORT_LEN);
    iomodule_cpu_core (ioread32(iomodule_drv.dev_ioaddr));
    
    return 0;

/* Huy khi goi ham that bai */
failed_allocate_memory:
    kfree(iomodule_drv.mem_addr);
failed_allocate_struct_memory:
    cdev_del(iomodule_drv.cdev_addr);
failed_registed_cdev:
    device_destroy(iomodule_drv.dev_class, iomodule_drv.dev_num);
failed_registed_device:
    class_destroy(iomodule_drv.dev_class);
failed_registed_devclass:
    unregister_chrdev_region(iomodule_drv.dev_num, 1);
failed_registed_devnum:
    return ret;
}

static void __exit read_write_IOmodule_cleanup(void)
{
    /* Giai phong PRR */
    iounmap(iomodule_drv.dev_ioaddr);
    //release_region(PRR_PORT_BASE, PRR_PORT_LEN);

    /* Giai phong bo nho memory */
    kfree(iomodule_drv.mem_addr->input_addr);
    kfree(iomodule_drv.mem_addr);
    /* Xoa bo nho cdev da cap phat */
    cdev_del(iomodule_drv.cdev_addr);

    /* Xoa Device file */
    device_destroy(iomodule_drv.dev_class, iomodule_drv.dev_num);
    class_destroy(iomodule_drv.dev_class);

    /* Giai phong Device Number */
    unregister_chrdev_region(iomodule_drv.dev_num, 1);

    pr_info("Exit iomodule driver\n");
}

module_init(read_write_IOmodule_init);
module_exit(read_write_IOmodule_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HUNGTHETRAN");
MODULE_DESCRIPTION("Read and write to memory region on G1E");
MODULE_INFO(G1E,"iW-RainboW-G20M");