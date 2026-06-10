
#include "debug.h"
#include "string.h"
#include "usb_drv.h"
#include "usb_protocol.h"
#include "usb_ms_protocol.h"

static USBC_MS_HandlerTypeDef ms;

/*************************************************
Function: USB_MS_ConfigCallbackRegistry
Description: USB MS协议用户程序注册
Input: -NONE
Output: -None
Return: -None
*************************************************/
void USB_MS_HandlerRegistry(USB_MS_UserCallBack_t callback,
                            uint8_t *Buffer)
{
    ms.CallBack = callback;
    ms.Buffer = Buffer;
}

/*************************************************
Function: USBC_VarInit
Description: USB MS全局变量初始化
Input: -NONE
Output: -None
Return: -None
*************************************************/
void USBC_VarInit_MSC(void)
{
    _memset((uint8_t *)&ms.CBW, 0, 31);
    _memset((uint8_t *)&ms.CSW, 0, 13);
    ms.CSW.dCSWSignsture = 0x53425355; // USBS
    ms.LogicBlockAddr = 0;
    ms.TransLength = 0;
    ms.TransOffset = 0;
    ms.TxRxCounter = 0;
    ms.status = USB_STATUS_IDE;
}

/*************************************************
Function: USB_MS_GetDescriptor
Description: USB standard command GetDescriptor
Input: -None
Output: -None
Return: -None
*************************************************/
static void USB_MS_GetDescriptor(USB_REQ_TypeDef *usb_request)
{
    uint8_t USBDEV_Descriptor[USB_MS_CONFIG_LEN];

    switch (_byte_high(usb_request->Value))
    {
    case DEVICE_TYPE:
        _memcpy(USBDEV_Descriptor, (uint8_t *)usb_ms_Device_Descriptor, 0x12);
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
        drv_usbc_writeep0data((uint8_t *)USBDEV_Descriptor, usb_request->Length > usb_ms_Device_Descriptor[0] ? 8 : usb_request->Length, PACKET_END);
        break;
    case CONFIG_TYPE:
        _memcpy(USBDEV_Descriptor, (uint8_t *)usb_ms_Configuration_Descriptor, USB_MS_CONFIG_LEN);
        USBDEV_Descriptor[22] = gUSBC_PacketSize & 0xff;
        USBDEV_Descriptor[23] = gUSBC_PacketSize >> 8;
        USBDEV_Descriptor[29] = gUSBC_PacketSize & 0xff;
        USBDEV_Descriptor[30] = gUSBC_PacketSize >> 8;
        drv_usbc_ep0senddata(usb_request->Length, (uint8_t *)USBDEV_Descriptor, USB_MS_CONFIG_LEN);
        break;
    case STRING_TYPE:
        switch (_byte_low(usb_request->Value))
        {
        case 0:
            drv_usbc_writeep0data((uint8_t *)usb_ms_LanguageID, usb_request->Length > usb_ms_LanguageID[0] ? usb_ms_LanguageID[0] : usb_request->Length, PACKET_END);
            break;
        case 1:
            drv_usbc_writeep0data((uint8_t *)usb_ms_StrDescManufacturer, usb_request->Length > usb_ms_StrDescManufacturer[0] ? usb_ms_StrDescManufacturer[0] : usb_request->Length, PACKET_END);
            break;
        case 2:
            drv_usbc_writeep0data((uint8_t *)usb_ms_StrDescProduct, usb_request->Length > usb_ms_StrDescProduct[0] ? usb_ms_StrDescProduct[0] : usb_request->Length, PACKET_END);
            break;
        case 3:
            drv_usbc_writeep0data((uint8_t *)usb_ms_StrDescSerialNumber, usb_request->Length > usb_ms_StrDescSerialNumber[0] ? usb_ms_StrDescSerialNumber[0] : usb_request->Length, PACKET_END);
            break;
        default:
            break;
        }
        break;
    case DEVICE_QUALIFIER:
        drv_usbc_writeep0data((uint8_t *)usb_ms_Device_Qualifier_Descriptor, usb_request->Length > usb_ms_Device_Qualifier_Descriptor[0] ? 8 : usb_request->Length, PACKET_END);
        break;
    case OTHER_SPEED:
        if (usb_request->Length < 10)
        {
            _memcpy(USBDEV_Descriptor, (uint8_t *)usb_ms_Configuration_Descriptor, USB_MS_CONFIG_LEN);
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
Function: USBC_Ep0Handler_MSC
Description: USB MS协议下EP0的中断处理
Input: -None
Output: -None
Return: -None
*************************************************/
void USBC_Ep0Handler_MSC(void)
{
    USB_REQ_TypeDef usb_request;
    /*获取USB命令请求数据*/
    drv_usbc_readepxdata(CONTROL_EP, (uint8_t *)&usb_request, 8);

    if ((usb_request.RequestType & USB_REQUEST_MASK) == USB_REQUEST_CLASS)
    {
        switch (usb_request.Request)
        {
        case GET_MAX_LUN:
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY;
            drv_usbc_writeep0data((uint8_t *)&usb_request.Request, 1, PACKET_END);
            break;
        case MASS_STORAGE_RESET:
            // write FAddr 0
            gUSBC_CommonReg->FADDRR = 0;
            // write    Index 1
            gUSBC_CommonReg->EINDEX = INDEX_EP1;
            // Flush Tx Ep FIFO
            gUSBC_IndexReg->TXCSR_L = DEV_TXCSR_FLUSH_FIFO;
            // Flush Rx Ep FIFO
            gUSBC_IndexReg->RXCSR_L = DEV_RXCSR_FLUSH_FIFO;
            // write    Index 0
            gUSBC_CommonReg->EINDEX = CONTROL_EP;
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
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
            USB_MS_GetDescriptor(&usb_request);
            break;
        case GET_CONFIGURATION:
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
            drv_usbc_writeep0data((uint8_t *)&usb_request.Request, 1, PACKET_END);
            break;
        case SET_CONFIGURATION:
            // set ServiceRxPktRdy to clear RxPktRdy and set DataEnd
            gUSBC_IndexReg->E0CSR_L = DEV_CSR0_SERVICE_RXPKTRDY | DEV_CSR0_DATAEND;
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

/*************************************************
Function: enum_realDisk
Description: 枚举容量
Input: -lba  ：块大小
Output: -buffer ：
Return: -None
*************************************************/
static void enum_realDisk(uint32_t lba, uint8_t *buffer)
{
    uint32_t fat_sum;
    uint8_t *addr;
    uint16_t reserve;
    uint32_t dbr_offset;

    addr = (uint8_t *)&usb_ms_DBR[0];
    reserve = *(addr + 0x0e);
    fat_sum = *(addr + 0x17);
    fat_sum = (fat_sum << 8) + *(addr + 0x16);

    dbr_offset = usb_ms_MBR[454];

    if (lba == 0)
    {
        _memcpy(buffer, (uint8_t *)usb_ms_MBR, 512);
    }
    else if (lba == dbr_offset)
    {
        _memcpy(buffer, (uint8_t *)usb_ms_DBR, 512);
    }
    else if ((lba == (reserve + dbr_offset)) ||
             (lba == (fat_sum + reserve + dbr_offset)))
    {
        _memset(buffer, 0x00, 512);

        buffer[0] = 0xf8;
        buffer[1] = 0xff;
        buffer[2] = 0xff;
        buffer[3] = 0xff;
    }
    else
    {
        _memset(buffer, 0x00, 512);
    }
}

/*************************************************
Function: Vendor_ReadData
Description: 读扇区数据
Input: -None
Output: -None
Return: -None
*************************************************/
static void Vendor_ReadData(void)
{
    // write data from buffer to TX
    if (ms.TransLength)
    {
#if 1 // 关联USB_MS_BulkIn(),同步修改#if 条件
        USB_MS_Send(BULKIN_EP, (uint8_t *)ms.Buffer, 512);
        ms.TransLength--;
        ms.LogicBlockAddr++;
        ms.TransOffset++;
#else
        if (gUSBC_PacketSize == USB_MAX_PACKET_SIZE_V20)
        {
            USB_MS_Send(BULKIN_EP, (uint8_t *)ms.Buffer, gUSBC_PacketSize);
            ms.TransLength--;
            ms.LogicBlockAddr++;
            ms.TransOffset++;
            ms.TxRxCounter = 0;
        }
        else
        {
            USB_MS_Send(BULKIN_EP, (uint8_t *)(ms.Buffer + ms.TxRxCounter * gUSBC_PacketSize), gUSBC_PacketSize);
            ms.TxRxCounter++;
            if (ms.TxRxCounter == 8)
            {
                ms.TxRxCounter = 0;
                ms.TransLength--;
                ms.TransOffset++;
                ms.LogicBlockAddr++;
            }
        }
#endif
    }
    else
    {
        ms.CBW.CBWCB[0] = 0xFF;
        USB_MS_Send(BULKIN_EP, (uint8_t *)&ms.CSW, 13);
    }
}

/*************************************************
Function: Vendor_WriteData
Description: 写扇区数据
Input: -None
Output: -None
Return: -None
*************************************************/
static void Vendor_WriteData(void)
{
    // receive data from RX FIFO to buffer
    if (gUSBC_PacketSize == USB_MAX_PACKET_SIZE_V20)
    {
        USB_MS_Receive(BULKOUT_EP, (uint8_t *)(ms.Buffer), gUSBC_PacketSize);
        ms.TransLength--;
        ms.TransOffset++;
    }
    else
    {
        USB_MS_Receive(BULKOUT_EP, (uint8_t *)(ms.Buffer + ms.TxRxCounter * gUSBC_PacketSize), gUSBC_PacketSize);

        ms.TxRxCounter++;
        if (ms.TxRxCounter == 8)
        {
            ms.TxRxCounter = 0;
            ms.TransOffset++;
            ms.TransLength--;
        }
    }
    // clear RxPktRdy bit
    drv_usbc_clearrx();

    if (ms.CallBack)
    {
        // 如果注册了回调函数，需要自己处理接收完成后的状态
        if (ms.TransLength == 0)
        {
            ms.TxRxCounter = 0;
            ms.status = USB_STATUS_END;
        }
        ms.CallBack((void *)&ms);
    }
    else
    {
        if (ms.TransLength == 0)
        {
            ms.CBW.CBWCB[0] = 0xFF;
            USB_MS_Send(BULKIN_EP, (uint8_t *)&ms.CSW, 13);
        }
    }
}

uint16_t mach_AlignTo512(uint32_t length)
{
    uint16_t n = 0;
    if (length == 0)
    {
        return n;
    }

    n = length >> 9;
    n++;
    return n;
}
/*************************************************
Function: USB_MS_BulkOut
Description: 处理USB bulkout数据
Input: -None
Output: -None
Return: -None
*************************************************/
void USB_MS_BulkOut(void)
{
    uint16_t uiRxCount;

    uiRxCount = gUSBC_IndexReg->RXCOUNTR_H;
    uiRxCount <<= 8;
    uiRxCount += gUSBC_IndexReg->RXCOUNTR_L;

    if (uiRxCount == 0)
        return;

    if (uiRxCount == 31)
    {
        USB_MS_Receive(BULKOUT_EP, (uint8_t *)&ms.CBW, 31);
        drv_usbc_clearrx();

        ms.CSW.dCSWTag = ms.CBW.dCBWTag; // TAG
        ms.CSW.dCSWDataResidue = 0;
        ms.CSW.bCSWStatus = 0;

        switch (ms.CBW.CBWCB[0])
        {
        case TEST_UNIT_READY:
        case MEDIUM_REMOVAL:
            ms.CBW.CBWCB[0] = 0xFF;
            USB_MS_Send(BULKIN_EP, (uint8_t *)&ms.CSW, 13);
            break;
        case INQUIRY:
            USB_MS_Send(BULKIN_EP, (uint8_t *)usb_ms_SCSIInquiry, 36);
            break;
        case READ_FORMAT_CAPACITY:
            USB_MS_Send(BULKIN_EP, (uint8_t *)usb_ms_SCSIDATA_ReadCapacities, 12);
            break;
        case READ_CAPACITY:
            USB_MS_Send(BULKIN_EP, (uint8_t *)usb_ms_SCSIDATA_ReadCapacities + 4, 8);
            break;
        case MODE_SENSE_6:
            USB_MS_Send(BULKIN_EP, (uint8_t *)usb_ms_SCSIDATA_Mode_Sense_1a, 4);
            break;
        case REQUEST_SENSE:
            USB_MS_Send(BULKIN_EP, (uint8_t *)usb_ms_SCSIDATA_RequestSense, 18);
            break;
        case READ_10:
            ms.LogicBlockAddr = (uint32_t)(((uint32_t)ms.CBW.CBWCB[2] << 24) +
                                           ((uint32_t)ms.CBW.CBWCB[3] << 16) +
                                           ((uint32_t)ms.CBW.CBWCB[4] << 8) +
                                           ((uint32_t)ms.CBW.CBWCB[5]));
            ms.TransLength = (uint16_t)(((uint16_t)ms.CBW.CBWCB[7] << 8) +
                                        ((uint16_t)ms.CBW.CBWCB[8]));
            enum_realDisk(ms.LogicBlockAddr, (uint8_t *)ms.Buffer);
            Vendor_ReadData();
            break;
        default:

            if (ms.CallBack)
            {
                // 如果注册了回调函数，需要自己处理接收完成后的状态
                ms.status = USB_STATUS_IDE;
                ms.CallBack((void *)&ms);
                return;
            }
            else
            {
                ms.CSW.bCSWStatus = 1;
                ms.CBW.CBWCB[0] = 0xFF;
                USB_MS_Send(BULKIN_EP, (uint8_t *)&ms.CSW, 13);
            }
        }
    }
    else
    {
        Vendor_WriteData();
    }
}

/*************************************************
Function: USB_MS_BulkIn
Description: 处理USB bulkIn数据
Input: -None
Output: -None
Return: -None
*************************************************/
void USB_MS_BulkIn(void)
{
    switch (ms.CBW.CBWCB[0])
    {
    case READ_10:
#if 1 // 关联条件Vendor_ReadData(),同步修改#if
        enum_realDisk(ms.LogicBlockAddr, (uint8_t *)ms.Buffer);
#else
        if (ms.TxRxCounter == 0)
        {
            enum_realDisk(ms.LogicBlockAddr, (uint8_t *)ms.Buffer);
        }
#endif
        Vendor_ReadData();
        break;
    case 0xFF:
        break;
    default:
        ms.CBW.CBWCB[0] = 0xFF;
        USB_MS_Send(BULKIN_EP, (uint8_t *)&ms.CSW, 13);
    }
}

/*************************************************
Function: USB_MS_EpxHandler
Description: USB MS协议下EPx的中断处理
Input: -None
Output: -None
Return: -None
*************************************************/
void USB_MS_RxHandler(void)
{
    uint8_t uReg;

    gUSBC_CommonReg->EINDEX = BULKOUT_EP;

    uReg = gUSBC_IndexReg->RXCSR_L;

    if (uReg & DEV_RXCSR_SENT_STALL)
    {
        uReg &= ~DEV_RXCSR_SEND_STALL;
        uReg |= DEV_RXCSR_CLR_DATA_TOG;
        gUSBC_IndexReg->RXCSR_L = uReg;
    }

    if (uReg & DEV_RXCSR_RXPKTRDY)
    {
        USB_MS_BulkOut();
    }
}

/*************************************************
Function: USB_MS_EpxHandler
Description: USB MS协议下Tx的中断处理
Input: -None
Output: -None
Return: -None
*************************************************/
void USB_MS_TxHandler(void)
{
    USB_MS_BulkIn();
}
