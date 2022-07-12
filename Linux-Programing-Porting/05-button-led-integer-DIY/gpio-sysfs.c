#include <linux/module.h>  /* This module defines macro such as module_init/module_exit */
#include <linux/fs.h>      /* This module defines functions such as allocate major/minor number */
#include <linux/cdev.h>    /* This module defines functions such as kmalloc */
#include <linux/uaccess.h> /* This module defines functions such as copy_to_user/copy_from_user */
#include <linux/gpio.h>    /* This module defines functions such as gpio_request/gpio_set_value */

#define DRIVER_AUTHOR "hungthetran tranthehung1406@gmail.com"
#define DRIVER_DESC   "GPIO sysfs"
#define DRIVER_VERS   "1.0"

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

volatile unsigned int GPIO_LED = 31;  //Default

static int32_t _value = 0;
static char _direct[8] = {};

struct _gpio_drv {
    struct kobject *kobj_control;
    struct kobject *kobj_init;
} gpio_drv;

/*************** Function Prototypes *******************/
static int      __init gpio_sysfs_init(void);
static void     __exit gpio_sysfs_exit(void);

/***************** Sysfs functions *******************/
static ssize_t  export_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t  export_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count);
static ssize_t  unexport_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t  unexport_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count);
static ssize_t  direction_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t  direction_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count);
static ssize_t  value_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t  value_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count);

struct kobj_attribute export = __ATTR(export, 0220, export_show, export_store);
struct kobj_attribute unexport = __ATTR(unexport, 0220, unexport_show, unexport_store);
struct kobj_attribute direction = __ATTR(direction, 0660, direction_show, direction_store);
struct kobj_attribute value = __ATTR(value, 0660, value_show, value_store);


/* Initialize group attrs */
static struct attribute *attrs_control[] = {
    &direction.attr,
    &value.attr,
    NULL,
};

static struct attribute_group attrs_control_group = {
    .attrs = attrs_control,
};

/* This function will be called when we read the sysfs file */
static ssize_t export_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return 0;
}

/* This function will be called when we write the sysfsfs file */
static ssize_t export_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
    pr_info("Export GPIO\n");
    int32_t ret = 0;
    int32_t numb = 0; 
    char gpio_numb[8] = {};

    sscanf(buf, "%d", &numb);
    if (numb < 0 || numb > 65)
        return -1;
    if (GPIO_LED != numb){
        GPIO_LED = numb;

        ret = sprintf(gpio_numb, "gpio%d", GPIO_LED);
        if (ret < 0){
            pr_err("Cannot sprintf......\n");
            return -1;
        }
        pr_err("gpio_numb: %s\n",gpio_numb);
        /* 01. It will create a directory under "/sys" , [firmware_kobj, class_kobj] */
        gpio_drv.kobj_control = kobject_create_and_add( (const char *)gpio_numb , gpio_drv.kobj_init);

        if (sysfs_create_group(gpio_drv.kobj_control, &attrs_control_group)) {
            pr_err("Cannot create sysfs file......\n");
            goto done;
        }

        /* 03. Request GPIO */ 
        gpio_request(numb, "led");
    }

    pr_info("Done! Export gpio!\n");
    return count;

done:
    kobject_put(gpio_drv.kobj_control);    
    return count;
}

static ssize_t unexport_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "0\n");
}

/* This function will be called when we write the sysfsfs file */
static ssize_t unexport_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
    pr_info("Unexport GPIO\n");
    sysfs_remove_group(gpio_drv.kobj_control , &attrs_control_group);/* 02 */
    kobject_put(gpio_drv.kobj_control);
    return count;
}

static ssize_t direction_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s", _direct);
}

/* This function will be called when we write the sysfsfs file */
static ssize_t direction_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{

    switch (count - 1) {
        case 2: 
            /* in */
            if (gpio_direction_input(GPIO_LED)){
                pr_err("Fail set direction\n");
                return -1;
            }
            break;
        case 3: 
            /* out */
            if(gpio_direction_output( GPIO_LED , 0)){
                pr_err("Fail set direction\n");
                return -1;
            }
            break;
        default:
            return count;
    }
    sscanf(buf, "%s", _direct);
    return count;
}

static ssize_t value_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", _value);
}

/* This function will be called when we write the sysfsfs file */
static ssize_t value_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
    int32_t numb  = 0; 
    sscanf(buf, "%d", &numb);

    switch (numb) {
        case 0: /* off */
            gpio_set_value(GPIO_LED, 0);
            break;
        case 1: /* on */
            gpio_set_value(GPIO_LED, 1);
            break;

        default:
            return count;
    }
    sscanf(buf, "%d", &_value);

    return count;
}

/* Initialize group attrs */
static struct attribute *attrs_init[] = {
    &export.attr,
    &unexport.attr,
    NULL,
};

static struct attribute_group attrs_init_group = {
    .attrs = attrs_init,
};

static int __init gpio_sysfs_init(void)   /* Constructor */
{
    /* 01. It will create a directory under "/sys" , [firmware_kobj, class_kobj] */
    gpio_drv.kobj_init = kobject_create_and_add("bbb_gpios", NULL);

    /* 02. Creating group sys entry under "/sys/bbb_gpios/" */  
    if (sysfs_create_group(gpio_drv.kobj_init, &attrs_init_group)) {
        pr_err("Cannot create sysfs file......\n");
        goto rm_kobj;
    }
    pr_info("Hello! Initizliaze successfully!\n");
    return 0;

rm_kobj:
    kobject_put(gpio_drv.kobj_init);
    return -1;
}

static void __exit gpio_sysfs_exit(void)   /* Destructor */
{
    sysfs_remove_group(gpio_drv.kobj_init , &attrs_init_group);  /* 02 */
    kobject_put(gpio_drv.kobj_init);                        /* 01 */

    pr_info("Good bye my fen !!!\n");
}

module_init(gpio_sysfs_init);
module_exit(gpio_sysfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_VERSION(DRIVER_VERS); 