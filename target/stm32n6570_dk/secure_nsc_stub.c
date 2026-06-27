/*
 *  TrustZone secure callable stub (初版)
 *
 *  Cube AppliSecure ビルドの secure_nsclib.o が使える場合は，
 *  本ファイルの代わりにそちらをリンクすること。
 */
#include <stdint.h>
#include "system_stm32n6xx.h"
#define TOPPERS_MACRO_ONLY
#include "discovery_n6570.h"

uint32_t
SECURE_SystemCoreClockUpdate(void)
{
	/* FSBL/Secure 起動済み想定。CPU_CLOCK_HZ と一致させる */
	SystemCoreClock = CPU_CLOCK_HZ;
	return SystemCoreClock;
}
