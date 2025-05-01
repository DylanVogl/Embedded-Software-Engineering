/*
 * Gyro_Driver.h
 *
 *  Created on: Apr 27, 2025
 *      Author: dvogl
 */
#ifndef INC_GYRO_DRIVER_H_
#define INC_GYRO_DRIVER_H_

#include "stm32f4xx_hal.h"

#include <stdio.h>

// Gryo read and write
#define GYRO_READ_REGISTER (1 << 7)
#define GYRO_WRITE_REGISTER 0

// Register addresses
#define GYRO_WHO_AM_I 0x0F
#define GYRO_CTRL_REG1 0x20
#define GYRO_CTRL_REG2 0x21
#define GYRO_CTRL_REG3 0x22
#define GYRO_CTRL_REG4 0x23
#define GYRO_CTRL_REG5 0x24
#define GYRO_REFERENCE 0x25
#define GYRO_OUT_TEMP 0x26
#define GYRO_STATUS_REG 0x27
#define GYRO_OUT_X_L 0x28
#define GYRO_OUT_X_H 0x29
#define GYRO_OUT_Y_L 0x2A
#define GYRO_OUT_Y_H 0x2B
#define GYRO_OUT_Z_L 0x2C
#define GYRO_OUT_Z_H 0x2D
#define GYRO_FIFO_CTRL_REG 0x2E
#define GYRO_FIFO_SRC_REG 0x2F
#define GYRO_INT1_CFG 0x30
#define GYRO_INT1_SRC 0x31
#define GYRO_INT1_THS_XH 0x32
#define GYRO_INT1_THS_XL 0x33
#define GYRO_INT1_THS_YH 0x34
#define GYRO_INT1_THS_YL 0x35
#define GYRO_INT1_THS_ZH 0x36
#define GYRO_INT1_THS_ZL 0x37
#define GYRO_INT1_DURATION 0x38


#define GYRO_SCK_PIN GPIO_PIN_7
#define GYRO_SCK_PORT GPIOF

#define GYRO_MISO_PIN GPIO_PIN_8
#define GYRO_MISO_PORT GPIOF

#define GYRO_MOSI_PIN GPIO_PIN_9
#define GYRO_MOSI_PORT GPIOF

#define GYRO_CS_PIN GPIO_PIN_1
#define GYRO_CS_PORT GPIOC

#define GYRO_CTRL_REG1_CONFIG 0b00011111
#define GYRO_CTRL_REG4_CONFIG 0b00000000
#define GYRO_CTRL_REG5_CONFIG 0b01000000


// Initialize the Gyro
void Gyro_Init(void);

// Get and print the device ID
void Gyro_GetDeviceID(void);

int16_t Gyro_GetYAngle(void);

void Gyro_ResetYAngle(void);

// Power the Gyro
void Gyro_PowerOn(void);

// Configure the Gyro registers
void Gyro_ConfigureRegister(uint8_t registerAddress, uint8_t writtenValue, SPI_HandleTypeDef *hspi5);

// Read the Gyro registers
uint8_t Gyro_ReadRegister(uint8_t registerAddress);

// Verify the HAL status for SPI communication
void Gyro_VerifyHALStatus(void);

// Manually enable slave communication
void Gyro_EnableSlaveCommunication(void);

// Manually disable slave communication
void Gyro_DisableSlaveCommunication(void);





#endif /* INC_GYRO_DRIVER_H_ */
