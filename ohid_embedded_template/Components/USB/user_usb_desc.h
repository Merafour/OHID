/**
  ******************************************************************************
  * @file    usb_desc.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Descriptor Header for Joystick Mouse Demo
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USER_USB_DESC_H_
#define _USER_USB_DESC_H_
#include <stdint.h>

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define HID_DESCRIPTOR_TYPE                     0x21

enum string_desc_type {
	STR_DESC_LAN        = 0x00,    // Language
	STR_DESC_MANUF      = 0x01,    // Manufacturer
	STR_DESC_PRODUCE    = 0x02,    // Product
	STR_DESC_SN         = 0x03,    // SerialNumber
};

extern uint8_t string_unicode_buf[(32<<1)+2];
extern uint8_t descriptor_convert_unicode(const char string[], uint8_t unicode[], const uint16_t size);
extern uint8_t descriptor_convert_unicode_sn(uint8_t unicode[]);
extern uint8_t usb_uid_convert_sn(const uint8_t ID[12], uint8_t sn[16]);
extern uint8_t usb_uid_convert_sn_str(const uint8_t id[12], char sn[22]);

#define  USB_DESC_TYPE_DEVICE                           0x01U
#define  USB_DESC_TYPE_CONFIGURATION                    0x02U
#define  USB_DESC_TYPE_STRING                           0x03U
#define  USB_DESC_TYPE_INTERFACE                        0x04U
#define  USB_DESC_TYPE_ENDPOINT                         0x05U
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                 0x06U
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION        0x07U
#define  USB_DESC_TYPE_IAD                              0x0BU
#define  USB_DESC_TYPE_BOS                              0x0FU


#endif /* _USER_USB_DESC_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
