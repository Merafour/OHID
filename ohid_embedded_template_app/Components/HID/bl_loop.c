/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : Bootloader.
* Description        : Boot Loop.
* Description        : PX4 Bootloader (https://github.com/Merafour/Bootloader)
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#include "main.h"
#include "Queue/merafour_queue_hid.h"
#include "Hardware/hw_flash.h"
#include "BL/jump.h"
#include "HID/bl.h"
#include "OHID/OHID_Pack.h"
#include "Hardware/HW.h"
#include "USB/user_usb_desc.h"

static struct mQueue_hid_t mq_hid;
static union OpenAgreementHID_t blPack;

void bl_init(void)
{
    mQueue_hid_init(&mq_hid);
}

static uint8_t bl_busy = 0;
void bl_receive(const uint8_t Recv[64])
{
    const uint8_t rPtr = mq_hid.rPtr;
    const uint8_t wPtr = mq_hid.wPtr;
    uint8_t wNext = wPtr+1;
    uint8_t ii;
    if(wNext>=MQUEUE_HID_SIZE) wNext=0;
    if(wNext != rPtr)
    {
        for(ii=0; ii<64; ii++) mq_hid.Datas[wPtr][ii] = Recv[ii];
        __asm("CPSID  I");
        mq_hid.wPtr = wNext;
        __asm("CPSIE  I");
    }
    else bl_busy = 1;
}

// 注: 在 USB模块中实现该函数
__weak void bl_send(uint8_t bin[], const uint16_t len)
{
	;
}

uint32_t fw_crc32(const uint32_t AppAddress, uint32_t CRCin, const uint32_t appSize, const uint32_t crc_size)
{
    uint8_t code[64];
    uint32_t len, pos;
    if(appSize<crc_size) return 0x0000;
    for(pos=0; pos<crc_size; pos+=sizeof(code))
    {
        len = crc_size-pos;
        if(len>sizeof(code)) len=sizeof(code);
        hw_flash_read(AppAddress+pos, code, len);
        CRCin = ohid_crc32(code, len, CRCin);
    }
    return CRCin;
}

static union {
	uint8_t bin[8];
	uint32_t Vector[2];
} VectorTable = {.Vector={0xFFFFFFFF, 0xFFFFFFFF}};
extern void read_mcu_uid(uint8_t id[12]);
enum sign_unlock_t {
    SIGN_UNLOCK_NONE          = 0x00,   
    SIGN_UNLOCK_SIGN          = 0x16,   
    SIGN_UNLOCK_ERASE         = 0x50,   
    SIGN_UNLOCK_REBOOT        = 0x72,   
	SIGN_UNLOCK_BOOT          = 0x85,   
    SIGN_UNLOCK_PROGRAM       = 0x98,   
    SIGN_UNLOCK_SAFE          = 0x34,   
};
static enum sign_unlock_t  sign_unlock = SIGN_UNLOCK_NONE;
#define   SIGN_TIMOUT_MS     (1000*12)
static uint32_t sign_timout_ms=0;
static uint8_t SN[16];
static volatile uint8_t sign_datas[16];
static const uint8_t sign_unlock_flag[7] = {
	SIGN_UNLOCK_NONE, 
	SIGN_UNLOCK_SIGN, 
	SIGN_UNLOCK_ERASE,
	SIGN_UNLOCK_REBOOT, 
	SIGN_UNLOCK_BOOT, 
	SIGN_UNLOCK_PROGRAM, 
	SIGN_UNLOCK_SAFE,
};

int bl_loop(const uint32_t _time_ms)
{
    volatile int len;
    const uint32_t appSize=app_size_max;
    volatile uint32_t address=0x00;
	volatile uint32_t wCRCin=0x0000, CRC16=0x0000;
	uint32_t Size;
	enum OHID_CMD cmd;
	enum OHID_UNLOCK unlock;
	uint8_t SIGN[16];
	uint8_t Datas[16];
	uint8_t signature[16];
    if((SIGN_UNLOCK_NONE!=sign_unlock) && (sign_timout_ms<_time_ms))
    {
		sign_unlock = SIGN_UNLOCK_NONE;
        sign_timout_ms = _time_ms+SIGN_TIMOUT_MS;
    }
	if(bl_busy)
	{
		bl_busy=0;
		OHIDS_fail(&blPack, OHID_CMD_BASE_FAIL, OHID_ERR_CODE_BUSY);
		hw_delay_ms(1);
		bl_send(blPack.bin, OHID_pack_size(blPack));
		hw_delay_ms(1);
	}
	memset(&blPack,0,sizeof(blPack));
	len = 0;
	Size = mQueue_hid_peeks(&mq_hid, blPack.bin, sizeof(blPack.bin)/64);
	if(0==Size)
	{
		hw_delay_ms(1);
		return -1;
	}
	if(OHID_PACK_HEAD_MASTRT!=blPack.ohid.HEAD) 
	{
		mQueue_hid_delete(&mq_hid, 1);
		return -2;
	}
	len = _OHIDM_Decode(&blPack, Size*64);
	if(-4==len)
	{
		hw_delay_ms(1);
		return -3;
	}
    if(len < 4)
	{
		mQueue_hid_delete(&mq_hid, 1);
		OHIDS_fail(&blPack, OHID_CMD_BASE_FAIL, OHID_ERR_CODE_PACK);
		bl_send(blPack.bin, OHID_pack_size(blPack));
		hw_delay_ms(1);
		return -4;
	}
	mQueue_hid_delete(&mq_hid, (len+63)/64);
	cmd = (enum OHID_CMD)blPack.ohid.cmd;
	switch(OHID_CMD_GET(cmd))
	{
	case OHID_CMD_BASE_SYNC:
		{
			uint8_t id[12];
			memset(SN, 0, sizeof(SN));
			hw_uid_read(id);
			usb_uid_convert_sn(id, (uint8_t*)SN);
			OHIDS_Base_sync(&blPack, OHID_ERR_CODE_NONE, OHID_TARGET_ID, app_size_max>>8, OHID_RUN_MODE_KEYBOARD, (char*)SN, OHID_BL_Version);
			sign_unlock = SIGN_UNLOCK_NONE;
		}
		break;
	case OHID_CMD_IAP_SIGN:
		for(len=0; len<16; len++) 
		{
			SIGN[len]  = blPack.ohid.data[1+1+len];
			Datas[len] = blPack.ohid.data[1+17+1+len];
			signature[len] = 0;
		}
		unlock = (enum OHID_UNLOCK)blPack.ohid.data[0];
		OHID_Signature(Datas, OHID_signature_key_main, OHID_signature_params1, (uint8_t)unlock, signature);
		if(OHID_UNLOCK_SIGN==unlock) 
		{
			for(len=0; len<16; len++) if(Datas[len] != SN[len]) {len=0; break;}
		}
		else 
		{
			for(len=0; len<16; len++) if(Datas[len] != sign_datas[len]) {len=0; break;}
		}
		if(16 != len) 
		{
			OHIDS_IAP_Sign(&blPack, OHID_ERR_CODE_SIGN_DATE, unlock, Datas);
			break;
		}
		OHID_Signature(Datas, OHID_signature_key_main, OHID_signature_params1, unlock, signature);
		for(len=0; len<16; len++) 
		{
			Datas[len] += ((_time_ms + (_time_ms>>8) + (_time_ms>>16))&0xFF);
			sign_datas[len] = Datas[len];
		}
		for(len=0; len<16; len++) if(SIGN[len] != signature[len]) {len=0; break;}
		if(16 != len) 
		{
			OHIDS_IAP_Sign(&blPack, OHID_ERR_CODE_SIGN, unlock, Datas);
			break;
		}
		if((OHID_UNLOCK_SIGN!=unlock) && (SIGN_UNLOCK_SIGN!=sign_unlock))
		{
			OHIDS_IAP_Sign(&blPack, OHID_ERR_CODE_SIGN_SHAKE1, unlock, Datas);
			break;
		}
		sign_timout_ms = _time_ms+SIGN_TIMOUT_MS;
		if(unlock<=OHID_UNLOCK_SAFE) 
		{
			sign_unlock = (enum sign_unlock_t)sign_unlock_flag[unlock];
			OHIDS_IAP_Sign(&blPack, OHID_ERR_CODE_NONE, unlock, Datas);
		}
		else
		{
			sign_unlock = SIGN_UNLOCK_NONE;
			OHIDS_IAP_Sign(&blPack, OHID_ERR_CODE_SIGN_SHAKE2, unlock, Datas);
		}
		break;
	case OHID_CMD_IAP_ERASE:
		if(SIGN_UNLOCK_ERASE!=sign_unlock)
		{
			OHIDS_IAP_Erase(&blPack, OHID_ERR_CODE_SIGN_INVALID, 0xFF);
			break;
		}
		address = OHID_get_32b(&blPack, 0);
		Size = OHID_get_32b(&blPack, 4);
		if((address+Size)>(appSize) )
		{
			OHIDS_IAP_Erase(&blPack, OHID_ERR_CODE_OVER_SIZE, 0xFF);			
			break;
		}
		hw_flash_erase_vector(ApplicationAddress);
		OHIDS_IAP_Erase(&blPack, OHID_ERR_CODE_NONE, 0xFF);
		sign_timout_ms = _time_ms+SIGN_TIMOUT_MS;
		mQueue_hid_init(&mq_hid);
		break;
	case OHID_CMD_IAP_REBOOT:
		if(SIGN_UNLOCK_REBOOT!=sign_unlock)
		{
			OHIDS_IAP_Reboot(&blPack, OHID_ERR_CODE_SIGN_INVALID, _time_ms&0xFF);
			break;
		}
		OHIDS_IAP_Reboot(&blPack, OHID_ERR_CODE_NONE, _time_ms&0xFF);
		bl_send(blPack.bin, OHID_pack_size(blPack));
		hw_delay_ms(3); // 3 ms
		__disable_irq();
		NVIC_SystemReset();
		break;
	case OHID_CMD_IAP_RCRC:
		address = OHID_get_32b(&blPack, 0);
		Size = OHID_get_32b(&blPack, 4);
		wCRCin  = 0;
		if(((address+Size)>=(appSize)) || (Size<sizeof(VectorTable)) )
		{
			OHIDS_IAP_RCRC(&blPack, OHID_ERR_CODE_OVER_SIZE, address, Size, wCRCin);
			break;
		}
		if((0==address) && (0xFFFFFFFF!=VectorTable.Vector[0]) && (0xFFFFFFFF!=VectorTable.Vector[1]))
		{
			wCRCin = ohid_crc32(VectorTable.bin, sizeof(VectorTable.bin), 0x00);
			wCRCin = fw_crc32(ApplicationAddress+address+sizeof(VectorTable), wCRCin, appSize, Size-sizeof(VectorTable));
		}
		else
		{
			wCRCin = fw_crc32(ApplicationAddress+address, 0, appSize, Size);
		}
		OHIDS_IAP_RCRC(&blPack, OHID_ERR_CODE_NONE, address, Size, wCRCin);
		break;
	default:
		OHIDS_fail(&blPack, cmd, OHID_ERR_CODE_INVALID);
		sign_unlock = SIGN_UNLOCK_NONE;
		break;
	}
	bl_send(blPack.bin, OHID_pack_size(blPack));
	return 0;
}

