/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2007,2011,2015 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
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
 */
#ifndef TOPPERS_TARGET_TIMER_H
#define TOPPERS_TARGET_TIMER_H

/*
 *  タイマドライバ（NUCLEO_F401RE用）
 */
#ifdef USE_SYSTICK_AS_TIMETICK

/*
 *  プロセッサ依存部で定義する
 */
#include "core_timer.h"

#else /* USE_SYSTICK_AS_TIMETICK */
// #ifdef USE_TIM_AS_HRT
#if 1
#include <sil.h>
#include "stm32h7xx.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_tim.h"


/*
 *  タイマ割込みハンドラ登録のための定数
 */
#define INHNO_TIMER		(TIM8_UP_TIM13_IRQn + 16)	/* 割込みハンドラ番号 */
#define INTNO_TIMER		(TIM8_UP_TIM13_IRQn + 16)	/* 割込み番号 */
#define INTPRI_TIMER	(TMAX_INTPRI - 1)	/* 割込み優先度 */
#define INTATR_TIMER	TA_NULL				/* 割込み属性 */

#ifndef TOPPERS_MACRO_ONLY

extern TIM_HandleTypeDef htim8;		/* 割込み用 */
extern TIM_HandleTypeDef htim13;		/* フリーラン */

/*
 *  高分解能タイマの起動処理
 */
extern void	target_hrt_initialize(intptr_t exinf);

/*
 *  高分解能タイマの停止処理
 */
extern void	target_hrt_terminate(intptr_t exinf);

/*
 *  高分解能タイマの現在のカウント値の読出し
 */
Inline HRTCNT
target_hrt_get_current(void)
{
	return((HRTCNT)__HAL_TIM_GET_COUNTER(&htim13));
}

/*
 *  高分解能タイマへの割込みタイミングの設定
 *
 *  高分解能タイマを，hrtcntで指定した値カウントアップしたら割込みを発
 *  生させるように設定する．
 */
Inline void
target_hrt_set_event(HRTCNT hrtcnt)
{
#if 0
	__HAL_TIM_SET_COUNTER(&htim8, 0);
	__HAL_TIM_SET_AUTORELOAD(&htim8, hrtcnt);
	HAL_TIM_Base_Start(&htim8);
#else
#if 0
	syslog( LOG_NOTICE, "target_hrt_set_event hrtcnt=%d", hrtcnt);
#endif
	/* hrtcnt がゼロの場合長時間割込みがかからない
	 * hrtcnt が 0 になることはないはずだが、万一に備えて 0 から変更する
	 */
	if(hrtcnt==0)
		hrtcnt = 1;

    __HAL_TIM_SET_AUTORELOAD(&htim8, hrtcnt);
    __disable_irq();  // ここへ来たということは割込み許可されている。
                      // 以下の2つの間で割込みが入ると 1μ秒以下で終わることが保証できない。
                      // タイマー時間が十分長い場合は、不要だが、ここでは最短 1μ秒となることを仮定する。
    __HAL_TIM_SET_COUNTER(&htim8, 0);          // カウンタを 0にする
    __HAL_TIM_CLEAR_FLAG(&htim8, TIM_SR_UIF);  // 割込みフラグをクリア
    __enable_irq();
#endif
}

/*
 *  高分解能タイマ割込みの要求
 *
 */
Inline void target_hrt_raise_event(void)
{
#if 1
	htim8.Instance->EGR = TIM_EVENTSOURCE_UPDATE;
#endif
}

/*
 *  割込みタイミングに指定する最大値
 */
/* #define HRTCNT_BOUND	4000000002U */
/*
 * 16bit カウンタを使用するため、収まる値とする
 */
#define HRTCNT_BOUND	50000		/* 50msec */
#define TCYC_HRTCNT		65535
/*
 *  高分解能タイマ割込みハンドラ
 */
extern void	target_hrt_handler(void);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* USE_TIM_AS_HRT */
#endif /* USE_SYSTICK_AS_TIMETICK */

#endif /* TOPPERS_TARGET_TIMER_H */
