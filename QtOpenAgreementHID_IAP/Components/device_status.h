/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : OpenAgreementHID.
* Description        : 状态机
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#ifndef _DEVICE_STATUS_H_
#define _DEVICE_STATUS_H_

#ifdef __cplusplus
extern "C"
{
#endif

enum DEVICE_STATUS {
    DEVICE_NONE         = 0x00,
    DEVICE_IDEL         = 0x01,
    DEVICE_STOP         = 0x02,
    DEVICE_OPEN         = 0x03,
    DEVICE_SCAN         = 0x04,
    DEVICE_SYNC         = 0x05,
    DEVICE_REBOOT       = 0x06,
    DEVICE_ERASE        = 0x07,
    DEVICE_WRITE        = 0x08,
    DEVICE_READ         = 0x09,
    DEVICE_GCRC         = 0x0A,
    DEVICE_JUMP         = 0x0B,
    DEVICE_WATTING      = 0x0C,
} ;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_STATUS_H_
