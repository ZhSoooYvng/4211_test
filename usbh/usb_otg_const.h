/**
 * @file usb_otg_const.h
 * @author Product application department 
 * @brief usb otg endpoint macro header file
 * @version V1.0
 * @date 2025-04-03
 * 
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 * 
 */

#ifndef  _USBOTG_CONST_H
#define	 _USBOTG_CONST_H

#ifdef __cplusplus
extern "C"{
#endif

// 头文件包含
#include "usb_otg_reg.h"

// 全局变量定义
//endpoint define
#define CONTROL_EP 0
#define DATA_IN_EP 1
#define DATA_OUT_EP 1
#define INDEX_EP1 1
#define INDEX_EP2 2
#define INDEX_EP3 3
#define INDEX_EP4 4
#define INDEX_EP5 5
#define INDEX_EP6 6
#define INDEX_EP7 7

// bulk transfer packet size 
#define USB_MAX_PACKET_SIZE 512
#define USB_MAX_PACKET_SIZE_LOW 0x00
#define USB_MAX_PACKET_SIZE_HIGH 0x02
#define _USB_MAX_PACKET_SIZE_V11 64
#define USB_MAX_PACKET_SIZE_LOW_V11 0x40
#define USB_MAX_PACKET_SIZE_HIGH_V11 0x00

// Host CSR0 Bit Define 
#define HOST_CSR0_RXPKTRDY 0x01
#define HOST_CSR0_TXPKTRDY 0x02
#define HOST_CSR0_RXSTALL 0x04
#define HOST_CSR0_SETUP 0x08
#define HOST_CSR0_ERROR 0x10
#define HOST_CSR0_REQPKT 0x20
#define HOST_CSR0_STATUSPKT 0x40
#define HOST_CSR0_NAK_TIMEOUT 0x80

// TX Register Bit Low as Host 
#define HOST_TXCSR_TXPKTRDY 0x01
#define HOST_TXCSR_FIFO_NOT_EMPTY 0x02
#define HOST_TXCSR_ERROR 0x04
#define HOST_TXCSR_FLUSH_FIFO 0x08
#define HOST_TXCSR_RXSTALL 0x20
#define HOST_TXCSR_CLR_DATA_TOG 0x40
#define HOST_TXCSR_NAK_TIMEOUT 0x80

// TX Register Bit High as Host 
#define HOST_TXCSR_FRC_DATA_TOG 0x08
#define HOST_TXCSR_DMA_ENAB 0x10
#define HOST_TXCSR_TXMODE 0x20
#define HOST_TXCSR_ISO 0x40
#define HOST_TXCSR_AUTOSET 0x80

// RX Register Bit Low as Host 
#define HOST_RXCSR_RXPKTRDY 0x01
#define HOST_RXCSR_FIFOFULL 0x02
#define HOST_RXCSR_ERROR 0x04
#define HOST_RXCSR_DATA_ERROR 0x08
#define HOST_RXCSR_NAK_TIMEOUT 0x08
#define HOST_RXCSR_FLUSH_FIFO 0x10
#define HOST_RXCSR_REQPKT 0x20
#define HOST_RXCSR_RXSTALL 0x40
#define HOST_RXCSR_CLR_DATA_TOG 0x80

// RX Register Bit High as Host 
#define HOST_RXCSR_INCOMP_RX 0x01
#define HOST_RXCSR_DMAMODE 0x08
#define HOST_RXCSR_DISNYET 0x10
#define HOST_RXCSR_DMA_ENAB 0x20
#define HOST_RXCSR_AUTOREQ 0x40
#define HOST_RXCSR_AUTOCLEAR 0x80

// Transmit direct	
#define TX 0x01
#define RX 0x00

// DEVCTL
#define OTG_SESSION 0x01
#define OTG_HOST_REQ 0x02
#define OTG_HOST_MODE 0x04
#define OTG_LSDEV 0x20
#define OTG_FSDEV 0x40
#define OTG_B_DEVICE 0x80

// assign address as 3 to mass storage device, hub not support 
#define USB_MASS_STORAGE_DEV_ADDR 0x03

#ifndef READ
#define READ 0
#endif

#ifndef WRITE
#define WRITE 1
#endif

#ifdef __cplusplus
}
#endif

#endif /* _USBOTG_CONST_H */
