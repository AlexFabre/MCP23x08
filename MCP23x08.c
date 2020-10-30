/*
 * MCP23x08.c
 *
 *  Created on: 14 oct 2020
 *      Author: a.fabre
 */
#include "MCP23x08.h"


/**
 * @fun: 	uint8_t MCP23x08_init(MCP23x08_t *driver)
 * @brief  	MCP23x08 initialize driver
 * @param  	MCP23x08_t *driver
 * @note
 * @retval 	1 success | 0 error
 */
uint8_t MCP23x08_init(MCP23x08_t *driver)
{
	uint8_t iocon;

	if (driver->com.protocole == MCP23S08_SPI)
	{
		osMutexWait(driver->com.mutex, osWaitForever);
		CS_ENABLE(driver);
		wait(1);
		CS_DISABLE(driver);
		osMutexRelease(driver->com.mutex);
	}

	/* IOCON Config register */
	MCP23x08_write(driver, MCP23x08_IOCON, driver->config.IOCON);
	iocon = MCP23x08_read(driver, MCP23x08_IOCON);

	if(iocon != driver->config.IOCON)
	{
		iocon = 0; /* Error occured */
	}
	else
	{
		iocon = 1;
	}

	MCP23x08_write(driver, MCP23x08_IODIR, driver->config.IODIR);
	MCP23x08_write(driver, MCP23x08_IPOL, driver->config.IPOL);
	MCP23x08_write(driver, MCP23x08_GPINTEN, driver->config.GPINTEN);
	MCP23x08_write(driver, MCP23x08_DEFVAL, driver->config.DEFVAL);
	MCP23x08_write(driver, MCP23x08_INTCON, driver->config.INTCON);
	MCP23x08_write(driver, MCP23x08_GPPU, driver->config.GPPU);
	MCP23x08_write(driver, MCP23x08_GPIO, driver->config.default_GPIO);
	MCP23x08_write(driver, MCP23x08_OLAT, driver->config.OLAT);

	return iocon;
}

/**
 * @fun: 	uint8_t MCP23x08_read(MCP23x08_t *driver, MCP23x08_register_t reg)
 * @brief  	MCP23x08 read specific register
 * @param  	MCP23x08_t *driver
 * @param	MCP23x08_register_t MCP23x08 register
 * @note
 * @retval 	value of register
 */
uint8_t MCP23x08_read(MCP23x08_t *driver, MCP23x08_register_t reg)
{
	uint8_t TxBuffer[4] = {0};
	uint8_t RxBuffer[4] = {0};
	uint8_t Opcode = driver->com.Opcode;

	if (driver->com.protocole == MCP23S08_SPI)
	{
		Opcode &= 0x03u; /* With SPI only A0 and A1 are used for address decoding */
	}

	TxBuffer[0] = (MCP23x08_READ_CONTROLE_BYTE + (Opcode << 1u));
	TxBuffer[1] = (uint8_t) reg;


	if (driver->com.protocole == MCP23S08_SPI)
	{
		osMutexWait(driver->com.mutex, osWaitForever);
		CS_ENABLE(driver);

		while (__HAL_SPI_GET_FLAG(driver->com.hspix, SPI_FLAG_BSY))
		{
			;
		}

		if (HAL_SPI_TransmitReceive(driver->com.hspix, (uint8_t*) TxBuffer, (uint8_t *) RxBuffer, 3, 2000) != HAL_OK) 
		{
			Error_Handler();
		}

		while (__HAL_SPI_GET_FLAG(driver->com.hspix, SPI_FLAG_BSY)) 
		{
			;
		}

		CS_DISABLE(driver);
		osMutexRelease(driver->com.mutex);
	}

	if (driver->com.protocole == MCP23008_I2C)
	{
		osMutexWait(driver->com.mutex, osWaitForever);

		if (HAL_I2C_Master_Transmit(driver->com.hi2c, (uint16_t) driver->com.Opcode, TxBuffer, 2, 1000) != HAL_OK) 
		{
			Error_Handler();
		}
		if (HAL_I2C_Master_Receive(driver->com.hi2c, (uint16_t) driver->com.Opcode, RxBuffer, 1, 1000) != HAL_OK) {
			Error_Handler();
		}

		osMutexRelease(driver->com.mutex);
	}

	return (int8_t) RxBuffer[2];
}

/**
 * @fun: 	uint8_t MCP23x08_readport(MCP23x08_t *driver)
 * @brief  	MCP23x08 read GPIO port
 * @param  	MCP23x08_t *driver
 * @note
 * @retval 	value of GPIO's port
 */
uint8_t MCP23x08_readport(MCP23x08_t *driver) 
{
	return MCP23x08_read(driver, MCP23x08_GPIO);
}

/**
 * @fun: 	void MCP23x08_write(MCP23x08_t *driver, MCP23x08_register_t reg, uint8_t value)
 * @brief  	MCP23x08 read specific register
 * @param  	MCP23x08_t *driver
 * @param  	MCP23x08_register_t MCP23x08 register
 * @param  	int8_t value of register
 * @note
 * @retval  none
 */
void MCP23x08_write(MCP23x08_t *driver, MCP23x08_register_t reg, uint8_t value)
{
	uint8_t TxBuffer[4] = {0};
	uint8_t RxBuffer[4] = {0};
	uint8_t Opcode = driver->com.Opcode;

	if (driver->com.protocole == MCP23S08_SPI)
	{
		Opcode &= 0x03u; /* With SPI only A0 and A1 are used for address decoding */
	}

	TxBuffer[0] = (MCP23x08_WRITE_CONTROLE_BYTE + (Opcode << 1u));
	TxBuffer[1] = (uint8_t) reg;
	TxBuffer[2] = value;


	if (driver->com.protocole == MCP23S08_SPI)
	{
		osMutexWait(driver->com.mutex, osWaitForever);
		CS_ENABLE(driver);

		while (__HAL_SPI_GET_FLAG(driver->com.hspix, SPI_FLAG_BSY))
		{
			;
		}

		if (HAL_SPI_TransmitReceive(driver->com.hspix, (uint8_t*) TxBuffer, (uint8_t *) RxBuffer, 3, 2000) != HAL_OK) 
		{
			Error_Handler();
		}

		while (__HAL_SPI_GET_FLAG(driver->com.hspix, SPI_FLAG_BSY)) 
		{
			;
		}

		CS_DISABLE(driver);
		osMutexRelease(driver->com.mutex);
	}

	if (driver->com.protocole == MCP23008_I2C)
	{
		osMutexWait(driver->com.mutex, osWaitForever);

		if (HAL_I2C_Master_Transmit(driver->com.hi2c, (uint16_t) driver->com.Opcode, TxBuffer, 3, 1000) != HAL_OK) 
		{
			Error_Handler();
		}

		osMutexRelease(driver->com.mutex);
	}
}
/**
 * @fun: 	void MCP23x08_writeport(MCP23x08_t *driver, uint8_t value)
 * @brief  	MCP23x08 write port
 * @param  	MCP23x08_t *device
 * @param	uint8_t value of port
 * @note
 * @retval 	none
 */
void MCP23x08_writeport(MCP23x08_t *driver, uint8_t value)
{
	MCP23x08_write(driver, MCP23x08_GPIO, value);
}