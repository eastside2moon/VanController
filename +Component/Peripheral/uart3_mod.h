#ifndef _UART3_H_
#define _UART3_H_




#include "hal_defs_stm.h"


#define  UART3_LEN_MASK   0x03FF
#define  UART3_BUF_LEN  1024
#define  UART3_BUF_ABOUT_FULL  894






void USART3_Init ( void );


uint16_t  USART3_UnreadLen ( void );
uint8_t  USART3_StsRd ( void );
void  USART3_StsSet ( uint8_t sts );
uint16_t  USART3_WrIdxRd ( void );
uint16_t  USART3_RdIdxRd ( void );
void  USART3_RdIdxSet ( uint16_t len );

void  USART3_RdIdxAdd ( uint16_t len );

uint8_t USART3_RdByte ( uint8_t *buf );


#endif











