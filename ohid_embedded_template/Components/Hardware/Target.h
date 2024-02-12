/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : Bootloader.
* Description        : Target.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#ifndef	_TARGET_H_
#define	_TARGET_H_

#include <stdint.h>

#include "OHID/OHID_Pack.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define OHID_TARGET        UNDEFINED
#define OHID_TARGET_ID     OHID_TARGET_BOARD(UNDEFINED)

#ifndef OHID_TARGET_ID
#error  "Please define OHID_TARGET_ID"
#endif
#ifndef OHID_TARGET
#error  "Please define OHID_TARGET"
#endif

#define OHID_BL_Version    "OHID BOOT V1.0.0"
extern const uint32_t ohid_target_id;

#ifdef __cplusplus
}
#endif
#endif	/* _TARGET_H_ */

