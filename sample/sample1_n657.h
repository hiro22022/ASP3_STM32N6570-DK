/*
 *  サンプルプログラム(N657最小)のヘッダファイル
 */
#ifndef TOPPERS_SAMPLE1_N657_H
#define TOPPERS_SAMPLE1_N657_H

#include <kernel.h>
#include "target_test.h"

#define MAIN_PRIORITY	5
#define MID_PRIORITY	10

#ifndef STACK_SIZE
#define STACK_SIZE	4096
#endif

#ifndef TOPPERS_MACRO_ONLY
extern void task1(intptr_t exinf);
extern void main_task(intptr_t exinf);
#endif

#endif /* TOPPERS_SAMPLE1_N657_H */
