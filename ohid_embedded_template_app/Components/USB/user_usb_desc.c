/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : @merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : Bootloader.
* Description        : Hardware.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "USB/user_usb_desc.h"
#include "BL/jump.h"
#include "Hardware/HW.h"

static const uint8_t hex2str[17] = "0123456789ABCDEF";
uint8_t usb_uid_convert_sn(const uint8_t ID[12], uint8_t sn[16])
{
	uint32_t state;
	uint8_t ii;
	uint8_t _id;
	state=0;
	for(ii=0; ii<16; ii++) 
	{
		state = ohid_crc32(ID, 12, state);
		_id = (state&0xFF);
		// 0-F
		sn[ii] = (((_id>>4)&0x0F) + _id);
		sn[ii] = hex2str[sn[ii]&0x0F];
	}
	return 16;
}

uint8_t usb_uid_convert_sn_str(const uint8_t id[12], char sn[22])
{
	sn[0] = 'E';
	sn[1] = hex2str[((bl_code>>4)&0x0F)];
	sn[2] = hex2str[((bl_code>>0)&0x0F)];
	sn[3] = '-';
	usb_uid_convert_sn(id, (uint8_t*)(&sn[4]));
	sn[16+4] = '\0';
	return 20;
}
uint8_t string_unicode_buf[(32<<1)+2];
uint8_t descriptor_convert_unicode(const char string[], uint8_t unicode[], const uint16_t size)
{
    uint16_t cnt;
    unicode[0] = 0x00;
    unicode[1] = USB_DESC_TYPE_STRING;
    for(cnt=1; cnt<(size>>1); cnt++)
    {
        if('\0'==string[cnt-1]) break;
        unicode[cnt<<1] = string[cnt-1];
        unicode[(cnt<<1)+1] = 0x00;
    }
    unicode[0] = size+2;
    return unicode[0];
}
uint8_t descriptor_convert_unicode_sn(uint8_t unicode[])
{
	uint8_t id[12];
	uint8_t cnt;
	uint8_t Len;
	char sn[22];
	hw_uid_read(id);
	for(cnt=0; cnt<sizeof(sn); cnt++) sn[cnt] = 0x00;
	Len = usb_uid_convert_sn_str(id, sn);
	sn[22-1] = '\0';
	return descriptor_convert_unicode(sn, unicode, Len);
}

