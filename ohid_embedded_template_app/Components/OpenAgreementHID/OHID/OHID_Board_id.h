/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : OpenAgreementHID.
* Description        : BOARD_ID 定义
* Description        : 注：
* Description        : OpenAgreementHID(OHID) 基于键盘与上位机通讯的需求而开发, 但也可用于非键盘设备, BOARD_ID 用于识别唯一一款设备
* Description        : BOARD_ID 分为三个字段 DEVICE, SUB, INDEX, 分别是 : 设备类, 子类, 序号
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#ifndef _OHID_BOARD_ID_H_
#define _OHID_BOARD_ID_H_

#include <stdint.h>
#include "OHID/OHID_Board.h"
#include "OHID.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OHID_TARGET_BOARD(target)   (OHID_BOARD_##target)

#ifndef   OHID_BOARDS_ID_LIST_EXP

enum OHID_BOARDS_ID_LIST {
    OHID_BOARD_UNDEFINED              = OHID_BOARD_ID(0x00,                         OHID_BOARD_DEVICE_UNDEFINED,  0x0000),  // 未定义设备
    OHID_BOARD_MACHINE                = OHID_BOARD_ID(OHID_BOARD_SUB_KEYBOARD_K104, OHID_BOARD_DEVICE_MACHINE,    0x0000),    // 104 键机械轴
    OHID_BOARD_OPTICAL                = OHID_BOARD_ID(OHID_BOARD_SUB_KEYBOARD_K104, OHID_BOARD_DEVICE_OPTICAL,    0x0000),    // 104 键光轴
    OHID_BOARD_MAGNETIC               = OHID_BOARD_ID(OHID_BOARD_SUB_KEYBOARD_K104, OHID_BOARD_DEVICE_MAGNETIC,   0x0000),    // 104 键磁轴
};

#endif  // OHID_BOARD_ID_EXP

#ifdef __cplusplus
}
#endif

#endif // _OHID_BOARD_ID_H_
