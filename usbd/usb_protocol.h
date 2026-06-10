
#ifndef __USB_PROTOCOL_H__
#define __USB_PROTOCOL_H__

#include "type.h"

#define USB_VERSION_1p1 0 /**< 0:USB2.0; 1:USB1.1 */

#define DT_DEVICE 1
#define DT_CONFIGURATION 2
#define DT_STRING 3
#define DT_INTERFACE 4
#define DT_ENDPOINT 5
#define DT_DEVICE_QUALIFIER 6
#define DT_OTHER_SPEED_CONDIGURATION 7
#define DT_INTERFACE_POWER 8

#define TRANS_ISO 0x01
#define TRANS_BULK 0x02
#define TRANS_INT 0x03

#define DEVICE_TYPE 0x1
#define CONFIG_TYPE 0x2
#define STRING_TYPE 0x3
#define INTERFACE_TYPE 0x4
#define ENDPOINT_TYPE 0x5
#define DEVICE_QUALIFIER 0x6
#define OTHER_SPEED 0x7
#define INTERFACE_POWER 0x8

#define USB_REQUEST_MASK 0x60
#define USB_REQUEST_BASE 0X00
#define USB_REQUEST_CLASS 0x20

/*USB request*/
#define GET_STATUS 0x00
#define CLEAR_FEATURE 0x01
#define SET_FEATURE 0x03
#define SET_ADDRESS 0x05
#define GET_DESCRIPTOR 0x06
#define SET_DESCRIPTOR 0x07
#define GET_CONFIGURATION 0x08
#define SET_CONFIGURATION 0x09
#define GET_INTERFACE 0x0A
#define SET_INTERFACE 0x0B
#define SYNCH_FRAME 0x0C

#define SET_LINE_CODING 0x20
#define GET_LINE_CODING 0x21
#define SET_CONTROL_LINR_STATE 0x22
#define SEND_BREAK 0x23

/*MS request*/
#define GET_MAX_LUN 0xFE
#define MASS_STORAGE_RESET 0xFF
/*HID request*/
#define REPORT_TYPE 0x22
/*CCID request*/
#define ABORT 0x01
#define GET_CLOCK_FREQUENCIER 0x02
#define GET_DATA_RATES 0x03
/*MS request*/
#define TEST_UNIT_READY 0x00
#define REQUEST_SENSE 0X03
#define INQUIRY 0X12
#define MODE_SENSE_6 0X1A
#define START_STOP 0X1B
#define MEDIUM_REMOVAL 0X1E
#define READ_FORMAT_CAPACITY 0X23
#define READ_CAPACITY 0X25
#define READ_10 0X28
#define WRITE_10 0X2A
#define VERIFY 0X2F
#define MODE_SENSE_10 0x5A

typedef struct
{
    uint8_t RequestType;
    uint8_t Request;
    uint16_t Value;
    uint16_t Index;
    uint16_t Length;
} USB_REQ_TypeDef;

/*************************************************
Function: USB_TestMode
Description: USB 测试 （支持USB高速模式测试）
Input:-mode :测试模式
Output: 无
Return: 无
Others: 无
*************************************************/
void USB_TestMode(uint8_t mode);

/*************************************************
Function: USB_GetStatus
Description: USB标准命令GetStatus
Input:-requestType:请求类型
Output: 无
Return: 无
Others: 无
*************************************************/
void USB_GetStatus(uint8_t requestType);

#endif
