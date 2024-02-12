/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : Bootloader.
* Description        : jump to app.
* Description        : PX4 Bootloader (https://github.com/Merafour/Bootloader)
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#include "BL/jump.h"
#include "Hardware/HW.h"
#include "main.h"
#include "OHID/OHID_Pack.h"

uint8_t bl_code = 0x00;
const uint32_t ohid_target_id = OHID_TARGET_ID;
extern  const uint32_t __Vectors;
extern  const uint32_t __Vectors_End;
extern  const uint32_t __Vectors_Size;
//const uint32_t vectors_code_addr = (uint32_t)&__Vectors;
//const uint32_t vectors_code_end  = (uint32_t)&__Vectors_End;
const uint32_t vectors_code_size = (uint32_t)&__Vectors_Size;

#define   OHID_TARGET_CRC    (0x80151274)
#define   OHID_TARGET_SIGN   (0x13480151)
__attribute__ ((section ("ohid_target"))) const uint32_t a_ohid_target_sign[] = {
	OHID_TARGET_ID,
	OHID_TARGET_CRC,                   // appCRC
	0xFFFFFFFF,                        // random
	0xFFFFFFFF,                        // signature
	0xFFFFFFFF,                        // FW_Size
	0xFFFFFFFF,                        // FW_CRC
	0xFFFFFFFF,                        // BL ID encrypt
	0xFFFFFFFF,                        // BL ID encrypt
	0xFFFFFFFF,                        // BL ID encrypt
	0xFFFFFFFF,                        // FW ID encrypt
	0xFFFFFFFF,                        // FW ID encrypt
	0xFFFFFFFF,                        // FW ID encrypt
	0xFFFFFFFF,                        // FW ID encrypt
	0xFFFFFFFF,                        // FW ID encrypt
	0xFFFFFFFF,                        // FW ID encrypt
	0xFFFFFFFF,                        // FW ID encrypt
	0xFFFFFFFF,                        // FW ID encrypt
	0xFFFFFFFF,                        // SN
	0xFFFFFFFF,                        // SN
	0xFFFFFFFF,                        // SN
	0xFFFFFFFF,                        // SN
	0xFFFFFFFF,                        // SN
	0xFFFFFFFF,                        // SN
	0xFFFFFFFF,                        // SN
	0xFFFFFFFF,                        // SN
};

typedef  void (*pFunction)(void);

__weak void close_device(void)
{
}

static void do_jump(uint32_t stacktop, uint32_t entrypoint)
{
	pFunction Jump_To_Application;
#if 0
	asm volatile(
		"msr msp, %0	\n"
		"bx	%1	\n"
		: : "r"(stacktop), "r"(entrypoint) :);

	// just to keep noreturn happy
	for (;;) ;
#else
	Jump_To_Application = (pFunction) entrypoint;
	/* Initialize user application's Stack Pointer */
	__set_MSP(stacktop);
	Jump_To_Application();
	// app do not return!!!
	hw_delay_ms(100); 
	__disable_irq();
	NVIC_SystemReset();
#endif
}

void jump_app_sign(const uint32_t app_load_address, const uint32_t fw_size)
{
	const uint32_t* const app_vectors = (const uint32_t*)(app_load_address);
	//volatile uint32_t appCRC = 0xFFFFFFFF;
	volatile uint32_t cnt = 0;
	volatile uint32_t random = 0;
	volatile uint32_t signature = 0;
	volatile uint32_t FW_Size = 0;
	volatile uint32_t FW_CRC = 0;
	
	for(cnt = 10; cnt<(vectors_code_size>>2); cnt++)
	{
		if(OHID_TARGET_SIGN==app_vectors[cnt]) 
		{
			//appCRC    = app_vectors[cnt-5]; 
			random    = app_vectors[cnt-4];
			//signature = app_vectors[cnt-3];
			//FW_Size   = app_vectors[cnt-2];
			//FW_CRC    = app_vectors[cnt-1];
			signature = (random^(ohid_target_id+OHID_TARGET_CRC));
			FW_Size   = fw_size;
			FW_CRC    = ohid_crc32((uint8_t*)(app_load_address+0x200), FW_Size, 0x00);
			hw_flash_write(app_load_address+4*(cnt-3), (uint8_t*)&signature, sizeof(signature));
			hw_flash_write(app_load_address+4*(cnt-2), (uint8_t*)&FW_Size,   sizeof(FW_Size));
			hw_flash_write(app_load_address+4*(cnt-1), (uint8_t*)&FW_CRC,    sizeof(FW_CRC));
			break;
		}
	}
}
void jump_to_app(const uint32_t app_load_address, const uint32_t fw_size)
{
	const uint32_t* const app_vectors = (const uint32_t*)(app_load_address);
	volatile uint32_t appCRC = 0xFFFFFFFF;
	volatile uint32_t target_id;// = 0xFFFFFFFF;
	volatile uint32_t cnt = 0;
	volatile uint32_t random = 0;
	volatile uint32_t signature = 0;
	volatile uint32_t FW_Size = 0;
	volatile uint32_t FW_CRC = 0;
	uint32_t addr;
	
	//HAL_FLASH_Unlock();
	for(cnt = 10; cnt<((vectors_code_size+64)>>2); cnt++)
	{
		if(OHID_TARGET_SIGN==app_vectors[cnt]) 
		{
			addr = app_vectors[cnt-6];
			if((addr>app_load_address) && (addr<(app_load_address+fw_size))) 
			{
				target_id = *(const uint32_t*)addr;
			}
			else target_id = 0xFFFFFFFF;
			appCRC    = app_vectors[cnt-5]; 
			random    = app_vectors[cnt-4];
			signature = app_vectors[cnt-3];
			FW_Size   = app_vectors[cnt-2];
			FW_CRC    = app_vectors[cnt-1];
			break;
		}
	}
	/* Enable the SYSCFG peripheral clock*/ 
	//HAL_FLASH_Lock();

	// app_load_address+0xC8 地址设置一随机数作为校验
	if(OHID_TARGET_CRC!=appCRC) 
	{
		bl_code = 0x01;
		return ;
	}
	// board_id 错误表示板子与硬件不匹配,不能执行该固件
	if(ohid_target_id!=target_id)
	{
		bl_code = 0x06;
		return ;
	}
	/*
	 * We refuse to program the first word of the app until the upload is marked
	 * complete by the host.  So if it's not 0xffffffff, we should try booting it.
	 */
	if (app_vectors[0] == 0xFFFFFFFF) {
		bl_code = 0x02;
		return;
	}

	/*
	 * The second word of the app is the entrypoint; it must point within the
	 * flash area (or we have a bad flash).
	 */
	if (app_vectors[1] < app_load_address) {
		bl_code = 0x03;
		return;
	}

	if (app_vectors[1] >= (app_load_address + fw_size)) {
		bl_code = 0x04;
		return;
	}
	// check sp
	if (0x20000000 != (app_vectors[0] & 0x2FFE0000 ))
	{
		bl_code = 0x05;
		return ;
	}
	if(0xFFFFFFFF==random)  
	{
		uint32_t id[12/4];
		uint32_t wRandom = 0;
		hw_uid_read((uint8_t*)id);
		wRandom = id[0] + id[1] + id[2] + ohid_target_id + (uint32_t)(&jump_to_app);
		// Write
		/*if((addr>app_load_address) && (addr<(app_load_address+fw_size))) 
		{
			hw_flash_write(app_load_address+4*(cnt-4), (uint8_t*)&wRandom, sizeof(wRandom));
		}*/
		hw_flash_write(app_load_address+4*(cnt-4), (uint8_t*)&wRandom, sizeof(wRandom));
	}
	else
	{
		if(signature != (random^(ohid_target_id+OHID_TARGET_CRC)))
		{
			bl_code = 0x07;
			return ;
		}
		if(FW_Size>fw_size)
		{
			bl_code = 0x08;
			return ;
		}
		if(FW_CRC != ohid_crc32((uint8_t*)(app_load_address+0x200), FW_Size, 0x00))
		{
			bl_code = 0x09;
			return ;
		}
	}
	close_device();
	/* switch exception handlers to the application */
	//SCB_VTOR = app_load_address;
	//NVIC_SetVectorTable(app_load_address);
	/* extract the stack and entrypoint from the app vector table and go */
	do_jump(app_vectors[0], app_vectors[1]);
}
