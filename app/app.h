#ifndef APP_H_
#define APP_H_

/*-----------------------------------------------------------------------------------------------*/
#include "lib_stm32.h"
#include "GPIO_STM32F407.h"
#include "RCC_STM32F407.h"
#include "SSD_1306.h"
#include "I2C_STM32F407.h"
//#include "USB_FS_STM32F407.h"

#include <string.h>


/*defaines*/
#define SSD1306_I2C_ADDR 0x78 /*адрес I2C экрана*/


//void Work_USB();
void Work_MSO();
//void Work_USART();
/*-----------------------------------------------------------------------------------------------*/

#endif
