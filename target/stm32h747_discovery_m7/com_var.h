#ifndef COM_VAR_H
#define COM_VAR_H

#include "syslog.h"

#define N_LOGBUF  64

struct  com_var{
    uint32_t        free_count;
    uint32_t        tim1_count;
    int8_t          push_hsem;

    uint32_t        ipi_7to4;            /* IPI bit (see target_ipi.h) */
    uint32_t        ipi_4to7;            /* IPI bit (see target_ipi.h) */

    /* Cortex-M4 コア ⇒ Cortex-M7 コア */
    SYSLOG          logbuf[ N_LOGBUF ];
    uint16_t        logbuf_wp;
    uint16_t        logbuf_rp;
    uint16_t        logbuf_lost;
};


#define COM_AREA ((volatile struct com_var*)0x10040000)
#define COM_INIT()       memset( (void*)COM_AREA, 0, sizeof( struct com_var ))

#define COM_FREE_COUNT  (COM_AREA->free_count)
#define COM_TIM1_COUNT  (COM_AREA->tim1_count)
#define COM_PUSH_HSEM   (COM_AREA->push_hsem)
#define COM_IPI_7to4    (COM_AREA->ipi_7to4)
#define COM_IPI_4to7    (COM_AREA->ipi_4to7)


#define COM_LOGBUF_PUT__( plogbuf )   COM_FREE_COUNT++;

#define COM_LOGBUF_PUT_( plogbuf )                                   \
            (void)HAL_HSEM_FastTake( 0 );  /* コア間割込みをかける */\
            HAL_HSEM_Release( 0, 0 );

#define COM_LOGBUF_PUT( plogbuf )                                   \
    do{                                                             \
        uint16_t   next_wp;                                         \
        next_wp = COM_AREA->logbuf_wp + 1;                          \
        if( next_wp >= N_LOGBUF )                                   \
            next_wp  = 0;                                           \
        if( COM_AREA->logbuf_rp == next_wp )    /* 等しければ、バッファ一杯、捨てる */ \
            COM_AREA->logbuf_lost++;                                \
        else{                                                       \
            (COM_AREA->logbuf[COM_AREA->logbuf_wp]) = *plogbuf;     \
            COM_AREA->logbuf_wp = next_wp;                          \
        }                                                           \
    } while(0)

#define COM_LOGBUF_GET( plogbuf, pres )                             \
    do{                                                             \
        uint16_t next_rp;                                           \
        if( COM_AREA->logbuf_rp == COM_AREA->logbuf_wp )    /* 等しければ、バッファは空 */ \
            *pres = 0;                                              \
        else{                                                       \
            *plogbuf = (COM_AREA->logbuf[COM_AREA->logbuf_rp]);     \
            next_rp = COM_AREA->logbuf_rp + 1;                      \
            if( next_rp >= N_LOGBUF )                               \
                next_rp = 0;                                        \
            COM_AREA->logbuf_rp = next_rp; /* 最後に更新する */      \
            *pres = 1;                                              \
        }                                                           \
    } while(0)

#endif /* COM_VAR_H */
