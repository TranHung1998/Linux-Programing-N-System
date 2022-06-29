#ifndef __BUTTON_LED_H__
#define __BUTTON_LED_H__

#define GPIO_0_ADDR_BASE    		    0x44E07000
#define GPIO_0_ADDR_SIZE		        (0x44E07FFF - 0x44E07000)

#define GPIO_SETDATAOUT_OFFSET		    0x194
#define GPIO_CLEARDATAOUT_OFFSET	    0x190
#define GPIO_OE_OFFSET		    	    0x134
#define GPIO_DATAOUT_OFFSET             0x13C
#define GPIO_DATAIN_OFFSET              0x138
#define DEBOUNCEENABLE_OFFSET           0x150
#define GPIO_DEBOUNCINGTIME_OFFSET      0x154

#define DEBOUNCING_VALUE                255

#define GPIO_0_26                       20       // P9_41 <=> GPIO_0_20 BUTTON
#define GPIO_0_31   			        31       // P9_13 <=> GPIO_0_31 LED  

#define BTN_PIN                         20
#define LED_PIN                         31  

#define BTN_POS                         ( 1 << 20 )
#define LED_POS                         ( 1 << 31 )
                    


#endif
