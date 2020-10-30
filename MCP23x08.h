/*
 * MCP23x08.h
 *
 *  Created on: 14 oct 2020
 *      Author: j.marcialis
 */

/*
------------------------------------------------------------
Exemple of initialisation
------------------------------------------------------------
MCP23x08_t MCP23S08_CIP0059_MOTOR = {
	.com.protocole = MCP23S08_SPI,
	.com.hspix = hspi2,
	.com.mutex = hspi2_mutex,
	.com.Opcode = 0b101,
	
	.config.INTCON = 0xFF,
	.config.IODIR = 0xFF,
};

MCP23x08_init(&MCP23S08_CIP0059_MOTOR);

------------------------------------------------------------
Exemple of structure to hold read function result
------------------------------------------------------------
typedef union MCP23x08_NAME_s {
	uint8_t GPIO;
	struct {
		uint8_t GP0:1;
		uint8_t GP1:1;
		uint8_t GP2:1;
		uint8_t GP3:1;
		uint8_t GP4:1;
		uint8_t GP5:1;
		uint8_t GP6:1;
		uint8_t GP7:1;
	};
} MCP23x08_NAME_t;

MCP23x08_NAME_t driver;
driver.GPIO = MCP23x08_readport(&MCP23S08_CIP0059_MOTOR);

if (driver.GP0 == 1)
{
	...
}

*/


#ifndef _MCP23X08_H_
#define _MCP23X08_H_

#include <stdint.h>

#define MCP23x08_READ_CONTROLE_BYTE 	0x41u
#define MCP23x08_WRITE_CONTROLE_BYTE 	0x40u

typedef enum MCP23x08_register_e {
	MCP23x08_IODIR,			/* IODIR: I/O DIRECTION REGISTER */
	MCP23x08_IPOL,			/* IPOL: INPUT POLARITY PORT REGISTER If a bit is set, the corresponding GPIO register bit will reflect the inverted value on the pin */
	MCP23x08_GPINTEN,		/* INTERRUPT-ON-CHANGE CONTROL REGISTER */
	MCP23x08_DEFVAL,		/* DEFAULT COMPARE REGISTER FOR INTERRUPT-ON-CHANGE */
	MCP23x08_INTCON,		/* INTERRUPT CONTROL REGISTER */
	MCP23x08_IOCON,			/* I/OEXPANDER CONFIGURATION REGISTER */
	MCP23x08_GPPU,			/* GPPU: GPIO PULL-UP RESISTOR REGISTER */
	MCP23x08_INTF,			/* READ ONLY : INTF: INTERRUPT FLAG REGISTER (ADDR 0x07) */
	MCP23x08_INTCAP,		/* READ ONLY : INTCAP: INTERRUPT CAPTURED VALUE FOR PORT REGISTER */
	MCP23x08_GPIO,			/* Port adress */
	MCP23x08_OLAT,			/* OUTPUT LATCH REGISTER */
} MCP23x08_register_t;

typedef enum MCP23x08_version_e {
	MCP23x08_UNDEFINED,
	MCP23S08_SPI,
	MCP23008_I2C,
} MCP23x08_version_t;

typedef struct MCP23x08_config_s {
	uint8_t IODIR;
	uint8_t IPOL;
	uint8_t GPINTEN;
	uint8_t DEFVAL;
	uint8_t INTCON;
	uint8_t IOCON;
	uint8_t GPPU;
	uint8_t default_GPIO;
	uint8_t OLAT;
} MCP23x08_config_t;

typedef struct MCP23x08_com_s{
	MCP23x08_version_t 		protocole;
	I2C_HandleTypeDef* 		hi2c;
	SPI_HandleTypeDef*		hspix;
	osMutexId* 				mutex;
	uint8_t 				Opcode:3;
} MCP23x08_com_t;

typedef struct MCP23x08_s {
	MCP23x08_com_t		com;
	MCP23x08_config_t 	config;
} MCP23x08_t;

uint8_t MCP23x08_init(MCP23x08_t *driver);
uint8_t MCP23x08_read(MCP23x08_t *driver, MCP23x08_register_t reg);
uint8_t MCP23x08_readport(MCP23x08_t *driver);
void MCP23x08_write(MCP23x08_t *driver, MCP23x08_register_t reg, uint8_t value);
void MCP23x08_writeport(MCP23x08_t *driver, uint8_t value);

#endif /* _MCP23X08_H_ */
