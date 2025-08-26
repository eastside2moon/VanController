
//#include <stm32f10x.h>
//#include <ctype.h>


#include <stdarg.h>
#include <string.h>
#include <stdio.h>


#include "stm32f1xx_hal.h"
#include "uart1_print.h"





extern 	UART_HandleTypeDef huart1;

//==========================================================//
//�������ƣ�	USART1_Printf
//�������ܣ�	���ڵ��Դ�ӡ����
//��ڲ�����	���ⳤ������
//���ز�����	��
//˵����		  USART_FLAG_TC�ĳ�USART_FLAG_TXE���⿪���״����ݷ���
//          ���ֽڶ�ʧ�����ڰ��Ӵ�ӡ��Ϣ����
//          ��ʽ�磺USART1_Printf("\r\nbuf[0]=%x\r\n",buf[0]);
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
//�������ƣ�	USART1_IRQHandler
//�������ܣ�	�����жϺ���
//��ڲ�����	��
//���ز�����	��
//˵����		  ����1�жϽ������ݴ洢���ṹ��
//          USART1_RxStruct.USART1_RxCount&=0x3F�ʹ��ڽ���buf�ռ��й�
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
