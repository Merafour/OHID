/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : Bootloader.
* Description        : Flash.
* Description        : https://www.cnblogs.com/dreamrj/p/14044349.html
* Description        : STM32F401CCU6 ==> Flash(256KB) + RAM(64KB)
* Description        : Flash(256KB) = 16+16+16+16 +64 +128
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#include "Hardware/hw_flash.h"

#if 0
* flash parameters that we should not really know */
static struct {
	uint32_t	sector_number;
	uint32_t	size;
} flash_sectors[] = {

	/* Physical FLASH sector 0 is reserved for bootloader and is not
	 * the table below.
	 * N sectors may aslo be reserved for the app fw in which case
	 * the zero based define BOARD_FIRST_FLASH_SECTOR_TO_ERASE must
	 * be defined to begin the erase above of the reserved sectors.
	 * The default value of BOARD_FIRST_FLASH_SECTOR_TO_ERASE is 0
	 * and begins flash erase operations at phsical sector 1 the 0th entry
	 * in the table below.
	 * A value of 1 for BOARD_FIRST_FLASH_SECTOR_TO_ERASE would reserve
	 * the 0th entry and begin erasing a index 1 the third physical sector
	 * on the device.
	 *
	 * When BOARD_FIRST_FLASH_SECTOR_TO_ERASE is defined APP_RESERVATION_SIZE
	 * must also be defined to remove that additonal reserved FLASH space
	 * from the BOARD_FLASH_SIZE. See APP_SIZE_MAX below.
	 */

	{0x01, 16 * 1024},
	{0x02, 16 * 1024},
	{0x03, 16 * 1024},
	{0x04, 64 * 1024},
	{0x05, 128 * 1024},
	{0x06, 128 * 1024},
	{0x07, 128 * 1024},
	{0x08, 128 * 1024},
	{0x09, 128 * 1024},
	{0x0a, 128 * 1024},
	{0x0b, 128 * 1024},
	/* flash sectors only in 2MiB devices */
	{0x10, 16 * 1024},
	{0x11, 16 * 1024},
	{0x12, 16 * 1024},
	{0x13, 16 * 1024},
	{0x14, 64 * 1024},
	{0x15, 128 * 1024},
	{0x16, 128 * 1024},
	{0x17, 128 * 1024},
	{0x18, 128 * 1024},
	{0x19, 128 * 1024},
	{0x1a, 128 * 1024},
	{0x1b, 128 * 1024},
};
#endif

#define OnChipFlashAddress    ((uint32_t)0x08000000)   //

#define FLASH_SIZE_MAX                 (128*1024)                      // 128K
#define APP_SIZE_MAX                   ((128-16)*1024)                 // 116K 
const uint32_t hw_flash_addr_start = OnChipFlashAddress;
const uint32_t hw_flash_addr_end = OnChipFlashAddress+FLASH_SIZE_MAX;
const uint32_t hw_flash_size_max = FLASH_SIZE_MAX;
const uint32_t ApplicationAddress = (OnChipFlashAddress+((uint32_t)0x00004000));   //APP程序起始地址
const uint32_t app_size_max = APP_SIZE_MAX;
const uint16_t hw_flash_data_sectors_bytes = 1024; // 每个物理扇区(或页)的块数量,这里按页擦除，每页 1024B

void NVIC_SetVectorTable(const uint32_t app_load_address)
{ 
	// REF: SystemInit();
	SCB->VTOR = app_load_address; /* Vector Table Relocation in Internal SRAM. */
}

int hw_flash_erase(void)      
{
	volatile int status;
    FLASH_EraseInitTypeDef flash_eraseop;
    uint32_t erase_addr;
	//__disable_irq();
	HAL_FLASH_Unlock();

	flash_eraseop.TypeErase = FLASH_TYPEERASE_SECTORS;
	flash_eraseop.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	flash_eraseop.Banks = FLASH_BANK_1;
	flash_eraseop.Sector     = 1;  //擦除扇区的首  如果要擦除第五扇区，此处就填入5
	flash_eraseop.NbSectors  = 5; //擦除扇区的尾，如果擦除第五扇区，就填入6
	status = HAL_FLASHEx_Erase( &flash_eraseop, &erase_addr);
	HAL_FLASH_Lock();
	//__enable_irq();
	return status;
}

void hw_flash_write(const uint32_t addr, const uint8_t data[], const uint32_t Bytes)
{
	uint32_t ii,OneWord;
	HAL_FLASH_Unlock(); 
	for(ii=0;ii<Bytes;ii=ii+4)
	{
		OneWord=(data[3+ii] << 24)+(data[2+ii] << 16)+(data[1+ii] << 8)+ data[ii];
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr+ii, OneWord);
	}
	HAL_FLASH_Lock();
}

void hw_flash_read(uint32_t Address, uint8_t buf[], int32_t Bytes)
{
	int32_t ii;
	for(ii=0; ii < Bytes; ii++) buf[ii] = *(__IO uint8_t*) Address++;
}

void hw_flash_erase_vector(const uint32_t vector_address)
{
	uint32_t vector[2] = {0x00000000, 0x00000000};
	__disable_irq();
	hw_flash_write(vector_address, (uint8_t*)vector, sizeof(vector));
	__enable_irq();;
}
