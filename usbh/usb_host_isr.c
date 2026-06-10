/**
 * @file usb_host_isr.c
 * @author Product application department
 * @brief usb host interrupt function
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
 * @function     USBHost_EP0IntrRoutine
 * @brief        端点0常规终端处理
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBHost_EP0IntrRoutine(void)
{
    __IO uint8_t ucReg = 0;

    /* enable ep0 register map to be accessed */
    gUSBC_ComReg->EINDEX = CONTROL_EP;

    ucReg = gUSBC_IdxReg->E0CSR_L;

    if (ucReg & HOST_CSR0_RXPKTRDY) // RxPktRdy
    {
        /* if more than one packet,should wait for all packets received. */
        g_flgHostTxRxDone = 0;

        /* host receive data sent from device */
        USBHost_Ep0RxDone();
    }
    else
    {
        /* transfer successfully(setup,out status phase) */
        g_flgHostTxRxDone = 1; // normal TxRxDone
    }
}

/*  All other Exception Process*/
/**
 * @function     ExceptionProc
 * @brief        All other Exception Process
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
// #pragma interrupt on
void ExceptionProc(void)
{
}
// #pragma interrupt off

/**
 * @function     USBDev_ISR
 * @brief        USB Host中断服务程序
 * @retval       NONE
 * @attention
 * @author     AUTHOR
 */
void USBDev_ISR(void)
{
    __IO uint8_t ucReg;
    g_DMA_INTR |= read_port_byte(DMA_INTR_r);

    ucReg = gUSBC_ComReg->INTRRX_L; // 获取接收中断标志位
    if (ucReg & (1 << USB_ENDPOINT_INDEX))
    {
        // select ep1 by set index register
        gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;

        // read RXCSR register
        ucReg = gUSBC_IdxReg->RXCSR_L;

        // whether set RxPktRdy
        if (ucReg & HOST_RXCSR_RXPKTRDY)
        {
            g_flgHostTxRxDone = 1; // normal TxRxDone
        }
        else
        {
            g_flgHostTxRxDone = 2; // abnormal TxRxDone

            // check RxStall bit
            if (ucReg & HOST_RXCSR_RXSTALL)
            {
                // clear RxStall bit
                ucReg &= 0xbf;
                // clear data toggle
                ucReg |= 0x80;
                gUSBC_IdxReg->RXCSR_L = ucReg;

                g_hostStatus = HOST_RECEIVE_STALL;
            }

            // check Error status
            if (ucReg & HOST_RXCSR_ERROR)
            {
                // clear Error bit
                ucReg &= 0xfb;
                gUSBC_IdxReg->RXCSR_L = ucReg;
                g_hostStatus = HOST_RECEIVE_ERROR;
            }

            // check NAK Timeout */
            if (ucReg & HOST_RXCSR_NAK_TIMEOUT)
            {
                // clear  NAK bit
                ucReg &= 0xdf;
                gUSBC_IdxReg->RXCSR_L = ucReg;

                ucReg &= 0xf7;
                gUSBC_IdxReg->RXCSR_L = ucReg;
                g_hostStatus = HOST_RECEIVE_NAK_TIME;
            }
        }
    }
    else
    {
        ucReg = gUSBC_ComReg->INTRTX_L; // 发送中断标志位寄存器
        if (ucReg & (1 << USB_ENDPOINT_INDEX))
        {
            // select out ep register map */
            gUSBC_ComReg->EINDEX = USB_ENDPOINT_INDEX;

            // read TXCSR register */
            ucReg = gUSBC_IdxReg->TXCSR_L;

            /* whether set TxPktRdy */
            if (!(ucReg & HOST_TXCSR_TXPKTRDY))
            {
                /* for write_2a operation, indicate that a data packet had been transmitted. */
                g_flgHostTxRxDone = 1; // normal TxRxDone
            }
            else
            {
                g_flgHostTxRxDone = 2; // abnormal TxRxDone
                // check RxStall bit
                if (ucReg & HOST_TXCSR_RXSTALL)
                {
                    // clear RxStall bit
                    ucReg &= 0xdf;
                    // clear data toggle
                    ucReg |= 0x40;

                    gUSBC_IdxReg->TXCSR_L = ucReg;
                    g_hostStatus = HOST_RECEIVE_STALL;
                }

                // check Error status
                if (ucReg & HOST_TXCSR_ERROR)
                {
                    // clear Error bit
                    ucReg &= 0xfb;
                    gUSBC_IdxReg->TXCSR_L = ucReg;
                    g_hostStatus = HOST_RECEIVE_ERROR;
                }

                // check NAK Timeout
                if (ucReg & HOST_TXCSR_NAK_TIMEOUT)
                {
                    ucReg &= 0x7f;
                    gUSBC_IdxReg->TXCSR_L = ucReg;
                    g_hostStatus = HOST_RECEIVE_NAK_TIME;
                }
            }
        }
        else
        {
            if (ucReg & USB_INTERRUPT_EP0)
            {
                // to do endpoint 0,ep0 transfer and receive
                //  are same interrupt bit in INTRTX register
                USBHost_EP0IntrRoutine();
            }
            else
            {
                ucReg = gUSBC_ComReg->INTRUSB;
                if (ucReg & USB_INTERRUPT_CONNECT)
                {
                    // a device insert into host
                    if (!(ucReg & USB_INTERRUPT_DISCON))
                        // set a flag to do device enumeration
                        g_flgHostDeviceInsert = 1;
                }
                if ((ucReg & USB_INTERRUPT_DISCON))
                {
                    // device unplug from host
                    // enable ep0 register map to be accessed
                    // W8(USBD_COMMON_INDEX, CONTROL_EP);

                    // clear soft conn
                    // W8(USBD_COMMON_UCSR,0 );

                    // device unplug flag
                    g_flgHostDeviceInsert = 0;
                }
            }
        }
    }
}
