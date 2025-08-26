
typedef  enum
{

    LTE_ON,
    LTE_SIM,
    LTE_CNETING,
    LTE_SVR,

} LTE_STS_T;




#define    LTE_BAT_ON		GPIO_PIN_RESET
#define    LTE_BAT_OFF		GPIO_PIN_SET




uint8_t LTE_StsIndn ( LTE_STS_T sts );

void Mod_UartSendBuf ( char *str, uint16_t Len );


void LTE_ModInit ( void );
void LTE_PwrEN ( GPIO_PinState PinState  );
void LTE_PwrPls ( void );



char  	LTE_AlysCreg ( char *str, uint8_t len  );
char		*LTE_ReadLac ( void );
char		*LTE_ReadCell ( void );

//////解析和读取   信号
void  LTE_AlysCsq ( char *str, uint8_t len  );
uint8_t  	LTE_ReadRssi ( void );


//////解析和读取   IMEI
char		*LTE_ReadImei ( void );
void  LTE_AlysImei ( char *str, uint8_t len  );

//////解析和读取   CCID
void  LTE_AlysCcid ( char *str, uint8_t len  );

char		*LTE_ReadCcid ( void );


