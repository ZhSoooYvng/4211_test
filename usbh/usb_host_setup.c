/**
 * @file usb_host_setup.c
 * @author Product application department
 * @brief usb host setup function
 * @version V1.0
 * @date 2025-04-03
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 *
 */

// 头文件包含
#include "usb_host_setup.h"
#include <stdio.h>
#include "stdbool.h"

#define PRINT 1

// 函数定义

/**
 * @function    USBHost_ClearFeature
 * @brief       清楚特性指令
 * @param[in]   packetSize   包大小
 * @retval      执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_ClearFeature(uint8_t packetSize)
{
    return USBHost_ControlTransfer(USBReq_ClearFeature, 0, packetSize);
}

/**
 * @function   USBHost_GetStatus
 * @brief      获取状态
 * @retval     NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_GetStatus(void)
{
}

/**
 * @function    USBHost_SetConfiguration
 * @brief       设置配置
 * @param[in]   packetSize   包大小
 * @retval      执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_SetConfiguration(uint8_t packetSize)
{

    return USBHost_ControlTransfer((uint8_t *)USBReq_SetConfiguration, 0, packetSize);
}

/**
 * @function    USBHost_GetDeviceDescriptor
 * @brief       获取设备描述符
 * @param[in]   OneOrAll 0：first time to get device descriptor  1：all
 * @param[out]  arrDeviceDesc   接收数据缓冲区
 * @retval      执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_GetDeviceDescriptor(uint8_t OneOrAll, uint8_t arrDeviceDesc[])
{
    uint8_t pGet_Descriptor[8], packetSize;

    memcpy(pGet_Descriptor, USBReq_GetDescriptor, 8);

    if (OneOrAll == 0)
    {
        /* the first time to get device descriptor */
        pGet_Descriptor[6] = 0x40;
        packetSize = 0x40;
    }
    else
        packetSize = arrDeviceDesc[7];

    return USBHost_ControlTransfer(pGet_Descriptor, arrDeviceDesc, packetSize);
}

/**
 * @function    USBHost_GetConfigDescriptor
 * @brief       获取配置描述符
 * @param[in]   OneOrAll 0：first time to get device descriptor  1：all
 * @param[out]  arrConfigDesc   接收数据缓冲区
 * @param[in]   packetSize      包大小
 * @retval      执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_GetConfigDescriptor(uint8_t OneOrAll, uint8_t arrConfigDesc[], uint8_t packetSize)
{
    uint8_t pDescriptor[8];
    /* Get_Descriptor points to data stored in code segment,so need to copy to RAM. */
    memcpy(pDescriptor, USBReq_GetDescriptor, 8);

    /* descriptor type */
    pDescriptor[3] = 0x02;

    /* change data length */
    if (OneOrAll == 0)
        pDescriptor[6] = 0x09;
    else
        pDescriptor[6] = 0x20;

    return USBHost_ControlTransfer(pDescriptor, arrConfigDesc, packetSize);
}

/**
 * @function    USBHost_GetStringDescriptor
 * @brief       获取字符串描述符
 * @param[in]   index    字符串偏移
 * @param[out]  arrStringDesc   接收数据缓冲区
 * @param[in]   packetSize      包大小
 * @retval      执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_GetStringDescriptor(uint8_t index, uint8_t arrStringDesc[], uint8_t packetSize)
{
    uint8_t pDescriptor[8], ret;
    /* Get_Descriptor points to data stored in code segment,so need to copy to RAM. */
    memcpy(pDescriptor, USBReq_GetDescriptor, 8);

    /* descriptor type */
    pDescriptor[3] = 0x03;

    // string descriptor index
    pDescriptor[2] = 0;
    pDescriptor[6] = 0x02;

    // get LANGID length
    ret = USBHost_ControlTransfer(pDescriptor, arrStringDesc, packetSize);
    if (ret)
        return ret;

    // get LANGID
    pDescriptor[6] = arrStringDesc[0];
    ret = USBHost_ControlTransfer(pDescriptor, arrStringDesc, packetSize);
    if (ret)
        return ret;

    // string length of this LANGID
    pDescriptor[2] = index;
    pDescriptor[4] = arrStringDesc[2];
    pDescriptor[5] = arrStringDesc[3];
    pDescriptor[6] = 0x02;
    ret = USBHost_ControlTransfer(pDescriptor, arrStringDesc, packetSize);
    if (ret)
        return ret;

    // get the string
    pDescriptor[6] = arrStringDesc[0];
    return USBHost_ControlTransfer(pDescriptor, arrStringDesc, packetSize);
}

/**
 * @function    USBHost_SetInterface
 * @brief       设置接口
 * @param[in]   packetSize      包大小
 * @retval      执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_SetInterface(uint8_t packetSize)
{
    return USBHost_ControlTransfer((uint8_t *)USBReq_SetInterface, 0, packetSize);
}

/**
 * @function    USBHost_SetAddress
 * @brief       设置设备地址
 * @param[in]   address       设备地址
 * @param[in]   packetSize    包大小
 * @retval      执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_SetAddress(uint8_t address, uint8_t packetSize)
{
    uint8_t bufUsbReq[8];
    uint8_t ret;

    /* ensure ep0 control/status regesters appeare in the memory map. */
    // gUSBC_ComReg->EINDEX = CONTROL_EP;

    /* bufUsbReq points to data stored in code segment,so need to copy to RAM. */
    memcpy(bufUsbReq, USBReq_SetAddress, 8);

    /* send set address command to device */
    bufUsbReq[2] = address;

    ret = USBHost_ControlTransfer(bufUsbReq, 0, packetSize);
    if (ret)
        return ret;

    /* write address to FAddr */
    gUSBC_ComReg->FADDRR = address;
    return 0;
}

/**
 * @function    USBHost_GetMaxLUN
 * @brief       获取最大逻辑单元数
 * @param[in]   packetSize   包大小
 * @param[out]  lun          返回最大逻辑单元数
 * @retval      执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_GetMaxLUN(uint8_t packetSize, uint8_t *lun)
{
    return USBHost_ControlTransfer((uint8_t *)USBReq_GetMaxLUN, lun, packetSize);
}

/**
 * @function    USBHost_ControlTransfer
 * @brief       控制端点传输接口
 * @param[in]   setupPacket   指令包缓冲区地址
 * @param[in]   arrData       返回数据缓冲区地址
 * @param[out]  packetSize    包大小
 * @retval      执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_ControlTransfer(uint8_t *setupPacket, uint8_t *arrData, uint8_t packetSize)
{
    uint8_t i = 0, ret, flag = 0;
    unsigned short dataLength = 0;

    /* ensure ep0 control/status registers appeare in the memory map. */
    gUSBC_ComReg->EINDEX = CONTROL_EP;

    /* send a setup packet to device */
    ret = USBHost_SendSetupPacket(setupPacket);
    if (ret)
    {
        printf("SendSetPacket filed\r\n");
        USBHost_ClearERNBit();
        return ret;
    }
    else
    {
        // printf("success");
    }

    // delay memonet for device
    while (dataLength < 0x0FFF)
        dataLength++;

    /* get data length will be read */
    dataLength = setupPacket[7];
    dataLength = dataLength << 8;
    dataLength += setupPacket[6];

    /* get data from device,IN data phase.If datalength is 0,means that its a ZERO request. */
    if (dataLength)
    {
        i = 0;
        flag = 1;
        while (dataLength)
        {
            /* Enter In data phase to read device descriptor. */
            ret = USBHost_DataInPhase();
            if (ret)
            {
                USBHost_ClearERNBit();
                return ret;
            }

            /* if data length received is 0 ,its means no data can be read from device. */
            if (g_hostEp0RxCount == 0)
                break;

            /* Now only get the first 36bytes data,for string descriptor it is not enough. */
            if (g_hostEp0RxCount <= 36)
                memcpy(arrData + i * packetSize, g_hostdatabuf, g_hostEp0RxCount);
            else
                memcpy(arrData + i * packetSize, g_hostdatabuf, 36);

            /* if data length less than packet size,its means no data can be read from device. */
            if (g_hostEp0RxCount < packetSize)
                break;

            i++;
            dataLength -= packetSize;
        }
    }

    if (flag)
        /* out status phase */
        ret = USBHost_StatusOutPhase();
    else
        /* in status phase */
        ret = USBHost_StatusInPhase();

    if (ret)
    {
        USBHost_ClearERNBit();
        return 0;
    }
    else
        return 0;
}

/**
 * @function   USBHost_SendSetupPacket
 * @brief      发送setup包
 * @param[in]   ucSetupPack  发送数据地址
 * @retval     执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_SendSetupPacket(uint8_t ucSetupPack[])
{
    uint8_t ucReg;

    /* write into ep0's FIFO */
    USB_WriteFIFO(USB_FIFO_ENDPOINT_0, ucSetupPack, 8);

    ucReg = 0x0a;

    g_flgHostTxRxDone = 0;
    /* enable ep0 register map to be accessed */
    gUSBC_ComReg->EINDEX = CONTROL_EP;
    /* set SetupPkt and TxPktRdy */
    gUSBC_IdxReg->E0CSR_L = ucReg;

    /* wait for the interrupt that indicate the setup packet had been transmitted */
    USBHost_WaitTxRxDone(1);

    if (g_flgHostTxRxDone == 1)
        return 0;
    else
        return 1;
}

/**
 * @function   USBHost_TransferSetupPacket
 * @brief      指令发送同时接收数据
 * @param[in]  ucSetupPack  指令包
 * @retval     执行结果  0：成功  其它：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_TransferSetupPacket(uint8_t ucSetupPack[])
{
    uint8_t ret, flag = 0;
    unsigned short dataLength;

    /* ensure ep0 control/status registers appeare in the memory map. */
    // gUSBC_ComReg->EINDEX = CONTROL_EP;

    /* get data length will be read */
    dataLength = ucSetupPack[7];
    dataLength = dataLength << 8;
    dataLength += ucSetupPack[6];

    /* send a setup packet to device */
    ret = USBHost_SendSetupPacket(ucSetupPack);
    if (ret)
    {
        USBHost_ClearERNBit();
        return ret;
    }

    if (dataLength)
    {
        flag = 1;
        while (dataLength)
        {
            /* Enter In data phase to read device descriptor. */
            ret = USBHost_DataInPhase();
            if (ret)
            {
                USBHost_ClearERNBit();
                return ret;
            }

            /* if data length received is 0 ,its means no data can be read from device. */
            if (g_hostEp0RxCount == 0)
                break;

            dataLength -= g_hostEp0RxCount;
        }
        ret = USBHost_DataInPhase();
        if (ret)
        {
            USBHost_ClearERNBit();
            return ret;
        }
    }

    if (flag)
        /* out status phase */
        ret = USBHost_StatusOutPhase();
    else
        /* in status phase */
        ret = USBHost_StatusInPhase();

    if (ret)
    {
        USBHost_ClearERNBit();
        return 0;
    }
    else
        return 0;
}

/**
 * @function   USBHost_ClearERNBit
 * @brief      USB host清除错误、stall、NAK标志位
 * @retval     NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_ClearERNBit(void)
{

    /* clear Error bit,RxStall bit and NAK Timeout bit */
    gUSBC_IdxReg->E0CSR_L = 0;
}

/**
 * @function     USBHost_DataInPhase
 * @brief        发送IN令牌
 * @retval       执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_DataInPhase(void) // only used on Control transfer
{
    uint8_t ucReg;

    ucReg = 0x20; /* set ReqPkt = 1 to requset a In transaction */

    g_flgHostTxRxDone = 0;
    gUSBC_ComReg->EINDEX = CONTROL_EP;
    gUSBC_IdxReg->E0CSR_L = ucReg;

    /* wait for the interrupt that indicate the data packet had been received */

    USBHost_WaitTxRxDone(1);

    if (g_flgHostTxRxDone == 1)
        return 0;
    else
        return 1; // can check g_hostStatus to judge which error occured.
}

/**
 * @function     USBHost_StatusInPhase
 * @brief        接收状态阶段
 * @retval       执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_StatusInPhase(void) // only used on Control transfer
{
    uint8_t ucReg;
    ucReg = 0x60; // set ReqPkt and StatusPkt

    g_flgHostTxRxDone = 0;
    gUSBC_ComReg->EINDEX = CONTROL_EP;
    gUSBC_IdxReg->E0CSR_L = ucReg;

    /* wait for the interrupt that indicate the setup packet had been transmitted */

    USBHost_WaitTxRxDone(1);

    if (g_flgHostTxRxDone == 1)
        return 0;
    else
        return 1; // can check g_hostStatus to judge which error occured.
}

/**
 * @function     USBHost_StatusOutPhase
 * @brief        发送状态阶段
 * @retval      执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_StatusOutPhase(void) // only used on Control transfer
{
    uint8_t ucReg;
    ucReg = 0x42; // set TxRdyPkt and StatusPkt

    g_flgHostTxRxDone = 0;
    gUSBC_ComReg->EINDEX = CONTROL_EP;
    gUSBC_IdxReg->E0CSR_L = ucReg;

    /* wait for the interrupt that indicate the setup packet had been transmitted */

    USBHost_WaitTxRxDone(1);

    if (g_flgHostTxRxDone == 1)
        return 0;
    else
        return 1; // can check g_hostStatus to judge which error occured.
}

/**
 * @function     USBHost_InitUsbDevInsert
 * @brief        设备枚举
 * @retval       successfully return true,else return false.
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_InitUsbDevInsert(void)
{
    uint8_t ucMode = 0, ret = 0, ucAddr;
    uint8_t controlPacketSize;
    uint8_t maxLun = 0;
    static uint8_t databuf[1024] = {0};

    // ucMode = gUSBC_fifoReg->OTGCTRL;

    /* host detect a full-speed or high-speed device */
    ucMode = gUSBC_ComReg->UCSR;

    if (g_usbVer == 1)
    {
        ucMode |= USB_POWER_HS_ENAB; // high speed
    }
    else
    {
        ucMode &= (~USB_POWER_HS_ENAB); // full speed
    }

    /* enable high-speed,so after reset,MUSB will negotiate for high-speed operation. */
    gUSBC_ComReg->UCSR = ucMode;

    /* reset bus */
    USBHost_BusReset();

    ucMode = 0;
    while (ucMode < 3)
    {
        // read device descriptor
        ret = USBHost_GetDeviceDescriptor(0, (uint8_t *)USB_Device_Descriptor);
        if (ret)
        {
            if (ucMode == 2)
            {
                printf("Get Device Descriptor fialed\r\n");
                return false;
            }
            else
            {
                USBHost_ClearFeature(0x40);
                ucMode++;
            }
        }
        else
        {
            ucMode = 0;

#if PRINT
            printf("\r\nDevice Descriptor\r\n");
            for (int i = 0; i < 18; i++)
            {
                printf("%#x ", USB_Device_Descriptor[i]);
            }
            printf("\r\n\r\n");
#endif /* PRINT */

            break;
        }
    }
    delayms(5);

    /* max packet size of ep0 */
    controlPacketSize = USB_Device_Descriptor[7];

    /* set device address,now one HOST can only connect with one device(don't support hub),
     * set device address as 2 */
    ucAddr = USB_MASS_STORAGE_DEV_ADDR;

    while (ucMode < 3)
    {
        // set device a valid address,it's better from address 2.
        ret = USBHost_SetAddress(ucAddr, controlPacketSize);
        if (ret)
        {
            if (ucMode == 2)
                return false;
            else
            {
                USBHost_ClearFeature(controlPacketSize);
                ucMode++;
            }
        }
        else
        {
            ucMode = 0;
            break;
        }
    }
    delayms(5);

    /* get device descriptor again */
    while (ucMode < 3)
    {
        ret = USBHost_GetDeviceDescriptor(1, (uint8_t *)USB_Device_Descriptor);
        if (ret)
        {
            if (ucMode == 2)
                return false;
            else
            {
                USBHost_ClearFeature(controlPacketSize);
                ucMode++;
            }
        }
        else
        {
            ucMode = 0;
            break;
        }
    }
    delayms(5);

    /* get 9 byte config descriptor,the first time only get default cofiguration data */
    while (ucMode < 3)
    {
        ret = USBHost_GetConfigDescriptor(0, (uint8_t *)USB_Configuration_Descriptor, controlPacketSize);
        if (ret)
        {
            if (ucMode == 2)
            {
                printf("Get Config Descritor failed\r\n");
                return false;
            }
            else
            {
                USBHost_ClearFeature(controlPacketSize);
                ucMode++;
            }
        }
        else
        {
#if PRINT
            printf("\r\nConfig Descritor\r\n");
            for (int i = 0; i < 64; i++)
            {
                printf("%#x ", USB_Configuration_Descriptor[i]);
            }
            printf("\r\n\r\n");
#endif

            ucMode = 0;
            break;
        }
    }
    delayms(5);

    /* get detailed config descriptor contains interface and endpoint descriptor */
    while (ucMode < 3)
    {
        ret = USBHost_GetConfigDescriptor(1, USB_Configuration_Descriptor, controlPacketSize);
        if (ret)
        {
            if (ucMode == 2)
            {
                printf("Get interface and endpoint descriptor failde\r\n");
                return false;
            }
            else
            {
                USBHost_ClearFeature(controlPacketSize);
                ucMode++;
            }
        }
        else
        {
            int i = 0, k = 0;

#if PRINT
            printf("\r\nInterface and endpoint descriptor\r\n");
            while (USB_Configuration_Descriptor[i] != 0)
            {
                for (; i < (USB_Configuration_Descriptor[k] + k); i++)
                {
                    printf("%#x ", USB_Configuration_Descriptor[i]);
                }
                printf("\r\n");
                k = i;
            }
            printf("\r\n");
#endif /* PRINT */

            ucMode = 0;
            break;
        }
    }
    delayms(5);

    // check if it is a mass storage(interface 0x08) bulk-only(interface 0x06) device
    if (USB_Configuration_Descriptor[14] != 0x08 || USB_Configuration_Descriptor[15] != 0x06)
        return false;

    // get device's Manufacturer string descriptor
    if (USB_Device_Descriptor[14])
    {
        while (ucMode < 3)
        {
            ret = USBHost_GetStringDescriptor(USB_Device_Descriptor[14], (uint8_t *)USB_String_Descriptor, controlPacketSize);
            if (ret)
            {
                if (ucMode == 2)
                {
                    printf("Get Decive String Descriptor failed");
                    return false;
                }
                else
                {

                    USBHost_ClearFeature(controlPacketSize);
                    ucMode++;
                }
            }
            else
            {
#if PRINT
                printf("String Descritor");
                for (int i = 2; i < 64; i++)
                {
                    printf("%c", USB_String_Descriptor[i]);
                }
                printf("\r\n");
#endif /* PRINT */

                ucMode = 0;
                break;
            }
        }
    }
    delayms(5);

    // get device's Product string descriptor
    if (USB_Device_Descriptor[15])
    {
        while (ucMode < 3)
        {
            memset_f(USB_String_Descriptor, 0, 64);
            ret = USBHost_GetStringDescriptor(USB_Device_Descriptor[15], (uint8_t *)USB_String_Descriptor, controlPacketSize);
            if (ret)
            {
                if (ucMode == 2)
                {
                    printf("Get Device Product String Descriptor failed");
                    return false;
                }
                else
                {
                    USBHost_ClearFeature(controlPacketSize);
                    ucMode++;
                }
            }
            else
            {
#if PRINT
                printf("Product String Descritor");
                for (int i = 2; i < 64; i++)
                {
                    printf("%c", USB_String_Descriptor[i]);
                }
                printf("\r\n\r\n");
#endif /* PRINT */

                ucMode = 0;
                break;
            }
        }
    }
    delayms(5);

    // get device's serial number string descriptor
    if (USB_Device_Descriptor[16])
    {
        while (ucMode < 3)
        {
            ret = USBHost_GetStringDescriptor(USB_Device_Descriptor[16], (uint8_t *)USB_String_Descriptor, controlPacketSize);
            if (ret)
            {
                if (ucMode == 2)
                {
                    printf("Get serial number string descriptor failed");
                    return false;
                }
                else
                {
                    USBHost_ClearFeature(controlPacketSize);
                    ucMode++;
                }
            }
            else
            {
#if PRINT
                printf("Serial number String Descritor");
                for (int i = 2; i < 64; i++)
                {
                    printf("%c", USB_String_Descriptor[i]);
                }
                printf("\r\n\r\n");
#endif /* PRINT */

                ucMode = 0;
                break;
            }
        }
    }
    delayms(5);

    /* set configuration to device */
    while (ucMode < 3)
    {
        ret = USBHost_SetConfiguration(controlPacketSize);
        if (ret)
        {
            if (ucMode == 2)
                return false;
            else
            {
                USBHost_ClearFeature(controlPacketSize);
                ucMode++;
            }
        }
        else
        {
            ucMode = 0;
            break;
        }
    }
    delayms(5);

    /* set interface to device */
    while (ucMode < 3)
    {
        ret = USBHost_SetInterface(controlPacketSize);
        if (ret)
        {
            if (ucMode == 2)
                return false;
            else
            {
                USBHost_ClearFeature(controlPacketSize);
                ucMode++;
            }
        }
        else
        {
            ucMode = 0;
            break;
        }
    }
    delayms(5);

    // get max LUN
    while (ucMode < 3)
    {
        ret = USBHost_GetMaxLUN(controlPacketSize, &maxLun);
        if (ret)
        {
            if (ucMode == 2)
                break; // don't take care the max LUN
            else
            {
                USBHost_ClearFeature(controlPacketSize);
                ucMode++;
            }
        }
        else
        {
            // g_currentLUN &=0xF3;
            // g_currentLUN |=(maxLun & 3)<<2;//set max LUN
            g_currentLUN = maxLun;
#if PRINT
            printf("MaxLun = %u\r\n", maxLun);
#endif /* PRINT */

            break;
        }
    }
    delayms(5);

    // config host rx and tx end point
    USBHost_ConfigEpType((uint8_t *)USB_Configuration_Descriptor);
    delayms(5);
    printf("config host rx and tx end point success \r\n");

    printf("Test ready start\r\n");
    USBHost_TestUnitReady();
    for (uint8_t i = 0; i < 13; i++)
    {
        printf(" %02x ", g_cswPacket[i]);
    }
    printf("\r\n");
    printf("Test ready done\r\n");

    printf("USBHost_ReadCapacity10 start \r\n");
    USBHost_ReadCapacity10((uint8_t *)databuf);
    printf("USBHost_ReadCapacity10 done \r\n");

    for (uint8_t i = 0; i < 13; i++)
    {
        printf(" %02x ", databuf[i]);
    }
    printf("\r\n");

    return true;
}

/**
 * @function     USBHost_BusReset
 * @brief        总线复位
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_BusReset(void)
{
    uint8_t ucMode;

    gUSBC_ComReg->FADDRR = 0;

    /* send reset signal to bus */
    ucMode = gUSBC_ComReg->UCSR;

    ucMode |= USB_POWER_RESET;

    gUSBC_ComReg->UCSR = ucMode;

    // delay 10ms
    delay(0x180000); // reset signal must hold at least 10ms

    /* clear reset signal */
    ucMode = gUSBC_ComReg->UCSR;
    ucMode &= 0xF7;
    gUSBC_ComReg->UCSR = ucMode;

    // delay 10ms
    delay(0x180000);

    if (ucMode & 0x10)
    {
        g_usbVer = 1; // USB2.0
        gTxMaxPacket = USB_MAX_PACKET_SIZE;
        gRxMaxPacket = USB_MAX_PACKET_SIZE;
    }
    else
    {
        g_usbVer = 0; // USB1.1
        gTxMaxPacket = _USB_MAX_PACKET_SIZE_V11;
        gRxMaxPacket = _USB_MAX_PACKET_SIZE_V11;
    }
}

/**
 * @function     USBHost_ConfigEpType
 * @brief        设置端点的类型
 * @param[in]    pHost_Config_Descriptor    配置描述符地址
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_ConfigEpType(uint8_t pHost_Config_Descriptor[])
{
    uint8_t ucReg;

    /*set register index*/
    gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;

    ucReg = 0x20; // bulk only

    if (pHost_Config_Descriptor[20] & 0x80)
        ucReg |= pHost_Config_Descriptor[27] & 0xF;
    else
        ucReg |= pHost_Config_Descriptor[20] & 0xF;

    /* set device bulk-out endpoint number and transfer protocol */
    gUSBC_IdxReg->TXTYPE = ucReg;

    /* set max packet size */
    gUSBC_IdxReg->TXMAXP_L = pHost_Config_Descriptor[29];
    gUSBC_IdxReg->TXMAXP_H = pHost_Config_Descriptor[30];

    ucReg = gUSBC_IdxReg->TXCSR_L;
    /* set flushfifo=1 so that flush FIFO buff,set ClrDataTog=1 */
    ucReg |= HOST_TXCSR_FLUSH_FIFO | HOST_TXCSR_CLR_DATA_TOG;
    gUSBC_IdxReg->TXCSR_L = ucReg;

    /*set register index*/
    gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;
    ucReg = 0x20; // bulk only
    if (pHost_Config_Descriptor[20] & 0x80)
        ucReg |= pHost_Config_Descriptor[20] & 0xF;
    else
        ucReg |= pHost_Config_Descriptor[27] & 0xF;

    /* set device bulk-in endpoint number and transfer protocol */
    gUSBC_IdxReg->RXTYPE = ucReg;

    /* set max packet size */
    gUSBC_IdxReg->RXMAXP_L = pHost_Config_Descriptor[22];
    gUSBC_IdxReg->RXMAXP_H = pHost_Config_Descriptor[23];

    ucReg = gUSBC_IdxReg->RXCSR_L;
    /* set flushfifo=1, ClrDataTog=1 */
    ucReg |= HOST_RXCSR_FLUSH_FIFO | HOST_RXCSR_CLR_DATA_TOG;
    gUSBC_IdxReg->RXCSR_L = ucReg;

    /*config rx/tx fifo size*/
    if (g_usbVer == 1)
    {
        // usb 2.0, fifo size is 512B
        gUSBC_fifoReg->RXFIFOSZ = 0x06;
        gUSBC_fifoReg->TXFIFOSZ = 0x06;
    }
    else
    {
        gUSBC_fifoReg->RXFIFOSZ = 0x03;
        gUSBC_fifoReg->TXFIFOSZ = 0x03;
    }
}

/**
 * @function     USBHost_Ep0RxDone
 * @brief        端点0接收处理
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_Ep0RxDone(void)
{
    uint8_t ucTemp;

    ucTemp = gUSBC_IdxReg->E0COUNTR_L;
    ucTemp &= 0x7f;

    g_hostEp0RxCount = ucTemp;

    /* count is 0,no data and return. */
    if (!ucTemp)
    {
        /* for host in status stage,will generate a Rx interrupt,but no data received */
        g_flgHostTxRxDone = 1; // normal TxRxDone

        /* clear RxPktRdy */
        gUSBC_IdxReg->E0CSR_L = 0;
        return;
    }

    USB_ReadFIFO(USB_FIFO_ENDPOINT_0, g_hostdatabuf, ucTemp);
    g_flgHostTxRxDone = 1; // normal TxRxDone

    /* set FlushFIFO,so flush fifo of ep0 and clear RxPktRdy */
    gUSBC_IdxReg->E0CSR_L = 0x80;
}
