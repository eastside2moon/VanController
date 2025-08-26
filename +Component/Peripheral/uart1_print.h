#ifndef _UART_H_
#define _UART_H_


#include "hal_defs_stm.h"




#define 	_PRINT_BUG_FILE		USART1_Printf ( "\r\n_DBG:%s\r\n",__FILE__ );
#define 	_PRINT_BUG_LINE	 	USART1_Printf ( "\r\n_DBG:%d\r\n",__LINE__ );  






void USART1_Printf ( char *fmt, ... );
void USART1_PrfNul ( char *fmt, ... );
_Bool USART1_PrintfBuf ( unsigned char *data, unsigned int data_len );
#endif











