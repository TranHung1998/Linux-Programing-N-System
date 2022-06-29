#include <linux/module.h>
#include <linux/fs.h>   

#define DRIVER_AUTHOR "hungthetran tranthehung1406@gmail.com"
#define DRIVER_DESC   "Hello world kernel module"
#define DRIVER_VERS   "1.0"

static int  __init hello_world_init(void)
{
    printk(KERN_EMERG "Hello world kernel module\n");
    return 0;
}


static void  __exit hello_world_exit(void)
{
    printk(KERN_EMERG "Goodbye\n");
}


module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);  
MODULE_VERSION(DRIVER_VERS);
