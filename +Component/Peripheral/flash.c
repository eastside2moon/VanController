
#include <stm32f10x.h>
#include "flash.h"



//==========================================================//

//函数名称：	APP_FlashUpdate
//函数功能：
//入口参数：
//返回参数：	无
//说明：

//==========================================================//
uint32_t APP_FlashUpdate ( uint32_t addr, char *str, uint16_t len )
{
//    static	uint8_t   last_ch, last_flag = 0;
//    static	uint16_t  i;
//    //////////////////////
//    FLASH_Unlock();								//解锁
//    //////////真正的应用程序起始地址

//    ////如果上次剩了一个字节

//    if ( last_flag == 1 )
//        {
//            //加上这次的一个字节，一起写
//            FLASH_ProgramHalfWord ( addr, BUILD_UINT16 ( last_ch, str[0] ) );		//写入
//            addr += 2;
//            str++;
//            len--;
//        }

//    ////如果len是奇数

//    if ( len % 2 == 1 )
//        {
//            last_ch = * ( str + len - 1 ); //保存好最后一个字节,先不写进去
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
//            FLASH_ProgramHalfWord ( addr + 2 * i, BUILD_UINT16 ( * ( str + i * 2 + 0 ), * ( str + i * 2 + 1 ) )  );					//写入
//        }

//    //////地址修改，下次从新地址继续写
//    addr +=  len ;
//    FLASH_Lock();					//上锁
    return addr;
}
