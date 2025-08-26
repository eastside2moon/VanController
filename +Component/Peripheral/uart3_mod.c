
//#include <stm32f10x.h>
//#include <ctype.h>


//#include <stdarg.h>
//#include <string.h>
//#include <stdio.h>
//#include <stdint.h>

#include "stm32f1xx_hal.h"

#include "uart3_mod.h"


//#include "prj_config.h"
//#include "IO.h"
//#include "delay.h"



static uint8_t uart3_rx_buf[UART3_BUF_LEN];
static uint16_t  mod_uart_wr_idex = 0;
static uint16_t  mod_uart_rd_idex = 0;


uint8_t usart3_idle_cnt = 0;
uint8_t   uart_rcvflag = 0;


extern 	UART_HandleTypeDef huart3;
extern 	DMA_HandleTypeDef hdma_usart3_rx;



void USART3_IRQHandlerData ( void );
uint16_t  USART3_UnreadLen ( void );




void USART3_Init ( void )
{
    __HAL_UART_ENABLE_IT ( &huart3, UART_IT_IDLE );
    HAL_UART_Receive_DMA ( &huart3, uart3_rx_buf, UART3_BUF_LEN );
    CLEAR_BIT ( huart3.Instance->CR3, USART_CR3_EIE );		/////��������ж�ʹ��λ
}



void USART3_IRQHandlerData ( void )
{
    if ( ( __HAL_UART_GET_FLAG ( &huart3, UART_FLAG_IDLE ) != RESET ) ) //idle��־����λ
        {
            __HAL_UART_CLEAR_IDLEFLAG ( &huart3 ); //�����־λ
            usart3_idle_cnt++;
            mod_uart_wr_idex	=	UART3_BUF_LEN - __HAL_DMA_GET_COUNTER ( &hdma_usart3_rx );
            //           temp = UartHandle.Instance->SR;  //���״̬�Ĵ���SR,��ȡSR�Ĵ�������ʵ�����SR�Ĵ����Ĺ���
            //            temp = UartHandle.Instance->DR; //��ȡ���ݼĴ����е�����
            //             HAL_UART_DMAStop(&huart2); //
            //            temp  =  __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);// ��ȡDMA��δ��������ݸ���
            //            rx_len =  sizeof(rx_buffer) - temp; //�ܼ�����ȥδ��������ݸ������õ��Ѿ����յ����ݸ���
            uart_rcvflag  =  1;       /////��ʶ���������ˣ�֪ͨ��������ȡ
        }
}



uint16_t  USART3_UnreadLen ( void )
{
    return ( ( mod_uart_wr_idex -  mod_uart_rd_idex ) & UART3_LEN_MASK );
}






uint16_t  USART3_Poll ( void )
{
    extern void uart_indication ( void );

    if (  mod_uart_wr_idex !=  mod_uart_rd_idex )
        {
            uart_indication();
        }

    return 0;
}


uint8_t  USART3_StsRd ( void )
{
    return uart_rcvflag;
}

uint16_t  USART3_WrIdxRd ( void )
{
    return mod_uart_wr_idex;
}
uint16_t  USART3_RdIdxRd ( void )
{
    return mod_uart_rd_idex;
}

void  USART3_RdIdxSet ( uint16_t len )
{
    mod_uart_rd_idex = len;
    mod_uart_rd_idex &= UART3_LEN_MASK;
}


void  USART3_StsSet ( uint8_t sts )
{
    uart_rcvflag  =  sts;
}

void  USART3_RdIdxAdd ( uint16_t len )
{
    mod_uart_rd_idex += len;
    mod_uart_rd_idex &= UART3_LEN_MASK;
}

uint8_t  *USART3_RcvBufPtr ( void )
{
    return &uart3_rx_buf[0];
}





uint8_t USART3_RdByte ( uint8_t *buf )
{
    if (  mod_uart_rd_idex == mod_uart_wr_idex )
        {
            *buf = 0;
            return 1;
        }

    *buf = uart3_rx_buf[mod_uart_rd_idex];
    mod_uart_rd_idex++;
    mod_uart_rd_idex &= UART3_LEN_MASK;
    return 0;
}


