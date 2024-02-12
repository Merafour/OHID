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
#include <string.h>
#include "OHID/OHID_Pack.h"

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

/******************************* 基础指令 ***********************************/
void OHIDM_Base_mix(union OpenAgreementHID_t* const Pack, const enum OHID_MIX_ORDER order)
{
    OHIDM_init(Pack, OHID_CMD_BASE_MIX);
    OHID_add_8b(Pack, order);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDM_Base_mix_arg(union OpenAgreementHID_t* const Pack, const enum OHID_MIX_ORDER order, const uint8_t arg)
{
    OHIDM_init(Pack, OHID_CMD_BASE_MIX);
    OHID_add_8b(Pack, order);
    OHID_add_8b(Pack, arg);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Base_mix(union OpenAgreementHID_t* const Pack, const enum OHID_MIX_ORDER order)
{
    OHIDS_init(Pack, OHID_CMD_BASE_MIX);
    OHID_add_8b(Pack, order);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Base_mix_arg(union OpenAgreementHID_t* const Pack, const enum OHID_MIX_ORDER order, const uint8_t arg)
{
    OHIDS_init(Pack, OHID_CMD_BASE_MIX);
    OHID_add_8b(Pack, order);
    OHID_add_8b(Pack, arg);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDM_None(union OpenAgreementHID_t* const Pack, const enum OHID_CMD cmd)
{
    OHIDM_init(Pack, cmd);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_None(union OpenAgreementHID_t* const Pack, const enum OHID_CMD cmd)
{
    OHIDS_init(Pack, cmd);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Base_sync(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const enum OHID_BOARDS_ID_LIST board_id, const uint16_t fw_size, const enum OHID_RUN_MODE run_mode, const char SN[16], const char version[16])
{
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_BASE_SAFE, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_BASE_SYNC);
        OHID_add_32b(Pack, (uint32_t)board_id);
        OHID_add_16b(Pack, fw_size);
        OHID_add_8b(Pack, (uint8_t)run_mode);
        OHID_add_bin(Pack, (const uint8_t*)SN, 16);
        OHID_add_bin(Pack, (const uint8_t*)version, 16);
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
void OHIDM_Base_SAFE(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t SN[20], const uint8_t Encryption[32])
{
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_BASE_SAFE));
    else      OHIDM_init(Pack, OHID_CMD_BASE_SAFE);
    OHID_add_bin(Pack, (const uint8_t*)SN, 20);
    OHID_add_bin(Pack, (const uint8_t*)Encryption, 32);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Base_SAFE(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t SN[20], const uint8_t Encryption[32])
{
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_BASE_SAFE, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_BASE_SAFE);
        OHID_add_bin(Pack, (const uint8_t*)SN, 20);
        OHID_add_bin(Pack, (const uint8_t*)Encryption, 32);
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}

/******************************* IAP 指令 ***********************************/
void OHIDM_IAP_Sign(union OpenAgreementHID_t* const Pack, const enum OHID_UNLOCK unlock, const uint8_t SIGN[16], const uint8_t Datas[16])
{
    OHIDM_init(Pack, OHID_CMD_IAP_SIGN);
    Pack->ohid.data[Pack->ohid.length++] = unlock;
    OHID_add_bin(Pack, SIGN, 16);
    OHID_add_bin(Pack, Datas, 16);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_IAP_Sign(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const enum OHID_UNLOCK unlock, const uint8_t Datas[16])
{
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_IAP_SIGN, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_IAP_SIGN);
        Pack->ohid.data[Pack->ohid.length++] = unlock;
        OHID_add_bin(Pack, Datas, 16);
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
void OHIDM_IAP_Erase(union OpenAgreementHID_t* const Pack, const uint32_t address, const uint32_t eSize)
{
    OHIDM_init(Pack, OHID_CMD_IAP_ERASE);
    OHID_add_32b(Pack, address);
    OHID_add_32b(Pack, eSize);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_IAP_Erase(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t progress)
{
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_IAP_ERASE, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_IAP_ERASE);
        Pack->ohid.data[Pack->ohid.length++] = progress;
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
void OHIDM_IAP_Reboot(union OpenAgreementHID_t* const Pack, const uint8_t rand)
{
    OHIDM_init(Pack, OHID_CMD_IAP_REBOOT);
    OHID_add_8b(Pack, rand);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_IAP_Reboot(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t rand)
{
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_IAP_REBOOT, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_IAP_REBOOT);
        OHID_add_8b(Pack, rand);
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
void OHIDM_IAP_Jump(union OpenAgreementHID_t* const Pack, const uint32_t address, const uint32_t Size, const uint32_t CRCin)
{
    OHIDM_init(Pack, OHID_CMD_IAP_JUMP);
    OHID_add_32b(Pack, address);
    OHID_add_32b(Pack, Size);
    OHID_add_32b(Pack, CRCin);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_IAP_Jump(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint32_t address, const uint32_t Size, const uint32_t CRCin)
{
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_IAP_JUMP, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_IAP_JUMP);
        OHID_add_32b(Pack, address);
        OHID_add_32b(Pack, Size);
        OHID_add_32b(Pack, CRCin);
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
void OHIDM_IAP_Program(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint32_t address, const uint16_t Size, const uint8_t binary[])
{
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_IAP_PROGRAM));
    else      OHIDM_init(Pack, OHID_CMD_IAP_PROGRAM);
    OHID_add_32b(Pack, address);
    OHID_add_16b(Pack, Size);
    for(uint8_t kk=0; kk<Size; kk++) Pack->ohid.data[Pack->ohid.length++] = binary[kk];
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_IAP_Program(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint32_t address, const uint16_t Size, const uint8_t binary[])
{
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_IAP_PROGRAM, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_IAP_PROGRAM);
        OHID_add_32b(Pack, address);
        OHID_add_16b(Pack, Size);
        for(uint8_t kk=0; kk<Size; kk++) Pack->ohid.data[Pack->ohid.length++] = binary[kk];
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
void OHIDM_IAP_RCRC(union OpenAgreementHID_t* const Pack, const uint32_t address, const uint32_t Size)
{
    OHIDM_init(Pack, OHID_CMD_IAP_RCRC);
    OHID_add_32b(Pack, address);
    OHID_add_32b(Pack, Size);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_IAP_RCRC(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint32_t address, const uint32_t Size, const uint32_t CRCin)
{
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_IAP_RCRC, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_IAP_RCRC);
        OHID_add_32b(Pack, address);
        OHID_add_32b(Pack, Size);
        OHID_add_32b(Pack, CRCin);
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}

/******************************* 驱动指令 ***********************************/
// Param
void OHIDM_Driver_Param(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[15], const enum OHID_PARAM_ITEM Item[15], const uint16_t Value[15])
{
    uint8_t ii;
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_DRIVER_PARAM));
    else      OHIDM_init(Pack, OHID_CMD_DRIVER_PARAM);
    for(ii=0; ii<15; ii++)
    {
        Pack->ohid.data[Pack->ohid.length++] = MKEY[ii];
        Pack->ohid.data[Pack->ohid.length++] = (uint8_t)Item[ii];
        OHID_add_16b(Pack, Value[ii]);
    }
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Driver_Param(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t MKEY[15], const enum OHID_PARAM_ITEM Item[15], const uint16_t Value[15])
{
    uint8_t ii;
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_DRIVER_PARAM, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_DRIVER_PARAM);
        for(ii=0; ii<15; ii++)
        {
            Pack->ohid.data[Pack->ohid.length++] = MKEY[ii];
            Pack->ohid.data[Pack->ohid.length++] = (uint8_t)Item[ii];
            OHID_add_16b(Pack, Value[ii]);
        }
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
// MT, Reference Wooting
void OHIDM_Driver_MT(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[12], const uint8_t KEY1[12], const uint8_t KEY2[12], const uint8_t delay[12])
{
    uint8_t ii;
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_DRIVER_MT));
    else      OHIDM_init(Pack, OHID_CMD_DRIVER_MT);
    for(ii=0; ii<12; ii++)
    {
        Pack->ohid.data[Pack->ohid.length++] = MKEY[ii];
        Pack->ohid.data[Pack->ohid.length++] = KEY1[ii];
        Pack->ohid.data[Pack->ohid.length++] = KEY2[ii];
        OHID_add_16b(Pack, delay[ii]);
    }
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Driver_MT(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t MKEY[12], const uint8_t KEY1[12], const uint8_t KEY2[12], const uint8_t delay[12])
{
    uint8_t ii;
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_DRIVER_MT, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_DRIVER_MT);
        for(ii=0; ii<12; ii++)
        {
            Pack->ohid.data[Pack->ohid.length++] = MKEY[ii];
            Pack->ohid.data[Pack->ohid.length++] = KEY1[ii];
            Pack->ohid.data[Pack->ohid.length++] = KEY2[ii];
            OHID_add_16b(Pack, delay[ii]);
        }
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}

// TGL, Reference Wooting
void OHIDM_Driver_TGL(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[15], const uint8_t KEY1[15], const uint8_t delay[15])
{
    uint8_t ii;
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_DRIVER_TGL));
    else      OHIDM_init(Pack, OHID_CMD_DRIVER_TGL);
    for(ii=0; ii<15; ii++)
    {
        Pack->ohid.data[Pack->ohid.length++] = MKEY[ii];
        Pack->ohid.data[Pack->ohid.length++] = KEY1[ii];
        OHID_add_16b(Pack, delay[ii]);
    }
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Driver_TGL(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t MKEY[15], const uint8_t KEY1[15], const uint8_t delay[15])
{
    uint8_t ii;
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_DRIVER_TGL, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_DRIVER_TGL);
        for(ii=0; ii<15; ii++)
        {
            Pack->ohid.data[Pack->ohid.length++] = MKEY[ii];
            Pack->ohid.data[Pack->ohid.length++] = KEY1[ii];
            OHID_add_16b(Pack, delay[ii]);
        }
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}

// DKS, Reference Wooting
void OHIDM_Driver_DKS(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[6], const uint8_t KEY1[6], const uint8_t KEY2[6], const uint8_t KEY3[6], const uint8_t KEY4[6], const uint8_t TrPs1[6], const uint8_t TrPs2[6], const uint8_t TrPs3[6], const uint8_t TrPs4[6], const uint8_t mm1[6])
{
    uint8_t ii;
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_DRIVER_DKS));
    else      OHIDM_init(Pack, OHID_CMD_DRIVER_DKS);
    for(ii=0; ii<6; ii++)
    {
        Pack->ohid.data[Pack->ohid.length++] = MKEY[ii];
        Pack->ohid.data[Pack->ohid.length++] = KEY1[ii];
        Pack->ohid.data[Pack->ohid.length++] = KEY2[ii];
        Pack->ohid.data[Pack->ohid.length++] = KEY3[ii];
        Pack->ohid.data[Pack->ohid.length++] = KEY4[ii];
        Pack->ohid.data[Pack->ohid.length++] = TrPs1[ii];
        Pack->ohid.data[Pack->ohid.length++] = TrPs2[ii];
        Pack->ohid.data[Pack->ohid.length++] = TrPs3[ii];
        Pack->ohid.data[Pack->ohid.length++] = TrPs4[ii];
        Pack->ohid.data[Pack->ohid.length++] = mm1[ii];
    }
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Driver_DKS(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t MKEY[6], const uint8_t KEY1[6], const uint8_t KEY2[6], const uint8_t KEY3[6], const uint8_t KEY4[6], const uint8_t TrPs1[6], const uint8_t TrPs2[6], const uint8_t TrPs3[6], const uint8_t TrPs4[6], const uint8_t mm1[6])
{
    uint8_t ii;
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_DRIVER_DKS, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_DRIVER_DKS);
        for(ii=0; ii<5; ii++)
        {
            Pack->ohid.data[Pack->ohid.length++] = MKEY[ii];
            Pack->ohid.data[Pack->ohid.length++] = KEY1[ii];
            Pack->ohid.data[Pack->ohid.length++] = KEY2[ii];
            Pack->ohid.data[Pack->ohid.length++] = KEY3[ii];
            Pack->ohid.data[Pack->ohid.length++] = KEY4[ii];
            Pack->ohid.data[Pack->ohid.length++] = TrPs1[ii];
            Pack->ohid.data[Pack->ohid.length++] = TrPs2[ii];
            Pack->ohid.data[Pack->ohid.length++] = TrPs3[ii];
            Pack->ohid.data[Pack->ohid.length++] = TrPs4[ii];
            Pack->ohid.data[Pack->ohid.length++] = mm1[ii];
        }
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}

// AKS, Reference 海盗船
void OHIDM_Driver_AKS3(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[8], const uint8_t KEY1[8], const uint8_t KEY2[8], const uint8_t KEY3[8], const uint8_t mm1[8], const uint8_t mm2[8], const uint8_t mm3[8])
{
    uint8_t ii;
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_DRIVER_AKS3));
    else      OHIDM_init(Pack, OHID_CMD_DRIVER_AKS3);
    for(ii=0; ii<8; ii++)
    {
        Pack->ohid.data[Pack->ohid.length++] = MKEY[ii];
        Pack->ohid.data[Pack->ohid.length++] = KEY1[ii];
        Pack->ohid.data[Pack->ohid.length++] = KEY2[ii];
        Pack->ohid.data[Pack->ohid.length++] = KEY3[ii];
        Pack->ohid.data[Pack->ohid.length++] = mm1[ii];
        Pack->ohid.data[Pack->ohid.length++] = mm2[ii];
        Pack->ohid.data[Pack->ohid.length++] = mm3[ii];
    }
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Driver_AKS3(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t MKEY[8], const uint8_t KEY1[8], const uint8_t KEY2[8], const uint8_t KEY3[8], const uint8_t mm1[8], const uint8_t mm2[8], const uint8_t mm3[8])
{
    uint8_t ii;
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_DRIVER_AKS3, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_DRIVER_AKS3);
        for(ii=0; ii<8; ii++)
        {
            Pack->ohid.data[Pack->ohid.length++] = MKEY[ii];
            Pack->ohid.data[Pack->ohid.length++] = KEY1[ii];
            Pack->ohid.data[Pack->ohid.length++] = KEY2[ii];
            Pack->ohid.data[Pack->ohid.length++] = KEY3[ii];
            Pack->ohid.data[Pack->ohid.length++] = mm1[ii];
            Pack->ohid.data[Pack->ohid.length++] = mm2[ii];
            Pack->ohid.data[Pack->ohid.length++] = mm3[ii];
        }
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
// Trigger
void OHIDM_Driver_Trigger(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MM, const uint8_t Reset, const uint8_t RTmm, const uint8_t RTreset, const uint8_t mm1, const uint8_t mm2, const uint8_t mm3)
{
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_DRIVER_TRIGGER));
    else      OHIDM_init(Pack, OHID_CMD_DRIVER_TRIGGER);
    Pack->ohid.data[Pack->ohid.length++] = MM;
    Pack->ohid.data[Pack->ohid.length++] = Reset;
    Pack->ohid.data[Pack->ohid.length++] = RTmm;
    Pack->ohid.data[Pack->ohid.length++] = RTreset;
    Pack->ohid.data[Pack->ohid.length++] = mm1;
    Pack->ohid.data[Pack->ohid.length++] = mm2;
    Pack->ohid.data[Pack->ohid.length++] = mm3;
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Driver_Trigger(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint8_t MM, const uint8_t Reset, const uint8_t RTmm, const uint8_t RTreset, const uint8_t mm1, const uint8_t mm2, const uint8_t mm3)
{
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_DRIVER_TRIGGER, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_DRIVER_TRIGGER);
        Pack->ohid.data[Pack->ohid.length++] = MM;
        Pack->ohid.data[Pack->ohid.length++] = Reset;
        Pack->ohid.data[Pack->ohid.length++] = RTmm;
        Pack->ohid.data[Pack->ohid.length++] = RTreset;
        Pack->ohid.data[Pack->ohid.length++] = mm1;
        Pack->ohid.data[Pack->ohid.length++] = mm2;
        Pack->ohid.data[Pack->ohid.length++] = mm3;
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
// RGB_PARAM
void OHIDM_Driver_RGB_PARAM(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t gray, const uint8_t mode, const uint8_t speed, const uint8_t sleep, const uint8_t reverse)
{
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_DRIVER_RGB_PARAM));
    else      OHIDM_init(Pack, OHID_CMD_DRIVER_RGB_PARAM);
    Pack->ohid.data[Pack->ohid.length++] = gray;
    Pack->ohid.data[Pack->ohid.length++] = mode;
    Pack->ohid.data[Pack->ohid.length++] = speed;
    Pack->ohid.data[Pack->ohid.length++] = sleep;
    Pack->ohid.data[Pack->ohid.length++] = reverse;
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Driver_RGB_PARAM(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code,  const uint8_t gray, const uint8_t mode, const uint8_t speed, const uint8_t sleep, const uint8_t reverse)
{
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_DRIVER_RGB_PARAM, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_DRIVER_RGB_PARAM);
        Pack->ohid.data[Pack->ohid.length++] = gray;
        Pack->ohid.data[Pack->ohid.length++] = mode;
        Pack->ohid.data[Pack->ohid.length++] = speed;
        Pack->ohid.data[Pack->ohid.length++] = sleep;
        Pack->ohid.data[Pack->ohid.length++] = reverse;
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
// KRGB, 提供以下三种接口
void OHIDM_Driver_krgb1(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint32_t krgb[15])
{
    uint8_t col;
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_DRIVER_KRGB));
    else      OHIDM_init(Pack, OHID_CMD_DRIVER_KRGB);
    for(col=0; col<15; col++) OHID_add_32b(Pack, krgb[col]);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Driver_krgb1(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint32_t krgb[15])
{
    uint8_t col;
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_DRIVER_KRGB, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_DRIVER_KRGB);
        for(col=0; col<15; col++) OHID_add_32b(Pack, krgb[col]);
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
void OHIDM_Driver_krgb2(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[15], const uint8_t  RGBR[15], const uint8_t RGBG[15], const uint8_t RGBB[15])
{
    uint8_t col;
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_DRIVER_KRGB));
    else      OHIDM_init(Pack, OHID_CMD_DRIVER_KRGB);
    for(col=0; col<15; col++)
    {
        Pack->ohid.data[Pack->ohid.length++] = MKEY[col];
        Pack->ohid.data[Pack->ohid.length++] = RGBR[col];
        Pack->ohid.data[Pack->ohid.length++] = RGBG[col];
        Pack->ohid.data[Pack->ohid.length++] = RGBB[col];
    }
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Driver_krgb2(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code,  const uint8_t MKEY[15], const uint8_t  RGBR[15], const uint8_t RGBG[15], const uint8_t RGBB[15])
{
    uint8_t col;
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_DRIVER_KRGB, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_DRIVER_KRGB);
        for(col=0; col<15; col++)
        {
            Pack->ohid.data[Pack->ohid.length++] = MKEY[col];
            Pack->ohid.data[Pack->ohid.length++] = RGBR[col];
            Pack->ohid.data[Pack->ohid.length++] = RGBG[col];
            Pack->ohid.data[Pack->ohid.length++] = RGBB[col];
        }
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
void OHIDM_Driver_krgb3(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint8_t MKEY[15], const uint32_t RGB[15])
{
    uint8_t col;
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_DRIVER_KRGB));
    else      OHIDM_init(Pack, OHID_CMD_DRIVER_KRGB);
    for(col=0; col<15; col++)
    {
        Pack->ohid.data[Pack->ohid.length++] = MKEY[col];
        Pack->ohid.data[Pack->ohid.length++] = (RGB[col]>>16)&0xFF;
        Pack->ohid.data[Pack->ohid.length++] = (RGB[col]>>8)&0xFF;
        Pack->ohid.data[Pack->ohid.length++] = (RGB[col]>>0)&0xFF;
    }
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Driver_krgb3(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code,  const uint8_t MKEY[15], const uint32_t RGB[15])
{
    uint8_t col;
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_DRIVER_KRGB, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_DRIVER_KRGB);
        for(col=0; col<15; col++)
        {
            Pack->ohid.data[Pack->ohid.length++] = MKEY[col];
            Pack->ohid.data[Pack->ohid.length++] = (RGB[col]>>16)&0xFF;
            Pack->ohid.data[Pack->ohid.length++] = (RGB[col]>>8)&0xFF;
            Pack->ohid.data[Pack->ohid.length++] = (RGB[col]>>0)&0xFF;
        }
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
// IRGB
void OHIDM_Driver_IRGB(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint32_t irgb[15])
{
    uint8_t ii;
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_DRIVER_IRGB));
    else      OHIDM_init(Pack, OHID_CMD_DRIVER_IRGB);
    for(ii=0; ii<15; ii++) OHID_add_32b(Pack, irgb[ii]);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Driver_IRGB(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint32_t irgb[15])
{
    uint8_t ii;
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_DRIVER_IRGB, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_DRIVER_IRGB);
        for(ii=0; ii<15; ii++) OHID_add_32b(Pack, irgb[ii]);
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}
// Macro
void OHIDM_Driver_Macro(union OpenAgreementHID_t* const Pack, const uint8_t write, const uint32_t macro[14], uint8_t len, const uint16_t offset)
{
    uint8_t ii;
    if(write) OHIDM_init(Pack, OHID_CMD_WRITE(OHID_CMD_DRIVER_MACRO));
    else      OHIDM_init(Pack, OHID_CMD_DRIVER_MACRO);
    OHID_add_16b(Pack, offset);
    if(len>14) len = 14;
    OHID_add_8b(Pack, len);
    for(ii=0; ii<len; ii++) OHID_add_32b(Pack, macro[ii]);
    Pack->ohid.checksum = OHID_Checksum(Pack);
}
void OHIDS_Driver_Macro(union OpenAgreementHID_t* const Pack, const enum OHID_ERR_CODE code, const uint32_t macro[14], uint8_t len, const uint16_t offset)
{
    uint8_t ii;
    if(OHID_ERR_CODE_NONE!=code)
    {
        OHIDS_fail(Pack, OHID_CMD_DRIVER_MACRO, code);
    }
    else
    {
        OHIDS_init(Pack, OHID_CMD_DRIVER_MACRO);
        OHID_add_16b(Pack, offset);
        if(len>14) len = 14;
        OHID_add_8b(Pack, len);
        for(ii=0; ii<len; ii++) OHID_add_32b(Pack, macro[ii]);
        Pack->ohid.checksum = OHID_Checksum(Pack);
    }
}


