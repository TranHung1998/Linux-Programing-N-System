#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/module.h>       /* Thu vien nay dinh nghia cac macro nhu module_init/module_exit */
#include <linux/fs.h>           /* Thu vien nay dinh nghia cac ham allocate major/minor number */
#include <linux/device.h>       /* Thu vien nay dinh nghia cac ham class_create/device_create */
#include <linux/cdev.h>         /* Thu vien nay dinh nghia cac ham cdev_init/cdev_add */
#include <linux/interrupt.h>	/* For IRQ */
#include <linux/gpio.h> 		/* For Legacy integer based GPIO */
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#include "button-led.h"

#define DRIVER_AUTHOR "hungthetran tranthehung1406@gmail.com"
#define DRIVER_DESC   "Led blinking"
#define DRIVER_VERS   "1.0"
#define DRIVER_CLASS_NAME   "iomodule_class"
#define DRIVER_FILE_NAME    "iomodule"
#define FIRSTMINOR          0
#define CNT                 1

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

struct _iomodule_drv 
{
    dev_t dev_num;
    struct class* dev_class;
    struct device* dev_create;
    uint32_t __iomem *base_addr;
    struct cdev *cdev_addr;
    int count;
} iomodule_drv;

volatile int32_t state;
static unsigned int irq;

static irqreturn_t btn_pushed_irq_handler(int irq, void *dev_idv)
{
	if (state == 0) {		
		*(iomodule_drv.base_addr + GPIO_SETDATAOUT_OFFSET / 4) |= LED_POS;
		state = 1;
	} else {
		*(iomodule_drv.base_addr + GPIO_CLEARDATAOUT_OFFSET / 4) |= LED_POS;
		state = 0;
	}
	pr_info("BTN interrupt - LED state is: %d\n", state);
	return IRQ_HANDLED;
}


static int iomodule_open (struct inode *inode, struct file *filp);
static int iomodule_close (struct inode *inode, struct file *filp);
static ssize_t iomodule_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
static ssize_t iomodule_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);

static struct file_operations file = 
{
    .open = iomodule_open,
    .release = iomodule_close,
    .owner = THIS_MODULE,
    .read = iomodule_read,
    .write = iomodule_write,
};


static int iomodule_open (struct inode *inode, struct file *filp)
{
    //do something
    iomodule_drv.count ++;
    pr_info("You have opened iomodule %d times\n", iomodule_drv.count);
    return 0;
}

static int iomodule_close (struct inode *inode, struct file *filp)
{
    //do something
    iomodule_drv.count --;
    pr_info("You have closed iomodule\n");
    return 0;
}

static ssize_t iomodule_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    char *kernel_buff = NULL;
    kernel_buff = kzalloc(count,GFP_KERNEL);
    if(!kernel_buff)
    {
        return -ENOMEM;
    }
        if(!count)
    {
        return -ENOMEM;
    }

    int len = sprintf(kernel_buff,"%X + %X + %X ",*(iomodule_drv.base_addr + GPIO_OE_OFFSET / 4),*(iomodule_drv.base_addr + DEBOUNCEENABLE_OFFSET / 4),*(iomodule_drv.base_addr + GPIO_DATAIN_OFFSET / 4));

    if(copy_to_user(buff,kernel_buff,count))
    {
        return -EFAULT;
    }

    kfree(kernel_buff);

    return 0;
}

static ssize_t iomodule_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    char *kernel_buff = NULL;
    kernel_buff = kzalloc(count,GFP_KERNEL);
    int led_state = 0, ret = 0;

    if(!kernel_buff)
    {
        return -ENOMEM;
    }

    if(!count)
    {
        kfree(kernel_buff);
        return -ENOMEM;
    }

    if( copy_from_user(kernel_buff,buff,count) )
    {
        kfree(kernel_buff);
        return -EFAULT;
    }

    ret = kstrtoint(kernel_buff, 10, &led_state);

    if(ret < 0){
        pr_info("Failed to cover string to unsigned long. Fault: %d\n",ret);
        return -ret;
    }

    if( led_state ){
        *(iomodule_drv.base_addr + GPIO_SETDATAOUT_OFFSET / 4) |= LED_POS;
    } else {
        *(iomodule_drv.base_addr + GPIO_CLEARDATAOUT_OFFSET / 4) |= LED_POS; 
    }

    kfree(kernel_buff);

    return 0;
}


static int __init read_write_IOmodule_init(void)
{
    int ret = 0;
    /* Cap phat Device number */
    iomodule_drv.dev_num = 0;
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

    iomodule_drv.base_addr = ioremap(GPIO_0_ADDR_BASE, GPIO_0_ADDR_SIZE);
	if(!iomodule_drv.base_addr){
        ret = -ENOMEM;
        pr_info("Failed ioremap");
        goto failed_remap_addr;
    }
		
    
	*(iomodule_drv.base_addr + GPIO_OE_OFFSET / 4) &= ~LED_POS;
    *(iomodule_drv.base_addr + GPIO_CLEARDATAOUT_OFFSET / 4) |= LED_POS; 

    // Config BTN as input mode 
	*(iomodule_drv.base_addr + GPIO_OE_OFFSET / 4) |= (1 << BTN_PIN);

    // Cannot setup debouncing like this
    //*(iomodule_drv.base_addr + DEBOUNCEENABLE_OFFSET / 4) |= (1 << BTN_PIN);
	//*(iomodule_drv.base_addr + GPIO_DEBOUNCINGTIME_OFFSET / 4) |= DEBOUNCING_VALUE;
    // Use kernel debouce setup instead
    gpio_set_debounce(BTN_PIN,150);

    uint8_t retval;
	irq = gpio_to_irq(BTN_PIN);
	retval = request_irq(irq, (irq_handler_t)btn_pushed_irq_handler, IRQF_TRIGGER_RISING, "iomodule", NULL);
        if ( retval < 0 )
        {
            pr_info("Failed request_irq");
            goto failed_gpio_to_irq;
        }
    
	return 0;

/* Huy khi goi ham that bai */
failed_gpio_to_irq:
    free_irq(gpio_to_irq(BTN_PIN), NULL);
failed_remap_addr:
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
    *(iomodule_drv.base_addr + GPIO_CLEARDATAOUT_OFFSET / 4) |= LED_POS; 
    free_irq(irq, NULL);

    /* Giai phong mapped memory */
    iounmap(iomodule_drv.base_addr);

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
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_VERSION(DRIVER_VERS); 