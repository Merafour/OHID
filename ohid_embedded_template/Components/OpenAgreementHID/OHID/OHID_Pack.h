/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : OpenAgreementHID.
* Description        : 开源键盘通信协议 API 接口.
* Description        : 这些接口对数据包进行封装,可以让对协议的操作更简单
* Description        : 注：
* Description        : OHIDM_* : 主机(电脑上位机)发送给从机的数据包
* Description        : OHIDS_* : 从机(键盘)发送给主机的数据包
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#ifndef _OHID_PACK_H_
#define _OHID_PACK_H_

#include <stdint.h>
#include "OHID/OHID_Port.h"
#include "OHID_Board_id.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 数据包格式:
 * | S/M    | Pack Len   | Des |        Head[4B]        | <Data> |
 * | Master | [4B]+[ nB] | CMD | 0xAA | len | cmd | crc | [ nB]  |
 * | Slave  | [4B]+[ nB] | ACK | 0xA5 | len | cmd | crc | [ nB]  |
 * head : 协议头;
 * len  : data长度;
 * CMD  : 命令编码;
 * CRC  : 累加和校验, 计算方式为:crc=head+len+cmd+<data.end>+1+2+3+…+(len+3), 具体参考 OHID_Checksum 函数;
 * data : 数据域, 最大长度为 64*4-4 B ;
 */

extern void OHIDM_Base_mix(union OpenAgreementHID_t* const Pack, const enum OHID_MIX_ORDER order);
extern void OHIDM_Base_mix_arg(union OpenAgreementHID_t* const Pack, const enum OHID_MIX_ORDER order, const uint8_t arg);
extern void OHIDS_Base_mix(union OpenAgreementHID_t* const Pack, const enum OHID_MIX_ORDER order);
extern void OHIDS_Base_mix_arg(union OpenAgreementHID_t* const Pack, const enum OHID_MIX_ORDER order, const uint8_t arg);
extern void OHIDM_None(union OpenAgreementHID_t* const Pack, const enum OHID_CMD cmd);
extern void OHIDS_None(union OpenAgreementHID_t* const Pack, const enum OHID_CMD cmd);
/******************************* 基础指令 ***********************************/
extern void OHIDS_Base_sync(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const enum OHID_BOARDS_ID_LIST board_id, const uint16_t fw_size, const enum OHID_RUN_MODE run_mode, const char SN[16], const char version[16]);
extern void OHIDM_Base_SAFE(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t SN[20], const uint8_t Encryption[32]);
extern void OHIDS_Base_SAFE(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t SN[20], const uint8_t Encryption[32]);

/******************************* BLE 指令 ***********************************/

/******************************* IAP 指令 ***********************************/
// 秘钥
extern const uint8_t OHID_signature_key_main[16];
// 签名参数
extern const uint8_t OHID_signature_params1[16];
extern const uint8_t OHID_signature_params2[16];
extern uint32_t ohid_crc32(const uint8_t *src, const uint32_t len, uint32_t state);
extern void OHID_Signature(const uint8_t Datas[16], const uint8_t KEYS[16], const uint8_t params[16], const uint8_t unlock, uint8_t SIGN[16]);
// signature
extern void OHIDM_IAP_Sign(union OpenAgreementHID_t* const Pack, const enum OHID_UNLOCK unlock, const uint8_t SIGN[16], const uint8_t Datas[16]);
extern void OHIDS_IAP_Sign(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const enum OHID_UNLOCK unlock, const uint8_t Datas[16]);
extern void OHIDM_IAP_Erase(union OpenAgreementHID_t* const Pack, const uint32_t address, const uint32_t eSize);
extern void OHIDS_IAP_Erase(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t progress);
extern void OHIDM_IAP_Reboot(union OpenAgreementHID_t* const Pack, const uint8_t rand);
extern void OHIDS_IAP_Reboot(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t rand);
extern void OHIDM_IAP_Jump(union OpenAgreementHID_t* const Pack, const uint32_t address, const uint32_t Size, const uint32_t CRCin);
extern void OHIDS_IAP_Jump(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint32_t address, const uint32_t Size, const uint32_t CRCin);
extern void OHIDM_IAP_Program(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint32_t address, const uint16_t Size, const uint8_t binary[]);
extern void OHIDS_IAP_Program(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint32_t address, const uint16_t Size, const uint8_t binary[]);
extern void OHIDM_IAP_RCRC(union OpenAgreementHID_t* const Pack, const uint32_t address, const uint32_t Size);
extern void OHIDS_IAP_RCRC(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint32_t address, const uint32_t Size, const uint32_t CRCin);

/******************************* 驱动指令 ***********************************/
// Param
extern void OHIDM_Driver_Param(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[15], const enum OHID_PARAM_ITEM Item[15], const uint16_t Value[15]);
extern void OHIDS_Driver_Param(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t MKEY[15], const enum OHID_PARAM_ITEM Item[15], const uint16_t Value[15]);
// MT, Reference Wooting
extern void OHIDM_Driver_MT(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[12], const uint8_t KEY1[12], const uint8_t KEY2[12], const uint8_t delay[12]);
extern void OHIDS_Driver_MT(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t MKEY[12], const uint8_t KEY1[12], const uint8_t KEY2[12], const uint8_t delay[12]);
// TGL, Reference Wooting
extern void OHIDM_Driver_TGL(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[15], const uint8_t KEY1[15], const uint8_t delay[15]);
extern void OHIDS_Driver_TGL(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t MKEY[15], const uint8_t KEY1[15], const uint8_t delay[15]);
// DKS, Reference Wooting
extern void OHIDM_Driver_DKS(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[6], const uint8_t KEY1[6], const uint8_t KEY2[6], const uint8_t KEY3[6], const uint8_t KEY4[6], const uint8_t TrPs1[6], const uint8_t TrPs2[6], const uint8_t TrPs3[6], const uint8_t TrPs4[6], const uint8_t mm1[6]);
extern void OHIDS_Driver_DKS(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t MKEY[6], const uint8_t KEY1[6], const uint8_t KEY2[6], const uint8_t KEY3[6], const uint8_t KEY4[6], const uint8_t TrPs1[6], const uint8_t TrPs2[6], const uint8_t TrPs3[6], const uint8_t TrPs4[6], const uint8_t mm1[6]);
// AKS, Reference 海盗船
extern void OHIDM_Driver_AKS3(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[8], const uint8_t KEY1[8], const uint8_t KEY2[8], const uint8_t KEY3[8], const uint8_t mm1[8], const uint8_t mm2[8], const uint8_t mm3[8]);
extern void OHIDS_Driver_AKS3(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t MKEY[8], const uint8_t KEY1[8], const uint8_t KEY2[8], const uint8_t KEY3[8], const uint8_t mm1[8], const uint8_t mm2[8], const uint8_t mm3[8]);
// Trigger
extern void OHIDM_Driver_Trigger(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MM, const uint8_t Reset, const uint8_t RTmm, const uint8_t RTreset, const uint8_t mm1, const uint8_t mm2, const uint8_t mm3);
extern void OHIDS_Driver_Trigger(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t MM, const uint8_t Reset, const uint8_t RTmm, const uint8_t RTreset, const uint8_t mm1, const uint8_t mm2, const uint8_t mm3);
// RGB_PARAM
extern void OHIDM_Driver_RGB_PARAM(union OpenAgreementHID_t* const Pack, const uint8_t Write, const uint8_t gray, const uint8_t mode, const uint8_t speed, const uint8_t sleep, const uint8_t reverse);
extern void OHIDS_Driver_RGB_PARAM(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code,  const uint8_t gray, const uint8_t mode, const uint8_t speed, const uint8_t sleep, const uint8_t reverse);
// KRGB, 提供以下三种接口
extern void OHIDM_Driver_krgb1(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint32_t krgb[15]);
extern void OHIDS_Driver_krgb1(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code,  const uint32_t krgb[15]);
extern void OHIDM_Driver_krgb2(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[15], const uint8_t  RGBR[15], const uint8_t RGBG[15], const uint8_t RGBB[15]);
extern void OHIDS_Driver_krgb2(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code,  const uint8_t MKEY[15], const uint8_t  RGBR[15], const uint8_t RGBG[15], const uint8_t RGBB[15]);
extern void OHIDM_Driver_krgb3(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[15], const uint32_t RGB[15]);
extern void OHIDS_Driver_krgb3(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code,  const uint8_t MKEY[15], const uint32_t RGB[15]);
// IRGB
extern void OHIDM_Driver_IRGB(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint32_t irgb[15]);
extern void OHIDS_Driver_IRGB(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint32_t irgb[15]);
// Macro
extern void OHIDM_Driver_Macro(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint32_t macro[14], uint8_t len, const uint16_t offset);
extern void OHIDS_Driver_Macro(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint32_t macro[14], uint8_t len, const uint16_t offset);

#ifdef __cplusplus
}
#endif

#endif // _OHID_PACK_H_
