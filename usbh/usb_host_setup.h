/**
 * @file usbhost_setup.h
 * @author Product application department
 * @brief usb host setup function
 * @version V1.0
 * @date 2025-04-03
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 *
 */

#ifndef _USB_HOST_SETUP_H
#define _USB_HOST_SETUP_H

#ifdef __cplusplus
extern "C"
{
#endif

// 头文件包含
#include "usb_host_iomacros.h"
#include "usb_otg_reg.h"
#include "usb_otg_const.h"
#include "usb_drv.h"

// 全局变量定义
// current endpoint MACRO
#ifdef USB_EP1_TEST
#define USB_ENDPOINT_FIFO_ADDR USB_FIFO_ENDPOINT_1
#define USB_ENDPOINT_INDEX INDEX_EP1
#define USB_ENDPOINT_PTR0 USB_FIFO_EP1TX_PTR0
#define USB_ENDPOINT_PTR1 USB_FIFO_EP1TX_PTR1
#endif
#ifdef USB_EP2_TEST
#define USB_ENDPOINT_FIFO_ADDR USB_FIFO_ENDPOINT_2
#define USB_ENDPOINT_INDEX INDEX_EP2
#endif
#ifdef USB_EP3_TEST
#define USB_ENDPOINT_FIFO_ADDR USB_FIFO_ENDPOINT_3
#define USB_ENDPOINT_INDEX INDEX_EP3
#endif
#ifdef USB_EP4_TEST
#define USB_ENDPOINT_FIFO_ADDR USB_FIFO_ENDPOINT_4
#define USB_ENDPOINT_INDEX INDEX_EP4
#endif
#ifdef USB_EP5_TEST
#define USB_ENDPOINT_FIFO_ADDR USB_FIFO_ENDPOINT_5
#define USB_ENDPOINT_INDEX INDEX_EP5
#endif
#ifdef USB_EP6_TEST
#define USB_ENDPOINT_FIFO_ADDR USB_FIFO_ENDPOINT_6
#define USB_ENDPOINT_INDEX INDEX_EP6
#endif
#ifdef USB_EP7_TEST
#define USB_ENDPOINT_FIFO_ADDR USB_FIFO_ENDPOINT_7
#define USB_ENDPOINT_INDEX INDEX_EP7
#endif


typedef void (*callback_t)(void *);
extern sUSBCIndexedReg *gUSBC_IdxReg;
extern callback_t g_Usb_CallbackIT;

#define USBHost_Write2a_Data_m(packetLen) USBHost_SendOutPacketWW(packetLen)
#define USBHost_ClearRx_m W8(USBH_RX_CSR_LOW, 0)
#define USBHost_RequestInTransactionWW_m \
g_flgHostTxRxDone = 0;               \
gUSBC_IdxReg->RXCSR_L = 0x20;

#define CCM_PHYPA (*(volatile uint16_t *)(CCM_BASE_ADDR + 0x0006))

typedef enum
{
    USBHOSTPHY_AUTO_OSC = 0,
    USBHOSTPHY_INTER_OSC = 0x2,
    USBHOSTPHY_EXTER_OSC = 0x3
} UsbHostPhySrcTypeDef;

typedef struct
{
    UsbHostPhySrcTypeDef osc;
    usbcversion_t version;
    callback_t CallBackIT;
} USBC_Host_HandleTypeDef;

    // usb host scsi
void USBHost_Inquiry12(uint8_t arrInquiry[]);
void USBHost_ReadCapacity25(uint8_t arrCapacity[]);
void USBHost_ReadCapacity23(uint8_t arrCapacity[]);
void USBHost_ReadCapacity10(uint8_t arrCapacity[]);
void USBHost_TestUnitReady(void);
void USBHost_Write10(unsigned long lba, unsigned short len, uint8_t *buf);
void USBHost_Read10(unsigned long lba, unsigned short len, uint8_t *buf);
void USBHost_Read10_MB(unsigned long lba, unsigned short len, uint8_t *buf);
void USBHost_Write10_MB(unsigned long lba, unsigned short len, uint8_t *buf);
// usb host control
void usbHostPhyInit(UsbHostPhySrcTypeDef src_type);
void USBHost_Init(USBC_Host_HandleTypeDef *husb);
void DMAWaitDone(void);
uint8_t USBHost_WaitTxRxDone(uint8_t flag);
uint8_t waitDeviceConnect(uint32_t delay_value);
void DMAWaitDone_Tx(void);
void USBHost_SetDeviceInsert(void);
void USBHost_SetDeviceDisconnect(void);
uint8_t USBHost_GetHostDeviceInsertFlag(void);
// usb host setup
uint8_t USBHost_ClearFeature(uint8_t packetSize);
void USBHost_GetStatus(void);
uint8_t USBHost_SetConfiguration(uint8_t packetSize);
uint8_t USBHost_GetDeviceDescriptor(uint8_t OneOrAll, uint8_t arrDeviceDesc[]);
uint8_t USBHost_GetConfigDescriptor(uint8_t OneOrAll, uint8_t arrConfigDesc[], uint8_t packetSize);
uint8_t USBHost_GetStringDescriptor(uint8_t index, uint8_t arrStringDesc[], uint8_t packetSize);
uint8_t USBHost_SetInterface(uint8_t packetSize);
uint8_t USBHost_SetAddress(uint8_t address, uint8_t packetSize);
uint8_t USBHost_GetMaxLUN(uint8_t packetSize, uint8_t *lun);
uint8_t USBHost_ControlTransfer(uint8_t *setupPacket, uint8_t *arrData, uint8_t packetSize);
uint8_t USBHost_SendSetupPacket(uint8_t ucSetupPack[]);
uint8_t USBHost_TransferSetupPacket(uint8_t ucSetupPack[]);
void USBHost_ClearERNBit(void);
uint8_t USBHost_DataInPhase(void);
uint8_t USBHost_StatusInPhase(void);
uint8_t USBHost_StatusOutPhase(void);
uint8_t USBHost_InitUsbDevInsert(void);
void USBHost_BusReset(void);
void USBHost_ConfigEpType(uint8_t pHost_Config_Descriptor[]);
void USBHost_Ep0RxDone(void);
void USBDev_ISR(void);
void DMATransfer(uint32_t memaddr, uint32_t len, uint16_t dcr);
uint8_t USBHost_InitUsbDevInsert(void);

#ifdef __cplusplus
}
#endif

#endif /* _USB_HOST_SETUP_H */
