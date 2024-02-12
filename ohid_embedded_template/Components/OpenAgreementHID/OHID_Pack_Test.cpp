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
#include <stdio.h>
#include <math.h>
#include "OHID/OHID_Pack.h"
#include "OHID_Pack_Test.h"

struct OHID_cmd_des_info{
    const enum OHID_CMD cmd;
    const char* const des;
};
static struct OHID_cmd_des_info cmd_des[] = {
/***************** Base ********************/
{OHID_CMD_BASE_FAIL,           "OHID_CMD_BASE_FAIL"},
{OHID_CMD_BASE_MIX,            "OHID_CMD_BASE_MIX"},
{OHID_CMD_BASE_SYNC,           "OHID_CMD_BASE_SYNC"},
{OHID_CMD_BASE_SAFE,           "OHID_CMD_BASE_SAFE"},
{OHID_CMD_BASE_EXPANDING,      "OHID_CMD_BASE_EXPANDING"},
/***************** BLE ********************/
{OHID_CMD_BLE,                 "OHID_CMD_BLE"},
{OHID_CMD_BRP,                 "OHID_CMD_BRP"},
/***************** BOOT ********************/
{OHID_CMD_IAP_SIGN,            "OHID_CMD_IAP_SIGN"},
{OHID_CMD_IAP_ERASE,           "OHID_CMD_IAP_ERASE"},
{OHID_CMD_IAP_REBOOT,          "OHID_CMD_IAP_REBOOT"},
{OHID_CMD_IAP_JUMP,            "OHID_CMD_IAP_JUMP"},
{OHID_CMD_IAP_PROGRAM,         "OHID_CMD_IAP_PROGRAM"},
{OHID_CMD_IAP_RCRC,            "OHID_CMD_IAP_RCRC"},
/***************** Driver ********************/
{OHID_CMD_DRIVER_PARAM,        "OHID_CMD_DRIVER_PARAM"},
{OHID_CMD_DRIVER_MT,           "OHID_CMD_DRIVER_MT"},
{OHID_CMD_DRIVER_TGL,          "OHID_CMD_DRIVER_TGL"},
{OHID_CMD_DRIVER_DKS,          "OHID_CMD_DRIVER_DKS"},
{OHID_CMD_DRIVER_AKS3,         "OHID_CMD_DRIVER_AKS3"},
{OHID_CMD_DRIVER_TRIGGER,      "OHID_CMD_DRIVER_TRIGGER"},
{OHID_CMD_DRIVER_RGB_PARAM,      "OHID_CMD_DRIVER_TRIGGER"},
{OHID_CMD_DRIVER_KRGB,         "OHID_CMD_DRIVER_KRGB"},
{OHID_CMD_DRIVER_IRGB,         "OHID_CMD_DRIVER_IRGB"},
{OHID_CMD_DRIVER_MACRO,        "OHID_CMD_DRIVER_MACRO"},
{(enum OHID_CMD)0x00,          "NONE"},
};
static int cmd_des_size = sizeof(cmd_des)/sizeof(cmd_des[0]);
const char* OHID_CMD_des(const enum OHID_CMD cmd)
{
    int i;
    static const char none[] = "NONE";
    static char des[64] = "NONE ";
    for(i=0; i<cmd_des_size; i++)
    {
        if(cmd==cmd_des[i].cmd) return cmd_des[i].des;
    }
    for(i=0; i<cmd_des_size; i++)
    {
        if((cmd&0x7F)==cmd_des[i].cmd)
        {
            strcpy(&des[4], cmd_des[i].des);
            return des;
        }
    }
    return none;
}

static void OHIDM_print_pack(union OpenAgreementHID_t* const Pack)
{
    union OpenAgreementHID_t pack;
    printf("\r\nPack len:%d cmd[0x%02X]:%s [", (Pack->ohid.length), Pack->ohid.cmd, OHID_CMD_des((enum OHID_CMD)(Pack->ohid.cmd)));
    for(int len=0; len<(OHID_HEAD_SIZE+Pack->ohid.length); len++)
    {
        printf(" 0x%02X", Pack->bin[len]);
        if(0x1F==(len&0x1F)) printf("\r\n");
    }
    printf("] \r\n");
    printf("OHIDM_decode:%d \r\n", OHIDM_decode(&pack, Pack->bin, OHID_ppack_size(Pack)));
}
static void OHIDS_print_pack(union OpenAgreementHID_t* const Pack)
{
    union OpenAgreementHID_t pack;
    printf("\r\nPack len:%d cmd[0x%02X]:%s [", (Pack->ohid.length), Pack->ohid.cmd, OHID_CMD_des((enum OHID_CMD)(Pack->ohid.cmd)));
    for(int len=0; len<(OHID_HEAD_SIZE+Pack->ohid.length); len++)
    {
        printf(" 0x%02X", Pack->bin[len]);
        if(0x1F==(len&0x1F)) printf("\r\n");
    }
    printf("] \r\n");
    printf("OHIDS_decode:%d \r\n", OHIDS_decode(&pack, Pack->bin, OHID_ppack_size(Pack)));
}

void OHID_Pack_Test(void)
{
    const uint8_t SN[] = "SN";
    const uint8_t Encryption[] = "Encryption";
    const uint8_t SIGN[32] = "3838270007774514";
    const uint8_t Datas[32] = __DATE__;
    const uint8_t MKEY[32]  = __DATE__;
    const enum OHID_PARAM_ITEM Item[32] = {OHID_PARAM_ITEM_FN1};
    const uint16_t Value[32] = {0xAA};
    const uint8_t KEY1[32] = __DATE__;
    const uint8_t KEY2[32] = __DATE__;
    const uint8_t KEY3[32] = __DATE__;
    const uint8_t KEY4[32] = __DATE__;
    const uint8_t delay[32] = __DATE__;
    const uint8_t TrPs1[32] = __DATE__;
    const uint8_t TrPs2[32] = __DATE__;
    const uint8_t TrPs3[32] = __DATE__;
    const uint8_t TrPs4[32] = __DATE__;
    //const uint8_t mm[32]  = __DATE__;
    const uint8_t mm1[32] = __DATE__;
    const uint8_t mm2[32] = __DATE__;
    const uint8_t mm3[32] = __DATE__;
    const uint8_t RGBR[15]   = __DATE__;
    const uint8_t RGBG[15]   = __DATE__;
    const uint8_t RGBB[15]   = __DATE__;
    const uint32_t krgb[15]  = {0xAA};
    const uint32_t RGB[15]   = {0xAA};
    const uint32_t irgb[15]  = {0xAA};
    const uint32_t macro[14] = {0xAA};
    const uint8_t write = 0x00;
    /******************************* Base ***********************************/
    printf("[%s--%d] Date[%s] Time[%s]\r\n", __func__, __LINE__, __DATE__, __TIME__);
    printf("[%s--%d] Date[%s]\r\n", __func__, __LINE__, __DATE__ "-" __TIME__);
    union OpenAgreementHID_t Pack;
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDS_fail(&Pack, OHID_CMD_BASE_MIX, OHID_ERR_CODE_ARG);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDM_None(&Pack, OHID_CMD_BASE_MIX);
    OHIDM_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDS_None(&Pack, OHID_CMD_BASE_MIX);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDS_Base_sync(&Pack, OHID_ERR_CODE_NONE, OHID_BOARD_MACHINE, 64*3, OHID_RUN_MODE_KEYBOARD, "1234567890ABCDEFH", "1234567890ABCDEFH");
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDM_Base_SAFE(&Pack, 0, SN, Encryption);
    OHIDM_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDS_Base_SAFE(&Pack, OHID_ERR_CODE_NONE, SN, Encryption);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDM_IAP_Sign(&Pack, OHID_UNLOCK_SIGN, SIGN, Datas);
    OHIDM_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDS_IAP_Sign(&Pack, OHID_ERR_CODE_NONE, OHID_UNLOCK_SIGN, Datas);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDM_IAP_Erase(& Pack, 0, 1024*64);
    OHIDM_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDS_IAP_Erase(&Pack, OHID_ERR_CODE_NONE, 0xFF);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDM_IAP_Reboot(&Pack, (rand()&0xFF));
    OHIDM_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDS_IAP_Reboot(&Pack, OHID_ERR_CODE_NONE, (rand()&0xFF));
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDM_IAP_Jump(&Pack, 0, 1024*64, (rand()&0xFFFF));
    OHIDM_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDS_IAP_Jump(&Pack, OHID_ERR_CODE_NONE, 0, 1024*64, (rand()&0xFFFF));
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDM_IAP_Program(&Pack, 1, 0, 32, Datas);
    OHIDM_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDS_IAP_Program(&Pack, OHID_ERR_CODE_NONE, 0, 32, Datas);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDM_IAP_RCRC(&Pack, 0, 1024*64);
    OHIDM_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDS_IAP_RCRC(&Pack, OHID_ERR_CODE_NONE, 0, 1024*64, (rand()&0xFFFF));
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDM_Driver_Param(&Pack, 1, MKEY, Item, Value);
    OHIDM_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDS_Driver_Param(&Pack, OHID_ERR_CODE_NONE, MKEY, Item, Value);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDM_Driver_MT(&Pack, write, MKEY, KEY1, KEY2, delay);
    OHIDM_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDS_Driver_MT(&Pack, OHID_ERR_CODE_NONE, MKEY, KEY1, KEY2, delay);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    OHIDM_Driver_TGL(&Pack, write, MKEY, KEY1, delay);
    OHIDM_print_pack(&Pack);
    OHIDS_Driver_TGL(&Pack, OHID_ERR_CODE_NONE, MKEY, KEY1, delay);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    // DKS, Reference Wooting
    OHIDM_Driver_DKS(&Pack, write, MKEY, KEY1, KEY2, KEY3, KEY4, TrPs1, TrPs2, TrPs3, TrPs4, mm1);
    OHIDM_print_pack(&Pack);
    OHIDS_Driver_DKS(&Pack, OHID_ERR_CODE_NONE, MKEY, KEY1, KEY2, KEY3, KEY4, TrPs1, TrPs2, TrPs3, TrPs4, mm1);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    // AKS, Reference 海盗船
    OHIDM_Driver_AKS3(&Pack, write, MKEY, KEY1, KEY2, KEY3, mm1, mm2, mm3);
    OHIDM_print_pack(&Pack);
    OHIDS_Driver_AKS3(&Pack, OHID_ERR_CODE_NONE, MKEY, KEY1, KEY2, KEY3, mm1, mm2, mm3);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    // Trigger
    OHIDM_Driver_Trigger(&Pack, write, 15, 3, 10, 3, 10, 20, 30);
    OHIDM_print_pack(&Pack);
    OHIDS_Driver_Trigger(&Pack, OHID_ERR_CODE_NONE, 15, 3, 10, 3, 10, 20, 30);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    // RGB_PARAM
    OHIDM_Driver_RGB_PARAM(&Pack, write, 3, 10, 2, 0, 0);
    OHIDM_print_pack(&Pack);
    OHIDS_Driver_RGB_PARAM(&Pack, OHID_ERR_CODE_NONE, 3, 10, 2, 0, 0);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    // KRGB, 提供以下三种接口
    OHIDM_Driver_krgb1(&Pack, write, krgb);
    OHIDM_print_pack(&Pack);
    OHIDS_Driver_krgb1(&Pack, OHID_ERR_CODE_NONE,  krgb);
    OHIDS_print_pack(&Pack);
    OHIDM_Driver_krgb2(&Pack, write, MKEY, RGBR, RGBG, RGBB);
    OHIDM_print_pack(&Pack);
    OHIDS_Driver_krgb2(&Pack, OHID_ERR_CODE_NONE,  MKEY, RGBR, RGBG, RGBB);
    OHIDS_print_pack(&Pack);
    OHIDM_Driver_krgb3(&Pack, write, MKEY, RGB);
    OHIDM_print_pack(&Pack);
    OHIDS_Driver_krgb3(&Pack, OHID_ERR_CODE_NONE,  MKEY, RGB);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    // IRGB
    OHIDM_Driver_IRGB(&Pack, write, irgb);
    OHIDM_print_pack(&Pack);
    OHIDS_Driver_IRGB(&Pack, OHID_ERR_CODE_NONE, irgb);
    OHIDS_print_pack(&Pack);
    printf("[%s--%d] \r\n", __func__, __LINE__);
    // Macro
    OHIDM_Driver_Macro(&Pack, write, macro, 14, 0);
    OHIDM_print_pack(&Pack);
    OHIDS_Driver_Macro(&Pack, OHID_ERR_CODE_NONE, macro, 14, 0);
    OHIDS_print_pack(&Pack);

    /******************************* Driver ***********************************/

    fflush(stdout);
}

static void signature_print(const char datas[], const uint8_t signature[16], const uint8_t cnt)
{
    uint8_t ii;
    printf("[%s--%d] signature ...%d\r\n", __func__, __LINE__, cnt);
    printf("[%s--%d] datas:[%s]\r\n", __func__, __LINE__, datas);
    printf("[%s--%d] signature:[ ", __func__, __LINE__);
    for(ii=0; ii<16; ii++) printf("%02XH ", signature[ii]);
    printf("]\r\n");
}
void OHID_signature_test(void)
{
    char SN1[32] = "3838270007774514";
    char SN2[32] = "3838270007774515";
    char SN3[32] = "6838270007774515";
    uint8_t signature[32];
    memset(signature, 0x00, sizeof(signature));
    OHID_Signature((const uint8_t*)SN1, OHID_signature_key_main, OHID_signature_params1, OHID_UNLOCK_SIGN, signature);
    printf("[%s--%d] signature :[ %02XH %02XH %02XH %02XH ]\r\n", __func__, __LINE__, signature[0], signature[1], signature[2], signature[3]);
    signature_print(SN1, signature, 1);
    memset(signature, 0x00, sizeof(signature));
    OHID_Signature((const uint8_t*)SN2, OHID_signature_key_main, OHID_signature_params1, OHID_UNLOCK_SIGN, signature);
    signature_print(SN2, signature, 2);
    memset(signature, 0x00, sizeof(signature));
    OHID_Signature((const uint8_t*)SN3, OHID_signature_key_main, OHID_signature_params1, OHID_UNLOCK_SIGN, signature);
    signature_print(SN3, signature, 3);
    fflush(stdout);
}
