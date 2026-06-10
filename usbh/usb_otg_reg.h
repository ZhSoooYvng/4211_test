/**
 * @file usb_otg_reg.h
 * @author Product application department 
 * @brief usb otg register header file
 * @version V1.0
 * @date 2025-04-03
 * 
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 * 
 */

#ifndef _USBOTG_REG_H
#define _USBOTG_REG_H

#ifdef __cplusplus
extern "C"{
#endif

// 头文件包含
#include "usb_host_cpm.h"

// 全局变量定义
 #define USB_EP1_TEST
//#define USB_EP2_TEST
//#define USB_EP3_TEST
//#define USB_EP4_TEST
//#define USB_EP5_TEST
//#define USB_EP6_TEST
//#define USB_EP7_TEST
//#define USB_FIFO_OFFSET 0x7000
#define USB_FIFO_OFFSET 0

//DMA related
#define USBDMA_BURST_LEN 0
#define USB_DATABUF_ADDR 0x00810000
#define USB_DATABUF_ADDR1 0x00820000

//Define the USB buffer start address
#define USB_BUFFER_ADDR (EVB_SRAM_ADDR+USB_FIFO_OFFSET)

//Define the USB buffer size(unit:byte)used in USB Demo
//#define USB_BUFFER_SIZE_H 0x40
//#define USB_BUFFER_SIZE_L 0x00

//usb FIFO register
//Word Access
#define USB_FIFO_ENDPOINT_0 (USBC_BASE_ADDR + 0x20)
#define USB_FIFO_ENDPOINT_1 (USBC_BASE_ADDR + 0x24)
#define USB_FIFO_ENDPOINT_2 (USBC_BASE_ADDR + 0x28)
#define USB_FIFO_ENDPOINT_3 (USBC_BASE_ADDR + 0x2C)
#define USB_FIFO_ENDPOINT_4 (USBC_BASE_ADDR + 0x30)
#define USB_FIFO_ENDPOINT_5 (USBC_BASE_ADDR + 0x34)
#define USB_FIFO_ENDPOINT_6 (USBC_BASE_ADDR + 0x38)
#define USB_FIFO_ENDPOINT_7 (USBC_BASE_ADDR + 0x3C)

//Half-Word Access
#define USB_FIFO_EP1TX_PTR0 (USBC_BASE_ADDR + 0x40)
#define USB_FIFO_EP1TX_PTR1 (USBC_BASE_ADDR + 0x42)
#define USB_FIFO_EP2TX_PTR0 (USBC_BASE_ADDR + 0x44)
#define USB_FIFO_EP2TX_PTR1 (USBC_BASE_ADDR + 0x46)
#define USB_FIFO_EP3TX_PTR0 (USBC_BASE_ADDR + 0x48)
#define USB_FIFO_EP3TX_PTR1 (USBC_BASE_ADDR + 0x4A)
#define USB_FIFO_EP0_PTR0 (USBC_BASE_ADDR + 0x58)

//USB Internal DMA Register
//DMA configurable up to 8 channels
#define DMA_INTR_r (USBC_BASE_ADDR +0x200) //8bit
//channel 1
#define DMA_CNTL1 (USBC_BASE_ADDR +0x204) //16bit
#define DMA_CNTL1_L (USBC_BASE_ADDR +0x204)
#define DMA_CNTL1_H (USBC_BASE_ADDR +0x205)
#define DMA_ADDR1 (USBC_BASE_ADDR +0x208) //32bit
#define DMA_COUNT1 (USBC_BASE_ADDR +0x20c) //32bit
//channel 2
#define DMA_CNTL2 (USBC_BASE_ADDR +0x214) //16bit
#define DMA_CNTL2_L (USBC_BASE_ADDR +0x214)
#define DMA_CNTL2_H (USBC_BASE_ADDR +0x215)
#define DMA_ADDR2 (USBC_BASE_ADDR +0x218) //32bit
#define DMA_COUNT2 (USBC_BASE_ADDR +0x21c) //32bit
//channel 3
#define DMA_CNTL3 (USBC_BASE_ADDR +0x224) //16bit
#define DMA_CNTL3_L (USBC_BASE_ADDR +0x224)
#define DMA_CNTL3_H (USBC_BASE_ADDR +0x225)
#define DMA_ADDR3 (USBC_BASE_ADDR +0x228) //32bit
#define DMA_COUNT3 (USBC_BASE_ADDR +0x22c) //32bit
//channel 4
#define DMA_CNTL4 (USBC_BASE_ADDR +0x234) //16bit
#define DMA_CNTL4_L (USBC_BASE_ADDR +0x234)
#define DMA_CNTL4_H (USBC_BASE_ADDR +0x235)
#define DMA_ADDR4 (USBC_BASE_ADDR +0x238) //32bit
#define DMA_COUNT4 (USBC_BASE_ADDR +0x23c) //32bit
//channel 5
#define DMA_CNTL5 (USBC_BASE_ADDR +0x244) //16bit
#define DMA_CNTL5_L (USBC_BASE_ADDR +0x244)
#define DMA_CNTL5_H (USBC_BASE_ADDR +0x245)
#define DMA_ADDR5 (USBC_BASE_ADDR +0x248) //32bit
#define DMA_COUNT5 (USBC_BASE_ADDR +0x24c) //32bit
//channel 6
#define DMA_CNTL6 (USBC_BASE_ADDR +0x254) //16bit
#define DMA_CNTL6_L (USBC_BASE_ADDR +0x254)
#define DMA_CNTL6_H (USBC_BASE_ADDR +0x255)
#define DMA_ADDR6 (USBC_BASE_ADDR +0x258) //32bit
#define DMA_COUNT6 (USBC_BASE_ADDR +0x25c) //32bit
//channel 7
#define DMA_CNTL7 (USBC_BASE_ADDR +0x264) //16bit
#define DMA_CNTL7_L (USBC_BASE_ADDR +0x264)
#define DMA_CNTL7_H (USBC_BASE_ADDR +0x265)
#define DMA_ADDR7 (USBC_BASE_ADDR +0x268) //32bit
#define DMA_COUNT7 (USBC_BASE_ADDR +0x26c) //32bit

//channel 8
#define DMA_CNTL8 (USBC_BASE_ADDR +0x274) //16bit
#define DMA_CNTL8_L (USBC_BASE_ADDR +0x274)
#define DMA_CNTL8_H (USBC_BASE_ADDR +0x275)
#define DMA_ADDR8 (USBC_BASE_ADDR +0x278) //32bit
#define DMA_COUNT8 (USBC_BASE_ADDR +0x27c) //32bit

#define INTR_DMA1 (1<<0)
#define INTR_DMA2 (1<<1)
#define INTR_DMA3 (1<<2)
#define INTR_DMA4 (1<<3)
#define INTR_DMA5 (1<<4)
#define INTR_DMA6 (1<<5)
#define INTR_DMA7 (1<<6)
#define INTR_DMA8 (1<<7)

//dma cntl
#define CNTL_DMAEN(x) ((x)<<0)
#define CNTL_DIRECTION(x) ((x)<<1)
#define CNTL_DMAMODE(x) ((x)<<2)
#define CNTL_INTERE(x) ((x)<<3)
#define CNTL_EP(x) ((x)<<4)
#define CNTL_BUSERR(x) ((x)<<8)
#define CNTL_BURSTMODE(x) ((x)<<9)					

//USB RqPktCount Register
#define RQPKTCOUNT

// 结构体、枚举变量定义 
// Data Struct of Common USB Registers
typedef struct _sUSBCCommonReg{
	__IO uint8_t	FADDRR;	//Function address register
	__IO uint8_t	UCSR;		//USB control and status register
	
	__IO uint8_t	INTRTX_L;	//Low byte of Interrupt register for Endpoint0 and Tx Endpoint
	__IO uint8_t	INTRTX_H;	//High byte of Interrupt register for Endpoint0 and Tx Endpoint
	
	__IO uint8_t	INTRRX_L;	//Low byte of Interrupt register for Rx Endpoint
	__IO uint8_t	INTRRX_H;	//High byte of Interrupt register for Rx Endpoint
	
	__IO uint8_t	INTRTXE_L;	//Low byte of Interrupt enable register for IntrTx
	__IO uint8_t	INTRTXE_H;	//High byte of Interrupt enable register for IntrTx
	
	__IO uint8_t	INTRRXE_L;	//Low byte of Interrupt enable register for IntrRx
	__IO uint8_t	INTRRXE_H;	//High byte of Interrupt enable register for IntrRx
	
	__IO uint8_t	INTRUSB;	//Interrupt register for common USB interrupts
	__IO uint8_t	INTRUSBE;	//Interrupt enable register for IntrUSB
	
	__IO uint8_t	FNUMR_L;	//Low byte of Frame number
	__IO uint8_t	FNUMR_H;	//High byte of Frame number
	
	__IO uint8_t	EINDEX;		//Index register for selecting the endpoint status and control register
	__IO uint8_t	TSTMODE;	//Enables the USB test modes
} sUSBCCommonReg;

// Data Struct of Indexed Registers
typedef struct _sUSBCIndexedReg{
	__IO uint8_t	TXMAXP_L;		//Low byte of Maximum packet size for peripheral Tx endpoint
	__IO uint8_t	TXMAXP_H;		//High byte of Maximum packet size for peripheral Tx endpoint
	union{
		  __IO uint8_t	E0CSR_L;	//Low byte of Control Status register for Endpoint0
		  __IO uint8_t	TXCSR_L;	//Low byte of Control Status register for peripheral Tx endpoint
		};
	union{
		  __IO uint8_t	E0CSR_H;	//High byte of Control Status register for Endpoint0
		  __IO uint8_t	TXCSR_H;	//High byte of Control Status register for peripheral Tx endpoint
		};
	__IO uint8_t	RXMAXP_L;			//Low byte of Maximum packet size for peripheral Rx endpoint
	__IO uint8_t	RXMAXP_H;			//High byte of Maximum packet size for peripheral Rx endpoint
	
	__IO uint8_t	RXCSR_L;			//Low byte of Control Status register for peripheral Rx endpoint
	__IO uint8_t	RXCSR_H;			//High byte of Control Status register for peripheral Rx endpoint
	union{
		  __IO uint8_t	E0COUNTR_L;	//Low byte of Number of received bytes in Endpoint0 FIFO
		  __IO uint8_t	RXCount_L;		//Low byte of Number of bytes in peripheral Rx endpoint FIFO
		};
	union{
		  __IO uint8_t	E0COUNTR_H;	//High byte of Number of received bytes in Endpoint0 FIFO
		  __IO uint8_t	RXCount_H;		//High byte of Number of bytes in peripheral Rx endpoint FIFO
		};
	__IO uint8_t	TXTYPE;
	union{
		  __IO uint8_t	NAKLIMIT0;
		  __IO uint8_t	TXINTERVAL;
		};
	__IO uint8_t RXTYPE;
	__IO uint8_t RXINTERVAL;

} sUSBCIndexedReg;

//Dynamic FIFO
typedef	struct	_sUSBCFIFO_Reg{
	__IO uint8_t	OTGCTRL;
	__IO uint8_t RESERVED;
	__IO uint8_t	TXFIFOSZ;	//Tx Endpoint FIFO size,double buffer only set in one register(TX_fifosz/RX_fifosz)
	__IO uint8_t	RXFIFOSZ;	//Rx Endpoint FIFO size,MAX FIFO size is 1024byte
	__IO uint8_t	TX_fifoadd_L;	//Tx Endpoint FIFO address(Low 8bit)
	__IO uint8_t	TX_fifoadd_H;	//Tx Endpoint FIFO address(High 8bit)
	__IO uint8_t	RX_fifoadd_L;	//Rx Endpoint FIFO address(Low 8bit)
	__IO uint8_t	RX_fifoadd_H;	//Rx Endpoint FIFO address(High 8bit)
} sUSBCFIFO_Reg;

/*	transfer status phase	*/
enum HOST_STATUS
{
	HOST_COMMAND_PHASE =0,
	HOST_TEST_UNIT_READY_PHASE,
	HOST_READ28_DATA_PHASE,
	HOST_WRITE2A_DATA_PHASE,
	HOST_MODE_SENSE_PHASE,
	HOST_INQUIRY_PHASE,
	HOST_READ_CAPACITY_PHASE,
	HOST_REQUEST_SENSE_PHASE,
	HOST_MEDIUM_REMOVAL_PHASE,
	HOST_RECEIVE_CSW,
	HOST_RECEIVE_STALL,
	HOST_RECEIVE_NAK_TIME,
	HOST_RECEIVE_ERROR,
	HOST_RECEIVE_CBW_FAIL,
	HOST_RECEIVE_PHASE_ERR,
	HOST_STATUS_OK
};

// 全局变量声明 
extern uint8_t USB_Device_Descriptor[];
extern uint8_t USB_Device_Qualifier_Descriptor[];
extern uint8_t USB_Configuration_Descriptor[];
extern uint8_t USB_String_Descriptor[];
extern const uint8_t USB_LANGID[];
extern const uint8_t USBReq_GetDescriptor[];
extern const uint8_t USBReq_SetAddress[];
extern const uint8_t USBReq_SetConfiguration[];
extern const uint8_t USBReq_SetInterface[];
extern uint8_t USBReq_ClearFeature[];
extern const uint8_t USBReq_GetMaxLUN[];
extern const uint8_t SCSIDATA_ReadCapacities[];
extern const uint8_t SCSIDATA_ReadFormatCapacities[];
extern const uint8_t SCSICmd_Packet[];
extern const uint8_t SCSICmd_ReadCapacity23[];
extern const uint8_t SCSICmd_ReadCapacity25[];
extern const uint8_t SCSICmd_ReadFormatCapacity23[];
extern const uint8_t SCSICmd_RequestSense[];
extern const uint8_t SCSICmd_MediumRemoval[];
extern const uint8_t SCSICmd_ModeSense06[];
extern const uint8_t SCSICmd_Inquiry12[];
extern const uint8_t SCSICmd_TestReady[];
extern const uint8_t SCSICmd_Read28[];
extern const uint8_t SCSICmd_Write2a[];
extern const uint8_t SCSICmd_ReadBootloadVer[];
extern const uint8_t SCSICmd_DownData2SRAM[];
extern const uint8_t SCSICmd_VendorReadData[];
extern const uint8_t SCSICmd_VendorWriteData[];

/* Global USB Register */
extern sUSBCCommonReg* gUSBC_ComReg;
extern sUSBCIndexedReg* gUSBC_IdxReg;
extern sUSBCFIFO_Reg* gUSBC_fifoReg; 

//global message queue for USB loop tasks.
extern __IO uint8_t g_msgflags;
extern uint8_t g_msgflags1;

/* current selected LUN,private data partition or security partition
   5..4--USB version,0--USB1.1,others--USB2.0
   3..2--max LUNs,
   1..0--reserved
 */
extern uint8_t g_currentLUN;
//USB version(1--USB2.0;0--USB1.1)
extern uint8_t g_usbVer;

//usb the max packet size
extern unsigned short gTxMaxPacket;
extern unsigned short gRxMaxPacket;

//Save USB Address
extern __IO uint8_t g_USBAddrFlag;
extern __IO uint8_t g_USBNewAddr;//indicate which command is processed currently
extern __IO uint8_t g_bulkPhase;

//indicate which sub command is processed currently
extern uint8_t g_subCmd;

extern uint8_t g_dev_std_req[];

//this data buffer is used by host control tranfer to hold data request by host
extern uint8_t g_hostdatabuf[];

//data count received by host ep0
extern uint8_t g_hostEp0RxCount;

//indicate each status during host communicate with device
extern uint8_t g_hostStatus;

//indicate whether Host TX or Rx is done
//0--not done,1--done and correctly,others--done and with error
extern __IO uint8_t g_flgHostTxRxDone;

//indicate whether Device TX or Rx is done
//0--not done,1--done
extern __IO uint8_t g_flgDeviceTxRxDone;

//indicate whether Security is done
//0--not done,1--done
extern __IO uint8_t g_flgSecurityDone;

//indicate if a USB device insert into host,1--yes, 0--no
extern __IO uint8_t g_flgHostDeviceInsert;

//CBW packet
extern uint32_t g_cbwPacket_l[];
extern uint8_t* g_cbwPacket;
	
//CSW packet
extern uint32_t g_cswPacket_l[];
extern uint8_t* g_cswPacket;

//USB data buffer1
extern uint32_t g_databuf[];

//indicate the start sector in reading device
extern uint32_t g_wrLBA;

//indicate the length in Once tranfer from PC to device
extern uint16_t g_transLens;

//indicate the transfer stage of USB1.1
extern uint8_t g_TxRxCounter;

//indicate the mode of reading device
extern uint8_t g_readMode;
extern __IO uint8_t g_DMA_INTR;

extern uint8_t VendorReadFlag;//读控制变量
extern uint32_t RWDataAddr;
extern uint32_t RWDataLength;

extern uint32_t InitData;

// 函数声明 
// USB Function definition
extern void memcp_f(uint8_t *dest,uint8_t *sour,uint16_t len);
extern void memset_f(uint8_t *addr,uint8_t value,uint16_t len);

extern void SetTX(void);
extern void ClearRX(void);

// Little-endian Swap Big-endian
extern uint32_t le32_to_cpu(uint32_t in);
extern uint16_t le16_to_cpu(uint16_t in);

extern void UsbSetTXsize(uint16_t size,uint8_t mode);
extern void UsbSetRXsize(uint16_t size,uint8_t mode);
extern uint16_t UsbSetMaxPacketSize(uint8_t size,uint8_t TxRx);
extern uint16_t UsbSetDoubleFIFO(uint8_t size,uint8_t TxRx);

extern void USB_ReadFIFO(unsigned long ep_fifo, uint8_t* pbuf, unsigned short len);
extern void USB_WriteFIFO(unsigned long ep_fifo, uint8_t *pbuf, unsigned short len);

extern void VendorReadData(uint8_t *tmpBuf,uint32_t len);
//USB Read data from Endpoint FIFO
extern void read_ep_buf(uint8_t *out_buf,uint32_t ep,uint16_t uiLens);

//USB write data to Endpoint FIFO
extern void write_ep_buf(uint32_t ep,uint8_t *out_buf,uint16_t uiLens);

//USB Suspend Interrupt
extern void USBDev_Suspend(void);

//USB Bus Reset Interrupt
extern void USBDev_BusReset(void);

//USB Endpoint0 Handler
extern void USBDev_Ep0Handler(void);

//Send packet to host.(BULK-IN to host)
//flag--0,normal,flag--1,CSW packet
extern void USBDev_Packet_BulkIn(uint8_t *pDataBuf,uint16_t uiLength,uint8_t flag);

//Dynamic function point
extern void (*dynamic)(void);

#ifdef __cplusplus
}
#endif

#endif	/* _USBOTG_REG_H */

