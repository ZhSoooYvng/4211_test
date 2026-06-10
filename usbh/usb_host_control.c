/**
 * @file usb_host_control.c
 * @author Product application department
 * @brief usb host control layer
 * @version V1.0
 * @date 2025-04-03
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 *
 */

// 头文件包含
#include "usb_host_setup.h"
#include "interrupt.h"

// 全局变量定义
/* Global USB Register */
sUSBCFIFO_Reg *gUSBC_fifoReg;
sUSBCCommonReg *gUSBC_ComReg;
sUSBCIndexedReg *gUSBC_IdxReg;

/* current selected LUN,private data partition or security partition
   5..4--USB version,0--USB1.1,others--USB2.0
   3..2--max LUNs,
   1..0--reserved
 */
uint8_t g_currentLUN;
// USB version(1--USB2.0;0--USB1.1)
uint8_t g_usbVer;

// usb the max packet size
unsigned short gTxMaxPacket = 0;
unsigned short gRxMaxPacket = 0;

uint8_t g_dev_std_req[8];

// this data buffer is used by host control tranfer to hold data request by host
uint8_t g_hostdatabuf[64];

// data count received by host ep0
uint8_t g_hostEp0RxCount;

// indicate each status during host communicate with device
uint8_t g_hostStatus;

// indicate whether Host TX or Rx is done
// 0--not done,1--done and correctly,others--done and with error
__IO uint8_t g_flgHostTxRxDone;

// indicate whether Device TX or Rx is done
// 0--not done,1--done
__IO uint8_t g_flgDeviceTxRxDone;

// indicate whether Security is done
// 0--not done,1--done
__IO uint8_t g_flgSecurityDone;

// indicate if a USB device insert into host,1--yes, 0--no
__IO uint8_t g_flgHostDeviceInsert;

// CBW packet
uint32_t g_cbwPacket_l[8];
uint8_t *g_cbwPacket = (uint8_t *)g_cbwPacket_l;

// CSW packet
uint32_t g_cswPacket_l[4];
uint8_t *g_cswPacket = (uint8_t *)g_cswPacket_l;

// USB data buffer1
uint32_t g_databuf[256];

// USB data buffer
// indicate the start sector in reading device
uint32_t g_wrLBA;

// Save USB Address
__IO uint8_t g_USBAddrFlag;

__IO uint8_t g_USBNewAddr;
// indicate which command is processed currently
__IO uint8_t g_bulkPhase;

// indicate which sub command is processed currently
uint8_t g_subCmd;

// indicate the length in Once tranfer from PC to device
uint16_t g_transLens;

// indicate the transfer stage of USB1.1
uint8_t g_TxRxCounter;

// indicate the mode of reading device
uint8_t g_readMode;

// global message queue for USB loop tasks.
__IO uint8_t g_msgflags;

uint8_t g_msgflags1;

// Suspend Mode Enable Flag(0-disable;1-enable)
uint8_t g_suspendMode;
__IO uint8_t g_DMA_INTR;

uint8_t VendorReadFlag = 0; // 读控制变量
uint32_t RWDataAddr = 0;
uint32_t RWDataLength = 0;
uint32_t InitData = 0;

// 函数定义
/**
 * @function   memcp_f
 * @brief      拷贝
 * @param[in]  dest  目标地址
 * @param[in]  sour  源地址
 * @param[in]  len   数据长度
 * @retval     NONE
 * @attention
 * @author     AUTHOR
 */
void memcp_f(uint8_t *dest, uint8_t *sour, uint16_t len)
{
    uint16_t i = 0;

    for (i = 0; i < len; i++)
        *(dest + i) = *(sour + i);
}

/**
 * @function   memset_f
 * @brief      fill a block of memory will a byte
 * @param[in]  addr 目标地址
 * @param[in]  value  填充的值
 * @param[in]  len    空间大小
 * @retval     NONE
 * @attention
 * @author     AUTHOR
 */
void memset_f(uint8_t *addr, uint8_t value, uint16_t len)
{
    uint16_t i = 0;

    for (i = 0; i < len; i++)
        addr[i] = value;
}

/**
 * @function   SetTX
 * @brief      设置发送模式
 * @retval     NONE
 * @attention
 * @author     AUTHOR
 */
void SetTX(void)
{
    uint8_t data8 = 0;

    data8 = gUSBC_IdxReg->TXCSR_L;
    data8 |= HOST_TXCSR_TXPKTRDY;
    gUSBC_IdxReg->TXCSR_L = data8;
}

/**
 * @function   ClearRX
 * @brief      清除发送状态
 * @retval     NONE
 * @attention
 * @author     AUTHOR
 */
void ClearRX(void)
{
    uint8_t data8 = 0;

    data8 = gUSBC_IdxReg->RXCSR_L;
    data8 &= ~HOST_RXCSR_RXPKTRDY;
    gUSBC_IdxReg->RXCSR_L = data8;
}

/**
 * @function    UsbSetTXsize
 * @brief       设置发送长度
 * @param[in]   size   长度
 * @param[in]   mode   模式 0：double buffer mode  1：single buffer mode
 * @retval      NONE
 * @attention
 * @author     AUTHOR
 */
void UsbSetTXsize(uint16_t size, uint8_t mode)
{
    uint8_t data8;

    if (size)
    {
        //		STH(size,TXMAXP);
        gUSBC_IdxReg->TXMAXP_L = size;
        gUSBC_IdxReg->TXMAXP_H = (size >> 8);
    }
    else
    {
        if (g_usbVer == 1)
        {
            gUSBC_IdxReg->TXMAXP_L = USB_MAX_PACKET_SIZE_LOW;
            gUSBC_IdxReg->TXMAXP_H = USB_MAX_PACKET_SIZE_HIGH;
        }
        else
        {
            gUSBC_IdxReg->TXMAXP_L = USB_MAX_PACKET_SIZE_LOW_V11;
            gUSBC_IdxReg->TXMAXP_H = USB_MAX_PACKET_SIZE_HIGH_V11;
        }
    }

    if (mode == 1) // double buffer mode
    {
        data8 = gUSBC_fifoReg->TXFIFOSZ;
        data8 |= 0x10; // enable double buffer
        gUSBC_fifoReg->TXFIFOSZ = data8;
        // Enable DMA mode 1 and Double Buffer Related AutoSet function
        gUSBC_IdxReg->TXCSR_H = HOST_TXCSR_AUTOSET | HOST_TXCSR_TXMODE | HOST_TXCSR_DMA_ENAB;
        // write_port_byte(TXCSR_H,TX_AutoSet|TX_Mode|TX_DMAReqEnab|TX_DMAReqMode);
    }
    else
    {
        data8 = gUSBC_fifoReg->TXFIFOSZ;
        data8 &= 0xef; // disable double buffer
        gUSBC_fifoReg->TXFIFOSZ = data8;
        gUSBC_IdxReg->TXCSR_H = HOST_TXCSR_TXMODE;
    }
}

/**
 * @function    UsbSetRXsize
 * @brief       设置接收长度
 * @param[in]   size   数据长度
 * @param[in]   mode   模式 0：double buffer mode  1：single buffer mode
 * @retval      NONE
 * @attention
 * @author     AUTHOR
 */
void UsbSetRXsize(uint16_t size, uint8_t mode)
{
    uint8_t data8;

    if (size)
    {
        //STH(size,RXMAXP);
        gUSBC_IdxReg->RXMAXP_L = size;
        gUSBC_IdxReg->RXMAXP_H = (size >> 8);
    }
    else
    {
        if (g_usbVer == 1)
        {
            gRxMaxPacket = USB_MAX_PACKET_SIZE;
            gTxMaxPacket = USB_MAX_PACKET_SIZE;
            gUSBC_IdxReg->RXMAXP_L = USB_MAX_PACKET_SIZE_LOW;
            gUSBC_IdxReg->RXMAXP_H = USB_MAX_PACKET_SIZE_HIGH;
        }
        else
        {
            gUSBC_IdxReg->RXMAXP_L = USB_MAX_PACKET_SIZE_LOW_V11;
            gUSBC_IdxReg->RXMAXP_H = USB_MAX_PACKET_SIZE_HIGH_V11;
        }
    }

    if (mode == 1) // double buffer mode
    {
        data8 = gUSBC_fifoReg->RXFIFOSZ;
        data8 |= 0x10; // enable double buffer
        gUSBC_fifoReg->RXFIFOSZ = data8;
        // Enable DMA mode 1 and Double Buffer Related AutoSet function
        // write_port_byte(RXCSR_H,RX_AutoClear|RX_DMAReqEnab);
        gUSBC_IdxReg->RXCSR_H = HOST_RXCSR_AUTOCLEAR | HOST_RXCSR_AUTOREQ | HOST_RXCSR_DMA_ENAB | HOST_RXCSR_DMAMODE;
    }
    else
    {
        data8 = gUSBC_fifoReg->RXFIFOSZ;
        data8 &= 0xef; // disable double buffer
        gUSBC_fifoReg->RXFIFOSZ = data8;
        gUSBC_IdxReg->RXCSR_H = 0;
    }
}

/**
 * @function    UsbSetMaxPacketSize
 * @brief       设置包最大长度
 * @param[in]   size   长度
 * @param[in]   TxRx   发送接收选择 0：接收  1：发送
 * @retval     实际设置的长度
 * @attention  for Read10 and so on
 * @author     AUTHOR
 */
uint16_t UsbSetMaxPacketSize(uint8_t size, uint8_t TxRx)
{
    uint16_t tmp;

    if (g_transLens < size)
    {
        if (TxRx == 0)
            gTxMaxPacket = 512 * g_transLens;
        else
            gRxMaxPacket = 512 * g_transLens;
        tmp = ((g_transLens - 1) << 11) | 0x200; // TxMAXP or RxMAXP
    }
    else
    {
        if (TxRx == 0)
            gTxMaxPacket = 512 * size;
        else
            gRxMaxPacket = 512 * size;
        tmp = ((size - 1) << 11) | 0x200;
    }

    return tmp;
}

/**
 * @function    UsbSetDoubleFIFO
 * @brief       设置DoubleFIFO
 * @param[in]   size   长度
 * @param[out]  TxRx   发送接收选择 0：发送  1：接收
 * @retval      实际设置长度
 * @attention   for Vendor Read and Vendor Write
 * @author     AUTHOR
 */
uint16_t UsbSetDoubleFIFO(uint8_t size, uint8_t TxRx)
{
    uint16_t tmp;

    if (g_transLens < size)
    {
        if (TxRx == 0)
            gTxMaxPacket = 512 * g_transLens;
        else
            gRxMaxPacket = 512 * g_transLens;
        tmp = ((g_transLens / 2 - 1) << 11) | 0x200;
    }
    else
    {
        if (TxRx == 0)
            gTxMaxPacket = 512 * size;
        else
            gRxMaxPacket = 512 * size;
        tmp = ((size / 2 - 1) << 11) | 0x200;
    }

    return tmp;
}

/**
 * @function   usbHostModeEnable
 * @brief      usb host mode enable
 * @retval     NONE
 */
void usbHostModeEnable(void)
{
    // when bit 7 set 1 means OTG_ID pull up to usb host mode
    (*(volatile unsigned char *)(0x40000040)) = 0x80;
    delayms(1);
}

/**
 * @brief usb host phy初始化
 * @param[in]   src_type   时钟源选择
 * @retval NONE
 */
void usbHostPhyInit(UsbHostPhySrcTypeDef src_type)
{
    // OTG_ID pin set 1 to usb host mode
    usbHostModeEnable();

    if (src_type == USBHOSTPHY_EXTER_OSC)
    {
        CPM_CHIPCFGR = (CPM_CHIPCFGR & 0x3FFFFFFF) | ((src_type & 0x03) << 30);

        // Config 12MHz Clk
        CPM_OSCE_STABLE = 0x3000;
        CPM_OCSR |= OSCEXT_EN; // Enable 12MHz Clock
    }

    CCM_PHYPA |= 0x0e00; // no need to supply VBUS.
                         // Enable PHY Power Switch
    _bit_set(CPM_CHIPCFGR, 1 << PWRCR_PHY_PSWEN_BIT); // enable usbphy power switch

    // delay at least 10us, sys clk is 40MHz, in release obj, one clock_cycle is 6 clock
    // so (40 * 10 / 6) is about 10us, we use 15us here.
    delay(10000);
    _bit_clr(CPM_CHIPCFGR, 1 << PWRCR_PHY_I_ISOEN_BIT);
    _bit_clr(CPM_CHIPCFGR, 1 << PWRCR_PHY_O_ISOEN_BIT); // disable usbphy isolation
    _bit_clr(CPM_CHIPCFGR, 1 << USBPHY_CFG_SRM);
    delay(10000);
    _bit_clr(CPM_CHIPCFGR, 1 << USBPHY_PLL_SRM);
    _bit_clr(CPM_CHIPCFGR, 1 << PWRCR_PHY_RSTMASK_BIT); // diable usbphy reset mask, release the reset signal
}

/**
 * @function    USBHost_Init
 * @brief       USB Host初始化
 * @param[in]   husb   USBC_Host_HandleTypeDef结构体指针
 * @retval      NONE
 * @attention  若USB晶振没有起振，程序会卡死在程序内部
 * @author     AUTHOR
 */
void USBHost_Init(USBC_Host_HandleTypeDef *husb)
{
#if 1
    INTX_DISABLE();

    if (husb->version == USB_VERSION_20)
    {
        g_usbVer = 1;
    }
    else
    {
        g_usbVer = 0;
    }
    drv_cpm_usbphydeinit();
    // 使能USB PHY模块
    usbHostPhyInit(husb->osc);

    // g_Usb_CallbackIT = husb->CallBackIT;

    INTX_ENABLE();

    /* Global USB Register */
    gUSBC_fifoReg = (sUSBCFIFO_Reg *)(USBC_BASE_ADDR + 0x60);
    gUSBC_ComReg = (sUSBCCommonReg *)USBC_BASE_ADDR;
    gUSBC_IdxReg = (sUSBCIndexedReg *)(USBC_BASE_ADDR + 0x10);
#endif

    if (g_usbVer == 1)
        gUSBC_ComReg->UCSR = (unsigned char)USB_POWER_HS_ENAB; // USB2.0
    else
        gUSBC_ComReg->UCSR = USB_POWER_SOFT_CONN; // USB1.1
    g_cbwPacket = (unsigned char *)g_cbwPacket_l;
    g_cswPacket = (unsigned char *)g_cswPacket_l;

    // CBW signature
    g_cbwPacket_l[0] = 0x55534243;

    // dCSWSignature
    g_cswPacket_l[0] = 0x55534253;
    g_cswPacket_l[2] = 0x0;
    g_cswPacket_l[3] = 0x0;

    g_msgflags = 0;

    // g_configWakeup=0;
    g_bulkPhase = 0xFF;
    // g_currentLUN = 0x10;

    gTxMaxPacket = USB_MAX_PACKET_SIZE; // USB 2.0 max packet size
    gRxMaxPacket = USB_MAX_PACKET_SIZE; // USB 2.0 max packet size

    // g_wrLength=0;
    g_wrLBA = 0;

    // g_inSuspendMode=0;//not in suspend state
    // g_readCapacityFlag=0;//capapcity should be read from device

    g_hostEp0RxCount = 0;
    g_hostStatus = 0;

    // set host/device/security module are in idle status
    g_flgHostTxRxDone = 1;

    // g_suspendSupport=0;

    // g_supportCPUWait=0;

    // select register index
    gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;

    // set host fifo at offset 0
    gUSBC_fifoReg->TX_fifoadd_L = ((USB_FIFO_OFFSET >> 3) & 0x00FF);
    gUSBC_fifoReg->TX_fifoadd_H = ((USB_FIFO_OFFSET >> 11) & 0x00FF);
    gUSBC_fifoReg->RX_fifoadd_L = ((USB_FIFO_OFFSET >> 3) & 0x00FF);
    gUSBC_fifoReg->RX_fifoadd_H = ((USB_FIFO_OFFSET >> 11) & 0x00FF);

#ifdef DMA_TRANS_ENABLE
    sUSBCIndexedReg->TXCSR_H = HOST_TXCSR_DMA_ENAB;
    sUSBCIndexedReg->RXCSR_H = HOST_RXCSR_DMA_ENAB;
#endif

    // select register index
    gUSBC_ComReg->EINDEX = CONTROL_EP; // select control endpoint

    // Init OTG Host mode Register
    gUSBC_ComReg->INTRUSBE = USB_INTERRUPT_DISCON | USB_INTERRUPT_RESET; // config interrupt  USB_INTERRUPT_CONNECT

    /* enable ep0 and ep1 tx interrupts */
    gUSBC_ComReg->INTRTXE_L = USB_INTERRUPT_EP0 | USB_TX_INTERRUPT_EP1; // config transimit interrupt

    /* clear other tx interrupts */
    gUSBC_ComReg->INTRTXE_H = 0;

    /* enable ep1 rx interrupt */
    gUSBC_ComReg->INTRRXE_L = USB_RX_INTERRUPT_EP1; // config receive interrupt

    /* clear other rx interrupts */
    gUSBC_ComReg->INTRRXE_H = 0;

    // gUSBC_fifoReg->OTGCTRL |= OTG_SESSION;		//set session=1 to enter A device mode

    /*set USB FADDR*/
    gUSBC_ComReg->FADDRR = 0;

    /*set USB configDATA*/ // 140723_configdata is read-only register
    // BMFDCR_B(EP_CONFIGDATA,data8);
    // data8 |= 0x20;		//usb transfer in Big-endian(little-endian is default)
    // BMTDCR_B(EP_CONFIGDATA,data8);

    // NO device insert into host
    g_flgHostDeviceInsert = 0;

    gUSBC_ComReg->UCSR |= USB_POWER_ENAB_SUSP; // enable suspend mode
}

/**
 * @brief 等待Device插入
 * @para[in] delay_value:等待值，0：死等待
 * @retval 0：设备插入   1：等待超时
 */
uint8_t waitDeviceConnect(uint32_t delay_value)
{
    uint8_t reg;
    uint8_t tmp;

    gUSBC_fifoReg->OTGCTRL |= OTG_SESSION; // start session

    do
    {
        reg = gUSBC_ComReg->INTRUSB;
        tmp = gUSBC_fifoReg->OTGCTRL;
        if ((tmp & OTG_SESSION) == 0) // if Vbus lower than 4.4v,session will be clear
        {
            gUSBC_fifoReg->OTGCTRL |= OTG_SESSION; // start session
        }
        if (delay_value > 1)
        {
            delay_value--;
        }
    } while (((reg & USB_INTERRUPT_CONNECT) == 0) && (delay_value != 1));

    if (delay_value == 1)
    {
        return 1;
    }
    else
    {

        USBHost_SetDeviceInsert();
        return 0;
    }
}

/**
 * @function    USB_ReadFIFO
 * @brief       USB Read data from Endpoint FIFO
 * @param[in]   ep_fifo   端点fifo地址
 * @param[out]  pbuf
 * @retval      NONE
 * @attention
 * @author     AUTHOR
 */
void USB_ReadFIFO(unsigned long ep_fifo, uint8_t *pbuf, unsigned short len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        pbuf[i] = read_port_byte(ep_fifo);
    }
}

/**
 * @function    USB_WriteFIFO
 * @brief       写数据到FIFO
 * @param[in]   ep_fifo  端点FIFO地址
 * @param[in]   pbuf     发送数据地址
 * @param[in]   len      发送数据长度
 * @retval      NONE
 * @attention
 * @author     AUTHOR
 */
void USB_WriteFIFO(unsigned long ep_fifo, uint8_t *pbuf, unsigned short len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        write_port_byte(ep_fifo, pbuf[i]);
    }
}

/**
 * @function    DMAWaitDone
 * @brief       等待DMA搬运结束
 * @retval      NONE
 * @attention
 * @author     AUTHOR
 */
void DMAWaitDone(void)
{
    while ((g_DMA_INTR & 0x01) != 0x01)
        ;
    g_DMA_INTR = 0x00;
}

/**
 * @function    DMAWaitDone_Tx
 * @brief       等待发送结束
 * @retval      NONE
 * @attention
 * @author     AUTHOR
 */
void DMAWaitDone_Tx(void)
{
    __IO uint16_t txcsr;
    __IO uint32_t count = 0;

    // Wait TxpktRdy is Clear
    do
    {
        txcsr = gUSBC_IdxReg->TXCSR_L;
        count++;
        if (count > 0x40000)
        {
            //__asm("bkpt 0");

            while (1)
                ;
        }
    } while ((txcsr & HOST_TXCSR_FIFO_NOT_EMPTY) == HOST_TXCSR_FIFO_NOT_EMPTY);
}

/**
 * @function    DMATransfer
 * @brief       DMA传输
 * @param[in]   memaddr   数据地址
 * @param[in]   len       数据长度
 * @param[in]   dcr       控制模式
 * @retval      NONE
 * @attention
 * @author     AUTHOR
 */
void DMATransfer(uint32_t memaddr, uint32_t len, uint16_t dcr)
{
    g_DMA_INTR = read_port_byte(DMA_INTR_r);
    g_DMA_INTR = 0x00;

    write_port_word(DMA_ADDR1, memaddr);

    write_port_word(DMA_COUNT1, len);

    write_port_halfword(DMA_CNTL1, dcr);
}

/**
 * @function    USBHost_SetDeviceInsert
 * @brief       设备接入
 * @retval      NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_SetDeviceInsert(void)
{
    g_flgHostDeviceInsert = 1;
}

/**
 * @function    USBHost_SetDeviceDisconnect
 * @brief       设备断开
 * @retval      NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_SetDeviceDisconnect(void)
{
    g_flgHostDeviceInsert = 0;
}

/**
 * @function    USBHost_GetHostDeviceInsertFlag
 * @brief       获取设备接入状态
 * @retval      NONE
 * @attention
 * @author     AUTHOR
 */
uint8_t USBHost_GetHostDeviceInsertFlag(void)
{
    return g_flgHostDeviceInsert;
}
