#ifndef _USB_HOST_INCLUDES_H
#define _USB_HOST_INCLUDES_H

#include "type.h"
#include "string.h"

//#include "memorymap.h"
#include "cpm_drv.h"
#include "usb_host_cpm.h"
#include "usb_host_iomacros.h"
#include "usb_otg_const.h"
#include "usb_otg_reg.h"
#include "usb_drv.h"

/** DEF */
typedef void(*callback_t)(void *);

extern callback_t g_Usb_CallbackIT;

#define USBHost_Write2a_Data_m(packetLen)	USBHost_SendOutPacketWW(packetLen)
#define USBHost_ClearRx_m	W8( USBH_RX_CSR_LOW,0)
#define USBHost_RequestInTransactionWW_m \
		g_flgHostTxRxDone=0;	\
		gUSBC_IdxReg->RXCSR_L = 0x20;	\
		
#define CCM_PHYPA				(*(volatile UINT16 *)(CCM_BASE_ADDR + 0x0006))
/** ENUM */

typedef enum{
    USBHOSTPHY_AUTO_OSC  = 0,
    USBHOSTPHY_INTER_OSC = 0x2,
	USBHOSTPHY_EXTER_OSC = 0x3
}UsbHostPhySrcTypeDef;

//typedef enum
//{
//	USB_VERSION_11 = 0,
//	USB_VERSION_20,
//}USBCVersion_Enum;

/** STRUCT */
typedef struct {
	UsbHostPhySrcTypeDef       osc;          
	usbcversion_t           version;      
  callback_t                 CallBackIT;   
}USBC_Host_HandleTypeDef;

/** usb host scsi ***********************************/
void USBHost_Inquiry12(unsigned char arrInquiry[]);
void USBHost_ReadCapacity25(unsigned char arrCapacity[]);
void USBHost_ReadCapacity23(unsigned char arrCapacity[]);
void USBHost_Write10(unsigned long lba, unsigned short len,unsigned char *buf);
void USBHost_Read10(unsigned long lba, unsigned short len,unsigned char *buf);
void USBHost_Read10_MB(unsigned long lba, unsigned short len,unsigned char *buf);
void USBHost_Write10_MB(unsigned long lba, unsigned short len,unsigned char *buf);

/** usb host control ***********************************/
void usbHostPhyInit(UsbHostPhySrcTypeDef src_type);
void USBHost_Init(USBC_Host_HandleTypeDef *husb);
void DMAWaitDone(void);
unsigned char USBHost_WaitTxRxDone(unsigned char flag);
uint8_t waitDeviceConnect(uint32_t delay_value);
void DMAWaitDone_Tx(void);
void USBHost_SetDeviceInsert(void);
void USBHost_SetDeviceDisconnect(void);
uint8_t USBHost_GetHostDeviceInsertFlag(void);

/** usb host setup */
uint8_t USBHost_ClearFeature(uint8_t packetSize);
void USBHost_GetStatus(void);
uint8_t USBHost_SetConfiguration(uint8_t packetSize);
uint8_t USBHost_GetDeviceDescriptor(uint8_t OneOrAll, uint8_t arrDeviceDesc[]);
uint8_t USBHost_GetConfigDescriptor(uint8_t OneOrAll, uint8_t arrConfigDesc[], uint8_t packetSize);
uint8_t USBHost_GetStringDescriptor(uint8_t index, uint8_t arrStringDesc[], uint8_t packetSize);
uint8_t USBHost_SetInterface(uint8_t packetSize);
uint8_t USBHost_SetAddress(uint8_t address, uint8_t packetSize);
uint8_t USBHost_GetMaxLUN(uint8_t packetSize,uint8_t *lun);
uint8_t USBHost_ControlTransfer(uint8_t* setupPacket, uint8_t* arrData, uint8_t packetSize);
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

#endif
