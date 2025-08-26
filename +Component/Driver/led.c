











//c lib



//mcu




//other





#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "main.h"


#include "key.h"
#include "led.h"


#define LED_CNT   5


static uint8_t led_sts[LED_CNT] = {0, 0, 0, 0, 0,};
static uint8_t LED_Blink_time[LED_CNT] = {0, 0, 0, 0, 0,};
static uint8_t LED_Blink_period[LED_CNT] = {5, 5, 5, 5, 5,};;
static uint8_t LED_Blink_check[LED_CNT] = {0, 0, 0, 0, 0,};



static  void Led_OnOff ( uint8_t leds, uint8_t mode );
void TASK_Led ( void const *argument );



//LED任务----------------------------------------
void TASK_Led ( void const *argument )
{
    uint8_t i = 0;
    ////开机自检一下LED
    Led_OnOff ( HAL_LED_ALL, HAL_LED_OFF );

    for (  i = 0; i < LED_CNT; i++ )
        {
            Led_OnOff ( 1 << i, HAL_LED_ON );
            osDelay ( 200 );
            Led_OnOff ( 1 << i, HAL_LED_OFF );
        }


    //////////////////////////////////////////
    LED_Blink ( HAL_LED_1, 0xFF, 5 );

    for ( ;; )
        {
            osDelay ( 100 );
            key_poll();

            for ( i = 0; i < LED_CNT; i++ )
                {
                    if ( ( LED_Blink_time[i] > 0 ) && LED_Blink_period[i] != 0 )
                        {
                            if ( ++LED_Blink_check[i] >= LED_Blink_period[i]  )
                                {
                                    led_sts[ i ] ^= 0x01;
                                    LED_Blink_check[i] = 0;

                                    if ( LED_Blink_time[i] != 0xFF )
                                        {


                        if ( ( led_sts[i] & 0x01 ) == 0x00 )
                                                {
                                                    LED_Blink_time[i]--;
                                                }
                                        }
                                }
                        }

                    Led_OnOff ( 1 << i, led_sts[ i ] & 0x01 );
                }
        }
}


void Led_OnOff ( uint8_t leds, uint8_t mode )
{
    //#if (defined FUNC_LED) && (FUNC_LED == FUNC_EN)
    //    ///////////////////////////////////////
    if ( leds & HAL_LED_1 )
        {
            if ( mode == HAL_LED_ON )
                {
                    LED1_TURN_ON();
                }

            else
                {
                    LED1_TURN_OFF();
                }
        }

    if ( leds & HAL_LED_2 )
        {
            if ( mode == HAL_LED_ON )
                {
                    LED2_TURN_ON();
                }

            else
                {
                    LED2_TURN_OFF();
                }
        }

    ///////////////////////////////////////

    if ( leds & HAL_LED_3 )
        {
            if ( mode == HAL_LED_ON )
                {
                    LED3_TURN_ON();
                }

            else
                {
                    LED3_TURN_OFF();
                }
        }

    ///////////////////////////////////////

    if ( leds & HAL_LED_4 )
        {
            if ( mode == HAL_LED_ON )
                {
                    LED4_TURN_ON();
                }

            else
                {
                    LED4_TURN_OFF();
                }
        }

    ///////////////////////////////////////

    if ( leds & HAL_LED_5 )
        {
            if ( mode == HAL_LED_ON )
                {
                    LED5_TURN_ON();
                }

            else
                {
                    LED5_TURN_OFF();
                }
        }

    //	#endif
}



void LED_Blink ( uint8_t led, uint8_t time, uint8_t period )
{
    uint8_t i;

    for ( i = 0; i < LED_CNT; i++ )
        {
            if ( ( led >> i ) & 0x01 )
                {

			led_sts[ i ] = 0x00;

                    LED_Blink_time[i] = time;
                    LED_Blink_period[i] = period ;
                    LED_Blink_check[i] = 0;
                }
        }
}

void LED_Set ( uint8_t led, uint8_t mode )
{
    uint8_t i;

    for ( i = 0; i < LED_CNT; i++ )
        {
            if ( ( led >> i ) & 0x01 )
                {
                    led_sts[ i ] = mode;
                    LED_Blink_time[i] = 0;
                }
        }
}
