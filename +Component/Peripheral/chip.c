#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "stm32f1xx.h"

#include "chip.h"
#include "hal_defs_stm.h"


uint8_t TempID[16];
uint16_t STM32_FLASH_SIZE;
char Rst_flag_str[16];
//==========================================================//

//函数名称：	Get_ChipID
//函数功能：	读取芯片唯一ID
//入口参数：	无
//返回参数：	无
//说明：

//==========================================================//

char *CHIP_GetId ( void )
{
    uint32_t temp0, temp1, temp2;
    static char ChipID[32];
    temp2 = * ( __IO uint32_t * ) ( 0x1FFFF7E8 ); //产品唯一身份标识寄存器（96位）
    temp1 = * ( __IO uint32_t * ) ( 0x1FFFF7EC );
    temp0 = * ( __IO uint32_t * ) ( 0x1FFFF7F0 );
    //ID码地址： 0x1FFFF7E8   0x1FFFF7EC  0x1FFFF7F0 ，只需要读取这个地址中的数据就可以了。
    STM32_FLASH_SIZE = * ( uint16_t * ) ( 0x1FFFF7E0 );
    snprintf ( &ChipID[0], 32,  "%08X%08X%08X", temp0, temp1, temp2 );
 //  snprintf ( &ChipID[0], 32,  "%08X%08X", temp0, temp1 );
    return &ChipID[0];
}



static  uint8_t Rst_flag = 0;

void CHIP_RstFlagInit ( void )
{
    Rst_flag 	= 	( RCC->CSR )  >> 24;
    memset ( &Rst_flag_str[0], 0, 16 );
	
    if ( __HAL_RCC_GET_FLAG ( RCC_FLAG_PINRST ) )
        strcat ( &Rst_flag_str[0], "PIN_" );

    if ( __HAL_RCC_GET_FLAG ( RCC_FLAG_PORRST ) )
        strcat ( &Rst_flag_str[0], "POR_" );

    if ( __HAL_RCC_GET_FLAG ( RCC_FLAG_SFTRST ) )
        strcat ( &Rst_flag_str[0], "SFT_" );
}

  



uint8_t CHIP_RstFlagRd ( void )
{
    return   Rst_flag;
}


char* CHIP_RstStr ( void )
{
    return   Rst_flag_str;
}

