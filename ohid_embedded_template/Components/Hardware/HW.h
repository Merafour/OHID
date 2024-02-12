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

#ifndef	_HARDWARE_H_
#define	_HARDWARE_H_

#include <stdint.h>

#include "Hardware/hw_flash.h"
//#include "Hardware/hw_timer.h"
#include "Hardware/Target.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void hw_uid_read(uint8_t id[12]);

#ifdef __cplusplus
}
#endif
#endif	/* _LED_WS2812_H_ */
												
/************************ (C) COPYRIGHT Fremont Micro Devices *****END OF FILE****/												




