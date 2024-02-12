/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : @merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : Bootloader.
* Description        : main.
* Description        : PX4 Bootloader (https://github.com/Merafour/Bootloader)
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Hardware/HW.h"
#include "BL/jump.h"
#include "OHID/OHID_Pack.h"
#include "HID/bl.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// Close SysTick
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  return HAL_OK;
}
static volatile uint32_t sys_time_ms=0;
uint32_t HAL_GetTick(void)
{
	uint32_t CNT = TIM2->CNT;
	uint32_t cnt = sys_time_ms&0x7FFF;
	// 正常计数, 32768,最大计时 32768ms， 详细请看定时器配置
	if(CNT>=cnt) 
	{
		sys_time_ms = (sys_time_ms&0xFFFF8000) + CNT;  // ms
	}
	else // 溢出
	{
		sys_time_ms = (sys_time_ms&0xFFFF8000) + CNT + 32768; // ms
	}
    return sys_time_ms;
}
void usbd_port_config(uint8_t state)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_11 | LL_GPIO_PIN_12;
	//GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;//LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;//LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;//LL_GPIO_PULL_UP;
    if (state)
    {
//        USB->CNTR &= ~(1 << 1); /* PWDN = 0, 退出断电模式 */
		GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;//LL_GPIO_MODE_ALTERNATE;
		LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else
    {
//        USB->CNTR |= 1 << 1;    /* PWDN = 1, 进入断电模式 */
		//__HAL_RCC_USB_OTG_FS_CLK_DISABLE();
		GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
		LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		LL_GPIO_ResetOutputPin(GPIOA, GPIO_InitStruct.Pin);
    }
}
void close_device(void)
{	
	// diseable irq
	__NVIC_DisableIRQ(OTG_FS_IRQn);
	__HAL_RCC_USB_OTG_FS_CLK_DISABLE();
	LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_ALL);
	LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_ALL);
	LL_RCC_PLL_Disable();
	LL_RCC_HSI_Enable();
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint32_t _time_ms=0;
	volatile uint32_t cnt = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	__HAL_RCC_USB_OTG_FS_CLK_DISABLE();
	MX_TIM2_Init();
	usbd_port_config(0);
	HAL_Delay(300);
	usbd_port_config(1);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);
	HAL_Delay(200);
	bl_init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  //MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  jump_app_sign(ApplicationAddress, 10*1024);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	_time_ms=HAL_GetTick();
	bl_loop(_time_ms);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_1)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 192, LL_RCC_PLLP_DIV_4);
  LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 192, LL_RCC_PLLQ_DIV_4);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(48000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

