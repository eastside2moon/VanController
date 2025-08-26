



#define 	_MGR_PRINT      USART1_Printf
//		#define 	_MGR_PRINT      USART1_PrfNul


//#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>



#include "cmsis_os.h"
#include "task.h"
#include "stm32f1xx_hal.h"
#include "led.h"
#include "net_communicate.h"
#include "uart1_print.h"



////////////////////////////////////////////////////////
#include "socket_buf.h"



#define  SYS_GetTick    		OSTimeGet
#define  SYS_TICKS_PER_SEC		OS_TICKS_PER_SEC
#define  SYS_DELAY_Ms(x)    		OSTimeDly( x/(1000/OS_TICKS_PER_SEC) )



static  char 	svr_addr[64]	=	"";
static  char 	svr_port[8]		=	"";

server_info_t   server_info =
{
    svr_addr,
    svr_port,
};


////////  发送缓存，后面从这里读取  2048   对应0x07FF
#define   SOCKET_BUF_MAX	2048
#define   SOCKET_BUF_MASK	0x07FF


static 	char   socket_send_buf[SOCKET_BUF_MAX];
static 	uint16_t   socket_send_len = 0;





uint8_t Socket_Start ( char *type, char *addr, char *port )
{
    //////10秒之内连上
    uint8_t wait_cnt = 10;

    memset(	svr_addr, 	0,	64   );
    memset(	svr_port, 	0,	8   );
    strncpy (  svr_addr, addr, MIN( strlen ( addr ), 64)  );
    strncpy (  svr_port, port, 8  );
    SVR_Connect();

    while (   wait_cnt--   )
    {
        if (  SVR_Status( ) == 1 )   break;

        osDelay ( 1000 );
    }

    //////////////////////////////////

    if (    wait_cnt == 0      )      ///////过了100秒都没连上，失败
    {
        return 1;
    }

    else
    {
        return 0;
    }
}






server_info_t  *SERVER_Info ( void )
{
    return 	&server_info;
}





uint8_t Socket_SendData ( char *payload, uint16_t len, uint8_t clog )
{

    memcpy ( socket_send_buf, payload, len );
    socket_send_len	=	len;

    MOD_SendReq();


    return 0;
}

uint16_t *SOCKET_SendLen ( void )
{

    return  &socket_send_len;
}

char *SOCKET_SendBuf ( void )
{

    return  &socket_send_buf[0];
}


