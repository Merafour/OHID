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
#include <stdint.h>
#include "OHID/OHID_Pack.h"

uint8_t OHID_sub_keyboard_get_pos(const OHID_board_sub_keyboard_t* const sub_keyboard, const uint16_t key_value)
{
    uint8_t row, col;
    uint8_t pos = 0xFF;
    if(0x00==key_value) return 0xFF; // invalid
    for(row=0; row<OHID_KB_ROWS; row++)
    {
        for(col=0; col<OHID_KB_COLS; col++)
        {
            if(key_value==sub_keyboard->keys[row][col])
            {
                pos = OHID_kb_get_pos(row,col);
                return pos;
            }
        }
    }
    return 0xFF;//pos;
}



