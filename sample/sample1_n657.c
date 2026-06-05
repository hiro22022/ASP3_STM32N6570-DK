/*
 *  サンプルプログラム(N657最小) - カーネル起動確認用
 */
#include "sample1_n657.h"
#include "led_btn_joy.h"

volatile unsigned int n657_task1_count;
volatile unsigned int n657_main_count;

void
task1(intptr_t exinf)
{
	(void)exinf;
	for (;;) {
		n657_task1_count++;
		(void)dly_tsk(1000);
	}
}

void
main_task(intptr_t exinf)
{
	(void)exinf;
	led_init();
	for (;;) {
		n657_main_count++;
		led_toggle(0);		/* LD1 (green) */
		(void)dly_tsk(500);
	}
}
