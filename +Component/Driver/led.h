
//#include "IO.h"
//#include "prj_config.h"
#include "hal_defs_stm.h"

//////////////1111111111111111111111111111111111111111

#define LED1_TURN_ON()    st(  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); 	)
#define LED1_TURN_OFF()   st(  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); 		)

//////////////222222222222222222222222222222222222222222

#define LED2_TURN_ON()    st(  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); 	)
#define LED2_TURN_OFF()   st(  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); 		)

//////////////3333333333333333333333333333333

#define LED3_TURN_ON()    st(  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET); 	)
#define LED3_TURN_OFF()   st(  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET); 		)

//////////////44444444444444444444444444444444444444

#define LED4_TURN_ON()    st(  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET); 	)
#define LED4_TURN_OFF()   st(  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET); 		)

//////////////////////55555555555555555555555555

#define LED5_TURN_ON()    st(  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET); 	)
#define LED5_TURN_OFF()   st(  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET); 		)





#define HAL_LED_1	 0x01
#define HAL_LED_2	 0x02
#define HAL_LED_3	 0x04
#define HAL_LED_4	 0x08
#define HAL_LED_5	 0x10
#define HAL_LED_ALL	 0x1F



#define HAL_LED_RUN  HAL_LED_1
#define HAL_LED_NET  HAL_LED_2
#define HAL_LED_SVR  HAL_LED_3
#define HAL_BEEP	 HAL_LED_5


#define HAL_LED_OFF	 0x00
#define HAL_LED_ON	 0x01



void LED_Set ( uint8_t led, uint8_t mode );
void LED_Blink ( uint8_t led, uint8_t time, uint8_t period );



