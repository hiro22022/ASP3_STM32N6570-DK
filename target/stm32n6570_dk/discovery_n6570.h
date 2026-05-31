/*
 *		STM32N6570-DK サポートモジュール
 */

#ifndef TOPPERS_DISCOVERY_N6570_H
#define TOPPERS_DISCOVERY_N6570_H

/*
 *  コアのクロック周波数（フェーズ3で更新）
 */
#define CPU_CLOCK_HZ	64000000

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

#endif /* TOPPERS_DISCOVERY_N6570_H */
