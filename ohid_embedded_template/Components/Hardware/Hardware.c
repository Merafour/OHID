/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : @merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : Bootloader.
* Description        : Hardware.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#include "Hardware/HW.h"

#define UID_Address   ((uint32_t)0x1FFFF7AC)

void hw_uid_read(uint8_t id[12])
{
	volatile uint32_t uid;
	uid = HAL_GetUIDw0();
	id[0] = uid&0xFF; id[1] = (uid>>8)&0xFF; id[2]  = (uid>>16)&0xFF; id[3]  = (uid>>24)&0xFF;
	uid = HAL_GetUIDw1();
	id[4] = uid&0xFF; id[5] = (uid>>8)&0xFF; id[6]  = (uid>>16)&0xFF; id[7]  = (uid>>24)&0xFF;
	uid = HAL_GetUIDw2();
	id[8] = uid&0xFF; id[9] = (uid>>8)&0xFF; id[10] = (uid>>16)&0xFF; id[11] = (uid>>24)&0xFF;
}

