
#ifndef __USB_CDC_PROTOCOL_H__
#define __USB_CDC_PROTOCOL_H__

#include "usb_drv.h"
#include "usb_protocol.h"

#define USB_CDC_CONFIG_LEN (0x43)

extern const uint8_t usb_cdc_Device_Descriptor[18];
// Device Qualifier Descriptor
extern const uint8_t usb_cdc_Device_Qualifier_Descriptor[10];

extern const uint8_t usb_cdc_Configuration_Descriptor[];
// USB Language ID
extern const uint8_t usb_cdc_LanguageID[4];
// Manufacturer String
extern const uint8_t usb_cdc_StrDescManufacturer[12];
// Product String
extern const uint8_t usb_cdc_StrDescProduct[30];
// Serial Number
extern const uint8_t usb_cdc_StrDescSerialNumber[34];

extern uint8_t usb_cdc_LineCoding[7];

/*************************************************
Function: USB_CDC_Receive
Description: USB端点接收一包数据
Input:  -usbEpx :USB接收端口号
        -buf    :接收数据的起始地址
Output: 无
Return: 接收数据的长度
Others: 无
*************************************************/
uint16_t USB_CDC_Receive(uint8_t usbEpx, uint8_t *buf);

/*************************************************
Function: USB_CDC_Send
Description: USB端点发送一包数据
Input:--usbEpx :USB发送端口号
      -buf:发送数据的起始地址
      -len:发送数据的长度
Output: 无
Return: 0:success 1:fail
Others: 无
*************************************************/
uint8_t USB_CDC_Send(uint8_t usbEpx, uint8_t *buf, uint32_t len);

extern void USBC_VarInit_CDC(void);
extern void USBC_Ep0Handler_CDC(void);

#endif
