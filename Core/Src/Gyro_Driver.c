/*
 * Gyro_Driver.c
 *
 *  Created on: Apr 27, 2025
 *      Author: dvogl
 */

#include "Gyro_Driver.h"


extern SPI_HandleTypeDef hspi5;

static HAL_StatusTypeDef halStatus;

int16_t yAngle = 0;

// Initialize the Gyro
void Gyro_Init(void)
{
    // Enable the SPI5 clock
    __HAL_RCC_SPI5_CLK_ENABLE();

    // Enable GPIO clocks for SCK, MISO, MOSI, and CS
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure GPIO for Gyro CS pin
    GPIO_InitTypeDef SPI_PinConfig = {0};
    SPI_PinConfig.Pin = GYRO_CS_PIN;
    SPI_PinConfig.Mode = GPIO_MODE_OUTPUT_PP;
    SPI_PinConfig.Pull = GPIO_NOPULL;
    SPI_PinConfig.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GYRO_CS_PORT, &SPI_PinConfig);


    // Configure GPIO for Gyro SCK pin
    SPI_PinConfig.Pin = GYRO_SCK_PIN;
    SPI_PinConfig.Mode = GPIO_MODE_AF_PP;
    SPI_PinConfig.Pull = GPIO_NOPULL;
    SPI_PinConfig.Speed = GPIO_SPEED_FREQ_LOW;
    SPI_PinConfig.Alternate = GPIO_AF5_SPI5;
    HAL_GPIO_Init(GYRO_SCK_PORT, &SPI_PinConfig);

    // Configure GPIO for Gyro MOSI pin
    SPI_PinConfig.Pin = GYRO_MOSI_PIN;
    SPI_PinConfig.Mode = GPIO_MODE_AF_PP;
    SPI_PinConfig.Pull = GPIO_NOPULL;
    SPI_PinConfig.Speed = GPIO_SPEED_FREQ_LOW;
    SPI_PinConfig.Alternate = GPIO_AF5_SPI5;
    HAL_GPIO_Init(GYRO_MOSI_PORT, &SPI_PinConfig);

    // Configure GPIO for Gyro MISO pin
    SPI_PinConfig.Pin = GYRO_MISO_PIN;
    SPI_PinConfig.Mode = GPIO_MODE_AF_PP;
    SPI_PinConfig.Pull = GPIO_NOPULL;
    SPI_PinConfig.Speed = GPIO_SPEED_FREQ_LOW;
    SPI_PinConfig.Alternate = GPIO_AF5_SPI5;
    HAL_GPIO_Init(GYRO_MISO_PORT, &SPI_PinConfig);

    // Set CS high (inactive state)
    Gyro_DisableSlaveCommunication();

    // Configure SPI5 settings
    hspi5.Instance = SPI5;
    hspi5.Init.Mode = SPI_MODE_MASTER;
    hspi5.Init.Direction = SPI_DIRECTION_2LINES;
    hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi5.Init.NSS = SPI_NSS_SOFT;
    hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    halStatus = HAL_SPI_Init(&hspi5);
    Gyro_VerifyHALStatus();

    // Configure Gyro registers
    Gyro_PowerOn(); // Power on
    Gyro_ConfigureRegister(GYRO_CTRL_REG4, GYRO_CTRL_REG4_CONFIG, &hspi5); // Configure register 4
    Gyro_ConfigureRegister(GYRO_CTRL_REG5, GYRO_CTRL_REG5_CONFIG, &hspi5); // Configure register 5
}

// Get and print the device ID
void Gyro_GetDeviceID(void)
{
    uint8_t deviceID = Gyro_ReadRegister(GYRO_WHO_AM_I);
    printf("Gyro Device ID: 0x%X\n", deviceID); // Print the device ID
}

int16_t Gyro_GetYAngle(void)
{
	int16_t angularRate = (Gyro_ReadRegister(GYRO_OUT_Y_H) << 8 | Gyro_ReadRegister(GYRO_OUT_Y_L));

	// Integrate angular rate
	angularRate = (angularRate/5000);

	// Add integrated angular rate to start angle
	yAngle += angularRate;
	// Update start angle as it is continuously called

	return yAngle;
}

void Gyro_ResetYAngle(void)
{
	yAngle = 0;
}

// Power the Gyro
void Gyro_PowerOn(void)
{
	Gyro_ConfigureRegister(GYRO_CTRL_REG1, GYRO_CTRL_REG1_CONFIG, &hspi5);
}

// Configure the Gyro registers
void Gyro_ConfigureRegister(uint8_t registerAddress, uint8_t writtenValue, SPI_HandleTypeDef *hspi5)
{
	uint8_t writeRegAddress = (GYRO_WRITE_REGISTER | registerAddress);
	uint16_t data = (writtenValue << 8 | writeRegAddress) ;

	// Enable slave communication (chip select active low)
	Gyro_EnableSlaveCommunication();

	// Send register address and value via SPI
	halStatus = HAL_SPI_Transmit(hspi5,(uint8_t*) &data, 2, HAL_MAX_DELAY);

	Gyro_VerifyHALStatus();

	// Disable slave communication (chip select inactive high)
	Gyro_DisableSlaveCommunication();
}

// Read the Gyro registers
uint8_t Gyro_ReadRegister(uint8_t registerAddress)
{
    uint8_t readValue = 0; // Variable to store the read data
    uint8_t readRegisterAddress = (GYRO_READ_REGISTER | registerAddress); // Variable to store register address
    uint16_t recievedData = 0x00;

    // Enable slave communication (chip select active low)
    Gyro_EnableSlaveCommunication();

    while(HAL_GPIO_ReadPin(GYRO_CS_PORT, GYRO_CS_PIN) != GPIO_PIN_RESET); // Wait for communication line to be low

    halStatus = HAL_SPI_TransmitReceive(&hspi5, &readRegisterAddress, (uint8_t*) &recievedData, 2, HAL_MAX_DELAY);

    Gyro_VerifyHALStatus();

    readValue = (0xFF00 & recievedData) >> 8;

    // Disable slave communication (chip select inactive high)
    Gyro_DisableSlaveCommunication();

    return readValue;
}

// Verify the HAL status for SPI communication
void Gyro_VerifyHALStatus(void)
{
    if (halStatus != HAL_OK)
    {
    	printf("SPI Communication Error: Status Code = %d\n", halStatus);
    	// Enter an assert infinite loop
    	while(1);
    }
}

// Manually enable slave communication
void Gyro_EnableSlaveCommunication(void)
{
    // Pull the CS pin low to enable slave communication
    HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_RESET);
}

// Manually disable slave communication
void Gyro_DisableSlaveCommunication(void)
{
	// Pull the CS pin high to disable slave communication
	HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_SET);
}
