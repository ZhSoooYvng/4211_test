
#include "debug.h"
#include "usb_drv.h"
#include "usb_cdc_protocol.h"

/*************************************************
Function: USB_CDC_Receive
Description: USB端点接收一包数据
Input:  -usbEpx :USB接收端口号
        -buf    :接收数据的起始地址
Output: 无
Return: 接收数据的长度
Others: 无
*************************************************/
uint16_t USB_CDC_Receive(uint8_t usbEpx, uint8_t *buf)
{
    return drv_usbc_receivedata(usbEpx, buf);
}

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
uint8_t USB_CDC_Send(uint8_t usbEpx, uint8_t *buf, uint32_t len)
{
    uint32_t n, m;
    uint32_t i;

    n = (len / gUSBC_PacketSize);
    m = (len % gUSBC_PacketSize);

    for (i = 0; i < n; i++)
    {
        if (drv_usbc_senddata(usbEpx, buf + gUSBC_PacketSize * i, gUSBC_PacketSize))
        {
            return 1;
        }
        else if (drv_usbc_senddatanull(usbEpx))
        {
            return 1;
        }
    }
    if (drv_usbc_senddata(usbEpx, buf + gUSBC_PacketSize * i, m))
    {
        return 1;
    }
    return 0;
}
