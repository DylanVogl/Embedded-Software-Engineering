/*
 * Button_Driver.c
 *
 *  Created on: Feb 11, 2025
 *      Author: dvogl
 */

#include "Button_Driver.h"

//1. A prototype to initialize the button.
void Button_Init()
{
	GPIO_InitTypeDef initializeButton;

	initializeButton.Pin = (GPIO_PIN_0);
	initializeButton.Mode = (GPIO_MODE_IT_RISING);
	initializeButton.Pull = (GPIO_PULLDOWN);
	initializeButton.Speed = (GPIO_SPEED_FREQ_MEDIUM);
	//initializeButton.Alternate = ();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	HAL_GPIO_Init((GPIOA), &initializeButton);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}



//2. A prototype that will return a boolean that will return true/false if the button is pressed or not.
bool Button_Press_Check(void)
{
    GPIO_PinState pin_state = HAL_GPIO_ReadPin(GPIOA, BUTTON_PIN); // Returns GPIO_PIN_SET or GPIO_PIN_RESET
    if (pin_state == GPIO_PIN_SET)
    {
        return true;
    }
    return false;
}

