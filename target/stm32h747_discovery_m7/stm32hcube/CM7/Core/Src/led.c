/*
 * TOPPERS BASE PLATFORM から抜粋して用いる
 *    TOPPERS_BASE_PLATFORM\asp\pdic\stm32f7xx\device.c より
 */

/*
 *  LED接続ポート
 *
 *  拡張I/OボードのLED1-3はプログラマブル入出力ポート0に
 *  にインバータを介して接続されている．
 */

typedef struct gio_confstruct
{
	uint32_t    clkbit;
	uint32_t    giobase;
	uint32_t    pinpos;
	uint32_t    pinpp;
} gio_conftype;

#if defined(TOPPERS_STM32F7_DISCOVERY)
static const gio_conftype led_confdata[] = {
	{ RCC_AHB1ENR_GPIOIEN, TADR_GPIOI_BASE, PINPOSITION1, GPIO_PULLUP}
};
#elif defined(TOPPERS_STM32F769_DISCOVERY)
static const gio_conftype led_confdata[] = {
	{ RCC_AHB1ENR_GPIOJEN, TADR_GPIOJ_BASE, PINPOSITION13, GPIO_PULLUP},
	{ RCC_AHB1ENR_GPIOJEN, TADR_GPIOJ_BASE, PINPOSITION5, GPIO_PULLUP}
};
#elif defined(TOPPERS_STM32F723_DISCOVERY)
static const gio_conftype led_confdata[] = {
	{ RCC_AHB1ENR_GPIOAEN, TADR_GPIOA_BASE, PINPOSITION7, GPIO_PULLUP},
	{ RCC_AHB1ENR_GPIOBEN, TADR_GPIOB_BASE, PINPOSITION1, GPIO_PULLUP}
};
#elif defined(TOPPERS_STM32F7_DISCOVERY)
static const gio_conftype led_confdata[] = {
	{ RCC_AHB1ENR_GPIOBEN, TADR_GPIOB_BASE, PINPOSITION0, GPIO_PULLUP},
	{ RCC_AHB1ENR_GPIOBEN, TADR_GPIOB_BASE, PINPOSITION7, GPIO_PULLUP},
	{ RCC_AHB1ENR_GPIOBEN, TADR_GPIOB_BASE, PINPOSITION14, GPIO_PULLUP}
};
#else
static const gio_conftype led_confdata[] = {
	{ RCC_AHB4ENR_GPIOIEN, TADR_GPIOI_BASE, PINPOSITION0, GPIO_PULLUP},
	{ RCC_AHB4ENR_GPIOIEN, TADR_GPIOI_BASE, PINPOSITION7, GPIO_PULLUP},
	{ RCC_AHB4ENR_GPIOIEN, TADR_GPIOI_BASE, PINPOSITION14, GPIO_PULLUP}
};
#endif
#define NUM_LED   (sizeof(led_confdata)/sizeof(gio_conftype))

/*
 *  LED接続ポート初期化
 */ 
void
led_init(intptr_t exinf)
{
	const gio_conftype *pled = &led_confdata[0];
	GPIO_Init_t GPIO_Init_Data;
	uint32_t i;

	for(i = 0 ; i < NUM_LED ; pled++, i++){
		sil_orw_mem((uint32_t *)(TADR_RCC_BASE+TOFF_RCC_AHB1ENR), pled->clkbit);
		/* モード設定 */
		GPIO_Init_Data.mode      = GPIO_MODE_OUTPUT;
		/* プルアップ プロダウン設定 */
		GPIO_Init_Data.pull      = pled->pinpp;
		/* 出力モード設定 */
		GPIO_Init_Data.otype     = GPIO_OTYPE_PP;
        /* スピード設定 */
		GPIO_Init_Data.speed     = GPIO_SPEED_HIGH;
		gpio_setup(pled->giobase, &GPIO_Init_Data, pled->pinpos);

		/* 消灯 */
		sil_wrw_mem((uint32_t *)(pled->giobase+TOFF_GPIO_BSRR), (1 << pled->pinpos)<<16);
	}
}

/*
 *  LED接続ポート読み出し
 */
uint_t
led_in(void)
{
	const gio_conftype *pled = &led_confdata[0];
	uint32_t data, i;

	for(i = 0, data = 0 ; i < NUM_LED ; pled++, i++){
		if((sil_rew_mem((uint32_t *)(pled->giobase+TOFF_GPIO_IDR)) & (1<<(pled->pinpos))) != 0)
			data |= (1<<i);
	}
	return data;
}

/*
 *  LED接続ポート書き込み
 */ 
void
led_out(unsigned int led_data)
{
	const gio_conftype *pled = &led_confdata[0];
	uint32_t reg1, reg2, i;

	/* 設定値はLED接続ビット以外は変更しない */
	for(i = 0 ; i < NUM_LED ; pled++, i++){
		reg1 = reg2 = 0;
		if((led_data & (1<<i)) != 0)
			reg2 = 1 << pled->pinpos;
		else
			reg1 = 1 << pled->pinpos;

		/* 書き込みデータを生成して書き込み */
		sil_wrw_mem((uint32_t *)(pled->giobase+TOFF_GPIO_BSRR), (reg1<<16) | reg2);
	}
}

/*
 * LEDとスイッチの個別設定・読み込み関数群
 */

/*
 *  LEDの状態保存用変数
 */	
unsigned int LedState;


/*
 *  LED点灯制御
 */
void 
set_led_state(unsigned int led, unsigned char state){
	if (state == ON) {
		LedState = LedState | led;
	} else {
		LedState = LedState & ~led;
	}
	led_out(LedState);
}

/* LED 番号 LED1～LED4 */
typedef enum  {
    LED1 = 0, LED2, LED3, LED4
} LED_Index;

/* LED 構成構造体 */
typedef struct {
    GPIO_Typedef  *gpio;
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

/* LED の初期化 */
void led_init()
{
    uint32_t temp;

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

void led_on( uint32_t no )
{
    if( no <= LED4 )
        HAL_GPIO_WritePin( led_conf[no].gpio, led_conf[no].Pin, GPIO_PIN_RESET );
}

void led_off( uint32_t no )
{
    if( no <= LED4 )
        HAL_GPIO_WritePin( led_conf[no].gpio, led_conf[no].Pin, GPIO_PIN_SET );
}
