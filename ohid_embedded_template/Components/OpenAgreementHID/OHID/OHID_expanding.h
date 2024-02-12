/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : OpenAgreementHID.扩展部分,用于扩展私有通讯.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#ifndef _OHID_EXPANDING_H_
#define _OHID_EXPANDING_H_

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 数据包格式:
 * | S/M    | Pack Len   | Des |        Head[4B]         | <Data> |
 * | Master | [4B]+[ nB] | CMD | 0xAA | len | 0x03 | crc | [ nB]  |
 * | Slave  | [4B]+[ nB] | ACK | 0xA5 | len | 0x03 | crc | [ nB]  |
 * head : 协议头;
 * len  : data长度;
 * CMD  : 命令编码, 固定为 0x03;
 * CRC  : 累加和校验, 计算方式为:crc=head+len+cmd+<data.end>+1+2+3+…+(len+3), 具体参考 OHID_Checksum 函数;
 * data : 数据域, 最大长度为 64*4-4 B ;
 */


#ifdef __cplusplus
}
#endif

#endif // _OHID_EXPANDING_H_
