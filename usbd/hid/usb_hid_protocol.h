
#ifndef __USB_HID_PROTOCOL_H__
#define __USB_HID_PROTOCOL_H__

#include "usb_drv.h"
#include "usb_protocol.h"

#define USB_HID_CONFIG_LEN 0x29

extern const uint8_t usb_hid_Device_Descriptor[18];
// Device Qualifier Descriptor
extern const uint8_t usb_hid_Device_Qualifier_Descriptor[10];

extern const uint8_t usb_hid_Configuration_Descriptor[];
/*report_Descriptor*/
extern const uint8_t usb_hid_ReportDescriptor[];
extern const uint8_t usb_hid_ReportDescriptor_V20[];
extern const uint8_t usb_hid_ReportDescriptor_V11[];
// USB Language ID
extern const uint8_t usb_hid_LanguageID[4];
// Manufacturer String
extern const uint8_t usb_hid_StrDescManufacturer[12];
// Product String
extern const uint8_t usb_hid_StrDescProduct[30];
// Serial Number
extern const uint8_t usb_hid_StrDescSerialNumber[34];

/*************************************************
Function: USB_HID_RxHandler
Description: USB HID协议下Rx的中断处理
Input: -RxINTFlag    : 非端口0的接收中断状态
Output: -None
Return: -None
*************************************************/
extern void USB_HID_RxHandler(uint8_t RxINTFlag);

/*************************************************
Function: USB_HID_Receive
Description: USB端点接收一包数据
Input:  -usbEpx :USB接收端口号
        -buf    :接收数据的起始地址
Output: 无
Return: 接收数据的长度
Others: 无
*************************************************/
extern uint16_t USB_HID_Receive(uint8_t usbEpx, uint8_t *buf);

/*************************************************
Function: USB_HID_Send
Description: USB端点发送一包数据
Input:--usbEpx :USB发送端口号
      -buf:发送数据的起始地址
      -len:发送数据的长度
Output: 无
Return: 无
Others: 无
*************************************************/
extern void USB_HID_Send(uint8_t usbEpx, uint8_t *buf, uint16_t len);

extern void USBC_VarInit_HID(void);
extern void USBC_Ep0Handler_HID(void);

#endif
