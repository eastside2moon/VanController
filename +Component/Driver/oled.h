#ifndef _OLED_H_
#define _OLED_H_





#define OLED_IO					SPI1



#define 	LCD_MAX_LINE_LENGTH		16



typedef struct
{

	_Bool device_ok;

} OLED_INFO;

extern OLED_INFO oled_info;




#define		OLCD_RST_OFF	HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,  GPIO_PIN_SET);
#define		OLCD_RST_ON		HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,  GPIO_PIN_RESET);

#define		OLCD_AO_H		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin,  GPIO_PIN_SET);
#define		OLCD_AO_L		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port,SPI1_CS_Pin,  GPIO_PIN_RESET);






void OLED_Init(void);

_Bool OLED_Exist(void);

void OLED_ClearScreen(void);

void OLED_ClearAt(unsigned char x);

void OLED_Dis12864_Pic(const unsigned char *dp);


void OLED_DisString8x16(unsigned char x, unsigned char y, char *fmt, ...);
void OLED_DisStringFmt(unsigned char line,  char *fmt, ...);

#endif
