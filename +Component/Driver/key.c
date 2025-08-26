




//c lib



//mcu



#include "stm32f1xx_hal.h"
#include "main.h"
//other



//self

#include "led.h"
#include "key.h"



static uint8_t key_value;
static uint8_t key_flag;




void key_init ( void )
{
//#if (defined FUNC_KEY) && (FUNC_KEY == FUNC_EN)
//    GPIO_InitTypeDef GPIO_InitStructure;
//    RCC_APB2PeriphClockCmd ( IO_MIX_PT_RCC ( HAL_KEY1_PORT ), ENABLE );
//    GPIO_InitStructure.GPIO_Pin = IO_MIX_PN ( HAL_KEY1_PIN );
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init ( IO_MIX_PT ( HAL_KEY1_PORT ), &GPIO_InitStructure );
//    RCC_APB2PeriphClockCmd ( IO_MIX_PT_RCC ( HAL_KEY2_PORT ), ENABLE );
//    GPIO_InitStructure.GPIO_Pin = IO_MIX_PN ( HAL_KEY2_PIN );
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init ( IO_MIX_PT ( HAL_KEY2_PORT ), &GPIO_InitStructure );
//    RCC_APB2PeriphClockCmd ( IO_MIX_PT_RCC ( HAL_KEY3_PORT ), ENABLE );
//    GPIO_InitStructure.GPIO_Pin = IO_MIX_PN ( HAL_KEY3_PIN );
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init ( IO_MIX_PT ( HAL_KEY3_PORT ), &GPIO_InitStructure );
//    RCC_APB2PeriphClockCmd ( IO_MIX_PT_RCC ( HAL_KEY4_PORT ), ENABLE );
//    GPIO_InitStructure.GPIO_Pin = IO_MIX_PN ( HAL_KEY4_PIN );
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init ( IO_MIX_PT ( HAL_KEY4_PORT ), &GPIO_InitStructure );
//    key_value = 0;
//    key_flag = 0;
//#endif
};

uint8_t key_read ( void )
{
    return key_value;
};




uint8_t key_poll ( void )
{

    static uint8_t key_value_temp;
    static uint8_t key_value_save;
    static uint8_t key_steady_cnt;
    key_value_temp = 0;

	

    if ( HAL_GPIO_ReadPin ( KEY1_GPIO_Port, KEY1_Pin ) == 0 )
        {
            key_value_temp  |= HAL_KEY_1;
        }

    if ( HAL_GPIO_ReadPin ( KEY2_GPIO_Port, KEY2_Pin ) == 0 )
        {
            key_value_temp  |= HAL_KEY_2;
        }

    if ( HAL_GPIO_ReadPin ( KEY3_GPIO_Port, KEY3_Pin ) == 0 )
        {
            key_value_temp  |= HAL_KEY_3;
        }


    ///////////////////////////////////////////////////////////////

    if ( key_value_temp != 0  )
        {
            if (  key_value_save != 	key_value_temp )
                {
                    key_value_save = 	key_value_temp;
                    key_steady_cnt = 0;
                }

            else
                {
                    key_steady_cnt++;
                }
        }

    else     ///////松手
        {
            key_value_save = 0;
            key_steady_cnt = 0;
            key_flag = 0;
        }

    /////////////////////////////////////
    if ( key_steady_cnt   >=   2  )
        {
            key_steady_cnt = 2;

            if (  key_flag == 0 )
                {
                    key_value = key_value_save;
                    key_flag = 1;
                    key_value_save = 0;
                }

            else
                {
                }
        }

    /////////////////////////////////////////////////////////////
    if (  key_flag == 1 )
        {
            app_KeyHandle();
            key_flag = 2;     //等待松手 保证只执行一次
        }


    return 		0;
};




uint8_t key_flag_rd ( void )
{
    return key_flag;
}

void key_flag_set ( uint8_t sts )
{
    key_value  = 0;
    key_flag = 0;
}





void app_KeyHandle ( void )
{
    switch ( key_read() )
        {
            case 5:
           //               MOD_Factory() ;
                break;

            case 1:
                LED_Blink ( HAL_LED_4, 1, 2 );
                break;

            case 2:
                LED_Blink ( HAL_LED_NET, 1, 2 );
			  LED_Set ( HAL_LED_4, HAL_LED_OFF );
                break;

            case 4:
                LED_Blink ( HAL_BEEP, 1, 2);
			  LED_Set ( HAL_LED_4, HAL_LED_ON );
                break;

        }
}







