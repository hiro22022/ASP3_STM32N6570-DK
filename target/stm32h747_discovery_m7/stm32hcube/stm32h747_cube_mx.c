/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "t_stddef.h"
extern const FP _kernel_vector_table[];             /* kernel_cfg.c に自動生成される */
const FP *_kernel_vt; // = _kernel_vector_table;    /* 参照がないとリンカで消される (--gc-sections) */
                        // 参照されない奴から参照されてもダメ

#include "stm32h7xx_hal.h"
#define CEC_CK_MCO1_Pin GPIO_PIN_8
#define CEC_CK_MCO1_GPIO_Port GPIOA

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    /*Configure GPIO pin : CEC_CK_MCO1_Pin */
    GPIO_InitStruct.Pin = CEC_CK_MCO1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
    HAL_GPIO_Init(CEC_CK_MCO1_GPIO_Port, &GPIO_InitStruct);

     _kernel_vt = _kernel_vector_table;        /* 参照がないとリンカで消される (--gc-sections) */
}

