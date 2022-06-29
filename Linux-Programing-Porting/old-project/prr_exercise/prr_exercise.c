#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h> 
#include <linux/device.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/of_device.h>

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

static int iomodule_open (struct inode *inode, struct file *filp);
static int iomodule_close (struct inode *inode, struct file *filp);
static ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
static ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
static int iomodule_cpu_core (unsigned int prr_register);
static struct _iomodule_platform_private_data *IOmodule_get_property_data (struct device *dev_addr);
static int platform_driver_probe_IOmodule(struct platform_device *pdev);
static int platform_driver_remove_IOmodule(struct platform_device *pdev);


struct _iomodule_drv_private_data 
{
    dev_t dev_num;
    struct class* dev_class;
    int count;
} iomodule_drv;

struct _iomodule_dev_private_data
{
     
    struct _iomodule_platform_private_data *platform_private;
    void __iomem* dev_ioaddr;
    struct device* dev_create;
    struct cdev *cdev_addr;
};

struct _iomodule_platform_private_data
{
    long int prr_address;
    
};

struct _iomodule_prr
{
    unsigned long cpu_core;
} iomodule_prr;

const struct of_device_id match_table[] = 
{
    {.compatible = "IOmodule1"},
    {.compatible = "IOmodule2"}
};



static struct file_operations file = 
{
    .open = iomodule_open,
    .release = iomodule_close,
    .owner = THIS_MODULE,
    .read = pcd_read,
    .write = pcd_write,
};
static struct platform_driver driver =
{
    .probe = platform_driver_probe_IOmodule,
    .remove = platform_driver_remove_IOmodule,
    .driver = {
        .of_match_table = match_table
    }
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

    return 0;
}

static ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{

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

static struct _iomodule_platform_private_data *IOmodule_get_property_data (struct device *dev_addr)
{
    int ret;
    struct device_node *device_node_addr = dev_addr->of_node;
    struct _iomodule_platform_private_data *plat_private_data;
    plat_private_data = kzalloc(sizeof(&plat_private_data),GFP_KERNEL);
    if (!plat_private_data)
    {
        pr_info("Cannot allocate memory");
        return ERR_PTR(-ENOMEM);
    }

    ret = of_property_read_u32(device_node_addr,"rvc,prr_addr",&plat_private_data->prr_address);
    if(ret < 0)
    {
        pr_info("No device tree property");
        return ERR_PTR(-EINVAL);
    }
    return plat_private_data;
}

static int platform_driver_probe_IOmodule(struct platform_device *pdev)
{
    pr_info("A device is detected");
    int ret;
    struct _iomodule_dev_private_data *dev_private_data;
    struct _iomodule_platform_private_data *plat_private_data;
    struct of_device_id *match; 
    struct device *dev_addr = &pdev->dev;

    /*Lay thong tin data tu device tree*/
    /*Kiem tra device tree da co hay ko*/   
    match = of_match_device(of_match_ptr(match_table),dev_addr);
    if (match)
    {
        pr_info("Device tree detected");
        plat_private_data = IOmodule_get_property_data(dev_addr);
        if(IS_ERR(plat_private_data))
        {
            return PTR_ERR(plat_private_data);
        }   
    }else
    {
        pr_info("No matching platform available\n");
    }

    dev_private_data = kzalloc(sizeof(&dev_private_data),GFP_KERNEL);
    if (!dev_private_data)
    {
        pr_info("Cannot allocate memory");
        return -ENOMEM;
    }

    /*get the prr address*/
    dev_private_data->platform_private->prr_address = plat_private_data->prr_address;
    pr_info("PRR address = %ld\n",plat_private_data->prr_address);

    dev_private_data->dev_create = device_create(iomodule_drv.dev_class, NULL, iomodule_drv.dev_num, NULL, DRIVER_FILE_NAME);
    if (IS_ERR(dev_private_data->dev_create))
    {
        pr_info("Failed create device");
        goto failed_registed_device;
    }

    /* Khoi tao Character Device (cdev) */
    /* Cap phat bo nho cho cdev do cdev la bien struct "dac biet" nen can dung ham nay khoi tao */
    dev_private_data->cdev_addr = cdev_alloc();
    /* Khoi tao cac truong cho cau truc cdev (cdev->ops = fops)*/
    cdev_init(dev_private_data->cdev_addr, &file);
    /* Dang ky cdev voi Linux Kernel*/
    ret = cdev_add(dev_private_data->cdev_addr, iomodule_drv.dev_num, 1);
    if (ret<0)
    {
        pr_info("Failed Registed cdev device");
        goto failed_registed_cdev;
    }

    return 0;
/* Huy khi goi ham that bai */
failed_registed_cdev:
    device_destroy(iomodule_drv.dev_class, iomodule_drv.dev_num);
failed_registed_device:
    class_destroy(iomodule_drv.dev_class);
} 

static int platform_driver_remove_IOmodule(struct platform_device *pdev)
{
    struct _iomodule_dev_private_data *dev_private_data = dev_get_drvdata(&pdev->dev);
    
    /* Giai phong PRR */
    iounmap(dev_private_data->dev_ioaddr);

    /* Xoa bo nho cdev da cap phat */
    cdev_del(dev_private_data->cdev_addr);
    device_destroy(iomodule_drv.dev_class, iomodule_drv.dev_num);
    class_destroy(iomodule_drv.dev_class);
    return 0;
}
static int __init platform_driver_IOmodule_init(void)
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
    /* yeu cau kernel cho phep su dung port region*/
    //iomodule_dev.dev_ioaddr = ioremap(PRR_PORT_BASE, PRR_PORT_LEN);
    //iomodule_cpu_core (ioread32(iomodule_dev.dev_ioaddr));
    
    platform_driver_register(&driver);

    return 0;
/* Huy khi goi ham that bai */
failed_registed_devclass:
    unregister_chrdev_region(iomodule_drv.dev_num, 1);
failed_registed_devnum:
    return ret;
}

static void __exit platform_driver_IOmodule_cleanup(void)
{

    /* Xoa Device file */
    device_destroy(iomodule_drv.dev_class, iomodule_drv.dev_num);
    class_destroy(iomodule_drv.dev_class);

    /* Giai phong Device Number */
    unregister_chrdev_region(iomodule_drv.dev_num, 1);

    pr_info("Exit iomodule driver\n");
}

module_init(platform_driver_IOmodule_init);
module_exit(platform_driver_IOmodule_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HUNGTHETRAN");
MODULE_DESCRIPTION("Read and write to memory region on G1E");
MODULE_INFO(G1E,"iW-RainboW-G20M");
