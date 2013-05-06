#ifndef AIC_I2C_H_
#define AIC_I2C_H_

#include <stdio.h>
#include "csl_i2c.h"

#define CSL_I2C_DATA_SIZE        (64)
#define CSL_I2C_OWN_ADDR         (0x2F)
#define CSL_I2C_SYS_CLK          (100)
#define CSL_I2C_BUS_FREQ         (10)
#define CSL_I2C_EEPROM_ADDR		 (0x50)
#define CSL_I2C_CODEC_ADDR		 (0x18)

CSL_Status initializeI2C(void);
CSL_Status AIC_Write(Uint16 regAddr, Uint16 regValue);
Uint16     AIC_Read(Uint16 regAddr);
CSL_Status setupAIC3204(void);

#endif /*AIC_I2C_H_*/
