/**
 * @file usb_host_iomacros.h
 * @author Product application department 
 * @brief usb host io macros definition
 * @version V1.0
 * @date 2025-04-03
 * 
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 * 
 */

#ifndef _USB_HOST_IOMACROS_H
#define _USB_HOST_IOMACROS_H

#ifdef __cplusplus
extern "C"{
#endif

// 头文件包含
#include "usb_host_cpm.h"

// 全局变量定义
//#define IO_READ8(p) (Uint8)(*(volatile Uint8 *)(p))
//#define IO_WRITE8(p,c) (*((volatile Uint8 *)(p)) = ((Uint8)(c)))

//#define IO_READ16(p) (Uint16)(*(volatile Uint16 *)(p))
//#define IO_WRITE16(p,v) (*((volatile Uint16 *)(p)) = (Uint16)(v))

//#define IO_READ32(p) (Uint32)(*(volatile Uint32 *)(p))
//#define IO_WRITE32(p,v) (*((volatile Uint32 *)(p)) = (Uint32)(v))

#define write_port_byte(port,adata) (*(volatile uint8_t *)(port))=((uint8_t)(adata))
#define read_port_byte(port) (*(volatile uint8_t *)(port))

#define write_port_halfword(port,adata) (*(volatile unsigned short *)(port))=((unsigned short)(adata))
#define read_port_halfword(port) (*(volatile unsigned short *)(port))

#define write_port_word(port,adata) (*(volatile unsigned long *)(port))=((unsigned long)(adata))
#define read_port_word(port) (*(volatile unsigned long *)(port))

// bit operation
//#define _bit(x) ((uint32_t)(1 << (x)))
//#define _bit_set(value, bit) ((value) |=  (bit))
//#define _bit_clr(value, bit) ((value) &= ~(bit))
//#define _bit_get(value, bit) ((value) &   (bit))
//#define _reg_chk(value, bit) ((value) &   (bit))
//#define _bit_and(value, bit) ((value) &=  (bit))

//#define _reg_write(reg, val) ((reg) = (val))
//#define _reg_read(reg) ((reg))
//#define _reg_clear(reg) ((reg) = (0x0))

//#define _reg_modify(reg, clearmask, setmask) _reg_write((reg), (((_reg_read(reg)) & (clearmask)) | (setmask)))

#ifdef __cplusplus
}
#endif

#endif /* _USB_HOST_IOMACROS_H */
