


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "main.h"
#include "uart3_mod.h"
#include "led.h"
///////////////////////////////////
#include "mod_api_ec600.h"


static uint8_t 	mod_rssi = 0;

static uint8_t 	reg_sts = 0;

static char mod_imei[15 + 1] = "123456789012345";
static char mod_ccid[20 + 1] = "12345678909876543210";
static char mod_lac[8] = "0000";
static char mod_cell[16] = "0000";


#define MAX_MSG_CNT 8
#define MAX_MSG_LEN 12
static char cmd_buf[MAX_MSG_CNT] [MAX_MSG_LEN];


///////////////////////////////////////////////////////////////////

//读取信号
uint8_t LTE_ReadRssi ( void )
{
    return mod_rssi;
}
//解析信号字符串
//  示例  +CSQ: 30,0
void  LTE_AlysCsq ( char *str, uint8_t len  )
{
    mod_rssi = atoi (  ( ( char * ) str ) + 6  );
}


////////////解析注册信息
//  示例 +CREG: 2,1,"3338","7A1F"
char  LTE_AlysCreg ( char *str, uint8_t len  )
{

    int32_t new_field;


    if(str != NULL)
    {

        /* 1. clear the cmd_buf[][] buffer */
        for (int8_t i = 0; i < MAX_MSG_CNT; i++)
        {
            (void)memset(cmd_buf[i], 0, (size_t)MAX_MSG_LEN);
        }
        ////2.分解数据
        for (int32_t i = 0, j = 0, k = 0; (str[i] != (uint8_t)'\n'); i++)
        {
            new_field = 0;

            if ((str[i] == (uint8_t)',') || (str[i] == (uint8_t)' ') )
            {
                cmd_buf[j][k] = (uint8_t)'\0';
                new_field = 1;
                j++;
                k = 0;

            }
            ////////////////////////
            if(new_field == 0)
            {
                cmd_buf[j][k] = str[i];
                k++;
            }
        }

        /////3.转换数据
        {

            reg_sts = atoi ( (char *)cmd_buf[2]  );

            memset ( &mod_lac[0], 0, 8  );
            strncpy ( &mod_lac[0], (char *)cmd_buf[3], 8 );

            memset ( &mod_cell[0], 0, 16  );
            strncpy ( &mod_cell[0], (char *)cmd_buf[4],  16 );



        }
    }

    return reg_sts;

}



char		*LTE_ReadLac ( void )
{
    return 	&mod_lac[0];
}

char		*LTE_ReadCell ( void )
{
    return 	&mod_cell[0];
}






//解析IMEI字符串
//  示例  865473039449288
void  LTE_AlysImei ( char *str, uint8_t len  )
{
    memcpy ( &mod_imei[0], str,  15  );
    //	USART1_PrintfBuf(  (uint8_t*)str,  15  );
}
/////读取IMEI
char *LTE_ReadImei ( void )
{
    return  &mod_imei[0] ;
}



//解析CCID字符串
//  示例  865473039449288

void  LTE_AlysCcid ( char *str, uint8_t len  )
{
    memcpy ( &mod_ccid[0], str,  20  );
    //	USART1_PrintfBuf(  (uint8_t*)str,  20  );
}
char *LTE_ReadCcid ( void )
{
    return  ( &mod_ccid[0] );
}



void LTE_ModInit ( void )
{
    USART3_Init();
    LTE_PwrEN (	LTE_BAT_ON	);
    LTE_PwrPls();
}


//////////////////// 模块电源使能，不一定都有，看板子硬件设计
void LTE_PwrEN ( GPIO_PinState PinState )
{
    ////打开模组电源
    HAL_GPIO_WritePin ( MOD_VCC_GPIO_Port, MOD_VCC_Pin, PinState );
}




//////////////////// 开机脉冲

void LTE_PwrPls ( void )
{
    /////开机脉冲
    HAL_GPIO_WritePin ( MOD_PWRKEY_GPIO_Port, MOD_PWRKEY_Pin, GPIO_PIN_RESET );
    osDelay ( 1000 );
    HAL_GPIO_WritePin ( MOD_PWRKEY_GPIO_Port, MOD_PWRKEY_Pin, GPIO_PIN_SET );
}





void Mod_UartSendBuf ( char *str, uint16_t Len )
{
    extern 	UART_HandleTypeDef huart3;
    HAL_UART_Transmit (	&huart3, ( uint8_t * ) str, Len, 1000 )  ;
}




uint8_t LTE_StsIndn ( LTE_STS_T sts )
{
    switch ( sts )
    {
    case LTE_ON    :
        LED_Blink 	( HAL_LED_NET, 0xFF, 5 );
        LED_Set 	( HAL_LED_SVR, HAL_LED_OFF );
        break;

    case LTE_SIM    :
        LED_Blink 	( HAL_LED_NET, 0xFF, 2 );
        LED_Set 	( HAL_LED_SVR, HAL_LED_OFF );
        break;

    case LTE_CNETING    :
        LED_Set 	( HAL_LED_NET, HAL_LED_ON );
        LED_Blink 	( HAL_LED_SVR, 0xFF, 2 );
        break;

    case LTE_SVR    :
        LED_Set 	( HAL_LED_NET, HAL_LED_ON );
        LED_Set 	( HAL_LED_SVR, HAL_LED_ON );
        break;


    ////////////////////////////////////////////
    default:
        break;
    }

    return 0;
}


