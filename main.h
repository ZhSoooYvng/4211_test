/**
 * @file main.h
 * @author Product application department
 * @brief  SDK 测试用例宏定义头文件，仅供参考
 * @version V1.0
 * @date 2025-03-01
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 *
 */
#ifndef _DEMO_H_
#define _DEMO_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "env_vir.h"

#define CIRCULAR_LEN  512

extern QueueType_t circular_queue;
extern uint8_t queue_buf[CIRCULAR_LEN];
extern float thermopile_data[8][8];

#ifdef __cplusplus
extern "C"
{
#endif

//  #define ALGO_DEMO
  #define UART_DEMO
//  #define CAN_DEMO
//  #define PWMT_DEMO
//  #define PIT_DEMO
//  #define TC_DEMO
//  #define WDT_DEMO
//  #define EPORT_DEMO
//  #define SPI_DEMO
//  #define ADC_DEMO
//  #define DAC_DEMO
//  #define RTC_DEMO
//  #define I2C_DEMO
//  #define I2S_DEMO
//  #define RESET_DEMO
//  #define LOWPOWER_DEMO
//  #define SSI_DEMO
//  #define SIDO_SD_TEST
//  #define ETH_DEMO
//  #define FREERTOS_DEMO  //需要打开options for Target,更改LVGL_MCU为LVGL_OS
//  #define IMGDEC_DEMO
//  #define AXIDMA_DEMO
//  #define I8080_DEMO
//  #define LCDC_DEMO
//  #define TOUCH_DEMO
//  #define DMA2D_DEMO
//  #define JPEG_ACCELERATE_DEMO
//  #define USB_HOST_DEMO
//  #define USB_DEVICE_DEMO
//  #define DAC_AUDIO_DEMO
//  #define NAND_FLASH_DEMO
//  #define IAP_DEMO
//  #define LVGL_DEMO
//  #define USBH_UVC_DEMO   //需要打开options for Target,更改LVGL_MCU为LVGL_OS
//  #define FREETYPE_DEMO
#ifdef ALGO_DEMO
#include "algo_test.h"
#endif

#ifdef UART_DEMO
#include "uart_test.h"
#endif

#ifdef CAN_DEMO
#include "can_test.h"
#endif

#ifdef PWMT_DEMO
#include "pwmt_test.h"
#endif

#ifdef PIT_DEMO
#include "pit32_test.h"
#endif

#ifdef TC_DEMO
#include "tc_test.h"
#endif

#ifdef WDT_DEMO
#include "wdt_test.h"
#endif

#ifdef EPORT_DEMO
#include "eport_test.h"
#endif

#ifdef SPI_DEMO
#include "spi_test.h"
#endif

#ifdef ADC_DEMO
#include "adc_test.h"
#endif

#ifdef DAC_DEMO
#include "dac_test.h"
#endif

#ifdef RTC_DEMO
#include "rtc_test.h"
#endif

#ifdef I2C_DEMO
#include "i2c_test.h"
#endif

#ifdef I2S_DEMO
#include "i2s_test.h"
#endif

#ifdef RESET_DEMO
#include "reset_test.h"
#endif

#ifdef LOWPOWER_DEMO
#include "lowpower_test.h"
#endif

#ifdef SSI_DEMO
#include "ssi_test.h"
#endif

#ifdef SIDO_SD_TEST
#include "sdio_test.h"
#endif

#ifdef ETH_DEMO
#include "eth_test.h"
#endif

#ifdef FREERTOS_DEMO
#include "freertos_test.h"
#endif

#ifdef IMGDEC_DEMO
#include "image_decoder_test.h"
#endif

#ifdef AXIDMA_DEMO
#include "axidma_test.h"
#endif

#ifdef I8080_DEMO
#include "i8080_test.h"
#endif

#ifdef LCDC_DEMO
#include "lcdc_test.h"
#endif

#ifdef TOUCH_DEMO
#include "tp_test.h"
#endif

#ifdef DMA2D_DEMO
#include "dma2d_test.h"
#endif

#ifdef LCDC_DMA2D_DEMO  
#include "lcdc_dma2d_test.h" 
#endif

#ifdef JPEG_ACCELERATE_DEMO
#include "jpeg_accelerator_test.h"
#endif

#ifdef USB_DEVICE_DEMO
#include "usb_device_test.h"
#endif

#ifdef USB_HOST_DEMO
#include "usb_host_test.h"
#endif

#ifdef DAC_AUDIO_DEMO
#include "audio_test.h"
#endif

#ifdef NAND_FLASH_DEMO
#include "nandflash_test.h"
#endif

#ifdef IAP_DEMO
#include "iap_test.h"
#endif

#ifdef LVGL_DEMO
#include "lvgl_test.h"
#endif

#ifdef USBH_UVC_DEMO
#include "usbh_uvc_test.h"
#endif

#ifdef FREETYPE_DEMO
#include "freetype_test.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
