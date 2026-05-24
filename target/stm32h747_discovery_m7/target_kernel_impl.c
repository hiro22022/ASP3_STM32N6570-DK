/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2016 by Embedded and Real-Time Systems Laboratory
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

/*
 * ターゲット依存モジュール（STM32H747_DISCOVERY用）
 */
#include "kernel_impl.h"
#include "stm32h747xx.h"
#include "stm32h7xx_hal.h"
#include "core_cm7.h"
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
void MX_GPIO_Init(void);

#if 0
/* LED 構成構造体 */
typedef struct {
    GPIO_TypeDef  *gpio;
    uint32_t      Pin;
} LED_conf;

/* LED1～LED4 の構成 */
const LED_conf led_conf[] =
{
    { GPIOI, GPIO_PIN_12 },      /* LED1 */
    { GPIOI, GPIO_PIN_13 },      /* LED2 */
    { GPIOI, GPIO_PIN_14 },      /* LED3 */
    { GPIOI, GPIO_PIN_15 },      /* LED4 */
};

void led_init(void);
#endif // 0

#include <sil.h>

/*
 *  起動直後の初期化(system_stm32f4xx.c)
 */
extern void SystemInit(void);

/*
 *  クロックの初期化(systemclock_config.c)
 */
extern void SystemClock_Config(void);

#if 0
/*
 *  バーナ出力用のUARTの初期化
 */
static void usart_early_init(void);
#endif // 0

/*
 *  エラー時の処理
 */
extern void Error_Handler(void);

/*
 *  起動時のハードウェア初期化処理
 */
void
hardware_init_hook(void) {
	SystemInit();

	/*
	 *  -fdata-sectionsを使用するとistkが削除され，
	 *  cfgのパス3のチェックがエラーとなるため，
	 *  削除されないようにする 
	 */
	SystemCoreClock = (uint32_t)istk;
}


/*
 * ターゲット依存部 初期化処理
 */
void
target_initialize(void)
{
#ifdef f401
	/*
	 *  HALによる初期化
	 *  HAL_Init() : stm32f4xx_hal.c の内容から必要な初期化のみ呼び出す．
	 */
	__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
	__HAL_FLASH_DATA_CACHE_ENABLE();
	__HAL_FLASH_PREFETCH_BUFFER_ENABLE();

	/*
	 *  クロックの初期化
	 */
	SystemClock_Config();

	/*
	 *  コア依存部の初期化
	 */
	core_initialize();

	/*
	 *  使用するペリフェラルにクロックを供給
	 */

	/*
	 *  UserLEDの初期化
	 */
	BSP_LED_Init(LED2);
	/*
	 *  バーナー出力用のシリアル初期化
	 */
	usart_early_init();
#else // f401
	core_initialize();

#endif // f401
}

/*
 * ターゲット依存部 終了処理
 */
void
target_exit(void)
{
	/* チップ依存部の終了処理 */
	core_terminate();
	while(1);
}

#if 0
static UART_HandleTypeDef UartHandle;

void
usart_early_init()
{
	usart_low_init();

	UartHandle.Instance          = USART_NAME; 
	UartHandle.Init.BaudRate     = BPS_SETTING;
	UartHandle.Init.WordLength   = UART_WORDLENGTH_9B;
	UartHandle.Init.StopBits     = UART_STOPBITS_1;
	UartHandle.Init.Parity       = UART_PARITY_ODD;
	UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode         = UART_MODE_TX_RX;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    
	if(HAL_UART_Init(&UartHandle) != HAL_OK) {
		Error_Handler();
	}
};
#endif // 0

#if 0
/*
 * エラー発生時の処理
 */
void
Error_Handler(void){
#if 0
	volatile int loop;
	BSP_LED_Init(LED2);
	while(1){
		for(loop = 0; loop < 0x100000; loop++);
		BSP_LED_Toggle(LED2);
	}
#endif
}
#endif // 0

#include "time_event.h"

/*
 *  HAL実行用の関数
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  return HAL_OK;
}

uint32_t HAL_GetTick(void)
{
  return current_hrtcnt/1000;
}

/*-------------------------------------------*/
#if 0
/* LED 番号 LED1～LED4 */
typedef enum  {
    LED1 = 0, LED2, LED3, LED4
} LED_Index;

/* LED の初期化 */
void led_init(void)
{
    uint32_t temp;
    int  i;

    /* RCC (クロック) の設定 */
    RCC_TypeDef  *rcc = RCC;
    temp = rcc->AHB4ENR;
    temp |= RCC_AHB4ENR_GPIOIEN;
    rcc->AHB4ENR = temp;

    /* GPIO コントロールレジスタの設定 */
    GPIO_InitTypeDef gpio_init;
    gpio_init.Mode      = GPIO_MODE_OUTPUT_OD;
    gpio_init.Pull      = GPIO_NOPULL;
    gpio_init.Speed     = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Alternate = 0x0;
    for( i = LED1; i <= LED4; i++ ){
        gpio_init.Pin   = led_conf[ i ].Pin;
        HAL_GPIO_Init( led_conf[ i ].gpio, &gpio_init );
    }
}

void led_set( int val )
{
    /* LED が同じ GPIO で並んでいることを利用して、１回の IO アクセスでよいように効率化したバージョン */
    uint32_t rs_val = 0;
    int i;
    for( i = LED1; i <= LED4; i++ ){
      if( val & ( 1 << i ) )
        rs_val |= 1 << (i+12+16);
      else
        rs_val |= 1 << (i+12);
    }
    /* GPIO のレジスタに直接書き込む */
    led_conf[0].gpio->BSRR = rs_val;
}
/*-------------------------------------------*/
#endif

#if 0
/*
 * libc.a(lib_a-init.o): in function `__libc_init_array':
 *    init.c:40: undefined reference to `_init'
 */
void _init(void){}
#endif // 0

void software_term_hook(void){}


/*-------------------------------------------*/
#include "task.h"
void
print_p_run(uint32_t no)
{
#if 0
	syslog( LOG_NOTICE, "print_p_run: no=%d", no);
	if( p_runtsk ){
		syslog( LOG_NOTICE, "wai_sem: p_runtsk   pc=%08x sp=%08x",
						p_runtsk->tskctxb.pc, p_runtsk->tskctxb.sp);
	}
	else {
		syslog( LOG_NOTICE, "wai_sem: p_runtsk=0" );
	}
	if( p_schedtsk ){
		syslog( LOG_NOTICE, "wai_sem: p_schedtsk pc=%08x sp=%08x",
						p_schedtsk->tskctxb.pc, p_schedtsk->tskctxb.sp);
	}
	else {
		syslog( LOG_NOTICE, "wai_sem: p_schedtsk=0" );
	}
	syslog( LOG_NOTICE, "wai_sem: ipsr=%08x", get_ipsr() );
#endif // 0
}
/*-------------------------------------------*/
