/* #[<PREAMBLE>]#
 * Don't edit the comments between #[<...>]# and #[</...>]#
 * These comment are used by tecsmerege when merging.
 *
 * attr access macro #_CAAM_#
 * hsem_bit_no      uint8_t          ATTR_hsem_bit_no
 * ipi_bit_flag     uint32_t*        ATTR_ipi_bit_flag
 *
 * call port function #_TCPF_#
 * call port: cHSEM signature: sHSEM context:task
 *   ER             cHSEM_lockPolling( );
 *   void           cHSEM_unlock( );
 *   uint32_t       cHSEM_isLocked( );
 *   void           cHSEM_enableInterrup( );
 *   void           cHSEM_disableInterrupt( );
 *   ER_UINT        cHSEM_getInterruptStatus( );
 *   void           cHSEM_clearInterrupt( );
 *
 * #[</PREAMBLE>]# */

/* Put prototype declaration and/or variale definition here #_PAC_# */
#include "tIPISenderUsingHSEM_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* entry port function #_TEPF_# */
/* #[<ENTRY_PORT>]# eIPISend
 * entry port: eIPISend
 * signature:  sIPISend
 * context:    any
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eIPISend_sendIPI
 * name:         eIPISend_sendIPI
 * global_name:  tIPISenderUsingHSEM_eIPISend_sendIPI
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
ER
eIPISend_sendIPI(CELLIDX idx, uint8_t ipino)
{
	ER		ercd = E_OK;
	CELLCB	*p_cellcb = GET_CELLCB(idx);
	bool_t	locked;

	/* 割込み禁止する */
	locked = sns_loc();
	if (!locked) {
		loc_cpu();
	}

	while(cHSEMGiantLock_lockPolling() != E_OK ){}	/* lock GiantLock */
	*ATTR_ipi_bit_flag |= (1<<ipino);
	cHSEMGiantLock_unlock();						/* unlock GiantLock */

	/* HSEM 0 を取得できるまで待つ */
	while( cHSEM_lockPolling() != E_OK ){}	/* いったんロックを取る */
	cHSEM_unlock();							/* ここで割込みが入る */
	if (!locked) {
		unl_cpu();
	}

	return(ercd);
}

/* #[<POSTAMBLE>]#
 *   Put non-entry functions below.
 * #[</POSTAMBLE>]#*/
