/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : Bootloader.
* Description        : Flash.
* Description        : https://www.cnblogs.com/dreamrj/p/14044349.html
* Description        : STM32F401CCU6 ==> Flash(256KB) + RAM(64KB)
* Description        : Flash(256KB) = 16+16+16+16 +64 +128
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#ifndef _HW_FLASH_H_
#define _HW_FLASH_H_

#include <stdint.h>
#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************ (C) COPYLEFT 2018 Merafour *************************
* Flash 分区
********************************************************************************
* 0 ~16K [0x0000-0x047FF]: BOOT
* 16~128K[0x4000-0x1FFFF]: Firmware
******************************************************************************/

extern const uint32_t hw_flash_addr_start;
extern const uint32_t hw_flash_addr_end;
extern const uint32_t hw_flash_size_max;
extern const uint32_t ApplicationAddress;
extern const uint32_t app_size_max;
extern const uint16_t hw_flash_sectors_bytes; 

extern void NVIC_SetVectorTable(const uint32_t app_load_address);
extern int  hw_flash_erase(void);
extern void hw_flash_write(const uint32_t addr, const uint8_t data[], const uint32_t Bytes);
extern void hw_flash_read(uint32_t Address, uint8_t buf[], int32_t Bytes);
extern void hw_flash_erase_vector(const uint32_t vector_address);

#ifdef __cplusplus
}
#endif
	
#endif /* _HW_FLASH_H_ */

