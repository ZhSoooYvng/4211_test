
#include "string.h"
#include "usb_drv.h"
#include "usb_protocol.h"
#include "usb_hid_protocol.h"

/*************************************************
Function: USBC_VarInit
Description: USB变量初始化
Output: -None
Return: -None
*************************************************/
void USBC_VarInit_HID(void)
{
}

/*************************************************
Function: USB_HID_GetStatus
Description: 获取报告
Input: -None
Output: -None
Return: -None
*************************************************/
static void USB_HID_GetReport(uint8_t length)
{
    uint8_t ReportBuf[USB_MAX_PACKET_SIZE_EP0];
    _memset(ReportBuf, 0, sizeof(ReportBuf));
    gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;
    drv_usbc_writeep0data(ReportBuf, length, PACKET_END);
}

/*************************************************
Function: USB_HID_SetReport
Description: 设置报告
Input: -None
Output: -None
Return: -None
*************************************************/
static void USB_HID_SetReport(void)
{
    gUSBC_CommonReg->EINDEX = CONTROL_EP;
    gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;
}

/*************************************************
Function: USB_HID_GetDescriptor
Description: USB standard command GetDescriptor
Input: -None
Output: -None
Return: -None
*************************************************/
static void USB_HID_GetDescriptor(USB_REQ_TypeDef *usb_request)
{
    uint8_t USBDEV_Descriptor[USB_HID_CONFIG_LEN];

    switch (_byte_high(usb_request->Value))
    {
    case DEVICE_TYPE:
        _memcpy(USBDEV_Descriptor, (uint8_t *)usb_hid_Device_Descriptor, 0x12);
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
        drv_usbc_writeep0data((uint8_t *)USBDEV_Descriptor, usb_request->Length > usb_hid_Device_Descriptor[0] ? 8 : usb_request->Length, PACKET_END);
        break;
    case CONFIG_TYPE:
        _memcpy(USBDEV_Descriptor, (uint8_t *)usb_hid_Configuration_Descriptor, USB_HID_CONFIG_LEN);
        if (gUSBC_PacketSize == USB_MAX_PACKET_SIZE_V20)
        {
            USBDEV_Descriptor[25] = 0x36;
        }
        else
        {
            USBDEV_Descriptor[25] = 0x34;
        }
        USBDEV_Descriptor[31] = gUSBC_PacketSize & 0xff;
        USBDEV_Descriptor[32] = gUSBC_PacketSize >> 8;
        USBDEV_Descriptor[38] = gUSBC_PacketSize & 0xff;
        USBDEV_Descriptor[39] = gUSBC_PacketSize >> 8;
        drv_usbc_ep0senddata(usb_request->Length, (uint8_t *)USBDEV_Descriptor, USB_HID_CONFIG_LEN);
        break;
    case STRING_TYPE:
        switch (_byte_low(usb_request->Value))
        {
        case 0:
            drv_usbc_writeep0data((uint8_t *)usb_hid_LanguageID, usb_hid_LanguageID[0], PACKET_END);
            break;
        case 1:
            drv_usbc_writeep0data((uint8_t *)usb_hid_StrDescManufacturer, usb_request->Length > usb_hid_StrDescManufacturer[0] ? usb_hid_StrDescManufacturer[0] : usb_request->Length, PACKET_END);
            break;
        case 2:
            drv_usbc_writeep0data((uint8_t *)usb_hid_StrDescProduct, usb_request->Length > usb_hid_StrDescProduct[0] ? usb_hid_StrDescProduct[0] : usb_request->Length, PACKET_END);
            break;
        case 3:
            drv_usbc_writeep0data((uint8_t *)usb_hid_StrDescSerialNumber, usb_request->Length > usb_hid_StrDescSerialNumber[0] ? usb_hid_StrDescSerialNumber[0] : usb_request->Length, PACKET_END);
            break;
        default:
            break;
        }
        break;
    case DEVICE_QUALIFIER:
        drv_usbc_writeep0data((uint8_t *)usb_hid_Device_Qualifier_Descriptor, usb_request->Length > usb_hid_Device_Qualifier_Descriptor[0] ? 8 : usb_request->Length, PACKET_END);
        break;
    case OTHER_SPEED:
        if (usb_request->Length < 10)
        {
            _memcpy(USBDEV_Descriptor, (uint8_t *)usb_hid_Configuration_Descriptor, USB_HID_CONFIG_LEN);
            USBDEV_Descriptor[1] = DT_OTHER_SPEED_CONDIGURATION;
            drv_usbc_writeep0data((uint8_t *)USBDEV_Descriptor, usb_request->Length, PACKET_END);
        }
        break;
    case REPORT_TYPE:
        if (gUSBC_PacketSize == USB_MAX_PACKET_SIZE_V20)
        {
            drv_usbc_writeep0data((uint8_t *)usb_hid_ReportDescriptor_V20, 0x36, PACKET_END);
        }
        else
        {
            drv_usbc_writeep0data((uint8_t *)usb_hid_ReportDescriptor_V11, 0x34, PACKET_END);
        }
        break;
    default:
        drv_usbc_ep0sendstall();
        break;
    }
}

/*************************************************
Function: USBC_Ep0Handler_HID
Description: USB CDC协议下EP0的中断处理
Input: -None
Output: -None
Return: -None
*************************************************/
void USBC_Ep0Handler_HID(void)
{
    USB_REQ_TypeDef usb_request;
    /*获取USB命令请求数据*/
    drv_usbc_readepxdata(CONTROL_EP, (uint8_t *)&usb_request, 8);

    switch (usb_request.Request)
    {
    case GET_STATUS:
        // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
        gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
        USB_GetStatus(usb_request.RequestType);
        break;
    case CLEAR_FEATURE:
        USB_HID_GetReport(usb_request.Length);
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
        USB_HID_GetDescriptor(&usb_request);
        break;
    case GET_CONFIGURATION:
        // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
        gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
        drv_usbc_writeep0data((uint8_t *)&usb_request.Request, 1, PACKET_END);
        break;
    case SET_CONFIGURATION:
        if (usb_request.RequestType == 0x21)
        {
            USB_HID_SetReport();
        }
        else
        {
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
        }
        break;
    case GET_INTERFACE:
        if (usb_request.RequestType == 0x21)
        {
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
        }
        else
        {
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;
            drv_usbc_writeep0data((uint8_t *)&usb_request.Request, 1, PACKET_END);
        }
        break;
    case SET_INTERFACE:
        gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
        break;
    default:
        drv_usbc_ep0sendstall();
    }
}
