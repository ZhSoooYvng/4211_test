
#include "usb_drv.h"
#include "usb_protocol.h"

const uint8_t test_packet[53] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
        0xAA, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
        0xEE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xDF,
        0xEF, 0xF7, 0xFB, 0xFD, 0xFC, 0x7E, 0xBF, 0xDF,
        0xEF, 0xF7, 0xFB, 0xFD, 0x7E};

const uint8_t txdat[2] = {0, 0};
/*************************************************
Function: USB_TestMode
Description: USB 测试 （支持USB高速模式测试）
Input:-mode :测试模式
Output: 无
Return: 无
Others: 无
*************************************************/
void USB_TestMode(uint8_t mode)
{
    switch (mode)
    {
    case 1: // Test_J
        gUSBC_CommonReg->TSTMODE = USB_TESTMODE_TESTJ;
        break;
    case 2: // Test_K
        gUSBC_CommonReg->TSTMODE = USB_TESTMODE_TESTK;
        break;
    case 3: // Test_SE0_NAK
        gUSBC_CommonReg->TSTMODE = USB_TESTMODE_SE0NAK;
        break;
    case 4: // Test_Packet
        drv_usbc_writeep0data((uint8_t *)test_packet, 53, PACKET_MID);
        gUSBC_CommonReg->TSTMODE = USB_TESTMODE_TESTPACKET;
        break;
    default:
        break;
    }
}

/*************************************************
Function: USB_GetStatus
Description: USB标准命令GetStatus
Input:-requestType:请求类型
Output: 无
Return: 无
Others: 无
*************************************************/
void USB_GetStatus(uint8_t requestType)
{
    switch (requestType)
    {
    case 0x80:
    case 0x81:
    case 0x82:
        drv_usbc_writeep0data((uint8_t *)txdat, 2, PACKET_END);
        break;
    default:
        drv_usbc_ep0sendstall();
        break;
    }
}
