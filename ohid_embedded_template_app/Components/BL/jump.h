/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : Bootloader.
* Description        : jump to app.
* Description        : PX4 Bootloader (https://github.com/Merafour/Bootloader)
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#ifndef _JUMP_H_
#define _JUMP_H_

/* Includes ---------------------------------------------------------------------*/
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

//extern const uint32_t vectors_code_addr;
//extern const uint32_t vectors_code_end;
extern const uint32_t vectors_code_size;

extern void close_device(void);
extern void jump_to_app(const uint32_t app_load_address, const uint32_t fw_size);
extern void jump_app_sign(const uint32_t app_load_address, const uint32_t fw_size);
extern uint8_t bl_code;
extern const uint32_t a_ohid_target_sign[];

#ifdef __cplusplus
}
#endif
	
#endif /* _JUMP_H_ */

