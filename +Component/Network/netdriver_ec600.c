


#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"


#include "task.h"

#include "uart3_mod.h"
#include "uart1_print.h"
#include "mod_api_ec600.h"
#include "socket_buf.h"
#include "netdriver_ec600.h"






void TASK_Net ( void const *argument );
static uint8_t  Uart_DataProcess ( void );
static void MOD_Send_CMD ( char *str );
static uint8_t UartChrgStr ( char *str, uint8_t len );
static void Mod_RcvProcess ( char *str, uint16_t len );



extern 	osThreadId myTask_netHandle;

#define 	    RESUME_NETTASK  xTaskAbortDelay ( myTask_netHandle );


static uint16_t  idc_cnt = 0;
static uint8_t  mod_idle_flag = 0;
static uint8_t  info_flag = 0;

const uint8_t Next_0_dly	= 0;
const uint8_t Next_1_imly	= 1;




static uint16_t Lte_Run_0_power ( void );
static uint16_t Lte_Run_1_info ( void );
static uint16_t Lte_Run_2_sim ( void );
static uint16_t Lte_Run_3_cnting ( void );
static uint16_t Lte_Run_4_cnted ( void );




typedef  enum
{

    LTE_STS_UNKNOW,
    LTE_STS_ON,
    LTE_STS_SIM,

    LTE_STS_TCP_CNETING,
    LTE_STS_TCP_CNETED,


} SIMCOM_STS_T;



static uint8_t  module_sts	 =	LTE_STS_UNKNOW;
static uint8_t  module_step = 1;



typedef  enum
{

    n0_at,
    n0_ok,
    n1_info,
    n1_firm,
    n1_imei,
    n1_ccid,

    n2_sim,

    ///////////////////////////////////

    n6_reg,
    n6_cnet_ok,
    n6_cnet_ardy,
    n6_ring,
    n6_error,
    n6_cclk,


    n6_deact,

    ///////////////////////////////////
    n7_sts_cnt_ok,
    n7_sts_closed,
    n7_sts_sending,
    n7_sts_beclosed,
    n7_send_ok,
    n7_send_failed,
    n7_sts_rcv,
    n7_csq,


    nx_max
} Sim_str_num_t;


typedef struct
{
    Sim_str_num_t Sim_str_num;
    char *Sim_str_content;

} Sim_str_t;

const Sim_str_t Sim_str[] =
{

    { n0_at,      "AT" },
    { n0_ok,      "OK" },

    { n1_info,    "AT+GMM" },
    { n1_firm,    "AT+GMR" },
    { n1_imei,  "AT+GSN" },
    { n1_ccid,  "+CCID: " },


    { n2_sim,     "+CPIN: READY" },


    ///////////////////////////////////////////

    { n6_reg,     "+CREG: 2,1" },
    { n6_cnet_ok, "CONNECT OK" },
    { n6_cnet_ardy, "ALREADY CONNECT" },
    { n6_ring, "RING" },
    { n6_error, "ERROR"},
    { n6_cclk, 	"+CCLK:"},


    { n6_deact,  "STATE: PDP DEACT" },


    { n7_sts_cnt_ok,   "+QISTATE: 0,\"" },
    { n7_sts_closed,  "STATE: TCP CLOSED" },
    { n7_sts_sending, ">" },
    { n7_sts_beclosed,    "CLOSED" },
    { n7_send_ok,    "SEND OK" },
    { n7_send_failed,   "SEND FAIL" },
    { n7_sts_rcv,    "+QIURC: \"recv\"," },
    { n7_csq,    "+CSQ:" },


    //CONNECT FAIL
    { nx_max,  "Undef" }
};




const uint8_t Chrg_0_swh_num_arr[   ]  =   {n0_at, n0_ok};
const uint8_t Chrg_1_swh_num_arr[   ]  =   {n1_info, n1_firm, n1_imei, n1_ccid };
const uint8_t Chrg_2_swh_num_arr[   ]  =   {n2_sim};

const uint8_t Chrg_3_swh_num_arr[   ]  =
{
    n6_cnet_ok, n6_cnet_ardy, n7_sts_cnt_ok, n7_sts_closed, n7_sts_beclosed,
    n6_ring,  n6_error,	n6_cclk,
    n7_csq,  n6_deact,

};

const uint8_t Chrg_4_swh_num_arr[   ]  =
{
    n6_ring, n6_reg,	n6_cclk,
    n6_cnet_ardy, n7_sts_cnt_ok, n7_sts_closed, n6_deact,
    n7_sts_sending, n7_sts_beclosed, n7_send_ok,
    n7_sts_rcv,
    n7_csq,
};




const uint8_t max_chrg_cnt[] =
{
    sizeof ( Chrg_0_swh_num_arr ) / sizeof ( Chrg_0_swh_num_arr[0] ),
    sizeof ( Chrg_1_swh_num_arr ) / sizeof ( Chrg_1_swh_num_arr[0] ),
    sizeof ( Chrg_2_swh_num_arr ) / sizeof ( Chrg_2_swh_num_arr[0] ),
    sizeof ( Chrg_3_swh_num_arr ) / sizeof ( Chrg_3_swh_num_arr[0] ),
    sizeof ( Chrg_4_swh_num_arr ) / sizeof ( Chrg_4_swh_num_arr[0] ),

};
const  uint8_t *Chrg_swh_ptr[ ] =
{
    Chrg_0_swh_num_arr,
    Chrg_1_swh_num_arr,
    Chrg_2_swh_num_arr,
    Chrg_3_swh_num_arr,
    Chrg_4_swh_num_arr,
};



uint16_t ( *Lte_run[] ) ( void ) =
{
    Lte_Run_0_power,
    Lte_Run_1_info,
    Lte_Run_2_sim,
    Lte_Run_3_cnting,
    Lte_Run_4_cnted,

};



static uint8_t Lte_process_0_pwr (   char *str, uint16_t len );
static uint8_t Lte_process_1_info (  char *str, uint16_t len );
static uint8_t Lte_process_2_sim (  char *str, uint16_t len );
static uint8_t Lte_process_3_cneting ( char *str, uint16_t len );
static uint8_t Lte_process_4_cnted ( char *str, uint16_t len );


static uint8_t ( *Lte_process_func[] ) ( char *str, uint16_t len ) =
{
    Lte_process_0_pwr,
    Lte_process_1_info,
    Lte_process_2_sim,
    Lte_process_3_cneting,
    Lte_process_4_cnted,


} ;





uint16_t Lte_Run_0_power ( void )
{
    static uint16_t next_dly;
    next_dly = 3000;
    LTE_StsIndn ( LTE_ON );

    switch ( module_step )
    {
    case 0:
        break;

    case 1:
    case 2:
    case 3:
        MOD_Send_CMD ( "" );
        USART1_Printf (	"\r\nSend AT" );
        break;

    //////断电一次
    case 4:
        LTE_PwrEN ( LTE_BAT_OFF );
        USART1_Printf (	"\r\nLTE_BAT_OFF" );
        next_dly = 100;
        break;

    ////通电
    case 5:
        LTE_PwrEN ( LTE_BAT_ON );
        USART1_Printf (	"\r\nLTE_BAT_ON" );
        next_dly = 1000;
        break;

    ////开机脉冲
    case 6:
        LTE_PwrPls();
        USART1_Printf (	"\r\nPwrPls" );
        next_dly = 2000;
        break;

    default:
        module_step  = 0;
        break;
    }

    module_step++;
    return next_dly;
}
uint16_t Lte_Run_1_info ( void )
{
    static uint16_t next_dly;
    LTE_StsIndn ( LTE_SIM );

    switch ( module_step )
    {
    case 0:
        //   Simcom_Send_CMD ( "&F" );
        next_dly = 1000;
        break;

    case 1:
        MOD_Send_CMD ( "E1" );
        osDelay ( 200 );
        MOD_Send_CMD ( "+GMR" );
        next_dly = 1000;
        break;

    case 2:
        MOD_Send_CMD ( "+CCID" );
        next_dly = 1000;
        break;//CCID

    case 3:
        MOD_Send_CMD ( "+GSN" );
        next_dly = 2000;
        break; //  IMEI

    case 4:
        MOD_Send_CMD ( "+GMR" );
        next_dly = 2000;
        break;  //soft revision

    case 5:
        MOD_Send_CMD ( "+GMI" );
        next_dly = 2000;
        break;  //soft revision

    case 6:
        MOD_Send_CMD ( "+GMM" );
        next_dly = 2000;
        break;  //soft revision

    case 7:
        MOD_Send_CMD ( "+GOI" );
        next_dly = 2000;
        break;  //soft revision

    case 8:
        //                Simcom_Send_CMD ( "+CIPHEAD=1" );
        next_dly = 200;
        break;

    case 9:
        MOD_Send_CMD ( "I" );
        next_dly = 2000;
        break;

    case 10:
        MOD_Send_CMD ( "+CLIP=1" );
        next_dly = 200;
        break;

    case 11:
        MOD_Send_CMD ( "S0=1" );      //响一次自动应答
        next_dly = 200;
        break;

    case 12:
        MOD_Send_CMD ( "+CREG=2" );
        next_dly = 200;
        break;  //soft revision

    case 13:

        next_dly = 200;
        break;

    default:
        module_step  = 0;
        module_sts = LTE_STS_SIM;
        break;
    }

    module_step++;
    return next_dly;
}



uint16_t Lte_Run_2_sim ( void )
{
    static uint16_t next_dly;
    next_dly = 3000;

    switch ( module_step )
    {
    case 0:
        ;

    case 1:
    case 2:
    case 3:
        MOD_Send_CMD ( "+CPIN?" );
        break;

    case 4:
        break;

    default:
        //直接回去重新开机
        module_step  = 5;
        module_sts = LTE_STS_UNKNOW;
        break;
    }

    module_step++;
    return next_dly;
}


uint16_t Lte_Run_3_cnting ( void )
{
#define START_SOCKET_STR			"+QIOPEN=1,0,\"TCP\",\"%s\",%s,0,1"
    static  char       TCP_DEST_ALL_string[128];
    static uint16_t next_dly;
    const 	uint8_t  lead3_max = 3;   //前面有4个步骤要做
    ////////////////////////////////////////////////////////////////
    next_dly = 2000;

    USART1_Printf (  "\r\n module3_step:%d ", module_step );

    switch ( module_step )
    {
    case 0:
        next_dly = 100;
        break;//IP头

    case 1:
        LTE_StsIndn ( LTE_CNETING );
        MOD_Send_CMD ( "+CCLK?" );
        next_dly = 100;
        break;//快发

    case 2:
        MOD_Send_CMD ( "+CREG?" );
        next_dly = 100;
        break;//电压

    case 3:
        MOD_Send_CMD ( "+CSQ" );
        next_dly = 100;
        break;//快发

    ////////////////////////////////////////////////////////////////////
    case lead3_max+1:
        MOD_Send_CMD ( "+QICLOSE=0" ); //
        next_dly = 2000;
        break;

    ///////////579
    case lead3_max+2:
    case lead3_max+4:
    case lead3_max+6:
        if (  strlen (  SERVER_Info()->addr  ) )
        {
            snprintf ( TCP_DEST_ALL_string, 128,
                       START_SOCKET_STR,
                       SERVER_Info()->addr,
                       SERVER_Info()->port
                     );
            //      ESP_Send_CMD (  ( char * ) TCP_DEST_ALL_string );
            MOD_Send_CMD ( ( char * ) TCP_DEST_ALL_string );
        }




        else
        {
            USART1_Printf ( "\r\n No server ip " );
        }

        next_dly = 5000;
        break;

    //// 6 8 10
    case lead3_max+3:
    case lead3_max+5:
    case lead3_max+7:
        MOD_Send_CMD ( "+QISTATE" );
        next_dly = 1000;
        break;

    default:
        ////////////////////反复连不上，不连了	,检查网络情况
        module_step  = 0;
        module_sts = LTE_STS_UNKNOW;
        USART1_Printf (  "\r\n default 6 "  );
        break;
    }

    module_step++;
    return next_dly;
}

uint16_t Lte_Run_4_cnted ( void )
{
    static uint16_t next_dly;
    char  app_len_buf[8];
    //   static    send_buf_arr_t  *send_buf_temp;
    const 	uint8_t lead4_max = 2;   //前面有3个步骤要做
    /////////////////////////////////////
    next_dly = 1000;
    LTE_StsIndn ( LTE_SVR );
    mod_idle_flag = 0;    //标识模组不空

    switch ( module_step )
    {
    case 0:

        break;

    case 1:
        MOD_Send_CMD ( "+CSQ" );
        break;//信号

    case 2:

        MOD_Send_CMD ( "+CREG?" );

        break;//电压

    //            case 3:
    //                MOD_Send_CMD ( "+CCLK?" );

    //                break;

    /////////////////////////////////////////////////////////////////////////////////
    case lead4_max+1:


        if ( *SOCKET_SendLen() > 0 )
        {
            Mod_UartSendBuf ( "AT+QISEND=0,", 12 );
            snprintf ( &app_len_buf[0], 7, "%d",   *SOCKET_SendLen() );
            Mod_UartSendBuf ( ( char * ) &app_len_buf[0], strlen ( ( char * ) &app_len_buf[0] ) );
            Mod_UartSendBuf ( "\r\n", 2 );

        }

        else
        {
            mod_idle_flag = 1;
            module_step++;
            next_dly = 10000;        //如果没事，10秒后再说，等着吧  查查网络状态
        }

        break;

    case lead4_max+2:

        Mod_UartSendBuf ( SOCKET_SendBuf(),   *SOCKET_SendLen() );

        *SOCKET_SendLen() = 0;
        next_dly = 3000;
        break;

    case lead4_max+3:
    case lead4_max+4:
    case lead4_max+5:
        MOD_Send_CMD ( "+QISTATE" );
        next_dly = 5000;
        break;

    case lead4_max+6:
        module_step  = 0;
        module_sts = LTE_STS_TCP_CNETING;
        //         net_close_cnt++;
        break;

    default:
        break;
    }

    module_step++;
    return next_dly;
}


void TASK_Net ( void const *argument )
{
    static uint8_t  pro_cnt = 0;
    static uint16_t self_next = 1000;       ////1000ms
    static uint32_t  OSTimeGet_start, OSTimeGet_used;
    static uint8_t  Wait = 0;
    LTE_ModInit (
    );

    for ( ;; )
    {
        if ( Wait ==	Next_0_dly )
        {
            OSTimeGet_start	=	 xTaskGetTickCount();
            osDelay ( self_next );
        }

        else
        {
            Wait =	Next_0_dly	;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////

        if ( USART3_StsRd()	==	1 )
        {
            pro_cnt = 0;

            while ( USART3_RdIdxRd()	!=	USART3_WrIdxRd() )
            {
                Wait  |=  Uart_DataProcess();
                //  if ( pro_cnt == 0 )
                {
                }
                pro_cnt++;
            }

            USART3_StsSet ( 0 );  //处理完了清除标志位，准备下一次
            OSTimeGet_used	=	 xTaskGetTickCount() - OSTimeGet_start;

            if ( OSTimeGet_used < self_next )
            {
                self_next -=	OSTimeGet_used;
            }
        }

        else
        {
            Wait	=	  Next_0_dly;
            self_next	=	( *Lte_run[module_sts] ) ();
        }

        //     USART1_Printf ( "module_sts_step  %d---%d---%d\r\n",  module_sts, module_step, self_next );
    }
}





void uart_indication ( void )
{
    extern 	uint8_t   uart_rcvflag ;
    idc_cnt++;

    if ( USART3_StsRd()  )
    {
        RESUME_NETTASK;
    }

    else
    {
        USART1_Printf (  "\r\n uart2_sts_busy..." );
    }
}





void MOD_Send_CMD (  char *str )
{
    //    HAL_UART_Transmit (	&huart3, ( uint8_t * ) "AT", 2, 1000 )  ;
    //    HAL_UART_Transmit (	&huart3, ( uint8_t * ) str, strlen ( str ), 1000 )  ;
    //    HAL_UART_Transmit (	&huart3, ( uint8_t * ) "\r\n", 2, 1000 )  ;
    Mod_UartSendBuf (	"AT", 2 );
    Mod_UartSendBuf (	str, strlen ( str ) );
    Mod_UartSendBuf (	"\r\n", 2 );
}




uint8_t  Uart_DataProcess ( void )
{
    extern uint8_t  *USART3_RcvBufPtr ( void )	;
    uint16_t len,	unread_len;
    static char  sim_uart_prcs_buf[UART3_BUF_LEN];
    uint8_t rlt	=	Next_0_dly;
    unread_len	 =	( uint16_t ) ( USART3_WrIdxRd() - USART3_RdIdxRd() & UART3_LEN_MASK );

    for ( len = 0; len < unread_len; len++ )
    {
        sim_uart_prcs_buf[len]	=	  USART3_RcvBufPtr() [ ( USART3_RdIdxRd() + len ) & UART3_LEN_MASK ];

        if ( ( sim_uart_prcs_buf[len - 1]	==	0x0D )	&&
                ( sim_uart_prcs_buf[len - 0]	==	0x0A )	&&
                ( 1 )
           )
            break;
    }

    ////////////////////////////////////////////////
    if ( len < unread_len )	 //打断出来的
    {
        sim_uart_prcs_buf[len + 1] = 0;
        USART3_RdIdxAdd ( len + 1 );
        rlt	=	( *Lte_process_func[module_sts] ) ( ( char * ) sim_uart_prcs_buf, len + 1 );
    }

    else   //没找到帧尾，可能就是错误帧
    {
        USART3_RdIdxSet ( USART3_WrIdxRd() );
        rlt	=	( *Lte_process_func[module_sts] ) ( ( char * ) sim_uart_prcs_buf, len  );
    }

    return rlt;
}



uint8_t Lte_process_0_pwr (   char *str, uint16_t len )
{
    uint8_t str_num = 0xFD;
    str_num--;
    str_num	=	UartChrgStr (	str, strlen ( ( char * ) str )	);

    if ( str_num	==	n0_at )
    {
        if ( info_flag == 0  )
        {
            module_sts	=	LTE_STS_ON;
            info_flag = 1;
        }

        else		/////如果不是第一次，就是直接去查sim卡
        {
            module_sts	=	LTE_STS_SIM;
        }

        module_step	=	0;
        return Next_0_dly;
    }

    return Next_0_dly;
    //	  USART1_Printf (  "\r\n str %s",  str );
    //
    //
    //    return Next_0_dly;
}
uint8_t Lte_process_1_info (  char *str, uint16_t len )
{
    char temp_buf[32];
    uint8_t str_num = 0xFD;
    uint8_t 	i;
    uint8_t ch;
    uint16_t	  unread1, unread2;
    //////////////////////////////////////////////////
    str_num--;
    str_num	=	UartChrgStr (	str, strlen ( ( char * ) str )	);

    switch ( str_num )
    {
    case 	n1_imei	:
        unread1  =   USART3_UnreadLen()  ;
        unread2  =  unread1  ;
        i = 5;

        //////有长度了，或者尝试次数够5次了
        while (  ( i-- ) && ( unread2 == 0 ) )
        {
            osDelay ( 5 );
            unread2  =   USART3_UnreadLen()  ;
        }

        //////////先剔除可能存在的回车
        for (   i = 0; i < 6; i++ )
        {
            USART3_RdByte ( ( uint8_t * ) &temp_buf[0]  );

            if (  isalnum ( temp_buf[0] )    )   break;
        }

        ////////////////////////////////   从1开始读取了
        for (   i = 1; i < 15; i++ )
        {
            USART3_RdByte ( ( uint8_t * ) &temp_buf[i]  );
        }

        temp_buf[i] = 0;
        /////////////////////////////////////////////////////////
        LTE_AlysImei ( &temp_buf[0], 16  );
        USART1_Printf (  "\r\n get imei: %d---%d---%16s",  unread1,   unread2, &temp_buf[0] );
        break;

    case 	n1_ccid	:
        /////////////////////////////////////////////////////////
        LTE_AlysCcid ( &str[7], 21  );
        USART1_Printf (  "\r\n get ccid: %20s",   &str[7]     );
        break;

    case n1_firm:
        for (   i = 0; i < 32; i++ )
        {
            USART3_RdByte ( &ch );

            if ( isgraph ( ch )   )
            {
                temp_buf[i] =   ch;
            }

            else
            {
                temp_buf[i] =   0;  //加上结束符
                break;
            }
        }


        return Next_1_imly;

    case n1_info:
        for (   i = 0; i < 32; i++ )
        {
            USART3_RdByte ( &ch );

            if ( isgraph ( ch )   )
            {
                temp_buf[i] =   ch;
            }

            else
            {
                temp_buf[i] =   0;  //加上结束符
                break;
            }
        }


        return Next_1_imly;

    default:
        break;
    }

    //////////////////////////////////////////////////
    return Next_0_dly;
}


uint8_t Lte_process_2_sim (  char *str, uint16_t len )
{
    uint8_t str_num = 0xFD;
    str_num--;
    str_num	=	UartChrgStr (	str, strlen ( ( char * ) str )	);

    if ( str_num	==	n2_sim )
    {
        module_sts	=	LTE_STS_TCP_CNETING;
        module_step	=	0;
        return Next_1_imly;
    }

    return Next_0_dly;
}


static uint8_t Lte_process_3_cneting ( char *str, uint16_t len )
{
    uint8_t str_num = 0xFD;
    str_num--;
    str_num	=	UartChrgStr (	str, strlen ( ( char * ) str )	);

    switch ( str_num )
    {
    case n6_cnet_ok:
    case n7_sts_cnt_ok:
        module_sts	=	LTE_STS_TCP_CNETED;
        module_step	=	0;
        return Next_1_imly;

    case n6_cnet_ardy:
    case n6_deact:
        MOD_Send_CMD ( "+QICLOSE=0" );
        module_sts = LTE_STS_SIM;
        module_step = 6;
        break;

    //            case n7_csq:
    //                LTE_AlysCsq ( str,  len  );
    //                break;



    case n6_cclk:
        //          Clock_Ntp_Set ( str );
        return Next_1_imly;



    ///////////////////////////////////////
    default:
        break;
    }

    return Next_0_dly;
}



static uint8_t Lte_process_4_cnted ( char *str, uint16_t len )
{
    uint8_t str_num = 0xFD;
    str_num--;
    str_num	=	UartChrgStr (	str, strlen ( ( char * ) str )	);

    switch ( str_num )
    {
    case n7_sts_cnt_ok:
        module_step = 1;
        return Next_1_imly;

    case n7_sts_sending:
        return Next_1_imly;

    case n7_send_ok:
        return Next_1_imly;

    case n7_sts_closed:
        module_sts	=	LTE_STS_TCP_CNETING;
        module_step	=	8;
        return Next_1_imly;

    case n6_deact:
        module_sts = LTE_STS_SIM;
        MOD_Send_CMD ( "+QICLOSE=0" );
        module_step = 5;
        return Next_1_imly;

    case n7_csq:
        LTE_AlysCsq ( str,  len  );
        return Next_1_imly;

    case n7_sts_rcv:
        Mod_RcvProcess ( str,  len );
        //	USART1_Printf(	"\r\n n7_sts_rcv  %d",  len	);
        break;

    case n6_reg:
        LTE_AlysCreg (  str, len );
        return Next_1_imly;

    case n6_cclk:
        //                Clock_Ntp_Set ( str );
        //    USART1_Printf (	"\r\n ntp  %.*s",  len	,  str );
        return Next_1_imly;

    default:
        break;
    }

    return Next_0_dly;
}



static uint8_t UartChrgStr ( char *str, uint8_t len )
{
    static uint8_t  rlt  = nx_max;
    static uint8_t i;
    char *rlt_ptr;

    for (  i = 0; i < max_chrg_cnt[module_sts]; ++i )
    {
        //这句设计非常关键,也非常巧妙,全是指针，数组，字符串,枚举
        //甚至整个函数以及相关配套的都很巧妙，说不定以后我自己都看不明白
        rlt_ptr   =   strstr (  str,  \
                                ( char * ) Sim_str[ ( Chrg_swh_ptr[module_sts] ) [i] ].Sim_str_content   );

        //

        if ( rlt_ptr == str )
        {
            return Sim_str[  ( Chrg_swh_ptr[module_sts] ) [i] ].Sim_str_num;
        }
    }

    if ( rlt >= nx_max )   rlt = nx_max;

    return rlt;
}






void SVR_Connect ( void )
{
    ///  2019年8月25日21:14:52  SIMCOM_STS_TCP_CNETING的时候不能这样
    ///  if ( (  module_sts == SIMCOM_STS_TCP_CNETING  )  ||  (  	module_sts == SIMCOM_STS_TCP_CNETED	     ) )
    if (    	module_sts == LTE_STS_TCP_CNETED	      )
    {
        module_sts = LTE_STS_TCP_CNETING;
        module_step = 4;
        RESUME_NETTASK  ;
    }
}



uint8_t  SVR_Status ( void )
{
    if ( ( module_sts ==  LTE_STS_TCP_CNETED )    &&
            ( 1 ) )
    {
        return 1;
    }

    return 0;
}



uint8_t  NET_Status ( void )
{
    if  ( module_sts >=  LTE_STS_TCP_CNETING )
    {
        return 		1;
    }

    return 		0;
}


void MOD_SendReq ( void )
{
    //////////模块空闲状态，并且是连上tcp状态后，不然会打乱前面几个状态
    if ( ( mod_idle_flag == 1 )  && ( module_sts	==	LTE_STS_TCP_CNETED )  )
    {
        // OSSemPost ( Sim_Fun_Semp );
        RESUME_NETTASK;
    }
}








void Mod_RcvProcess ( char *str, uint16_t len )
{
    ///定义相关变量
    static	uint16_t    pay_wr_index;        /////模组收到的数据，最大1460字节
    char *app_data;
    //  static    char app_data[1024];
    static	    uint16_t  app_data_len;
    uint16_t  	i, unread_len;
    ///   1.获取模组原始数据到缓存
    app_data_len =  atoi (  ( ( char * ) str ) + 17  );    //+QIURC: "recv",0,4
    //    fst_cln_pos = strchr ( ( char * ) str,  ':' );
    //    len -= fst_cln_pos - ( char * ) str  + 1;	 ///去掉模组带了头部长度  类似+IPD,4:  +IPD,62:     +1就是冒号本身
    //    len =    ( ( ( len ) < ( app_data_len ) ) ? ( len ) : ( app_data_len ) );
    //// 2. 分配内存，准备接收数据
    app_data = malloc ( app_data_len );

    if ( !app_data )
    {
        return ;
    }


    pay_wr_index = 0;

    if ( pay_wr_index < app_data_len )        // 说明不够，还有的被流量控制了
    {

        unread_len =	USART3_UnreadLen();

        while ( unread_len  )
        {
            //

            /////////////////////////////
            for (  i = 0;  i < unread_len;  i++ )
            {
                USART3_RdByte ( ( uint8_t * ) ( app_data + pay_wr_index ) )	;
                pay_wr_index++;

                if ( pay_wr_index >= app_data_len )
                {
                    break;
                }
            }

            if ( pay_wr_index >= app_data_len )
            {
                break;
            }

            ////2.  放开流量控制，让新数据进来
            //                    USART3_RtsEn ( 192 );
            ////3.  更新未读长度
            unread_len =	USART3_UnreadLen() 	;
            USART1_Printf ( "\r\n unread_len:%d", unread_len );
        }
    }

    //		USART1_Printf ( "\r\n pay_wr_index:%d", pay_wr_index );
    //////5.交后面程序处理载荷
    {
        App_RcvData (   (uint8_t*)app_data, pay_wr_index );
        //    Socket0_BufAdd (   app_data,       pay_wr_index );
    }
    //////6.释放相关内存
    free ( app_data );
}










