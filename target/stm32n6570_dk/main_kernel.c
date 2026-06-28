/*
 *  TOPPERS/ASP 起動用 main（STM32N6570-DK）
 *
 *  一時的に b6ec122 相当（syslog/UART 初期化なし）へ戻している。
 */
#include "main.h"

extern void sta_ker(void);

int b_sta_ker;

void
PeriphCommonClock_Config(void)
{
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CKPER;
	PeriphClkInitStruct.CkperClockSelection = RCC_CLKPCLKSOURCE_HSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
		Error_Handler();
	}
}

int main(void)
{
	HAL_Init();
	PeriphCommonClock_Config();
	SystemCoreClockUpdate();

	__enable_fault_irq();
	b_sta_ker = 1;
	sta_ker();

	while (1) {
	}
}
