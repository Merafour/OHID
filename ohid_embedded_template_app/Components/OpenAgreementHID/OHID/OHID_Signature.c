/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : OpenAgreementHID.
* Description        : 开源键盘通信协议 API 接口.
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#include <string.h>
#include "OHID/OHID_Pack.h"

const uint8_t OHID_signature_key_main[16] = {0x80,0x1F,0x0A,0x22, 0xBE,0xF0,0x3D,0xDC, 0xDB,0xF6,0x51,0x6F, 0xF8,0x63,0x01,0xBA};
const uint8_t OHID_signature_params1[16]  = {0x42,0x3B,0x90,0x04, 0xBD,0x08,0xB1,0x48, 0x41,0xD0,0xA5,0xCF, 0xC2,0x44,0xA3,0x04};
const uint8_t OHID_signature_params2[16]  = {0xD4,0x3A,0x7C,0x85, 0x6D,0x5D,0x7F,0x76, 0xED,0x23,0x9A,0x1A, 0x5D,0x00,0xC3,0xA4};

#if 0
static uint32_t crc32(const uint8_t *src, unsigned len, unsigned state)
{
	static uint32_t crctab[256];
	/* check whether we have generated the CRC table yet */
	/* this is much smaller than a static table */
	if (crctab[1] == 0) {
		for (unsigned i = 0; i < 256; i++) {
			uint32_t c = i;

			for (unsigned j = 0; j < 8; j++) {
				if (c & 1) {
					c = 0xedb88320U ^ (c >> 1);

				} else {
					c = c >> 1;
				}
			}

			crctab[i] = c;
		}
	}
	for (unsigned i = 0; i < len; i++) {
		state = crctab[(state ^ src[i]) & 0xff] ^ (state >> 8);
	}
	return state;
}
#else
uint32_t ohid_crc32(const uint8_t *src, const uint32_t len, uint32_t state)
{
	uint32_t crctab, cnt;
	uint32_t ii;
	for (ii = 0; ii < len; ii++) 
	{
		crctab = ((state ^ src[ii]) & 0xff);
		for (cnt = 0; cnt < 8; cnt++) 
		{
			if (crctab & 1) 
			{
				crctab = 0xedb88320U ^ (crctab >> 1);

			} else {
				crctab = crctab >> 1;
			}
		}
		state = crctab ^ (state >> 8);
	}
	return state;
}
#endif
void OHID_Signature(const uint8_t Datas[16], const uint8_t KEYS[16], const uint8_t params[16], const uint8_t unlock, uint8_t SIGN[16])
{
	uint32_t state;
    uint8_t datas[16];
	uint8_t cnt;
    for(cnt=0; cnt<16; cnt++) datas[cnt] = KEYS[cnt] + Datas[cnt] + params[cnt];
	state = 0;
    for(cnt=0; cnt<16; cnt++)
    {
        datas[cnt] += KEYS[cnt] ^ (state&0xFF);
        state = ohid_crc32(datas, 16, state)+unlock;
        SIGN[cnt] = ((state + (state>>8)) & 0xFF);
    }
}
