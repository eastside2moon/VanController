

///////           netdriver_ec600.h


#include "hal_defs_stm.h"


#define  UART3_LEN_MASK   0x03FF
#define  UART3_BUF_LEN  1024
#define  UART3_BUF_ABOUT_FULL  894


void USART3_Init ( void );
void SVR_Connect ( void );
uint8_t  SVR_Status ( void );
uint8_t  NET_Status ( void );
void MOD_SendReq ( void );
extern void App_RcvData (   uint8_t *app_data,       uint16_t len );











