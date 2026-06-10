/**
 * @file usb_test.c
 * @author Product application department
 * @brief  USB 模块测试用例，仅供参考
 * @version V1.0
 * @date 2025-03-02
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 *
 */


/**
 * @defgroup ThirdPart 第三方模块
 * @{
 *
 * @defgroup USBDEVICE USBDEVICE模块
 * @ingroup ThirdPart
 * @{
 */
 
// 头文件包含
#include "usb_device_test.h"


// 全局变量定义
#define USBMS_TEST 0
#define USBCDC_TEST 0
#define USBHID_TEST 1

static uint8_t DataBuf[0x200];

// 函数定义
/**
 * @brief USB 模块MS协议下用户自定义数据处理函数
 *
 * @param hms MS 实例
 */
void USB_UserAPDU(void *hms)
{
    USBC_MS_HandlerTypeDef *ms = (USBC_MS_HandlerTypeDef *)hms;
    //    uint16_t i;

    switch (ms->status)
    {
    case USB_STATUS_IDE:
        // 此状态仅接收到CBW包包头,如需要接收数据，在此状态下配置
        // 如果无接收数据，可以直接break;
        if (ms->CBW.CBWCB[0] == 0xC0)
        {
            ms->TransLength = 1;
            ms->TransOffset = 0;
            ms->TxRxCounter = 0; // reset TxRx counter
            ms->status = USB_STATUS_BUSY;
            return;
        }
        break;
    case USB_STATUS_BUSY:
        // 此状态下为接收数据，此处用户可以将接收缓冲区buffer的数据拷贝到应用数据缓冲区中
        return;
    case USB_STATUS_END:
        // 此状态下为接收完数据，用户可以将最后一包数据拷贝到应用数据缓冲区中
        break;
    }

    switch (ms->CBW.CBWCB[0])
    {
    case 0xC0:
        ms->CBW.CBWCB[0] = 0xFF;
        USB_MS_Send(BULKIN_EP, (uint8_t *)&ms->CSW, 13);
        break;
    case 0xC1:
        USB_MS_Send(BULKIN_EP, (uint8_t *)ms->Buffer, 0x200);
        ms->CBW.CBWCB[0] = 0xFF;
        USB_MS_Send(BULKIN_EP, (uint8_t *)&ms->CSW, 13);
        break;
    default:
        ms->CBW.CBWCB[0] = 0xFF;
        ms->CSW.bCSWStatus = 1;
        USB_MS_Send(BULKIN_EP, (uint8_t *)&ms->CSW, 13);
    }
}

/**
 * @brief USB 模块CDC协议下用户自定义数据处理函数
 *
 */
void USB_CDC_DoCmd(void)
{

    uint8_t Buffer[USB_MAX_PACKET_SIZE_V20];
    uint16_t recvLen = 0;
    uint16_t i = 0;

    _memset(Buffer, 0x00, sizeof(Buffer));
    recvLen = USB_CDC_Receive(BULKOUT_EP, (uint8_t *)Buffer);
    if (recvLen == 0)
        return;

    printf("\t Receive Data Length = 0x%04x:\r\n", recvLen);
    for (i = 0; i < recvLen; i++)
        printf("%02x ", Buffer[i]);
    printf("\r\n");
    USB_CDC_Send(BULKIN_EP, (uint8_t *)Buffer, recvLen);
}

/**
 * @brief USB 模块HID协议下用户自定义数据处理函数
 *
 */
void USB_HID_DoCmd(void)
{
    uint8_t Buffer[USB_MAX_PACKET_SIZE_V20];
    uint16_t recvLen = 0;
    uint16_t i = 0;

    _memset(Buffer, 0x00, sizeof(Buffer));
    recvLen = USB_HID_Receive(BULKOUT_EP, (uint8_t *)Buffer);
    if (recvLen == 0)
        return;

    Buffer[0] += 1;
    for (i = 1; i < recvLen; i++)
    {
        if (Buffer[i])
        {
            Buffer[i] += 1;
        }
    }
    //    _memset((Buffer+0x100), 0x5A, 0x100);
    printf("\t USB Receive Data Length = 0x%04x:\r\n", recvLen);
    for (i = 0; i < recvLen; i++)
        printf("%02x ", Buffer[i]);
    printf("\r\n");

    USB_HID_Send(BULKIN_EP, (uint8_t *)Buffer, gUSBC_PacketSize);
}

/**
 * @brief USB MS 模式测试用例
 *
 */
void USBC_MS_Demo(void)
{
    usbc_handle_t hUSB;

    hUSB.Ep0DataStage = 0;

#if USB_VERSION_1p1
    hUSB.version = USB_VERSION_11;
    hUSB.osc = CPM_USBPHY_EXTER_OSC;
#else
    hUSB.version = USB_VERSION_20;
    hUSB.osc = CPM_USBPHY_EXTER_OSC;
#endif

    printf("USB MS协议测试：%s \r\n", __FUNCTION__);

    USB_MS_HandlerRegistry(USB_UserAPDU, DataBuf);

    drv_usbc_varinit_callback = USBC_VarInit_MSC;

    drv_usbc_ep0handler_callback = USBC_Ep0Handler_MSC;

    drv_drv_usbc_Irq_callback = drv_usbc_Irq;

    drv_usbc_init(&hUSB);

    while (1)
    {
        if (gUSBC_RxINT_Flag & (1 << BULKOUT_EP))
        {
            gUSBC_RxINT_Flag &= ~(1 << BULKOUT_EP); // 清除标志位
            USB_MS_RxHandler();
        }
        if (gUSBC_TxINT_Flag & (1 << BULKIN_EP))
        {
            gUSBC_TxINT_Flag &= ~(1 << BULKIN_EP); // 清除标志位
            USB_MS_TxHandler();
        }
    }
}

/**
 * @brief USB CDC 模式测试用例
 *
 */

void USBC_CDC_Demo(void)
{
    usbc_handle_t hUSB;

    hUSB.Ep0DataStage = 0;

#if USB_VERSION_1p1
    hUSB.version = USB_VERSION_11;
    hUSB.osc = CPM_USBPHY_EXTER_OSC;
#else
    hUSB.version = USB_VERSION_20;
    hUSB.osc = CPM_USBPHY_EXTER_OSC;
#endif

    printf("USB CDC协议测试：%s \r\n", __FUNCTION__);

    drv_usbc_varinit_callback = USBC_VarInit_CDC;

    drv_usbc_ep0handler_callback = USBC_Ep0Handler_CDC;

    drv_drv_usbc_Irq_callback = drv_usbc_Irq;

    drv_usbc_init(&hUSB);

    while (1)
    {

        if ((gUSBC_RxINT_Flag & (1 << BULKOUT_EP)))
        {
            gUSBC_RxINT_Flag &= ~(1 << BULKOUT_EP); // 清除标志位
            USB_CDC_DoCmd();
        }
    }
}

/**
 * @brief USB HID 模式测试用例
 *
 */
void USBC_HID_Demo(void)
{
    usbc_handle_t hUSB;

    hUSB.Ep0DataStage = 0;

#if USB_VERSION_1p1
    hUSB.version = USB_VERSION_11;
    hUSB.osc = CPM_USBPHY_EXTER_OSC;
#else
    hUSB.version = USB_VERSION_20;
    hUSB.osc = CPM_USBPHY_EXTER_OSC;
#endif

    printf("USB HID协议测试：%s \r\n", __FUNCTION__);

    drv_usbc_varinit_callback = USBC_VarInit_HID;

    drv_usbc_ep0handler_callback = USBC_Ep0Handler_HID;

    drv_drv_usbc_Irq_callback = drv_usbc_Irq;

    drv_usbc_init(&hUSB);

    while (1)
    {
        if ((gUSBC_RxINT_Flag & (1 << BULKOUT_EP)))
        {
            gUSBC_RxINT_Flag &= ~(1 << BULKOUT_EP); // 清除标志位
            USB_HID_DoCmd();
        }
    }
}

/**
 * @brief  USB 模块测试示例函数
 *
 */
void usb_device_demo(void)
{

#if USBMS_TEST
    USBC_MS_Demo();
#elif USBCDC_TEST
    USBC_CDC_Demo();
#elif USBHID_TEST
    USBC_HID_Demo();
#endif
}

/** @} */  // 结束 USBDEVICE 模块分组
/** @} */  // 结束外设模块分组
