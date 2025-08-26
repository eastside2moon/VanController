




#define HAL_KEY_1	 0x01
#define HAL_KEY_2	 0x02
#define HAL_KEY_3	 0x04
#define HAL_KEY_4	 0x08




void key_init ( void );
uint8_t key_read ( void );
uint8_t key_poll ( void );

uint8_t key_flag_rd ( void );
void key_flag_set ( uint8_t sts );

void app_KeyHandle ( void );
