/* #[<PREAMBLE>]#
 * Don't edit the comments between #[<...>]# and #[</...>]#
 * These comment are used by tecsmerege when merging.
 *
 * attr access macro #_CAAM_#
 * hsem_bit_no      uint8_t          ATTR_hsem_bit_no
 * ipi_bit_flag     uint32_t*        ATTR_ipi_bit_flag
 *
 * call port function #_TCPF_#
 * call port: cIPIMain signature: sIPIMain context:non-task optional:true
 *   bool_t     is_cIPIMain_joined(int subscript)        check if joined
 *   ER             cIPIMain_main( subscript );
 *       subscript:  0...(NCP_cIPIMain-1)
 *
 * #[</PREAMBLE>]# */

/* Put prototype declaration and/or variale definition here #_PAC_# */
#include "tIPIReceiverUsingHSEM_tecsgen.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

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
/* #[<ENTRY_PORT>]# eCallback
 * entry port: eCallback
 * signature:  siHSEMCallback
 * context:    non-task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eCallback_callback
 * name:         eCallback_callback
 * global_name:  tIPIReceiverUsingHSEM_eCallback_callback
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eCallback_callback(CELLIDX idx)
{
	CELLCB	*p_cellcb = GET_CELLCB(idx);
	int_t	bit_no;
	uint32_t  bit_map;

	/* bit を落とすのは、ここだけなので、ロックを取らずして読み取ってもよい */
	while( (bit_map=*ATTR_ipi_bit_flag) != 0 ){
		loc_cpu();
		while(cHSEMGiantLock_lockPolling() != E_OK ){}	/* lock GiantLock */
		bit_no = bitmap32_search(bit_map);
		*ATTR_ipi_bit_flag &= ~(1<<bit_no);
		cHSEMGiantLock_unlock();						/* unlock GiantLock */
		unl_cpu();

		if( is_cIPIMain_joined(bit_no) ){
			cIPIMain_main(bit_no);
		}
	}
}

/* #[<POSTAMBLE>]#
 *   Put non-entry functions below.
 * #[</POSTAMBLE>]#*/
