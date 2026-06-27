#ifndef tPumpMainConverter__INLINE_H
#define tPumpMainConverter__INLINE_H

/* #[<PREAMBLE>]#
 * Don't edit the comments between #[<...>]# and #[</...>]#
 * These comment are used by tecsmerege when merging.
 *
 * call port function #_TCPF_#
 * call port: cMain signature: sPumpMain context:task
 *   void           cMain_Main( );
 *
 * #[</PREAMBLE>]# */

/* entry port function #_TEPF_# */
/* #[<ENTRY_PORT>]# eTaskBody
 * entry port: eTaskBody
 * signature:  sTaskBody
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eTaskBody_main
 * name:         eTaskBody_main
 * global_name:  tPumpMainConverter_eTaskBody_main
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
Inline void
eTaskBody_main(CELLIDX idx)
{
	CELLCB	*p_cellcb;
	p_cellcb = GET_CELLCB(idx);

    cMain_Main();
}

/* #[<POSTAMBLE>]#
 *   Put non-entry functions below.
 * #[</POSTAMBLE>]#*/

#endif /* tPumpMainConverter_INLINEH */
