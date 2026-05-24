/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2016 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 *  $Id: target_timer.c 648 2016-02-20 00:50:56Z ertl-honda $
 */

/*
 *		タイマドライバ（TIM用）
 *		 TIM2をフリーランニング，TIM5を割込み通知用に使用する．
 */

#include "kernel_impl.h"
#include "time_event.h"
#include "target_timer.h"
#include <sil.h>

#include "led_btn_joy.h"

/*
 *  STM32CubeのTIMのハンドル
 */
#ifdef f401
#else
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim13;
#endif /* f401 */

/*
 *  タイマの起動処理
 */
void
target_hrt_initialize(intptr_t exinf)
{
	/* main.c (STM32CubeMX生成) で初期化 */
#ifdef f401
	/*
	 * フリーランニングタイマ
	 */
	/* Enable Clock. Use APB1 timer clock */
	__HAL_RCC_TIM2_CLK_ENABLE();

	htim8.Instance = TIM2;
	htim8.Init.Period = 0xFFFFFFFF;
	htim8.Init.Prescaler = (CPU_CLOCK_HZ/1000000);
	htim8.Init.ClockDivision = 0;
	htim8.Init.CounterMode = TIM_COUNTERMODE_UP;

	if(HAL_TIM_Base_Init(&htim8) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}

	if(HAL_TIM_Base_Start(&htim8) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}

	/*
	 *  割込み通知用タイマ
	 */
	/* Enable Clock. Use APB1 timer clock */
	__HAL_RCC_TIM5_CLK_ENABLE();

	htim13.Instance = TIM5;
	htim13.Init.Period = 0xFF;
	htim13.Init.Prescaler = (CPU_CLOCK_HZ/1000000);
	htim13.Init.ClockDivision = 0;
	htim13.Init.CounterMode = TIM_COUNTERMODE_UP;

	/* One-pulse Mode */
	if(HAL_TIM_OnePulse_Init(&htim13,TIM_OPMODE_SINGLE) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}
	/* clear interrupt */
	__HAL_TIM_CLEAR_FLAG(&htim13, TIM_SR_UIF);
	/* enable interrupt */
	__HAL_TIM_ENABLE_IT(&htim13, TIM_IT_UPDATE);
#else // f401
  HAL_TIM_Base_Start_IT(&htim8);
  HAL_TIM_Base_Start(&htim13);
#endif // f401
}

/*
 *  タイマの停止処理
 */
void
target_hrt_terminate(intptr_t exinf)
{
	if(HAL_TIM_Base_Stop(&htim8) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}
	if(HAL_TIM_Base_Stop(&htim13) != HAL_OK) {
		/* Initialization Error */
		Error_Handler();
	}
}

/*
 *  タイマ割込みハンドラ
 */
void
target_hrt_handler(void)
{
#if 0
	/* Clear Event */
	__HAL_TIM_CLEAR_FLAG(&htim13, TIM_SR_UIF);
#endif
	/*
	 *  高分解能タイマ割込みを処理する．
	 */
	signal_time();
}
