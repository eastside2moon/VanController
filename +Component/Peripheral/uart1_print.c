
//#include <stm32f10x.h>
//#include <ctype.h>


#include <stdarg.h>
#include <string.h>
#include <stdio.h>


#include "stm32f1xx_hal.h"
#include "uart1_print.h"





extern 	UART_HandleTypeDef huart1;

//==========================================================//
//函数名称：	USART1_Printf
//函数功能：	串口调试打印函数
//入口参数：	任意长度数据
//返回参数：	无
//说明：		  USART_FLAG_TC改成USART_FLAG_TXE避免开机首次数据发送
//          首字节丢失，用于板子打印信息调试
//          格式如：USART1_Printf("\r\nbuf[0]=%x\r\n",buf[0]);
//==========================================================//

void USART1_Printf ( char *fmt, ... )
{
//#if (defined FUNC_UART1) && (FUNC_UART1 == FUNC_EN)
static    unsigned char UsartPrintfBuf[256];
    unsigned int len = 256;
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;
    va_start ( ap, fmt );
    memset ( &UsartPrintfBuf[0], 0, 256 );
    vsnprintf ( ( char * ) UsartPrintfBuf, 256, fmt, ap );
    va_end ( ap );

	////////////////////
	
	len	=	strlen((char *)pStr);
	
	HAL_UART_Transmit(	&huart1, pStr, len, 1000)  ;
	
//    while ( ( *pStr != 0 ) && ( len-- ) )
//        {
//            USART_SendData ( USART1, *pStr++ );

//            while ( USART_GetFlagStatus ( USART1, USART_FLAG_TC ) == RESET );

//            // while ( USART_GetFlagStatus ( USART1, USART_FLAG_TXE ) == RESET );
//        }

//#endif
}


_Bool USART1_PrintfBuf ( unsigned char *buf, unsigned int buf_len )
{
//    unsigned int i = 0;
//    char temp_ch;

//    if ( buf == ( void * ) 0 || buf_len == 0 )
//        return 1;

//    USART1_Printf ( "\r\n" );

//    for ( ; i < buf_len; )
//        {
//            temp_ch =  *buf++ ;

//            if ( isprint ( temp_ch ) )
//                USART1_Printf ( "0x%02X[ %c ]", temp_ch, temp_ch );

//            else
//                USART1_Printf ( "0x%02X[non]", temp_ch );

//            //////////////////////////////////////////////
//            if ( ( ++i % 8 ) == 0 )
//                USART1_Printf ( "\r\n" );

//            else
//                USART1_Printf ( "   " );
//        }

//    USART1_Printf ( "\r\n" );
    return 0;
}


//==========================================================//
//函数名称：	USART1_IRQHandler
//函数功能：	串口中断函数
//入口参数：	无
//返回参数：	无
//说明：		  串口1中断接收数据存储到结构体
//          USART1_RxStruct.USART1_RxCount&=0x3F和串口接收buf空间有关
//==========================================================//

void USART1_IRQHandler ( void )
{
#if (defined FUNC_UART1) && (FUNC_UART1 == FUNC_EN)
    OSIntEnter();

    if ( USART_GetITStatus ( USART1, USART_IT_RXNE ) != RESET )
        {
            USART_ReceiveData ( USART1 ) ;
            USART_ClearFlag ( USART1, USART_FLAG_RXNE );
        }

    OSIntExit();
#endif
}




void USART1_PrfNul ( char *fmt, ... )
{
}
