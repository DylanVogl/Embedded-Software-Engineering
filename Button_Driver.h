/*
 * Button_Driver.h
 *
 *  Created on: Feb 11, 2025
 *      Author: dvogl
 */

#ifndef BUTTON_DRIVER_H_
#define BUTTON_DRIVER_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>

/*
Create four (4) macros
1. A macro for the Button’s port value.
2. A macro for the Button’s pin number.
3. A macro dictating if the button is pressed.
a. This can be 1 or 0. The choice is yours!
4. A macro dictating if the button is not pressed or unpressed.
a. This can be 1 or 0 but needs to have the opposite value of
the pressed macro.
*/

#define BUTTON_PORT GPIOA_BASE_ADDRESS
#define BUTTON_PIN GPIO_PIN_0
#define BUTTON_PRESSED 1
#define BUTTON_UNPRESSED 0

//A prototype to initialize the button.
void Button_Init();

//A prototype that will return a boolean that will return true/false if the button is pressed or not.
bool Button_Press_Check();



#endif /* BUTTON_DRIVER_H_ */
