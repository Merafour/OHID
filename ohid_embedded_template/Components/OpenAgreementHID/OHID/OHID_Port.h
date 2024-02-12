/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : OpenAgreementHID.
* Description        : 注：
* Description        : OHIDM_* : 主机(电脑上位机)发送给从机的数据包
* Description        : OHIDS_* : 从机(键盘)发送给主机的数据包
********************************************************************************
* SAFE, ERASE, REBOOT, BOOT, WRITE, READ, 6个关键指令均需要解锁相应的安全等级,以确保设备的安全性
* 解锁需要二次握手, 且每种解锁操作都对应一组签名验证：
* 第一次握手:
*   1. 解锁指令 SIGN， 发送 SN 号 (SYNC返回的SN号) 和签名, 设备验证通过将返回一串随机数 Datas 用于第二次握手;
* 第二次握手:
*   2. 根据键盘返回的随机数 Datas 进行签名, 需要解锁的操作不同，签名算法也不相同 ;
*   3. 解锁指令 ERASE(eg.)，发送 Datas和签名, 设备验证失败将返回错误码 ;
*   4. 设备解锁安全操作成功, 安全操作窗口期为 120s, 超时后关闭安全窗口, 需重新解锁方可操作, 否则将返回错误码 SIGN_INVALID;
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#ifndef _OHID_PORT_H_
#define _OHID_PORT_H_

#include <stdint.h>
#include <string.h>
#include <OHID/OHID_type.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 数据包格式:
 * | S/M    | Pack Len   | Des |        Head[4B]        | <Data> |
 * | Master | [4B]+[ nB] | CMD | 0xAA | len | cmd | crc | [ nB]  |
 * | Slave  | [4B]+[ nB] | ACK | 0xA5 | len | cmd | crc | [ nB]  |
 * head : 协议头,注: head 用于区分 Master 和 Slave 角色;
 * len  : data长度;
 * CMD  : 命令编码;
 * CRC  : 累加和校验, 计算方式为:crc=head+len+cmd+<data.end>+1+2+3+…+(len+3), 具体参考 OHID_Checksum 函数;
 * data : 数据域, 最大长度为 64*4-4 B ;
 * Pack Len : 应为 64*n B(HID 全速设备单个数据包为 64字节)
 */

#define OHID_PACK_HEAD_MASTRT       (0xAA&0xFF)
#define OHID_PACK_HEAD_SLAVE        (0xA5&0xFF)
#define OHID_CMD_WRITE(cmd)         ((enum OHID_CMD)(0x80|(cmd&0x7F)))
#define OHID_CMD_GET(cmd)           (((uint8_t)cmd)&0x7F)
#define OHID_CMD_GET_FLAG(cmd)      (((uint8_t)cmd)&0x80)

#define OHID_MM_MAX                 (40)   // 4.0mm
#define OHID_MM_MIN                 ( 1)   // 0.1mm

enum OHID_CMD {
    /******************************* 基础指令 ***********************************/
    /*
     * 出错时返回 FAIL指令
     | S/M    | Pack Len    | Des  |        Head[4B]         | <Data>                             |
     | Slave  | [4B]+[2+nB] | FAIL | 0xAA | len | 0xFF | crc | Err Code[1B] | cmd[1B] | <arg>[1B] |
     */
    OHID_CMD_BASE_FAIL      = 0xFF,
    /*
     * 混杂指令,用于执行一些杂项操作
     | S/M    | Pack Len    | Des |        Head[4B]         | <Data>                |
     | Master | [4B]+[1+nB] | CMD | 0xAA | len | 0x00 | crc | order[1B] | <arg>[nB] |
     | Slave  | [4B]+[1+nB] | ACK | 0xA5 | len | 0x00 | crc | order[1B] | <arg>[nB] |
    */
    OHID_CMD_BASE_MIX       = 0x00,
    /*
     * 同步指令,用于获取设备的基础信息,通常为 Master 给 Slave 发送的第一个指令
     | S/M    | Pack Len   | Des  |        Head[4B]         | <Data>                                                             |
     | Master | [4B]+[ 0B] | SYNC | 0xAA | len | 0x01 | crc |                                                                    |
     | Slave  | [4B]+[41B] | ACK  | 0xA5 | len | 0x01 | crc | Board ID[4B] | fw_size[2B] | run_mode[1B] | SN[17B] | version[17B] |
    */
    OHID_CMD_BASE_SYNC      = 0x01,
    /*
     * 安全指令, 用于写入自定义序列号与加密信息, 该指令写操作需要解锁
     | S/M    | Pack Len   | Des  |        Head[4B]         | <Data>                  |
     | Master | [4B]+[52B] | SAFE | 0xAA | len | 0x02 | crc | SN[20] | Encryption[32] |
     | Slave  | [4B]+[52B] | ACK  | 0xA5 | len | 0x02 | crc | SN[20] | Encryption[32] |
    */
    OHID_CMD_BASE_SAFE      = 0x02,
    /*
     * 扩展指令, 用于添加自定义通讯
     | S/M    | Pack Len   | Des       |        Head[4B]         | <Data>    |
     | Master | [4B]+[ nB] | EXPANDING | 0xAA | len | 0x03 | crc | <arg>[nB] |
     | Slave  | [4B]+[ nB] | ACK       | 0xA5 | len | 0x03 | crc | <arg>[nB] |
    */
    OHID_CMD_BASE_EXPANDING = 0x03,
    /******************************* BLE 指令 ***********************************/
    /*
     * BLE 模组指令
     | S/M    | Pack Len    | Des  |        Head[4B]         | <Data>    |
     | Master | [4B]+[2+nB] | BLE  | 0xAA | len | 0x06 | crc | Bitmap[1B]| order[1B]| <arg>[nB]  |
     | Slave  | [4B]+[ 5B]  | ACK  | 0xA5 | len | 0x06 | crc | Bitmap[1B]| Leds[1B] | Pair[1B]   | Voltage[2B]|
    */
    OHID_CMD_BLE       = 0x06,
    /*
     * BLE上报指令
     | S/M    | Pack Len    | Des  |        Head[4B]         | <Data>    |
     | Master | [4B]+[17B]  | BRP  | 0xAA | len | 0x07 | crc | Bitmap[1B]| Key[16B] |
     | Slave  | [4B]+[ 5B]  | ACK  | 0xA5 | len | 0x06 | crc | Bitmap[1B]| Leds[1B] | Pair[1B]   | Voltage[2B]|
    */
    OHID_CMD_BRP       = 0x07,
    /******************************* IAP 指令 ***********************************/
    /*
     * signature, 签名,用于鉴别是否合法操作
     | S/M    | Pack Len   | Des  |        Head[4B]         | <Data>                               |
     | Master | [4B]+[33B] | SIGN | 0xAA | len | 0x08 | crc | UnLock[1B] | SIGN[16B]  | Datas[16B] |
     | Slave  | [4B]+[17B] | ACK  | 0xA5 | len | 0x08 | crc | UnLock[1B] | Datas[16B] |
    */
    OHID_CMD_IAP_SIGN   = 0x08,
    /*
     * 擦除
     | S/M    | Pack Len   | Des   |        Head[4B]         | <Data>                  |
     | Master | [4B]+[ 8B] | ERASE | 0xAA | len | 0x09 | crc | address[4B]  | Size[4B] |
     | Slave  | [4B]+[ 1B] | ACK   | 0xA5 | len | 0x09 | crc | progress[1B] |
    */
    OHID_CMD_IAP_ERASE  = 0x09,
    /*
     * 重启
     | S/M    | Pack Len   | Des    |        Head[4B]         | <Data>     |
     | Master | [4B]+[ 1B] | REBOOT | 0xAA | len | 0x0A | crc | <rand>[1B] |
     | Slave  | [4B]+[ 1B] | ACK    | 0xA5 | len | 0x0A | crc | <rand>[1B] |
    */
    OHID_CMD_IAP_REBOOT = 0x0A,
    /*
     * Jump app
     | S/M    | Pack Len   | Des  |        Head[4B]         | <Data>                            |
     | Master | [4B]+[10B] | JUMP | 0xAA | len | 0x0B | crc | address[4B] | Size[4B]  | CRC[4B] |
     | Slave  | [4B]+[10B] | ACK  | 0xA5 | len | 0x0B | crc | address[4B] | Size[4B]  | CRC[4B] |
    */
    OHID_CMD_IAP_JUMP   = 0x0B,
    /*
     * 编程指令,刷写固件或读取固件, wSize 需 4字节对齐
     | S/M    | Pack Len    | Des     |        Head[4B]         | <Data>                             |
     | Master | [4B]+[6+nB] | PROGRAM | 0xAA | len | 0x0C | crc | address[4B] | wSize[2B] | code[nB] |
     | Slave  | [4B]+[6+nB] | ACK     | 0xA5 | len | 0x0C | crc | address[4B] | wSize[2B] | code[nB] |
    */
    OHID_CMD_IAP_PROGRAM= 0x0C,
    /*
     * 获取 CRC
     | S/M    | Pack Len   | Des  |        Head[4B]         | <Data>                            |
     | Master | [4B]+[ 8B] | RCRC | 0xAA | len | 0x0D | crc | address[4B] | Size[4B]  |
     | Slave  | [4B]+[10B] | ACK  | 0xA5 | len | 0x0D | crc | address[4B] | Size[4B]  | CRC[4B] |
    */
    OHID_CMD_IAP_RCRC   = 0x0D,
    /******************************* 驱动指令 ***********************************/
    /*
     * Param
     * MKEY : <HID Usage Tables> - 10 Keyboard/Keypad Page (0x07) range[0x04-0xA4], [0xE0-0xE7]
     | S/M    | Pack Len   | Des   |        Head[4B]         | <Data>                                    |
     | Master | [4B]+[4nB] | PARAM | 0xAA | len | 0x10 | crc | MKEY[1B]  | Item[1B]  | Value [2B] | ...  | 0xFF 无效
     | Slave  | [4B]+[4nB] | ACK   | 0xA5 | len | 0x10 | crc | MKEY[1B]  | Item[1B]  | Value [2B] | ...  | 0xFF 无效
    */
    OHID_CMD_DRIVER_PARAM       = 0x10,
    /*
     * MT 功能设置, Reference Wooting
     * MKEY : <HID Usage Tables> - 10 Keyboard/Keypad Page (0x07) range[0x04-0xA4], [0xE0-0xE7]
     | S/M    | Pack Len   | Des  |        Head[4B]         | <Data>                                               |
     | Master | [4B]+[5nB] | MT   | 0xAA | len | 0x11 | crc | MKEY[1B]  | KEY1[1B]  | KEY2[1B]  | delay[2B] | ...  | 0xFF 无效
     | Slave  | [4B]+[5nB] | ACK  | 0xA5 | len | 0x11 | crc | MKEY[1B]  | KEY1[1B]  | KEY2[1B]  | delay[2B] | ...  | 0xFF 无效
    */
    OHID_CMD_DRIVER_MT        = 0x11,
    /*
     * TGL 功能设置, Reference Wooting
     * MKEY : <HID Usage Tables> - 10 Keyboard/Keypad Page (0x07) range[0x04-0xA4], [0xE0-0xE7]
     | S/M    | Pack Len   | Des  |        Head[4B]         | <Data>                                   |
     | Master | [4B]+[4nB] | TGL  | 0xAA | len | 0x12 | crc | MKEY[1B]  | KEY1[1B]  | delay[2B] | ...  | 0xFF 无效
     | Slave  | [4B]+[4nB] | ACK  | 0xA5 | len | 0x12 | crc | MKEY[1B]  | KEY1[1B]  | delay[2B] | ...  | 0xFF 无效
    */
    OHID_CMD_DRIVER_TGL       = 0x12,
    /*
     * DKS(Digital keying system)功能设置, Reference Wooting
     * MKEY : <HID Usage Tables> - 10 Keyboard/Keypad Page (0x07) range[0x04-0xA4], [0xE0-0xE7]
     | S/M    | Pack Len    | Des  |        Head[4B]         | <Data>                                                                                                                     |
     | Master | [4B]+[10nB] | DKS  | 0xAA | len | 0x13 | crc | MKEY[1B]  | KEY1[1B]  | KEY2[1B]  | KEY3[1B]  | KEY4[1B]  | TrPs1[1B]  | TrPs2[1B]  | TrPs3[1B]  | TrPs4[1B]  | mm1 | ...  | 0xFF 无效
     | Slave  | [4B]+[10nB] | ACK  | 0xA5 | len | 0x13 | crc | MKEY[1B]  | KEY1[1B]  | KEY2[1B]  | KEY3[1B]  | KEY4[1B]  | TrPs1[1B]  | TrPs2[1B]  | TrPs3[1B]  | TrPs4[1B]  | mm1 | ...  | 0xFF 无效
    */
    OHID_CMD_DRIVER_DKS       = 0x13,
    /*
     * AKS(Analog keying system)功能设置, Reference 海盗船
     * MKEY : <HID Usage Tables> - 10 Keyboard/Keypad Page (0x07) range[0x04-0xA4], [0xE0-0xE7]
     | S/M    | Pack Len   | Des  |        Head[4B]         | <Data1>                                                                        |<Datan>|
     | Master | [4B]+[7nB] | AKS3 | 0xAA | len | 0x14 | crc | MKEY[1B]  | KEY1[1B]  | KEY2[1B]  | KEY3[1B]  | mm1[1B]  | mm2[1B]  | mm3[1B]  | ...   | 0xFF 无效
     | Slave  | [4B]+[7nB] | ACK  | 0xA5 | len | 0x14 | crc | MKEY[1B]  | KEY1[1B]  | KEY2[1B]  | KEY3[1B]  | mm1[1B]  | mm2[1B]  | mm3[1B]  | ...   | 0xFF 无效
    */
    OHID_CMD_DRIVER_AKS3      = 0x14,
    /*
     * Trigger 设置
     | S/M    | Pack Len  | Des     |        Head[4B]         | <Data>                                                                           |
     | Master | [4B]+[7B] | TRIGGER | 0xAA | len | 0x15 | crc | MM[1B]  | Reset[1B]  | RTmm[1B]  | RTreset[1B]  | mm1[1B]  | mm2[1B]  | mm3[1B]  |
     | Slave  | [4B]+[7B] | ACK     | 0xA5 | len | 0x15 | crc | MM[1B]  | Reset[1B]  | RTmm[1B]  | RTreset[1B]  | mm1[1B]  | mm2[1B]  | mm3[1B]  |
    */
    OHID_CMD_DRIVER_TRIGGER    = 0x15,
    /*
     * RGB_PARAM 设置
     | S/M    | Pack Len  | Des     |        Head[4B]         | <Data>                                                    |
     | Master | [4B]+[5B] | TRIGGER | 0xAA | len | 0x16 | crc | Gray[1B] | MODE[1B] | SPEED[1B] | SLEEP[1B] | REVERSE[1B] |
     | Slave  | [4B]+[5B] | ACK     | 0xA5 | len | 0x16 | crc | Gray[1B] | MODE[1B] | SPEED[1B] | SLEEP[1B] | REVERSE[1B] |
    */
    OHID_CMD_DRIVER_RGB_PARAM    = 0x16,
    /*
     * RGB 设置, 根据 RGB 对应的按键的键值识别, 此方法使用的时候更方便, 但如果一个 RGB 没有对应的按键将不可设置
     * MKEY : <HID Usage Tables> - 10 Keyboard/Keypad Page (0x07) range[0x04-0xA4], [0xE0-0xE7]
     | S/M    | Pack Len   | Des  |        Head[4B]         | <Data1>                                          |<Datan>|
     | Master | [4B]+[4nB] | KRGB | 0xAA | len | 0x17 | crc | MKEY[1B]  | RGB.R[1B]  | RGB.G[1B]  | RGB.B[1B]  | ...   | 0xFF 无效
     | Slave  | [4B]+[4nB] | ACK  | 0xA5 | len | 0x17 | crc | MKEY[1B]  | RGB.R[1B]  | RGB.G[1B]  | RGB.B[1B]  | ...   | 0xFF 无效
    */
    OHID_CMD_DRIVER_KRGB      = 0x17,
    /*
     * RGB 设置, 根据 RGB 坐标识别, 此方法的通用性更高, IDX<128 为按键灯 (Matrix6*21) , IDX>=128 为侧灯
     | S/M    | Pack Len   | Des  |        Head[4B]         | <Data1>                                         |<Datan>|
     | Master | [4B]+[4nB] | IRGB | 0xAA | len | 0x18 | crc | IDX[1B]  | RGB.R[1B]  | RGB.G[1B]  | RGB.B[1B]  | ...   | 0xFF 无效
     | Slave  | [4B]+[4nB] | ACK  | 0xA5 | len | 0x18 | crc | IDX[1B]  | RGB.R[1B]  | RGB.G[1B]  | RGB.B[1B]  | ...   | 0xFF 无效
    */
    OHID_CMD_DRIVER_IRGB      = 0x18,
    /*
     * 宏参数设置
     | S/M    | Pack Len     | Des   |        Head[4B]         | <Data>                         |
     | Master | [4B]+[3+4nB] | MACRO | 0xAA | len | 0x19 | crc | offset[2B] | len[1B] | bin[nW] |
     | Slave  | [4B]+[3+4nB] | ACK   | 0xA5 | len | 0x19 | crc | offset[2B] | len[1B] | bin[nW] |
    */
    OHID_CMD_DRIVER_MACRO     = 0x19,
};

// 单个数据包最大长度 64*4 B
#define   OHID_HEAD_SIZE    (4)
#define   OHID_PORT_SIZE    (64*4)
#define   OHID_DATA_SIZE    (OHID_PORT_SIZE-OHID_HEAD_SIZE)
union OpenAgreementHID_t {
    uint8_t bin[OHID_PORT_SIZE];
    struct {
        uint8_t HEAD;
        uint8_t length;
        uint8_t cmd;
        uint8_t checksum;
        uint8_t data[OHID_DATA_SIZE];
    }ohid;
};

/**************************************** 协议接口函数 **********************************************/
#define OHID_pack_size(Pack)    (Pack.ohid.length+OHID_HEAD_SIZE)
#define OHID_ppack_size(Pack)   (Pack->ohid.length+OHID_HEAD_SIZE)
extern void OHIDM_init(union OpenAgreementHID_t* const Pack, const enum OHID_CMD cmd);
extern void OHIDS_init(union OpenAgreementHID_t* const Pack, const enum OHID_CMD cmd);
extern int OHID_copy(uint8_t bin[], const uint8_t src[], const uint16_t len);
extern uint8_t OHID_Checksum(const union OpenAgreementHID_t* const Pack);
extern int OHIDM_encode(union OpenAgreementHID_t* const Pack);
extern int OHIDS_encode(union OpenAgreementHID_t* const Pack);
extern int _OHIDM_Decode(union OpenAgreementHID_t* const Pack, const uint16_t bSize);
extern int _OHIDS_Decode(union OpenAgreementHID_t* const Pack, const uint16_t bSize);
extern int _OHIDM_decode(union OpenAgreementHID_t* const Pack, const uint8_t bin[], const uint16_t bSize);
extern int _OHIDS_decode(union OpenAgreementHID_t* const Pack, const uint8_t bin[], const uint16_t bSize);
extern int OHIDM_decode(union OpenAgreementHID_t* const Pack, const uint8_t bin[], const uint16_t bSize);
extern int OHIDS_decode(union OpenAgreementHID_t* const Pack, const uint8_t bin[], const uint16_t bSize);

/**************************************** 数据接口函数 **********************************************/
// 添加一个 32bit无符号数
extern int OHID_add_32b(union OpenAgreementHID_t* const Pack, const uint32_t value);
extern int OHID_add_16b(union OpenAgreementHID_t* const Pack, const uint16_t value);
extern int OHID_add_8b(union OpenAgreementHID_t* const Pack, const uint8_t value);
// 添加一串二进制数据到指令结尾处
extern int OHID_add_bin(union OpenAgreementHID_t* const Pack, const uint8_t bin[], const uint8_t blen);
// 获取一个 32bit无符号数
extern uint32_t OHID_get_32b(const union OpenAgreementHID_t* const Pack, const uint8_t pos);
extern uint16_t OHID_get_16b(const union OpenAgreementHID_t* const Pack, const uint8_t pos);
extern uint8_t  OHID_get_8b(const union OpenAgreementHID_t* const Pack, const uint8_t pos);
// 获取一串二进制数据
extern int OHID_get_bin(const union OpenAgreementHID_t* const Pack, const uint8_t pos, uint8_t bin[], const uint16_t bsize);

// 键盘接收到无效指令或者指令执行出错会返回 FAIL；
extern void OHIDS_fail(union OpenAgreementHID_t* const Pack, const enum OHID_CMD cmd, const enum OHID_ERR_CODE code);
extern void OHIDS_fail_arg2(union OpenAgreementHID_t* const Pack, const enum OHID_CMD cmd, const enum OHID_ERR_CODE code, const uint8_t arg1, const uint8_t arg2);
//static inline int OHID_get_fail(union OpenAgreementHID_t* const Pack, enum OHID_CMD* const cmd, enum OHID_ERR_CODE* const code)
//{
//    *cmd  = (enum OHID_CMD)OHID_get_8b(Pack, 0);
//    *code = (enum OHID_ERR_CODE)OHID_get_8b(Pack, 1);
//    return 0;
//}
#define OHID_get_fail_cmd(Pack)    ((enum OHID_CMD)OHID_get_8b(Pack, 0))
#define OHID_get_fail_code(Pack)   ((enum OHID_ERR_CODE)OHID_get_8b(Pack, 1))

#ifdef __cplusplus
}
#endif

#endif // _OHID_PORT_H_
