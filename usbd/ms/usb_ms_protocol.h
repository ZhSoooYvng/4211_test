#ifndef __USB_MS_PROTOCOL_H__
#define __USB_MS_PROTOCOL_H__

#include "usb_drv.h"
#include "usb_protocol.h"

#define USB_MS_CONFIG_LEN 0x20

typedef struct
{
    uint32_t dCBWSignature;
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t bmCBWFlags;
    uint8_t bCBWLUN;
    uint8_t bCBWCBLength;
    uint8_t CBWCB[16];
} CBW_TypeDef;

typedef struct
{
    uint32_t dCSWSignsture;
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t bCSWStatus;
} CSW_TypeDef;

enum
{
    USB_STATUS_IDE = 0,
    USB_STATUS_BUSY,
    USB_STATUS_END,
};

typedef void (*USB_MS_UserCallBack_t)(void *);

typedef struct
{
    __IO CBW_TypeDef CBW;
    __IO CSW_TypeDef CSW;
    __IO uint32_t TransLength;      // 以512字节为一包，计算接收多少个包
    __IO uint32_t TransOffset;      // 以512为一个偏移量
    __IO uint32_t TxRxCounter;      // 以64字节为一个偏移量
    __IO uint32_t LogicBlockAddr;   // 逻辑块地址
    __IO uint8_t *Buffer;           // 数据缓冲区
    __IO uint8_t status;            // 用户程序处理接收数据状态  0：无接收任务   1:处于接收任务中
    USB_MS_UserCallBack_t CallBack; // 用户功能回调函数
} USBC_MS_HandlerTypeDef;

extern const uint8_t usb_ms_Device_Descriptor[18];
// Device Qualifier Descriptor
extern const uint8_t usb_ms_Device_Qualifier_Descriptor[10];
extern const uint8_t usb_ms_Configuration_Descriptor[];
// standard string Descriptor,serial number
extern const uint8_t usb_ms_String_Descriptor[];

extern const uint8_t usb_ms_DBR[512];

extern const uint8_t usb_ms_MBR[512];

extern const uint8_t usb_ms_DirData[];
// USB Language ID
extern const uint8_t usb_ms_LanguageID[4];
// Manufacturer String
extern const uint8_t usb_ms_StrDescManufacturer[12];
// Product String
extern const uint8_t usb_ms_StrDescProduct[28];
// Serial Number
extern const uint8_t usb_ms_StrDescSerialNumber[34];

extern const uint8_t usb_ms_SCSIInquiry[36];

extern const uint8_t usb_ms_SCSIDATA_ReadCapacities[];

extern const uint8_t usb_ms_SCSIDATA_Mode_Sense_1a[4];

extern const uint8_t usb_ms_SCSIDATA_RequestSense[18];

extern uint16_t mach_AlignTo512(uint32_t length);

/*************************************************
Function: USB_MS_EpxHandler
Description: USB MS协议下EPx的中断处理
Input: -None
Output: -None
Return: -None
*************************************************/
extern void USB_MS_RxHandler(void);

/*************************************************
Function: USB_MS_EpxHandler
Description: USB MS协议下Tx的中断处理
Input: -None
Output: -None
Return: -None
*************************************************/
extern void USB_MS_TxHandler(void);

/*************************************************
Function: USB_MS_Receive
Description: USB端点接收一包数据
Input:  -usbEpx :USB接收端口号
        -buf    :接收数据的起始地址
Output: 无
Return: 接收数据的长度
Others: 无
*************************************************/
extern void USB_MS_Receive(uint8_t usbEpx, uint8_t *buf, uint16_t length);

/*************************************************
Function: USB_MS_send
Description: USB端点发送一包数据
Input:--usbEpx :USB发送端口号
      -buf:发送数据的起始地址
      -len:发送数据的长度
Output: 无
Return: 无
Others: 无
*************************************************/
extern void USB_MS_Send(uint8_t usbEpx, uint8_t *buf, uint16_t len);

/*************************************************
Function: USB_MS_ConfigCallbackRegistry
Description: USB MS协议用户程序注册
Input: -NONE
Output: -None
Return: -None
*************************************************/
void USB_MS_HandlerRegistry(USB_MS_UserCallBack_t callback, uint8_t *Buffer);

extern void USBC_VarInit_MSC(void);
extern void USBC_Ep0Handler_MSC(void);

#endif
