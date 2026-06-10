/**
 * @file usb_host_scsi.c
 * @author Product application department
 * @brief usb host scsi interfaces
 * @version V1.0
 * @date 2025-04-03
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 *
 */

// 头文件包含
#include "usb_host_setup.h"

// 函数定义

/**
 * @function     USBHost_WaitTxRxDone
 * @brief        等待接收或发送完成
 * @param[in]    flag    0：死等待  1：时间限制的等待
 * @retval       0：正常完成   1：超时
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_WaitTxRxDone(uint8_t flag)
{
    unsigned int counter = 0;
    while (!g_flgHostTxRxDone)
    {
        delayms(1);
        counter++;
        // 80MHz,about,4s
        if (flag && counter > 4000)
        {
            return (1);
        }
    }
    return (0);
}

/**
 * @function     USBHost_ReadDataFromFIFO
 * @brief        获取FIFO中的数据
 * @param[out]   buf      缓冲区指针
 * @param[in]    ucCount  数据长度
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_ReadDataFromFIFO(uint8_t *buf, uint16_t ucCount)
{
    /* Read data from ep0 fifo,clear Rx manually. */
    USB_ReadFIFO(USB_ENDPOINT_FIFO_ADDR, buf, ucCount);

    // clear RxPktRdy bit of RXCSR.
    ClearRX();
}

/**
 * @function     USBHost_WriteDataToFIFO
 * @brief        写数据到FIFO中
 * @param[in]    buf      缓冲区指针
 * @param[in]    ucCount  数据长度
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_WriteDataToFIFO(uint8_t *buf, uint16_t ucCount)
{
    /* Read data from ep0 fifo,clear Rx manually. */
    USB_WriteFIFO(USB_ENDPOINT_FIFO_ADDR, buf, ucCount);

    // clear RxPktRdy bit of RXCSR.
    // W8( USBH_RX_CSR_LOW,R8(USBH_RX_CSR_LOW) & 0xFE);
}

/**
 * @function     USBHost_ReadCSWFromFIFO
 * @brief        从FIFO中获取CSW包
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_ReadCSWFromFIFO(void)
{
    unsigned int i;

    for (i = 0; i < 13; i++)
    {
        g_cswPacket[i] = read_port_byte(USB_ENDPOINT_FIFO_ADDR);
    }
    // clear RxPktRdy bit of RXCSR.
    gUSBC_IdxReg->RXCSR_L &= 0xFE;
}

/**
 * @function     USBHost_RequestInTransactionWW
 * @brief        发起一个IN事务，没有等待
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_RequestInTransactionWW(void)
{
    g_flgHostTxRxDone = 0;

    // Set ReqPkt to ask a In data
    gUSBC_IdxReg->RXCSR_L = 0x20;
}

/**
 * @function     USBHost_RequestInTransaction
 * @brief        发起IN事务
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_RequestInTransaction(void)
{
    g_flgHostTxRxDone = 0;

    /* Set ReqPkt to ask a In data */
    gUSBC_IdxReg->RXCSR_L = 0x20;

    /* wait for the interrupt that indicate the setup packet had been transmitted */
    USBHost_WaitTxRxDone(1);

    gUSBC_IdxReg->RXCSR_L &= ~0x20;

    if (g_hostStatus == HOST_RECEIVE_STALL)
    {
        // clear endpoint stall
        USBReq_ClearFeature[4] = gUSBC_IdxReg->RXTYPE & 0x0F; // RX endpoint number
        USBReq_ClearFeature[4] |= 0x80;                       // IN endpoint

        USBHost_ClearFeature(0x40);

        // ensure DATA_IN_EP registers appeare in the memory map.
        gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;
    }
}

/**
 * @function     USBHost_SendOutCBWPacket
 * @brief        发送CBW包
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_SendOutCBWPacket(void)
{
    uint8_t ucTxCSRLow;

    /* set Mode to enable the ep direction as Tx */
    ucTxCSRLow = gUSBC_IdxReg->TXCSR_H;
    ucTxCSRLow |= 0x20;
    gUSBC_IdxReg->TXCSR_H = ucTxCSRLow;

    /* load packet into enp1 FIFO */
    write_port_word(USB_ENDPOINT_FIFO_ADDR, g_cbwPacket_l[0]);
    write_port_word(USB_ENDPOINT_FIFO_ADDR, g_cbwPacket_l[1]);
    write_port_word(USB_ENDPOINT_FIFO_ADDR, g_cbwPacket_l[2]);
    write_port_word(USB_ENDPOINT_FIFO_ADDR, g_cbwPacket_l[3]);
    write_port_word(USB_ENDPOINT_FIFO_ADDR, g_cbwPacket_l[4]);
    write_port_word(USB_ENDPOINT_FIFO_ADDR, g_cbwPacket_l[5]);
    write_port_word(USB_ENDPOINT_FIFO_ADDR, g_cbwPacket_l[6]);
    write_port_byte(USB_ENDPOINT_FIFO_ADDR, g_cbwPacket[28]);
    write_port_byte(USB_ENDPOINT_FIFO_ADDR, g_cbwPacket[29]);
    write_port_byte(USB_ENDPOINT_FIFO_ADDR, g_cbwPacket[30]);

    g_flgHostTxRxDone = 0;

    /* set TxPktRdy = 1 */
    ucTxCSRLow = gUSBC_IdxReg->TXCSR_L;
    ucTxCSRLow |= 0x01;
    gUSBC_IdxReg->TXCSR_L = ucTxCSRLow;

    /* wait for the interrupt that indicate the setup packet had been transmitted */
    USBHost_WaitTxRxDone(1);

    if (g_hostStatus == HOST_TXCSR_RXSTALL)
    {
        // clear endpoint stall
        printf("HOST_TXCSR_RXSTALL \r\n");
        USBReq_ClearFeature[4] = gUSBC_IdxReg->RXTYPE & 0x0F; // TX endpoint number
        USBHost_ClearFeature(0x40);

        // ensure DATA_IN_EP registers appeare in the memory map.
        gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;
    }
}

/**
 * @function     USBHost_SendOutPacket
 * @brief         数据OUT
 * @param[in]    pDataBuf    缓冲区地址
 * @param[in]    uiLength    数据长度
 * @retval       执行结果  0：成功  1：失败
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_SendOutPacket(uint8_t *pDataBuf, uint16_t uiLength)
{
    uint8_t ucTxCSRLow;
    /* select index register */
    //	W8(USBH_COMMON_INDEX, DATA_OUT_EP);

    /* set Mode to enable the ep direction as Tx */
    ucTxCSRLow = gUSBC_IdxReg->TXCSR_H;
    ucTxCSRLow |= 0x20;
    gUSBC_IdxReg->TXCSR_H = ucTxCSRLow;

    /* load packet into enp1 FIFO */
    USB_WriteFIFO(USB_ENDPOINT_FIFO_ADDR, pDataBuf, uiLength);

    g_flgHostTxRxDone = 0;

    /* set TxPktRdy = 1 */
    ucTxCSRLow = gUSBC_IdxReg->TXCSR_L;
    ucTxCSRLow |= 0x01;
    gUSBC_IdxReg->TXCSR_L = ucTxCSRLow;

    /* wait for the interrupt that indicate the setup packet had been transmitted */
    USBHost_WaitTxRxDone(1);

    if (g_hostStatus == HOST_TXCSR_RXSTALL)
    {
        // clear endpoint stall
        USBReq_ClearFeature[4] = gUSBC_IdxReg->TXTYPE & 0x0F; // TX endpoint number
        USBHost_ClearFeature(0x40);

        // ensure DATA_IN_EP registers appeare in the memory map.
        gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;
    }

    if (g_flgHostTxRxDone == 1)
        return 0;
    else
        return 1; /* can check g_hostStatus to judge which error occured. */
}

/**
 * @function     USBHost_ClearRx
 * @brief        清除接收控制寄存器的值
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_ClearRx(void)
{
    gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;
    gUSBC_IdxReg->RXCSR_L = 0;
}

/**
 * @function     USBHost_Inquiry12
 * @brief        查询设备信息
 * @param[out]   arrInquiry    接收缓冲区地址
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_Inquiry12(uint8_t arrInquiry[])
{
    //	unsigned short uiRxCount;

    g_hostStatus = HOST_INQUIRY_PHASE;

    // SCSI read cmd 0x28
    memcp_f(g_cbwPacket, (uint8_t *)SCSICmd_Inquiry12, 31);

    /*send out CBW packet*/
    USBHost_SendOutCBWPacket();

    /* Get data from device */
    USBHost_RequestInTransaction();

    USBHost_ReadDataFromFIFO(arrInquiry, 36);

    /* Ask for CSW packet */
    USBHost_RequestInTransaction();

    /*Get a CSW packet*/
    USBHost_ReadCSWFromFIFO();

    g_hostStatus = HOST_STATUS_OK;
}

/**
 * @function     USBHost_RequestSense
 * @brief        RequestSense
 * @param[out]   SCSIDATA_RequestSense 接收缓冲区地址
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_RequestSense(uint8_t SCSIDATA_RequestSense[18])
{

    g_hostStatus = HOST_REQUEST_SENSE_PHASE;

    memcp_f(g_cbwPacket, (uint8_t *)SCSICmd_RequestSense, 31);
    /*send out CBW packet*/
    USBHost_SendOutCBWPacket();

    USBHost_RequestInTransaction();
    /*Get packet from FIFO*/
    USBHost_ReadDataFromFIFO(SCSIDATA_RequestSense, 18);

    /* Ask for CSW packet */
    USBHost_RequestInTransaction();
    /*Get CSW packet*/
    USBHost_ReadCSWFromFIFO();
}

/**
 * @function     USBHost_ReadCapacity25
 * @brief        获取设备容量25
 * @param[out]   arrCapacity   接收缓冲区
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_ReadCapacity25(uint8_t arrCapacity[])
{
    uint8_t ret_value;
    uint8_t databuf[18];
    // g_hostStatus = HOST_READ_CAPACITY_PHASE;

    do
    {
        ret_value = 0;
        memcp_f(g_cbwPacket, (uint8_t *)SCSICmd_ReadCapacity25, 31);
        USBHost_SendOutCBWPacket();

        g_hostStatus = HOST_STATUS_OK;
        USBHost_RequestInTransaction();
        if (g_hostStatus == HOST_RECEIVE_STALL)
        {
            ret_value = 1;
        }

        if (ret_value == 0)
            USBHost_ReadDataFromFIFO(arrCapacity, 8);

        do
        {
            g_hostStatus = HOST_STATUS_OK;
            USBHost_RequestInTransaction();
        } while (g_hostStatus == HOST_RECEIVE_STALL);

        USBHost_ReadCSWFromFIFO();
        if (g_cswPacket[12])
        {
            USBHost_RequestSense(databuf);
        }
    } while (ret_value);

    g_hostStatus = HOST_STATUS_OK;
}

/**
 * @function     Clear_feature
 * @brief        清除特性
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void Clear_feature(void)
{
    uint8_t data8 = 0;

    data8 = gUSBC_IdxReg->RXTYPE;
    USBReq_ClearFeature[4] = (data8 & 0x0f);

    USBReq_ClearFeature[4] |= 0x80; // IN endpoint

    USBHost_ClearFeature(0x40);

    // ensure DATA_IN_EP registers appeare in the memory map.
    gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;
}

/**
 * @function     USBHost_ReadCapacity23
 * @brief        获取设备容量23
 * @param[in]    arrCapacity   接收缓冲区
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_ReadCapacity23(uint8_t arrCapacity[])
{
    uint8_t ret_value;
    uint8_t databuf[18];
    g_hostStatus = HOST_READ_CAPACITY_PHASE;

    do
    {
        ret_value = 0;
        memcp_f(g_cbwPacket, (uint8_t *)SCSICmd_ReadCapacity23, 31);
        USBHost_SendOutCBWPacket();

        g_hostStatus = HOST_STATUS_OK;
        USBHost_RequestInTransaction();
        if (g_hostStatus == HOST_RECEIVE_STALL)
        {
            ret_value = 1;
        }

        if (ret_value == 0)
            USBHost_ReadDataFromFIFO(arrCapacity, 12);

        do
        {
            g_hostStatus = HOST_STATUS_OK;
            USBHost_RequestInTransaction();
        } while (g_hostStatus == HOST_RECEIVE_STALL);

        USBHost_ReadCSWFromFIFO();
        if (g_cswPacket[12])
        {
            USBHost_RequestSense(databuf);
        }
    } while (ret_value);

    g_hostStatus = HOST_STATUS_OK;
}

extern const uint8_t SCSICmd_ReadCapacity10[];
void USBHost_ReadCapacity10(uint8_t arrCapacity[])
{
    uint8_t ret_value;
    uint8_t databuf[18];
    g_hostStatus = HOST_READ_CAPACITY_PHASE;

    do
    {
        ret_value = 0;
        memcp_f(g_cbwPacket, (uint8_t *)SCSICmd_ReadCapacity10, 31);
        USBHost_SendOutCBWPacket();

        g_hostStatus = HOST_STATUS_OK;
        USBHost_RequestInTransaction();
        if (g_hostStatus == HOST_RECEIVE_STALL)
        {
            ret_value = 1;
        }

        if (ret_value == 0)
            USBHost_ReadDataFromFIFO(arrCapacity, 12);

        do
        {
            g_hostStatus = HOST_STATUS_OK;
            USBHost_RequestInTransaction();
        } while (g_hostStatus == HOST_RECEIVE_STALL);

        USBHost_ReadCSWFromFIFO();
        if (g_cswPacket[12])
        {
            printf("USBHost_ReadCapacity10 failed \r\n");
            USBHost_RequestSense(databuf);
        }
    } while (ret_value);

    g_hostStatus = HOST_STATUS_OK;
}

/**
 * @function     USBHost_TestUnitReady
 * @brief        测试设备是否准备好
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_TestUnitReady(void)
{
    g_hostStatus = HOST_TEST_UNIT_READY_PHASE;

againt:
    memcp_f(g_cbwPacket, (uint8_t *)SCSICmd_TestReady, 31);

    /*send out CBW packet*/
    USBHost_SendOutCBWPacket();

    /* Ask for CSW packet */
    USBHost_RequestInTransaction();

    /*Get a CSW packet*/
    USBHost_ReadCSWFromFIFO();
    if (g_cswPacket[12] == 0x01)
    {
        delayms(10);
        printf("again Test ready \r\n");
        goto againt;
    }

    g_hostStatus = HOST_STATUS_OK;
}

/**
 * @function     USBHost_ModeSense
 * @brief        Mode sense Command For Host
 * @param[out]   SCSIDATA_Mode_Sense_1a   接收缓冲区
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_ModeSense(uint8_t SCSIDATA_Mode_Sense_1a[4])
{
    g_hostStatus = HOST_MODE_SENSE_PHASE;

    memcp_f(g_cbwPacket, (uint8_t *)SCSICmd_ModeSense06, 31);
    /*send out CBW packet*/
    USBHost_SendOutCBWPacket();

    USBHost_RequestInTransaction();

    /*Get packet from FIFO*/
    USBHost_ReadDataFromFIFO(SCSIDATA_Mode_Sense_1a, 4);

    /* Ask for CSW packet */
    USBHost_RequestInTransaction();
    /*Get CSW packet*/
    USBHost_ReadCSWFromFIFO();
}

/*
Description:Control Medium Remove For Host
Parameters:
    none
Return:
    none
*/
void USBHost_MediumRemoval(void)
{
    g_hostStatus = HOST_MEDIUM_REMOVAL_PHASE;

    memcp_f(g_cbwPacket, (uint8_t *)SCSICmd_MediumRemoval, 31);
    /*send out CBW packet*/
    USBHost_SendOutCBWPacket();

    // Ask for CSW packet
    USBHost_RequestInTransaction();

    /*Get CSW packet*/
    USBHost_ReadCSWFromFIFO();
}

/**
 * @function     USBHost_Read10
 * @brief        读取数据10
 * @param[in]    lba   地址
 * @param[in]    len   数据长度
 * @param[out]   buf   接收缓冲区
 * @retval       NONE
 * @attention   包结构
    g_cbwPacket[15]--SCSICMD
    g_cbwPacket[16]--
    g_cbwPacket[17]--lba[3]
    g_cbwPacket[18]--lba[2]
    g_cbwPacket[19]--lba[1]
    g_cbwPacket[20]--lba[0]
    g_cbwPacket[21]--reserved
    g_cbwPacket[22]--len[1]
    g_cbwPacket[23]--len[0]
    g_cbwPacket[24]--control
 * @author     AUTHOR
 */
void USBHost_Read10(unsigned long lba, unsigned short len, uint8_t *buf)
{
    unsigned short LbaLen = len;
    uint8_t *BufPtr = buf;

    gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;
    // SCSI read cmd 0x28
    memcp_f(g_cbwPacket, (uint8_t *)SCSICmd_Read28, 31);
    // CBWDataTransferLenth
    g_cbwPacket[8] = 0x00;
    g_cbwPacket[9] = (len << 1) & 0xff;
    g_cbwPacket[10] = (len >> 7) & 0xff;
    g_cbwPacket[11] = (len >> 15) & 0xff;
    // read lba start address
    g_cbwPacket[17] = (uint8_t)(lba >> 24);
    g_cbwPacket[18] = (uint8_t)(lba >> 16);
    g_cbwPacket[19] = (uint8_t)(lba >> 8);
    g_cbwPacket[20] = (uint8_t)(lba >> 0);
    // read len
    g_cbwPacket[22] = (uint8_t)(len >> 8);
    g_cbwPacket[23] = (uint8_t)(len >> 0);

    /*host send out CBW*/
    USBHost_SendOutCBWPacket();
    g_flgHostTxRxDone = 0;

    if (g_usbVer == 0)
    {
        LbaLen <<= 3;
    }
    // rec data
    while (LbaLen > 0)
    {
        /*host request a data in*/
        {USBHost_RequestInTransactionWW_m}
        // host wait for rec data done
        USBHost_WaitTxRxDone(1);
        g_flgHostTxRxDone = 0;
        // Read Data from USB Device
        VendorReadData(BufPtr, gRxMaxPacket);

        BufPtr += gRxMaxPacket;
        LbaLen--;
    }
    // rec CSW
    /*host request a data in*/
    {USBHost_RequestInTransactionWW_m} USBHost_WaitTxRxDone(1);
    USBHost_ReadCSWFromFIFO();
}

/**
 * @function     USBHost_Write10
 * @brief        写数据10
 * @param[in]    lba    地址
 * @param[in]    len    数据长度
 * @param[out]   buf    发送缓冲区
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_Write10(unsigned long lba, unsigned short len, uint8_t *buf)
{
    unsigned short LbaLen = len;
    uint8_t *BufPtr = buf;

    // SCSI read cmd 0x28
    memcp_f(g_cbwPacket, (uint8_t *)SCSICmd_Write2a, 31);
    // CBWDataTransferLenth
    g_cbwPacket[8] = 0x00;
    g_cbwPacket[9] = (len << 1) & 0xff;
    g_cbwPacket[10] = (len >> 7) & 0xff;
    g_cbwPacket[11] = (len >> 15) & 0xff;
    // read lba start address
    g_cbwPacket[17] = (uint8_t)(lba >> 24);
    g_cbwPacket[18] = (uint8_t)(lba >> 16);
    g_cbwPacket[19] = (uint8_t)(lba >> 8);
    g_cbwPacket[20] = (uint8_t)(lba >> 0);
    // read len
    g_cbwPacket[22] = (uint8_t)(len >> 8);
    g_cbwPacket[23] = (uint8_t)(len >> 0);

    /*host send out CBW*/
    USBHost_SendOutCBWPacket();
    g_flgHostTxRxDone = 0;

    if (g_usbVer == 0)
    {
        LbaLen <<= 3;
    }

    // rec data
    while (LbaLen > 0)
    {
        // Write Data to USB Device
        USBHost_WriteDataToFIFO(BufPtr, gTxMaxPacket);

        // set Mode to enable the ep direction as Tx
        gUSBC_IdxReg->TXCSR_H |= 0x20;
        // set TxPktRdy = 1
        gUSBC_IdxReg->TXCSR_L |= 0x01;
        /*set host module into BUSY mode*/
        g_flgHostTxRxDone = 0;
        USBHost_WaitTxRxDone(1);
        BufPtr += gTxMaxPacket;
        LbaLen--;
    }
    // rec CSW
    /*host request a data in*/
    {USBHost_RequestInTransactionWW_m} USBHost_WaitTxRxDone(1);
    USBHost_ReadCSWFromFIFO();
}

/**
 * @function     VendorReadData
 * @brief        获取数据
 * @param[in]    len    数据长度
 * @param[out]   tmpBuf 接收缓冲区
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void VendorReadData(uint8_t *tmpBuf, uint32_t len)
{
    USBHost_ReadDataFromFIFO(tmpBuf, len);
    // receive data from RX FIFO to buffer
    // DmaCtl = CNTL_DMAEN(1) | CNTL_DIRECTION(0) | CNTL_DMAMODE(0)
    //		| CNTL_INTERE(0) | CNTL_EP(USB_ENDPOINT_INDEX) | CNTL_BUSERR(0) | CNTL_BURSTMODE(USBDMA_BURST_LEN);
    // DMATransfer((uint32_t)tmpBuf, len, DmaCtl);
    // DMAWaitDone();
    // clear RxPktRdy bit
    ClearRX();
}

/**
 * @function     VendorReadBlock
 * @brief        获取块区数据
 * @param[in]    len    数据长短
 * @param[out]   tmpBuf 接收缓冲区
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void VendorReadBlock(uint8_t *tmpBuf, uint32_t len)
{
    uint8_t DmaCtl = 0;
    __IO uint8_t data8 = 0;

    // receive data from RX FIFO to buffer
    // Test
    // gUSBC_IdxReg->RXCSR_L |= HOST_RXCSR_FLUSH_FIFO;
    // Test
    DmaCtl = CNTL_DMAEN(1) | CNTL_DIRECTION(0) | CNTL_DMAMODE(1) | CNTL_INTERE(1) | CNTL_EP(USB_ENDPOINT_INDEX) | CNTL_BUSERR(0) | CNTL_BURSTMODE(USBDMA_BURST_LEN);
    DMATransfer((uint32_t)tmpBuf, len, DmaCtl);
    // clear RxPktRdy bit
    // ClearRX();
    // USBHost_RequestInTransactionWW_m
    DMAWaitDone();

    // Reset RxTx Setting
    // UsbSetTXsize(0,0);
    UsbSetRXsize(0, 0);
}

/**
 * @function     VendorWriteData
 * @brief        写数据
 * @param[in]    tmpBuf   发送缓冲区
 * @param[in]    len      数据长度
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void VendorWriteData(uint8_t *tmpBuf, uint32_t len)
{
    uint16_t DmaCtl = 0;

    // write data from buffer to TX
    UsbSetTXsize(0, 0);
    DmaCtl = CNTL_DMAEN(1) | CNTL_DIRECTION(1) | CNTL_DMAMODE(0) | CNTL_INTERE(1) | CNTL_EP(USB_ENDPOINT_INDEX) | CNTL_BUSERR(0) | CNTL_BURSTMODE(USBDMA_BURST_LEN);
    DMATransfer((uint32_t)tmpBuf, len, DmaCtl);
    DMAWaitDone();

    // Set TxPktRdy bit
    SetTX();
}

/**
 * @function     VendorWriteBlock
 * @brief        块区写
 * @param[in]    tmpBuf    发送缓冲区
 * @param[in]    len       数据长度
 * @retval
 * @attention
 * @author     AUTHOR
 */
void VendorWriteBlock(uint8_t *tmpBuf, uint32_t len)
{
    uint16_t DmaCtl = 0;

    // write data from buffer to TX
    DmaCtl = CNTL_DMAEN(1) | CNTL_DIRECTION(1) | CNTL_DMAMODE(1) | CNTL_INTERE(1) | CNTL_EP(USB_ENDPOINT_INDEX) | CNTL_BUSERR(0) | CNTL_BURSTMODE(USBDMA_BURST_LEN);

    DMATransfer((uint32_t)tmpBuf, len, DmaCtl);
    DMAWaitDone();
    // packet len is not multiply 512,last pack need manual Set TxPktRdy
    if (len & 0x1ff)
        SetTX();
    g_transLens = 0;
    DMAWaitDone_Tx();

    // Reset RxTx Setting
    // UsbSetRXsize(0,0);
    UsbSetTXsize(0, 0);
}

/**
 * @function     USBHost_Read10_MB
 * @brief        读取数据10,MB
 * @param[in]    lba    地址
 * @param[in]    len    长度
 * @param[out]   buf    接收缓冲区
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_Read10_MB(unsigned long lba, unsigned short len, uint8_t *buf)
{
    uint8_t *BufPtr = buf;

    g_transLens = len;
    gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;
    // SCSI read cmd 0x28
    memcp_f(g_cbwPacket, (uint8_t *)SCSICmd_Read28, 31);
    // CBWDataTransferLenth
    g_cbwPacket[8] = 0x00;
    g_cbwPacket[9] = (len << 1) & 0xff;
    g_cbwPacket[10] = (len >> 7) & 0xff;
    g_cbwPacket[11] = (len >> 15) & 0xff;
    // read lba start address
    g_cbwPacket[17] = (uint8_t)(lba >> 24);
    g_cbwPacket[18] = (uint8_t)(lba >> 16);
    g_cbwPacket[19] = (uint8_t)(lba >> 8);
    g_cbwPacket[20] = (uint8_t)(lba >> 0);
    // read len
    g_cbwPacket[22] = (uint8_t)(len >> 8);
    g_cbwPacket[23] = (uint8_t)(len >> 0);

    /*host send out CBW*/
    USBHost_SendOutCBWPacket();
    g_flgHostTxRxDone = 0;

    // rec data
    UsbSetRXsize(UsbSetDoubleFIFO(2, 1), 1);
    USBHost_RequestInTransactionWW_m
        VendorReadBlock(BufPtr, (len << 9));

    // rec CSW
    /*host request a data in*/
    // no need to request an IN transaction because DMA auto send an extra IN transaction.
    // USBHost_RequestInTransactionWW_m
    USBHost_WaitTxRxDone(1);
    USBHost_ReadCSWFromFIFO();
}

/**
 * @function     USBHost_Write10_MB
 * @brief        发送数据10,MB
 * @param[in]    lba    地址
 * @param[in]    len    长度
 * @param[in]    buf    接收缓冲区
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_Write10_MB(unsigned long lba, unsigned short len, uint8_t *buf)
{
    uint8_t *BufPtr = buf;

    g_transLens = len;
    // SCSI read cmd 0x28
    memcp_f(g_cbwPacket, (uint8_t *)SCSICmd_Write2a, 31);
    // CBWDataTransferLenth
    g_cbwPacket[8] = 0x00;
    g_cbwPacket[9] = (len << 1) & 0xff;
    g_cbwPacket[10] = (len >> 7) & 0xff;
    g_cbwPacket[11] = (len >> 15) & 0xff;
    // read lba start address
    g_cbwPacket[17] = (uint8_t)(lba >> 24);
    g_cbwPacket[18] = (uint8_t)(lba >> 16);
    g_cbwPacket[19] = (uint8_t)(lba >> 8);
    g_cbwPacket[20] = (uint8_t)(lba >> 0);
    // read len
    g_cbwPacket[22] = (uint8_t)(len >> 8);
    g_cbwPacket[23] = (uint8_t)(len >> 0);

    /*host send out CBW*/
    USBHost_SendOutCBWPacket();
    g_flgHostTxRxDone = 0;

    /*set host module into BUSY mode*/
    UsbSetTXsize(UsbSetDoubleFIFO(2, 0), 1);
    VendorWriteBlock(BufPtr, (len << 9));
    USBHost_WaitTxRxDone(1);

    // rec CSW
    /*host request a data in*/
    USBHost_RequestInTransactionWW_m
        USBHost_WaitTxRxDone(1);
    USBHost_ReadCSWFromFIFO();
}
