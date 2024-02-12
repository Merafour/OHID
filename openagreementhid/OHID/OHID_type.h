/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : OpenAgreementHID.
* Reference          : px4 bootloader
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#ifndef _OHID_TYPE_H_
#define _OHID_TYPE_H_

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * 常量(枚举)类型定义
*******************************************************************************/

enum OHID_ERR_CODE {
    OHID_ERR_CODE_INVALID       = 0xFF,    // 'invalid' response for bad commands
    OHID_ERR_CODE_OVER_ADDR     = 0x01,    // flash addr over
    OHID_ERR_CODE_OVER_SIZE     = 0x02,    // flash size over
    OHID_ERR_CODE_OVER_BUFF     = 0x03,    // buffer over
    OHID_ERR_CODE_ARG           = 0x04,    // 参数错误
    OHID_ERR_CODE_SIGN_INVALID  = 0x05,    // 签名失效
    OHID_ERR_CODE_SIGN          = 0x06,    // 签名错误
    OHID_ERR_CODE_SIGN_DATE     = 0x07,    // 签名数据错误
    OHID_ERR_CODE_SIGN_SHAKE1   = 0x08,    // handshake1
    OHID_ERR_CODE_SIGN_SHAKE2   = 0x09,    // handshake2
    OHID_ERR_CODE_BUSY          = 0x0A,    // 设备忙
    OHID_ERR_CODE_PROGRAM       = 0x0B,    // 编程错误
    OHID_ERR_CODE_STATUS        = 0x0C,    // 状态错误
    OHID_ERR_CODE_CRC           = 0x0D,    // CRC错误
	OHID_ERR_CODE_PACK          = 0x0E,    // pack错误
    OHID_ERR_CODE_NONE          = 0x00,    // 无错误
};

// MIX, 用于支持一些杂项
enum OHID_MIX_ORDER {
    // no arg
    OHID_MIX_NONE                    = 0x00,   //
    OHID_MIX_WRITE_PARAM_SAVE        = 0x01,   // only write
    OHID_MIX_WRITE_PARAM_RELOAD      = 0x02,   // only write
    OHID_MIX_WRITE_ADJ_CLEAR         = 0x03,   // only write
    OHID_MIX_WRITE_FACTORY           = 0x05,   // only write
    OHID_MIX_READ_MANUFACTURER       = 0x06,   // only read, Get Manufacturer
    // <Data> : | <arg> |.
    OHID_MIX_RGB_ON                  = 0x10,   // write/read
    OHID_MIX_RGB_BACK                = 0x11,   // write/read
    OHID_MIX_RGB_COLOR               = 0x12,   // write/read
    OHID_MIX_QUICK                   = 0x13,   // write/read
    OHID_MIX_TICKS                   = 0x14,   // write/read, 设置回报率
    // <Data> : | Date | 编译信息.
    OHID_MIX_ORDER_READ_BUILD        = 0x20,   // only write, 获取固件编译信息
    // <Data> : | GRAY1 | GRAY2 | GRAY3 | GRAY4 |, GRAYx 为 RGB 灰度, 0xFF 无效.
    OHID_MIX_ORDER_GRAY              = 0x21,   //  write/read, 写入 RGB 灰度 , arg = 0xFF 为无效
    // reserved
};

// BOOT解锁操作
enum OHID_UNLOCK {
    OHID_UNLOCK_NONE          = 0x00,   // 无效
    OHID_UNLOCK_SIGN          = 0x01,   // 签名
    OHID_UNLOCK_ERASE         = 0x02,   // 解锁擦除
    OHID_UNLOCK_REBOOT        = 0x03,   // 解锁重启
    OHID_UNLOCK_JUMP          = 0x04,   // 解锁跳转
    OHID_UNLOCK_PROGRAM       = 0x05,   // 解锁编程操作
    OHID_UNLOCK_SAFE          = 0x06,   // 解锁SN(加密)写操作
};
// 参数项
enum OHID_PARAM_ITEM {
    OHID_PARAM_ITEM_FN0    = 0x00,          // /
    OHID_PARAM_ITEM_FN1    = 0x01,          // /
    OHID_PARAM_ITEM_FN2    = 0x02,          // /
    OHID_PARAM_ITEM_FN3    = 0x03,          // /
    OHID_PARAM_ITEM_MODE   = 0x04,          // /
    OHID_PARAM_ITEM_MM     = 0x05,          // /
    OHID_PARAM_ITEM_MM1    = 0x06,          // /
    OHID_PARAM_ITEM_MM2    = 0x07,          // /
    OHID_PARAM_ITEM_MM3    = 0x08,          // /
    OHID_PARAM_ITEM_KEY1   = 0x09,          // /
    OHID_PARAM_ITEM_KEY2   = 0x0A,          // /
    OHID_PARAM_ITEM_KEY3   = 0x0B,          // /
    OHID_PARAM_ITEM_KEY4   = 0x0C,          // /
    OHID_PARAM_ITEM_TRPS1  = 0x0D,          // /
    OHID_PARAM_ITEM_TRPS2  = 0x0E,          // /
    OHID_PARAM_ITEM_TRPS3  = 0x0F,          // /
    OHID_PARAM_ITEM_TRPS4  = 0x10,          // /
    OHID_PARAM_ITEM_MACROA = 0x11,          // /
    OHID_PARAM_ITEM_MACROL = 0x12,          // /
    OHID_PARAM_ITEM_DELAY  = 0x13,          // /
    OHID_PARAM_ITEM_PHY    = 0x14,          // /
    OHID_PARAM_ITEM_MAX    = 0x15,          // /
};

// 按键工作模式定义
enum OHID_KB_MODE {
    OHID_KB_MODE_NORMAL                = 0x00,           // normal
    OHID_KB_MODE_SINGLE                = 0x01,           // wooting
    OHID_KB_MODE_RT                    = 0x02,           // wooting
    OHID_KB_MODE_MT                    = 0x03,           // wooting
    OHID_KB_MODE_TGL                   = 0x04,           // wooting
    OHID_KB_MODE_MACRO                 = 0x05,           //
    OHID_KB_MODE_FREE                  = 0x06,           //
    OHID_KB_MODE_DKS                   = 0x20,           // wooting
    OHID_KB_MODE_AKS3                  = 0x21,           //
};
// 设备工作模式
enum OHID_RUN_MODE {
    OHID_RUN_MODE_BOOT           = 0x00,    // BOOT mode
    OHID_RUN_MODE_KEYBOARD       = 0x01,    // APP mode
    OHID_RUN_MODE_BLE            = 0x02,    // BLE mode
    OHID_RUN_MODE_RGB            = 0x03,    // RGB mode
};

#define     OHID_DKS_PRESS               0x01                  // 按下
#define     OHID_DKS_PRESS_HOLD          0x02                  // 按下保持
#define     OHID_DKS_PRESS_BOTTON        0x04                  // 触底
#define     OHID_DKS_BOTTON_HOLD         0x18                  // 触底保持
#define     OHID_DKS_UP                  0x20                  // 抬手
#define     OHID_DKS_UP_HOLD             0x40                  // 抬手保持
#define     OHID_DKS_FREE                0x80                  // 完全释放
#define     OHID_ADD_DKS(status,dks)              ((status) |= dks)     //
#define     OHID_IS_DKS_PRESS(status)             (OHID_DKS_PRESS        == ((status)&OHID_DKS_PRESS))
#define     OHID_IS_DKS_PRESS_HOLD(status)        (OHID_DKS_PRESS_HOLD   == ((status)&OHID_DKS_PRESS_HOLD))
#define     OHID_IS_DKS_PRESS_BOTTON(status)      (OHID_DKS_PRESS_BOTTON == ((status)&OHID_DKS_PRESS_BOTTON))
#define     OHID_IS_DKS_BOTTON_HOLD(status)       (OHID_DKS_BOTTON_HOLD  == ((status)&OHID_DKS_BOTTON_HOLD))
#define     OHID_IS_DKS_UP(status)                (OHID_DKS_UP           == ((status)&OHID_DKS_UP))
#define     OHID_IS_DKS_UP_HOLD(status)           (OHID_DKS_UP_HOLD      == ((status)&OHID_DKS_UP_HOLD))
#define     OHID_IS_DKS_FREE(status)              (OHID_DKS_FREE         == ((status)&OHID_DKS_FREE))

/******************************************************************************
 * 数据结构定义
*******************************************************************************/

union OHID_uint16_t {
    uint16_t data;
    struct {
        uint8_t Byte0;
        uint8_t Byte1;
    }ohid;
};
union OHID_uint32_t {
    uint32_t data;
    struct {
        uint8_t Byte0;
        uint8_t Byte1;
        uint8_t Byte2;
        uint8_t Byte3;
    }ohid;
};

#ifdef __cplusplus
}
#endif

#endif // _OHID_TYPE_H_
