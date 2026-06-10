/**
 * @file usb_host_test.c
 * @author Product application department
 * @brief  USB 主机模块测试用例，仅供参考
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
 * @defgroup USBHOST USBHOST模块
 * @ingroup ThirdPart
 * @{
 */
 
// 头文件包含
#include "usb_host_test.h"
#include <stdio.h>
#include "stdbool.h"

//全局变量定义
extern uint32_t g_databuf[256]; /* usb host使用的缓冲区 */
extern sUSBCCommonReg *gUSBC_ComReg;

//函数定义
extern void (*USB_IRQHandler_Callback)(void);
extern void USBDev_ISR(void);

/**
 * @brief USB 主机模式读写测试函数
 * 
 */
void ReadAndWriteTest(void)
{
    uint32_t lba_number = 0;
    uint32_t i;

#if 0
    memset(g_databuf, 0, sizeof(g_databuf));
	USBHost_Inquiry12((uint8_t *)g_databuf);
    printf("Device Inquiry");
    for (i = 0; i < 200; i++)
    {
        printf("%#x ", g_databuf[i]);
    }
    printf("\r\n");
#endif

#if 0
    memset(g_databuf, 0, sizeof(g_databuf));
    USBHost_ReadCapacity23((uint8_t *)g_databuf);
    printf("Device Capacity23");
    for (i = 0; i < 256; i++)
    {
        printf("%#x ", g_databuf[i]);
    }
    printf("\r\n");
#endif

#if 0
    memset(g_databuf, 0, sizeof(g_databuf));
	USBHost_ReadCapacity25((uint8_t *)g_databuf);
    printf("Device Capacity25");
    for (i = 0; i < 256; i++)
    {
        printf("%#x ", g_databuf[i]);
    }
    printf("\r\n");
#endif

#if 1
    lba_number = 3;
    memset(g_databuf, 0xa5, 0x200);
    USBHost_Read10(lba_number, 1, (uint8_t *)g_databuf);
#endif

#if 0
    lba_number = 5;
    memset(g_databuf, 0x66, 0x200);
    USBHost_Write10(lba_number, 1, (uint8_t *)g_databuf);
#endif

#if 1
    // Read10 Command Testb
    lba_number = 3;
    memset(g_databuf, 0x00, 0x200);
    USBHost_Read10(lba_number, 1, (uint8_t *)g_databuf);
    printf("Device Read10");
    for (i = 0; i < 128; i++)
    {
        printf("%#x ", g_databuf[i]);
    }
    printf("\r\n");
#endif

    return;
}
/**
 * @brief USB 主机模式测试例程
 * 
 */

void usb_host_demo(void)
{
    uint8_t res = 0;
    uint8_t reg = 0;
    USBC_Host_HandleTypeDef usbHostInitStructure;

    printf("USB Host Test Start:");
    /* 初始化 */
    usbHostInitStructure.osc = USBHOSTPHY_EXTER_OSC;
    usbHostInitStructure.version = USB_VERSION_20;
    usbHostInitStructure.CallBackIT = NULL;

    USBHost_Init(&usbHostInitStructure);
    printf("Init success");
    USB_IRQHandler_Callback = USBDev_ISR;
    __disable_irq();
    NVIC_Init(6, 0, USBC_Irqn, 3);
    __enable_irq();

    while (1)
    {
        printf("Wait Device connect");
        res = waitDeviceConnect(DELAY_VALUE);
        if (res != 0)
        {
            printf("Time Out");
        }
        else
        {
            printf("Device insert");
            delayms(500);

            // Enumerate a removal hard disk,then enable USB DEV.
            reg = USBHost_InitUsbDevInsert();
            if (reg == false) // can't enumerate device which didn't support mass-storage-only and bulk-only protocol
            {
                printf("This Device can not support mass-storage-only and bulk-only protocol");
                gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;
                return;
            }

            ReadAndWriteTest();

            while (1)
            {
                if (USBHost_GetHostDeviceInsertFlag() == 0)
                {
                    break;
                }
            }
        }
    } /* while (1) */
}
/** @} */  // 结束 USBHOST 模块分组
/** @} */  // 结束外设模块分组