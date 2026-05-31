/*
 *  TrustZone secure callable stub (初版)
 *
 *  Cube AppliSecure ビルドの secure_nsclib.o が使える場合は，
 *  本ファイルの代わりにそちらをリンクすること。
 */
#include <stdint.h>
#include "system_stm32n6xx.h"

uint32_t
SECURE_SystemCoreClockUpdate(void)
{
	/* FSBL/Secure 起動済み想定。フェーズ3で本番化 */
	SystemCoreClock = 64000000U;
	return SystemCoreClock;
}
