
#include <stm32f10x.h>
#include "flash.h"



//==========================================================//

//�������ƣ�	APP_FlashUpdate
//�������ܣ�
//��ڲ�����
//���ز�����	��
//˵����

//==========================================================//
uint32_t APP_FlashUpdate ( uint32_t addr, char *str, uint16_t len )
{
//    static	uint8_t   last_ch, last_flag = 0;
//    static	uint16_t  i;
//    //////////////////////
//    FLASH_Unlock();								//����
//    //////////������Ӧ�ó�����ʼ��ַ

//    ////����ϴ�ʣ��һ���ֽ�

//    if ( last_flag == 1 )
//        {
//            //������ε�һ���ֽڣ�һ��д
//            FLASH_ProgramHalfWord ( addr, BUILD_UINT16 ( last_ch, str[0] ) );		//д��
//            addr += 2;
//            str++;
//            len--;
//        }

//    ////���len������

//    if ( len % 2 == 1 )
//        {
//            last_ch = * ( str + len - 1 ); //��������һ���ֽ�,�Ȳ�д��ȥ
//            last_flag = 1;
//            len--;
//        }

//    else
//        {
//            last_flag = 0;
//        }

//    ////////////////////////////
//    for ( i = 0; i < len / 2; i++ )
//        {
//            FLASH_ProgramHalfWord ( addr + 2 * i, BUILD_UINT16 ( * ( str + i * 2 + 0 ), * ( str + i * 2 + 1 ) )  );					//д��
//        }

//    //////��ַ�޸ģ��´δ��µ�ַ����д
//    addr +=  len ;
//    FLASH_Lock();					//����
    return addr;
}
