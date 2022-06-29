#include <linux/module.h>  /* Thu vien nay dinh nghia cac macro nhu module_init/module_exit */
#include <linux/fs.h>      /* Thu vien nay dinh nghia cac ham allocate major/minor number */
#include <linux/device.h>  /* Thu vien nay dinh nghia cac ham class_create/device_create */
#include <linux/cdev.h>    /* Thu vien nay dinh nghia cac ham cdev_init/cdev_add */

#define DRIVER_AUTHOR "hungthetran tranthehung1406@gmail.com"
#define DRIVER_DESC   "Led blinking"
#define DRIVER_VERS   "1.0"

struct m_foo_dev {
    dev_t dev_num;
    struct class *m_class;
    struct cdev m_cdev;
} mdev;

static int __init hello_world_init(void)
static void __exit hello_world_exit(void)

static int __init hello_world_init(void)   /* Constructor */
{   
    /* Dynamically allocate a device number for 1 (cat /proc/devices)*/

	if(alloc_chrdev_region(&mdev.dev_num,0,1,"m-cdev") < 0){
		pr_err("Alloc chrdev failed\n");
        return 1;
	}

    pr_info("Major = %d Minor = %d\n", MAJOR(mdev.dev_num), MINOR(mdev.dev_num));

    /* Create device class under /sys/class */
    if ((mdev.m_class = class_create(THIS_MODULE, "m_class")) == NULL) {
        pr_err("Class creation failed\n");
        goto rm_device_numb;
    }

    /* Create device file for the detected platform device */
    if ((device_create(mdev.m_class, NULL, mdev.dev_num, NULL, "m_device")) == NULL) {
        pr_err("Cannot create my device\n");
        goto rm_class;
    }

    pr_info("Hello world kernel module\n");
    return 0;

rm_class:
    class_destroy(mdev.m_class);
rm_device_numb:
    unregister_chrdev_region(mdev.dev_num, 1);
    return -1;
}

static void __exit hello_world_exit(void)   /* Destructor */
{
    device_destroy(mdev.m_class, mdev.dev_num);     /* 03 */
    class_destroy(mdev.m_class);                    /* 02 */
    unregister_chrdev_region(mdev.dev_num, 1);      /* 01 */

    pr_info("Goodbye\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_VERSION(DRIVER_VERS); 
