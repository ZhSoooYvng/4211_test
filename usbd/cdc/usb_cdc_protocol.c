
#include "usb_drv.h"
#include "usb_protocol.h"
#include "usb_cdc_protocol.h"

extern uint8_t g_enumeration; // 完成枚举标志 1完成
/*************************************************
Function: USBC_VarInit
Description: USB变量初始化
Output: -None
Return: -None
*************************************************/
void USBC_VarInit_CDC(void)
{
}

/*************************************************
Function: USB_CDC_GetDescriptor
Description: USB standard command GetDescriptor
Input: -None
Output: -None
Return: -None
*************************************************/
static void USB_CDC_GetDescriptor(USB_REQ_TypeDef *usb_request)
{
    uint8_t USBDEV_Descriptor[USB_CDC_CONFIG_LEN];

    //    _memcpy(USBDEV_Descriptor,(uint8_t *)usb_cdc_Configuration_Descriptor,USB_CDC_CONFIG_LEN);

    switch (_byte_high(usb_request->Value))
    {
    case DEVICE_TYPE:
        _memcpy(USBDEV_Descriptor, (uint8_t *)usb_cdc_Device_Descriptor, 0x12);
        if (gUSBC_PacketSize == USB_MAX_PACKET_SIZE_V20)
        {
            USBDEV_Descriptor[2] = 0x00;
            USBDEV_Descriptor[3] = 0x02;
        }
        else
        {
            USBDEV_Descriptor[2] = 0x10;
            USBDEV_Descriptor[3] = 0x01;
        }
        drv_usbc_writeep0data((uint8_t *)USBDEV_Descriptor, usb_request->Length > usb_cdc_Device_Descriptor[0] ? 8 : usb_request->Length, PACKET_END);
        break;
    case CONFIG_TYPE:
        _memcpy(USBDEV_Descriptor, (uint8_t *)usb_cdc_Configuration_Descriptor, USB_CDC_CONFIG_LEN);
        USBDEV_Descriptor[41] = gUSBC_PacketSize & 0xff;
        USBDEV_Descriptor[42] = gUSBC_PacketSize >> 8;
        USBDEV_Descriptor[57] = gUSBC_PacketSize & 0xff;
        USBDEV_Descriptor[58] = gUSBC_PacketSize >> 8;
        USBDEV_Descriptor[64] = gUSBC_PacketSize & 0xff;
        USBDEV_Descriptor[65] = gUSBC_PacketSize >> 8;
        drv_usbc_ep0senddata(usb_request->Length, (uint8_t *)USBDEV_Descriptor, USB_CDC_CONFIG_LEN);
        break;
    case STRING_TYPE:
        switch (_byte_low(usb_request->Value))
        {
        case 0:
            drv_usbc_writeep0data((uint8_t *)usb_cdc_LanguageID, usb_cdc_LanguageID[0], PACKET_END);
            break;
        case 1:
            drv_usbc_writeep0data((uint8_t *)usb_cdc_StrDescManufacturer, usb_request->Length > usb_cdc_StrDescManufacturer[0] ? usb_cdc_StrDescManufacturer[0] : usb_request->Length, PACKET_END);
            break;
        case 2:
            drv_usbc_writeep0data((uint8_t *)usb_cdc_StrDescProduct, usb_request->Length > usb_cdc_StrDescProduct[0] ? usb_cdc_StrDescProduct[0] : usb_request->Length, PACKET_END);
            break;
        case 3:
            drv_usbc_writeep0data((uint8_t *)usb_cdc_StrDescSerialNumber, usb_request->Length > usb_cdc_StrDescSerialNumber[0] ? usb_cdc_StrDescSerialNumber[0] : usb_request->Length, PACKET_END);
            break;
        default:
            break;
        }
        break;
    case DEVICE_QUALIFIER:
        drv_usbc_writeep0data((uint8_t *)usb_cdc_Device_Qualifier_Descriptor, usb_request->Length > usb_cdc_Device_Qualifier_Descriptor[0] ? 8 : usb_request->Length, PACKET_END);
        break;
    case OTHER_SPEED:
        if (usb_request->Length < 10)
        {
            USBDEV_Descriptor[1] = DT_OTHER_SPEED_CONDIGURATION;
            drv_usbc_writeep0data((uint8_t *)USBDEV_Descriptor, usb_request->Length, PACKET_END);
        }
        break;
    default:
        drv_usbc_ep0sendstall();
        break;
    }
}

/*************************************************
Function: USBC_Ep0Handler_CDC
Description: USB CDC协议下EP0的中断处理
Input: -None
Output: -None
Return: -None
*************************************************/
void USBC_Ep0Handler_CDC(void)
{
    USB_REQ_TypeDef usb_request;

    // 设置串口属性，非setup包，而是out包
    if (gUSBC_IndexReg->E0COUNTR_L != DEV_CSR0_DATAEND)
    {
        drv_usbc_writeepxdata(CONTROL_EP, usb_cdc_LineCoding, 7);
        gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;
        return;
    }

    /*获取USB命令请求数据*/
    drv_usbc_readepxdata(CONTROL_EP, (uint8_t *)&usb_request, 8);

    if ((usb_request.RequestType & USB_REQUEST_MASK) == USB_REQUEST_CLASS)
    {
        gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;
        switch (usb_request.Request)
        {
        case SET_LINE_CODING:
        case SET_CONTROL_LINR_STATE:
        case SEND_BREAK:
            break;
        case GET_LINE_CODING:
            drv_usbc_writeep0data((uint8_t *)usb_cdc_LineCoding, 7, PACKET_END);
            break;
        default:
            drv_usbc_ep0sendstall();
        }
    }
    else
    {
        switch (usb_request.Request)
        {
        case GET_STATUS:
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
            USB_GetStatus(usb_request.RequestType);
            break;
        case CLEAR_FEATURE:
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
            break;
        case SET_FEATURE:
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
            if (usb_request.RequestType == 0)
            {
                USB_TestMode(usb_request.Index);
            }
            break;
        case SET_ADDRESS:
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
            gUSBC_NewAddress = _byte_low(usb_request.Value);
            break;
        case GET_DESCRIPTOR:
            // set ServiceRxPktRdy to clear RxPktRdy
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;
            USB_CDC_GetDescriptor(&usb_request);
            break;
        case GET_CONFIGURATION:
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
            drv_usbc_writeep0data((uint8_t *)&usb_request.Request, 1, PACKET_END);
            break;
        case SET_CONFIGURATION:
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
            g_enumeration = 1; // 完成枚举标志 1完成
            break;
        case GET_INTERFACE:
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;
            drv_usbc_writeep0data((uint8_t *)&usb_request.Request, 1, PACKET_END);
            break;
        case SET_INTERFACE:
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
            break;
        default:
            drv_usbc_ep0sendstall();
        }
    }
}
