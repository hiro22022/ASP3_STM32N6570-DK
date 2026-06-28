/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2006-2016 by Embedded and Real-Time Systems Laboratory
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
 *  $Id: tUsart.c 648 2016-02-20 00:50:56Z ertl-honda $
 */

/*
 *		ARM PrimCell UART（PL011）用 簡易SIOドライバ
 */

#include <sil.h>
#include "stm32n6xx_hal.h"
#include "tUsart_tecsgen.h"

/*
 * USARTレジスタ定義
 */
#define USART_SR(x)		(x)
#define USART_DR(x)		(x + 0x04)
#define USART_BRR(x)	(x + 0x08)
#define USART_CR1(x)	(x + 0x0C)
#define USART_CR2(x)	(x + 0x10)
#define USART_CR3(x)	(x + 0x14)
#define USART_GTPR(x)	(x + 0x18)

static Inline void
usart_clear_error_flags(USART_TypeDef *usart)
{
	usart->ICR = USART_ICR_PECF | USART_ICR_FECF | USART_ICR_NECF
			| USART_ICR_ORECF | USART_ICR_IDLECF | USART_ICR_TCCF;
}

static Inline void
usart_disable_periph_interrupts(USART_TypeDef *usart)
{
	usart->CR3 &= ~USART_CR3_EIE;
	usart->CR1 &= ~(USART_CR1_RXNEIE_RXFNEIE | USART_CR1_TXEIE_TXFNFIE
			| USART_CR1_TCIE);
}

/*
 *  プリミティブな送信／受信関数
 */

/*
 *  受信バッファに文字があるか？
 */
Inline bool_t
usart_getready(CELLCB *p_cellcb)
{
#if 0 // STM32F401RE
	return (((USART_TypeDef *)ATTR_baseAddress)->ISR & UART_FLAG_RXNE) != 0;
#else // STM32F401RE
	return (((USART_TypeDef *)ATTR_baseAddress)->ISR & USART_ISR_RXNE_RXFNE) != 0;
#endif // STM32F401RE
}

/*
 *  送信バッファに空きがあるか？
 */
Inline bool_t
usart_putready(CELLCB *p_cellcb)
{
#if 0 // STM32F401RE
	return (((USART_TypeDef *)ATTR_baseAddress)->ISR & UART_FLAG_TXE) != 0;
#else // STM32F401RE
	return (((USART_TypeDef *)ATTR_baseAddress)->ISR & USART_ISR_TXE_TXFNF) != 0;
#endif // STM32F401RE
}

/*
 *  受信した文字の取出し
 */
Inline char
usart_getchar(CELLCB *p_cellcb)
{
#if 0 // STM32F401RE
	return((char) sil_rew_mem((void*)USART_DR(ATTR_baseAddress)) & 0xFF);
#else // STM32F401RE
	return (char)(((USART_TypeDef *)ATTR_baseAddress)->RDR);
#endif // STM32F401RE
}

/*
 *  送信する文字の書込み
 */
Inline void
usart_putchar(CELLCB *p_cellcb, char c)
{
	(void)p_cellcb;
	(((USART_TypeDef *)ATTR_baseAddress)->TDR) = (uint32_t)(uint8_t)c;
}

/*
 *  シリアルI/Oポートのオープン
 */
void
eSIOPort_open(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);
	USART_TypeDef *usart = (USART_TypeDef *)ATTR_baseAddress;

	/*
	 * target_initialize() の HAL 初期化済みなら再設定しない
	 * （LogTask からの 2 回目 open で USART を壊さない）
	 */
	if ((usart->CR1 & USART_CR1_UE) != 0U) {
		usart_clear_error_flags(usart);
		usart_disable_periph_interrupts(usart);
		return;
	}

	{
		uint32_t tmp, usartdiv, fraction;
		uint32_t src_clock;
		RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
		GPIO_InitTypeDef GPIO_InitStruct = {0};

		if (ATTR_baseAddress == USART1_BASE) {
			PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CKPER;
			PeriphClkInitStruct.CkperClockSelection = RCC_CLKPCLKSOURCE_HSI;
			(void)HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

			PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
			PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_CLKP;
			(void)HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

			__HAL_RCC_USART1_CLK_ENABLE();
			__HAL_RCC_GPIOE_CLK_ENABLE();
			GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
			HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
		}

		usart->CR1 &= ~USART_CR1_UE;
		usart->CR2 = 0U;
		usart->CR1 = 0U;
		usart->CR3 = 0U;

		if (ATTR_baseAddress == USART1_BASE) {
			src_clock = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_USART1);
		} else {
			src_clock = HAL_RCC_GetPCLK1Freq();
		}
		if (src_clock == 0U) {
			src_clock = HSI_VALUE;
		}

		tmp = (1000U * (src_clock / 100U)) / ((ATTR_bps / 100U) * 16U);
		usartdiv = (tmp / 1000U) << 4;
		fraction = tmp - ((usartdiv >> 4) * 1000U);
		fraction = ((16U * fraction) + 500U) / 1000U;
		usartdiv |= (fraction & 0x0FU);
		usart->BRR = usartdiv;

		usart->CR1 = USART_CR1_RE | USART_CR1_TE;
		usart->CR3 = USART_CR3_EIE;
		usart->CR1 |= USART_CR1_UE;
	}
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
eSIOPort_close(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	(void)p_cellcb;
	sil_andw((void*)USART_CR1(ATTR_baseAddress), ~USART_CR1_UE);
}

/*
 *  シリアルI/Oポートへの文字送信
 */
bool_t
eSIOPort_putChar(CELLIDX idx, char c)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (usart_putready(p_cellcb)) {
		usart_putchar(p_cellcb, c);
		return(true);
	}
	return(false);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
int_t
eSIOPort_getChar(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);

	if (usart_getready(p_cellcb)) {
		return((int_t)(uint8_t) usart_getchar(p_cellcb));
	}
	return(-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
eSIOPort_enableCBR(CELLIDX idx, uint_t cbrtn)
{
	CELLCB		*p_cellcb = GET_CELLCB(idx);
	USART_TypeDef *usart = (USART_TypeDef *)ATTR_baseAddress;

	(void)p_cellcb;

	switch (cbrtn) {
	case SIOSendReady:
		usart->CR1 |= USART_CR1_TXEIE_TXFNFIE;
		break;
	case SIOReceiveReady:
		usart->CR1 |= USART_CR1_RXNEIE_RXFNEIE;
		break;
	}
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
eSIOPort_disableCBR(CELLIDX idx, uint_t cbrtn)
{
	CELLCB		*p_cellcb = GET_CELLCB(idx);
	USART_TypeDef *usart = (USART_TypeDef *)ATTR_baseAddress;

	(void)p_cellcb;

	switch (cbrtn) {
	case SIOSendReady:
		usart->CR1 &= ~USART_CR1_TXEIE_TXFNFIE;
		break;
	case SIOReceiveReady:
		usart->CR1 &= ~USART_CR1_RXNEIE_RXFNEIE;
		break;
	}
}

/*
 *  シリアルI/Oポートに対する割込み処理
 */
void
eiISR_main(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);
	USART_TypeDef *usart = (USART_TypeDef *)ATTR_baseAddress;
	uint32_t isr = usart->ISR;

	if ((isr & (USART_ISR_ORE | USART_ISR_FE | USART_ISR_NE | USART_ISR_PE)) != 0U) {
		usart->ICR = USART_ICR_PECF | USART_ICR_FECF | USART_ICR_NECF
				| USART_ICR_ORECF;
	}

	if (usart_getready(p_cellcb)) {
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		ciSIOCBR_readyReceive();
	}
	if (usart_putready(p_cellcb)) {
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		ciSIOCBR_readySend();
	}
}
