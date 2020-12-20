/**
  ******************************************************************************
  * File Name          : I2C.h
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __i2c_H
#define __i2c_H
#ifdef __cplusplus
 extern "C" {
#endif

 #include "main.h"


extern I2C_HandleTypeDef hi2c1;



void MX_I2C1_Init(void);
void i2c_writeByte(uint8_t deviceAddress, uint8_t registerAddress, uint8_t d);
void i2c_writeBuffer(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* pBuffer, int numBytes);
uint8_t i2c_readByte(uint8_t deviceAddress, uint8_t registerAddress);

#ifdef __cplusplus
}
#endif
#endif /*__ i2c_H */

