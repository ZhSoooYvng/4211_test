/**
 * @file usb_uvc_lcd.h
 * @author Product application department
 * @brief   UVC摄像头 图像显示头文件，仅供参考
 * @version V1.0
 * @date 2025-07-30
 * 
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 * 
 */

 // 头文件包含
#include "type.h"

#ifndef __LCD_RGB_TEST_H
#define __LCD_RGB_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

// 全局变量定义
#define WHITE_565		(0xffff)
#define YELLOW_565		(0xffe0)
#define CYAN_565		(0x07ff)
#define GREEN_565		(0x0400)
#define MAGENTA_565		(0xf81f)
#define RED_565			(0xf800)
#define BLUE_565		(0x001f)
#define BLACK_565		(0x0000)

//函数声明
void uvc_lcd_rgb_init(void);
void uvc_lcd_jpeg_decode(uint32_t img_addr, uint32_t image_len, uint32_t out_addr);
void uvc_lcd_rgb_dma2d_draw(uint32_t *psrc, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t offsetline);

#ifdef __cplusplus
}
#endif

#endif
