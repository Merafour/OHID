/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V2.0.0
* Date               : 2024.2.9
* Description        : OpenHID Bootloader.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

#ifndef _OHID_BL_H_
#define _OHID_BL_H_
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

extern void bl_init(void);
extern void bl_receive(const uint8_t Recv[64]);
extern int bl_loop(const uint32_t _time_ms);

#ifdef __cplusplus
}
#endif
#endif // _OHID_BL_H_
