/*
 *  STM32N6570-DK 向け LED 制御（最小）
 *
 *  LD1 (green): PO1  点灯=High
 *  LD2 (red)  : PG10 点灯=Low
 */
#include "main.h"
#include "led_btn_joy.h"

#define N657_LED_NUM	2U

typedef struct {
	GPIO_TypeDef	*port;
	uint16_t		pin;
	uint8_t			active_high;
} n657_led_conf_t;

static const n657_led_conf_t n657_led_conf[N657_LED_NUM] = {
	{ GPIOO, GPIO_PIN_1,  1U },	/* LED1 */
	{ GPIOG, GPIO_PIN_10, 0U },	/* LED2 */
};

void
led_init(void)
{
	GPIO_InitTypeDef gpio = {0};
	uint32_t i;

	HAL_PWREx_EnableVddIO2();
	__HAL_RCC_GPIOO_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;

	for (i = 0; i < N657_LED_NUM; i++) {
		gpio.Pin = n657_led_conf[i].pin;
		HAL_GPIO_Init(n657_led_conf[i].port, &gpio);
		led_off(i);
	}
}

void
led_on(uint32_t no)
{
	if (no >= N657_LED_NUM) {
		return;
	}
	HAL_GPIO_WritePin(n657_led_conf[no].port, n657_led_conf[no].pin,
		n657_led_conf[no].active_high ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void
led_off(uint32_t no)
{
	if (no >= N657_LED_NUM) {
		return;
	}
	HAL_GPIO_WritePin(n657_led_conf[no].port, n657_led_conf[no].pin,
		n657_led_conf[no].active_high ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void
led_toggle(uint32_t no)
{
	if (no >= N657_LED_NUM) {
		return;
	}
	HAL_GPIO_TogglePin(n657_led_conf[no].port, n657_led_conf[no].pin);
}

void
led_set(int val)
{
	uint32_t i;

	for (i = 0; i < N657_LED_NUM; i++) {
		if (val & (1 << i)) {
			led_on(i);
		} else {
			led_off(i);
		}
	}
}

void
led_blink(int i, int j)
{
	(void)i;
	(void)j;
}

void
led_blink_btn(int i, int j)
{
	(void)i;
	(void)j;
}
