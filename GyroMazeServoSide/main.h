/*----------------------------------------------------------------------------
 * Name:    LED.h
 * Purpose: low level LED definitions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef __MAIN_H
#define __MAIN_H

#include "USART.h"
#include "stm32l4xx_hal.h"

static uint32_t USARTCount = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
//static void MX_TIM4_Init(void);
//static void MX_GPIO_Init(void);
//static void HAL_TIM_MspPostInit(void);

//void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
//void Error_Handler(void);
//UART4


#endif
