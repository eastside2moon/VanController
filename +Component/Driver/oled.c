

//
#include	<string.h>
#include "stm32f1xx_hal.h"

//OS
#include "cmsis_os.h"
#include "main.h"
//����


//Ӳ��
#include "oled.h"

//�ֿ�
#include "font.h"

//C��
#include <stdarg.h>
#include <stdio.h>


OLED_INFO oled_info;


extern SPI_HandleTypeDef hspi1;


/*
************************************************************
	�������ƣ�	OLED_Delay

	�������ܣ�	OLED��ʱ

	��ڲ�����	time����ʱʱ��

	���ز�����	��

	˵����		���ڵ�ǰ��ʱʱ��
************************************************************
*/
__inline static void OLED_Delay ( unsigned int time )
{
    osDelay ( time );
}

/*
************************************************************
	�������ƣ�	OLED_WriteData

	�������ܣ�	OLEDд��һ������

	��ڲ�����	byte����Ҫд�������

	���ز�����	д����

	˵����		0-�ɹ�		1-ʧ��
************************************************************
*/
_Bool OLED_WriteData ( unsigned char byte )
{
    OLCD_AO_H;
    HAL_SPI_Transmit ( &hspi1, &byte, 1, 2 );
    return 0;
}

/*
************************************************************
	�������ƣ�	OLED_WriteCom

	�������ܣ�	OLEDд��һ������

	��ڲ�����	cmd����Ҫд�������

	���ز�����	д����

	˵����		0-�ɹ�		1-ʧ��
************************************************************
*/
_Bool OLED_WriteCom ( unsigned char cmd )
{
    OLCD_AO_L;
    HAL_SPI_Transmit ( &hspi1, &cmd, 1, 2 );
    return 0;
}

void OLED_Reset ( void )
{
    OLCD_RST_ON;
    OLED_Delay ( 20 );
    OLCD_RST_OFF;
    OLED_Delay ( 10 );
}

/*
************************************************************
	�������ƣ�	OLED_Init

	�������ܣ�	OLED��ʼ��

	��ڲ�����	��

	���ز�����	��

	˵����
************************************************************
*/
void OLED_Init ( void )
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin ( GPIOA, GPIO_PIN_4 | GPIO_PIN_6, GPIO_PIN_RESET );
    /*Configure GPIO pins : PA4 PA6 */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init ( GPIOA, &GPIO_InitStruct );
    OLED_Reset();
#if 1
    OLED_WriteCom ( 0xAE ); //�ر���ʾ
    OLED_WriteCom ( 0x20 ); //Set Memory Addressing Mode
    OLED_WriteCom ( 0x10 ); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    OLED_WriteCom ( 0xb0 ); //Set Page Start Address for Page Addressing Mode,0-7
    OLED_WriteCom ( 0xa1 ); //0xa0��X��������ʾ��0xa1��X�᾵����ʾ
    OLED_WriteCom ( 0xc8 ); //0xc0��Y��������ʾ��0xc8��Y�᾵����ʾ
    OLED_WriteCom ( 0x00 ); //�����е�ַ��4λ
    OLED_WriteCom ( 0x10 ); //�����е�ַ��4λ
    OLED_WriteCom ( 0x40 ); //������ʼ�ߵ�ַ
    OLED_WriteCom ( 0x81 ); //���öԱȶ�ֵ
    OLED_WriteCom ( 0x7f ); //------
    OLED_WriteCom ( 0xa6 ); //0xa6,������ʾģʽ;0xa7��
    OLED_WriteCom ( 0xa8 ); //--set multiplex ratio(1 to 64)
    OLED_WriteCom ( 0x3F ); //------
    OLED_WriteCom ( 0xa4 ); //0xa4,��ʾ����RAM�ĸı���ı�;0xa5,��ʾ���ݺ���RAM������
    OLED_WriteCom ( 0xd3 ); //������ʾƫ��
    OLED_WriteCom ( 0x00 ); //------
    OLED_WriteCom ( 0xd5 ); //�����ڲ���ʾʱ��Ƶ��
    OLED_WriteCom ( 0xf0 ); //------
    OLED_WriteCom ( 0xd9 ); //--set pre-charge period//
    OLED_WriteCom ( 0x22 ); //------
    OLED_WriteCom ( 0xda ); //--set com pins hardware configuration//
    OLED_WriteCom ( 0x12 ); //------
    OLED_WriteCom ( 0xdb ); //--set vcomh//
    OLED_WriteCom ( 0x20 ); //------
    OLED_WriteCom ( 0x8d ); //--set DC-DC enable//
    OLED_WriteCom ( 0x14 ); //------
    OLED_WriteCom ( 0xaf ); //����ʾ
#else
    OLED_WriteCom ( 0xAE ); //display off
    OLED_WriteCom ( 0x00 );	//Set Memory Addressing Mode
    OLED_WriteCom ( 0x10 );	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    OLED_WriteCom ( 0x40 );	//Set Page Start Address for Page Addressing Mode,0-7
    OLED_WriteCom ( 0xb0 );	//Set COM Output Scan Direction
    OLED_WriteCom ( 0x81 ); //---set low column address
    OLED_WriteCom ( 0xff ); //---set high column address
    OLED_WriteCom ( 0xa1 ); //--set start line address
    OLED_WriteCom ( 0xa6 ); //--set contrast control register
    OLED_WriteCom ( 0xa8 );
    OLED_WriteCom ( 0x3f ); //--set segment re-map 0 to 127
    OLED_WriteCom ( 0xad ); //--set normal display
    OLED_WriteCom ( 0x8b ); //--set multiplex ratio(1 to 64)
    OLED_WriteCom ( 0x33 ); //
    OLED_WriteCom ( 0xc8 ); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    OLED_WriteCom ( 0xd3 ); //-set display offset
    OLED_WriteCom ( 0x00 ); //-not offset
    OLED_WriteCom ( 0xd5 ); //--set display clock divide ratio/oscillator frequency
    OLED_WriteCom ( 0x80 ); //--set divide ratio
    OLED_WriteCom ( 0xd9 ); //--set pre-charge period
    OLED_WriteCom ( 0x1f ); //
    OLED_WriteCom ( 0xda ); //--set com pins hardware configuration
    OLED_WriteCom ( 0x12 );
    OLED_WriteCom ( 0xdb ); //--set vcomh
    OLED_WriteCom ( 0x40 ); //0x20,0.77xVcc
    //	IIC_Write_Command(0x8d);//--set DC-DC enable
    //	IIC_Write_Command(0x14);//
    OLED_WriteCom ( 0xaf ); //--turn on oled panel
#endif
    oled_info.device_ok = 1;
}

/*
************************************************************
	�������ƣ�	OLED_Exist

	�������ܣ�	OLED�豸���߼��

	��ڲ�����	��

	���ز�����	0-δ��⵽	1-��⵽

	˵����
************************************************************
*/
_Bool OLED_Exist ( void )
{
    //���OLED
    if ( !OLED_WriteCom ( 0xAE ) )			//�ر���ʾ���ж�ACK
        oled_info.device_ok = 1;

    return oled_info.device_ok;
}

/*
************************************************************
	�������ƣ�	OLED_Address

	�������ܣ�	����OLED��ʾ��ַ

	��ڲ�����	x���е�ַ
				y���е�ַ

	���ز�����	��

	˵����
************************************************************
*/
void OLED_Address ( unsigned char x, unsigned char y )
{
    OLED_WriteCom ( 0xb0 + x );					//�����е�ַ
    //DelayUs(2);
    OLED_WriteCom ( ( ( y & 0xf0 ) >> 4 ) | 0x10 );	//�����е�ַ�ĸ�4λ
    //DelayUs(2);
    OLED_WriteCom ( y & 0x0f );					//�����е�ַ�ĵ�4λ
    //DelayUs(2);
}

/*
************************************************************
	�������ƣ�	OLED_ClearScreen

	�������ܣ�	OLEDȫ�����

	��ڲ�����	��

	���ز�����	��

	˵����
************************************************************
*/
void OLED_ClearScreen ( void )
{
    if ( oled_info.device_ok )
        {
            unsigned char i = 0, j = 0;

            for ( ; i < 8; i++ )
                {
                    OLED_WriteCom ( 0xb0 + i );
                    OLED_WriteCom ( 0x10 );
                    OLED_WriteCom ( 0x00 );

                    for ( j = 0; j < 132; j++ )
                        OLED_WriteData ( 0x00 );
                }
        }
}

/*
************************************************************
	�������ƣ�	OLED_ClearAt

	�������ܣ�	OLED���ָ����

	��ڲ�����	x����Ҫ�������

	���ز�����	��

	˵����
************************************************************
*/
void OLED_ClearAt ( unsigned char x )
{
    if ( oled_info.device_ok )
        {
            unsigned char i = 0;
            OLED_WriteCom ( 0xb0 + x );
            OLED_WriteCom ( 0x10 );
            OLED_WriteCom ( 0x00 );

            for ( ; i < 132; i++ )
                OLED_WriteData ( 0x00 );
        }
}

/*
************************************************************
	�������ƣ�	OLED_Dis12864_Pic

	�������ܣ�	��ʾһ��128*64��ͼƬ

	��ڲ�����	dp��ͼƬ����ָ��

	���ز�����	��

	˵����
************************************************************
*/
void OLED_Dis12864_Pic ( const unsigned char *dp )
{
    if ( oled_info.device_ok )
        {
            unsigned char i = 0, j = 0;

            for ( ; j < 8; j++ )
                {
                    OLED_Address ( j, 0 );

                    for ( i = 0; i < 128; i++ )
                        OLED_WriteData ( *dp++ );		//д���ݵ�LCD,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1
                }
        }
}

/*
************************************************************
	�������ƣ�	OLED_DisChar16x16

	�������ܣ�	��ʾ16x16�ĵ�������

	��ڲ�����	dp��ͼƬ����ָ��

	���ز�����	��

	˵����		��ʾ16x16����ͼ�񡢺��֡���Ƨ�ֻ�16x16���������ͼ��
************************************************************
*/
void OLED_DisChar16x16 ( unsigned short x, unsigned short y, const unsigned char *dp )
{
    if ( oled_info.device_ok )
        {
            unsigned char i = 0, j = 0;

            for ( j = 2; j > 0; j-- )
                {
                    OLED_Address ( x, y );

                    for ( i = 0; i < 16; i++ )
                        OLED_WriteData ( *dp++ );		//д���ݵ�OLED,ÿд��һ��8λ�����ݺ��е�ַ�Զ���1

                    x++;
                }
        }
}

void OLED_DisStringFmt ( unsigned char line,  char *fmt, ... )
{
    //	if(oled_info.device_ok)
    //	{
    unsigned char oled_disbuf[24];
    unsigned char i = 0, c = 0, ch = 0, y, totalLength ;
    va_list ap;
    unsigned char *str = oled_disbuf;
    va_start ( ap, fmt );
    vsnprintf ( ( char * ) oled_disbuf, 17, fmt, ap );
    va_end ( ap );
    y = 2;
    totalLength = ( uint8_t ) strlen ( ( char * ) oled_disbuf ); //

    //Write string
    for ( c = 0; ( c < totalLength ) && ( c < LCD_MAX_LINE_LENGTH ); c++ )
        {
            ch = *str - 32;
            OLED_Address ( line * 2 - 2, y );

            for ( i = 0; i < 8; i++ )
                OLED_WriteData ( F8X16[ ( ch << 4 ) + i] );

            OLED_Address ( line * 2 - 1, y );

            for ( i = 0; i < 8; i++ )
                OLED_WriteData ( F8X16[ ( ch << 4 ) + i + 8] );

            str++;
            y += 8;
        }
    //Write space
    for ( c = totalLength;  c < LCD_MAX_LINE_LENGTH; c++ )
        {
            ch	=	0;
            OLED_Address ( line * 2 - 2, y );

            for ( i = 0; i < 8; i++ )
                OLED_WriteData ( F8X16[ ( ch << 4 ) + i] );

            OLED_Address ( line * 2 - 1, y );

            for ( i = 0; i < 8; i++ )
                OLED_WriteData ( F8X16[ ( ch << 4 ) + i + 8] );

            y += 8;
        }
}

/*
************************************************************
	�������ƣ�	OLED_DisString8x16

	�������ܣ�	��ʾ8x16�ĵ�������

	��ڲ�����	x����ʾ��
				y����ʾ��
				fmt����������

	���ز�����	��

	˵����		����ʾ4��
************************************************************
*/
void OLED_DisString8x16 ( unsigned char x, unsigned char y, char *fmt, ... )
{
    if ( oled_info.device_ok )
        {
            unsigned char oled_disbuf[128];
            unsigned char i = 0, ch = 0;
            va_list ap;
            unsigned char *str = oled_disbuf;
            va_start ( ap, fmt );
            vsnprintf ( ( char * ) oled_disbuf, sizeof ( oled_disbuf ), fmt, ap );
            va_end ( ap );
            y += 2;

            while ( *str != '\0' )
                {
                    ch = *str - 32;

                    if ( y > 128 )
                        {
                            y = 2;
                            x += 2;
                        }

                    OLED_Address ( x, y );

                    for ( i = 0; i < 8; i++ )
                        OLED_WriteData ( F8X16[ ( ch << 4 ) + i] );

                    OLED_Address ( x + 1, y );

                    for ( i = 0; i < 8; i++ )
                        OLED_WriteData ( F8X16[ ( ch << 4 ) + i + 8] );

                    y += 8;
                    str++;
                }
        }
}
