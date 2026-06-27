/*
 *  サンプルプログラム(N657最小) - カーネル起動確認用
 */
#include "sample1_n657.h"
#include "led_btn_joy.h"
#include <t_syslog.h>

volatile unsigned int n657_task1_count;
volatile unsigned int n657_main_count;

/* dly_tsk の RELTIM はマイクロ秒単位（500000 = 0.5秒） */
#define DLY_1MS		1000U
#define DLY_500MS	500000U

void
task1(intptr_t exinf)
{
	(void)exinf;
	for (;;) {
		n657_task1_count++;
		(void)dly_tsk(DLY_1MS);
	}
}

void
main_task(intptr_t exinf)
{
	(void)exinf;
	led_init();
	syslog(LOG_NOTICE, "N657 sample started.");
	for (;;) {
		n657_main_count++;
		led_toggle(0);		/* LD1 (green) */
		(void)dly_tsk(DLY_500MS);
	}
}
