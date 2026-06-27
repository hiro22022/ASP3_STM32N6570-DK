/*
 *		STM32N6570-DK サポートモジュール
 */

#ifndef TOPPERS_DISCOVERY_N6570_H
#define TOPPERS_DISCOVERY_N6570_H

/*
 *  コアのクロック周波数
 *  FSBL SystemClock_Config: HSI 64MHz, PLL1 (M=4,N=75), IC1/2 → CPUCLK 600MHz
 *  secure_nsclib.o 利用時は SECURE_SystemCoreClockUpdate() の戻り値と一致させること
 */
#define CPU_CLOCK_HZ	600000000

/*
 *  割込み数（LTDC_UP_ERR_IRQn + 16）
 */
#define TMAX_INTNO (210)

/*
 *  微少時間待ちのための定義
 */
#define SIL_DLY_TIM1    162
#define SIL_DLY_TIM2    100

#ifndef TOPPERS_MACRO_ONLY
#include "stm32n657xx.h"
#endif /* TOPPERS_MACRO_ONLY */

/*
 *  USART関連の定義（ST-Link VCP: USART1 PE5/PE6, 115200）
 */
#define USART_INTNO (USART1_IRQn + 16)
#define USART_NAME  USART1
#define USART_BASE  USART1_BASE

/*
 *  ボーレート
 */
#define BPS_SETTING  (115200)

#endif /* TOPPERS_DISCOVERY_N6570_H */
