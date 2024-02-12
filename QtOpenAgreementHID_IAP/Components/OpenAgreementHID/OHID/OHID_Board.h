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
#ifndef _OHID_BOARD_H_
#define _OHID_BOARD_H_

#include <stdint.h>
#include "OHID/OHID_Port.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 数据包格式:
 * | board_id[32bit]                   |
 * | DEVICE[8b] | SUB[8b] | INDEX[16b] |
 * DEVICE: 设备类
 * SUB  : 子类
 * INDEX    : 序号
 */
#define  OHID_BOARD_ID(DEVICE,SUB,INDEX)     ( (uint32_t)((((DEVICE)<<24)&0xFF000000) | (((SUB)<<16)&0x00FF0000) | ((INDEX)&0xFFFF)) )
#define  OHID_GET_DEVICE(board_id)           ((board_id>>24)&0xFF)
#define  OHID_GET_SUB(board_id)              ((board_id>>16)&0xFF)
#define  OHID_GET_ID(board_id)               ((board_id)&0xFFFF)

enum OHID_BOARD_DEVICE {
    OHID_BOARD_DEVICE_UNDEFINED         = 0x00,   //  undefined, 未定义设备, 用于未明确定义的设备
    OHID_BOARD_DEVICE_MACHINE           = 0x01,   //  机械轴(mechanical axis)
    OHID_BOARD_DEVICE_OPTICAL           = 0x02,   //  光轴(optical axis)
    OHID_BOARD_DEVICE_MAGNETIC          = 0x03,   //  磁轴(magnetic axis)
    OHID_BOARD_DEVICE_BLE               = 0x04,   //  BLE
    OHID_BOARD_DEVICE_RGB               = 0x05,   //  RGBs)
};
/*
 * 键盘的子类为键盘的配列
 */
enum OHID_BOARD_SUB_KEYBOARD {
    OHID_BOARD_SUB_KEYBOARD_K104            = 0x00,   // 104键位(默认)
    OHID_BOARD_SUB_KEYBOARD_K99             = 0x01,   // 99键位
    OHID_BOARD_SUB_KEYBOARD_K87             = 0x02,   // 87键位
    OHID_BOARD_SUB_KEYBOARD_K75             = 0x03,   // 75键位
    OHID_BOARD_SUB_KEYBOARD_K61             = 0x04,   // 61键位
    OHID_BOARD_SUB_KEYBOARD_K00             = 0x05,   // Other 键位
};

// move to "OHID_Board_id.h"
//enum OHID_BOARDS_ID {
//    OHID_BOARD_MACHINE                = OHID_BOARD_ID(OHID_BOARD_SUB_KEYBOARD_K104, OHID_BOARD_DEVICE_MACHINE,  0x0000),    // 104 键机械轴
//    OHID_BOARD_OPTICAL                = OHID_BOARD_ID(OHID_BOARD_SUB_KEYBOARD_K104, OHID_BOARD_DEVICE_OPTICAL,  0x0000),    // 104 键光轴
//    OHID_BOARD_MAGNETIC               = OHID_BOARD_ID(OHID_BOARD_SUB_KEYBOARD_K104, OHID_BOARD_DEVICE_MAGNETIC, 0x0000),    // 104 键磁轴
//};

#define OHID_KB_ROWS          (6)
#define OHID_KB_COLS          (21)
typedef struct OHID_board_sub_keyboard {
    const uint16_t keys[OHID_KB_ROWS][OHID_KB_COLS];
    const char* const name[OHID_KB_ROWS][OHID_KB_COLS];
} OHID_board_sub_keyboard_t;

#define  OHID_kb_get_row(pos)      ((pos>>5)&0x07)
#define  OHID_kb_get_col(pos)      (pos&0x1F)
#define  OHID_kb_get_pos(row,col)  (((row&0x07)<<5) | (col&0x1F))

extern uint8_t OHID_sub_keyboard_get_pos(const OHID_board_sub_keyboard_t* const sub_keyboard, const uint16_t key_value);

#ifdef __cplusplus
}
#endif

#endif // _OHID_BOARD_H_
