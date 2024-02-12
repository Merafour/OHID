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
#ifndef _OHID_TEST_H_
#define _OHID_TEST_H_

#include <stdint.h>
#include "OHID/OHID_Port.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* OHID_CMD_des(const enum OHID_CMD cmd);
extern void OHID_Pack_Test(void);
extern void OHID_signature_test(void);

#ifdef __cplusplus
}
#endif

#endif // _OHID_TEST_H_
