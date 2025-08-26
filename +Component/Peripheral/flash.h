#ifndef _FLASH_H_
#define _FLASH_H_



#include "hal_defs_stm.h"






uint32_t APP_FlashUpdate ( uint32_t addr, char *str, uint16_t len );

#define   FLASH_PER_PAGE  0x0800
#define   APP_PAGE_NUM  32



#define  FLASH_INFO_ADDR			(FLASH_BASE + FLASH_PER_PAGE*(APP_PAGE_NUM-2))
#define  FLASH_OTA_PRAM				(FLASH_BASE + FLASH_PER_PAGE*(APP_PAGE_NUM-1))

#define  FLASH_APP_ADDR				(FLASH_BASE + FLASH_PER_PAGE*(APP_PAGE_NUM-0))
#define  FLASH_DEV_ADDR				(FLASH_BASE + FLASH_PER_PAGE*255)

#endif







