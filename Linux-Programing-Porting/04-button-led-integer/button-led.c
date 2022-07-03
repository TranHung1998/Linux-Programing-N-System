#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

#define DRIVER_AUTHOR "hungthetran tranthehung1406@gmail.com"
#define DRIVER_DESC   "Led blinking"
#define DRIVER_VERS   "1.0"

static unsigned int GPIO_LED = 30;
static unsigned int GPIO_BTN = 31;

static unsigned int irq;

static irqreturn_t btn_pushed_irq_handler(int irq, void *dev_idv)
{
	int state;

	state = gpio_get_value(GPIO_LED);
	if (state == 0)
		gpio_set_value(GPIO_LED, 1);
	else
		gpio_set_value(GPIO_LED, 0);

	pr_info("BTN interrupt - LED state is: %d\n", state);
	return IRQ_HANDLED;
}

static int __init btn_init(void)
{
	gpio_request(GPIO_LED, "led");
	gpio_direction_output(GPIO_LED, 0);
    gpio_set_value(GPIO_LED, 0);

	gpio_request(GPIO_BTN, "button");
	gpio_direction_input(GPIO_BTN);

	uint8_t retval;
	irq = gpio_to_irq(GPIO_BTN);
	retval = request_irq(irq, (irq_handler_t)btn_pushed_irq_handler, IRQF_TRIGGER_RISING, "button-led", NULL);

	pr_info("Hello! Initizliaze successfully!\n");
	return 0;
}

static void __exit btn_exit(void)
{
    gpio_set_value(GPIO_LED, 0);

	free_irq(irq, NULL);

	gpio_free(GPIO_LED);
	gpio_free(GPIO_BTN);

	pr_info("Good bye my fen !!!\n");
}

module_init(btn_init);
module_exit(btn_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_VERSION(DRIVER_VERS); 