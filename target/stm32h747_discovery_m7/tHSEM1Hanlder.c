/* #[<PREAMBLE>]#
 * Don't edit the comments between #[<...>]# and #[</...>]#
 * These comment are used by tecsmerege when merging.
 *
 * call port function #_TCPF_#
 * call port: cHSEM signature: sHSEM context:task
 *   ER             cHSEM_lock( );
 *   void           cHSEM_unlock( );
 *   uint32_t       cHSEM_isLocked( );
 *   void           cHSEM_enableInterrup( );
 *   void           cHSEM_disableInterrupt( );
 *   ER_UINT        cHSEM_getInterruptStatus( );
 *   void           cHSEM_clearInterrupt( );
 *
 * #[</PREAMBLE>]# */

/* Put prototype declaration and/or variale definition here #_PAC_# */
#include "tHSEM1Hanlder_tecsgen.h"
#include "syslog.h"
#include "com_var.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* entry port function #_TEPF_# */
/* #[<ENTRY_PORT>]# eCallback
 * entry port: eCallback
 * signature:  siHSEMCallback
 * context:    non-task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eCallback_callback
 * name:         eCallback_callback
 * global_name:  tHSEM1Hanlder_eCallback_callback
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eCallback_callback()
{
    SYSLOG  log;
    int     res = 0;
    COM_LOGBUF_GET( &log, &res );
    while( res ){
        cSysLog_write_(LOG_NOTICE, &log);
        COM_LOGBUF_GET( &log, &res );
    }
}

/* #[<POSTAMBLE>]#
 *   Put non-entry functions below.
 * #[</POSTAMBLE>]#*/
