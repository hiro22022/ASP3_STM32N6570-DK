/* #[<PREAMBLE>]#
 * Don't edit the comments between #[<...>]# and #[</...>]#
 * These comment are used by tecsmerege when merging.
 *
 * call port function #_TCPF_#
 * call port: cCallback signature: siHSEMCallback context:non-task optional:true
 *   bool_t     is_cCallback_joined(int subscript)        check if joined
 *   void           cCallback_callback( subscript );
 *       subscript:  0...(NCP_cCallback-1)
 *
 * #[</PREAMBLE>]# */

/* Put prototype declaration and/or variale definition here #_PAC_# */
#include "tHSEMBody_tecsgen.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_hsem.h"
#include "syslog.h"

static const unsigned char bitmap_search_table[] = { 0, 1, 0, 2, 0, 1, 0,
												3, 0, 1, 0, 2, 0, 1, 0 };

static int_t
bitmap32_search(uint32_t bitmap)
{
	uint_t	n = 0U;

	if(bitmap == 0)		/* 0 の場合は -1 を返す */
		return -1;

	if ((bitmap & 0xffffU) == 0U) {
		bitmap >>= 16;
		n += 16;
	}
	if ((bitmap & 0x00ffU) == 0U) {
		bitmap >>= 8;
		n += 8;
	}
	if ((bitmap & 0x0fU) == 0U) {
		bitmap >>= 4;
		n += 4;
	}
	return(n + bitmap_search_table[(bitmap & 0x0fU) - 1]);
}

/* entry port function #_TEPF_# */
/* #[<ENTRY_PORT>]# eMain
 * entry port: eMain
 * signature:  siHandlerBody
 * context:    non-task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eMain_main
 * name:         eMain_main
 * global_name:  tHSEMBody_eMain_main
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eMain_main()
{
	/* Put statements here #_TEFB_# */
	uint32_t isr;
	int_t	num;

	if(((SCB->CPUID & 0x000000F0) >> 4 )== 0x7)
		isr = HSEM->C1MISR;
	else
		isr = HSEM->C2MISR;

#if 0
	syslog( LOG_NOTICE, "HSEM interrupt isr=%08x  ***All int enabled***", isr );
#endif

	num = bitmap32_search(isr);
	if( num < 0 )		/* 恐らく割込みは既に処理されている */
		return;
	if( num >= 32 )		/* 返ってくることはありえないが、一応ガード */
		return;
	eHSEM_clearInterrupt( num );	/* 割込み要求をクリア */
	if( num >= N_CP_cCallback )
		return;			/* 範囲外の番号 */

#if 0
	syslog( LOG_NOTICE, "HSEM interrupt no=%d joined=%d", num, is_cCallback_joined(num) );
#endif

	if( is_cCallback_joined(num))
		cCallback_callback( num );
}

/* #[<ENTRY_PORT>]# eHSEM
 * entry port: eHSEM
 * signature:  sHSEM
 * context:    task
 * entry port array size:  NEP_eHSEM
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eHSEM_lockPolling
 * name:         eHSEM_lockPolling
 * global_name:  tHSEMBody_eHSEM_lockPolling
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eHSEM_lockPolling(int_t subscript)
{
	/* Put statements here #_TEFB_# */
	if( HAL_HSEM_FastTake( (subscript) ) == HAL_OK )
		return E_OK;
	else
		return E_WBLK;
}

/* #[<ENTRY_FUNC>]# eHSEM_unlock
 * name:         eHSEM_unlock
 * global_name:  tHSEMBody_eHSEM_unlock
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eHSEM_unlock(int_t subscript)
{
	/* Put statements here #_TEFB_# */
	HAL_HSEM_Release((subscript), 0);
}

/* #[<ENTRY_FUNC>]# eHSEM_isLocked
 * name:         eHSEM_isLocked
 * global_name:  tHSEMBody_eHSEM_isLocked
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
uint32_t
eHSEM_isLocked(int_t subscript)
{
	/* Put statements here #_TEFB_# */
	/* 1 なら取られている, 0 なら取られていない */
	return HAL_HSEM_IsSemTaken((subscript));
}

/* #[<ENTRY_FUNC>]# eHSEM_enableInterrup
 * name:         eHSEM_enableInterrup
 * global_name:  tHSEMBody_eHSEM_enableInterrup
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eHSEM_enableInterrup(int_t subscript)
{
	/* Put statements here #_TEFB_# */
#if 0
	syslog( LOG_NOTICE, "HSEM C1IER=%08x before", HSEM->C1IER );
#endif
	__HAL_HSEM_ENABLE_IT( __HAL_HSEM_SEMID_TO_MASK(subscript) );
#if 0
	syslog( LOG_NOTICE, "HSEM C1IER=%08x after", HSEM->C1IER );
#endif
}

/* #[<ENTRY_FUNC>]# eHSEM_disableInterrupt
 * name:         eHSEM_disableInterrupt
 * global_name:  tHSEMBody_eHSEM_disableInterrupt
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eHSEM_disableInterrupt(int_t subscript)
{
	/* Put statements here #_TEFB_# */
	__HAL_HSEM_DISABLE_IT( __HAL_HSEM_SEMID_TO_MASK(subscript) );
}

/* #[<ENTRY_FUNC>]# eHSEM_getInterruptStatus
 * name:         eHSEM_getInterruptStatus
 * global_name:  tHSEMBody_eHSEM_getInterruptStatus
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER_UINT
eHSEM_getInterruptStatus(int_t subscript)
{
	ER_UINT		ercd = E_OK;

	/* Put statements here #_TEFB_# */
	ercd = __HAL_HSEM_GET_IT(__HAL_HSEM_SEMID_TO_MASK(subscript));

	return(ercd);
}

/* #[<ENTRY_FUNC>]# eHSEM_clearInterrupt
 * name:         eHSEM_clearInterrupt
 * global_name:  tHSEMBody_eHSEM_clearInterrupt
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eHSEM_clearInterrupt(int_t subscript)
{
	/* Put statements here #_TEFB_# */
	__HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(subscript));
}

/* #[<POSTAMBLE>]#
 *   Put non-entry functions below.
 * #[</POSTAMBLE>]#*/
