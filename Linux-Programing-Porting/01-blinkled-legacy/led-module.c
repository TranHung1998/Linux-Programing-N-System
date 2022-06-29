#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include "led-module.h"

#define DRIVER_AUTHOR "hungthetran tranthehung1406@gmail.com"
#define DRIVER_DESC   "Led blinking"
#define DRIVER_VERS   "1.0"

uint32_t __iomem *base_addr;

static int __init led_init(void)
{
	base_addr = ioremap(GPIO_0_ADDR_BASE, GPIO_0_ADDR_SIZE);
	if(!base_addr)
		return -ENOMEM;

	*(base_addr + GPIO_OE_OFFSET / 4) &= ~GPIO_0_31;
	*(base_addr + GPIO_SETDATAOUT_OFFSET / 4) |= GPIO_0_31;

	return 0;
}

static void __exit led_exit(void)
{
	*(base_addr + GPIO_CLEARDATAOUT_OFFSET / 4) |= GPIO_0_31; 
	iounmap(base_addr);

}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_VERSION(DRIVER_VERS); 
