/**
 * @file usb_host_cpm.h
 * @author Product application department 
 * @brief usb host clock power module header file
 * @version V1.0
 * @date 2025-04-03
 * 
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 * 
 */

#ifndef _USB_HOST_CPM_H
#define _USB_HOST_CPM_H

#ifdef __cplusplus
extern "C" {
#endif

// 头文件包含
#include "type.h"

// 全局变量定义

#define CPM_CHIPCFGR (*(volatile unsigned int *)(CPM_BASE_ADDR + 0x2c))
#define CPM_OCSR (*(volatile unsigned int *)(CPM_BASE_ADDR + 0x20))
	
#define CPM_MULTI_CGTCR (*(volatile uint32_t*)(CPM_BASE_ADDR + 0x3C))
#define CPM_SYS_CGTCR (*(volatile uint32_t*)(CPM_BASE_ADDR + 0x40))
#define CPM_AHB3_CGTCR (*(volatile uint32_t*)(CPM_BASE_ADDR + 0x44))
#define CPM_ARITH_CGTCR (*(volatile uint32_t*)(CPM_BASE_ADDR + 0x48))
#define CPM_IPS_CGTCR (*(volatile uint32_t*)(CPM_BASE_ADDR + 0x4C))

#define CPM_VCCG_TRIMR (*(volatile uint32_t*)(CPM_BASE_ADDR + 0x50))
#define CPM_VCCL_TRIMR (*(volatile uint32_t*)(CPM_BASE_ADDR + 0x54))
#define CPM_VCCV_TRIMR (*(volatile uint32_t*)(CPM_BASE_ADDR + 0x58))
#define CPM_VCCC_TMR (*(volatile uint32_t*)(CPM_BASE_ADDR + 0x5C))

#define CPM_O8M_TRIMR (*(volatile uint32_t*)(CPM_BASE_ADDR + 0x60))
#define CPM_O600M_TRIMR (*(volatile uint32_t*)(CPM_BASE_ADDR + 0x68))

#define CPM_OSCE_STABLE (*(volatile uint32_t*)(CPM_BASE_ADDR + 0x78))

// CPM_SRSTCR1 
#define USBC_RST_RELEASE_BIT (8)
#define USBPHY_RST_RELEASE_BIT (13)

#define USBPHY_CFG_SRM (29)
#define USBPHY_PLL_SRM (28)

#define PWRCR_PHY_PSWEN_BIT (24)
#define PWRCR_PHY_I_ISOEN_BIT (27)
#define PWRCR_PHY_O_ISOEN_BIT (26)
#define PWRCR_PHY_RSTMASK_BIT (25)

#define OSCEXT_EN (1<<4)

// CPM_SCDIVR 
#define PERDIV_UPDATE (1<<0)
#define SYSDIV_UPDATE (1<<1)
#define OSC160M_DIVEN (1<<23)
#define USBPHY_DIVEN (1<<7)
#define EPT_DIV_MASK (0xF<<24)
#define OSC600M_DIV_MASK (0xFF)
#define USBPHY30M_DIV_MASK (0x3F)

#define OSC600M_DIV_BITS_MASK (0xFF)

#ifdef __cplusplus
}
#endif

#endif /* _USB_HOST_CPM_H */

