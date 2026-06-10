/**
 * @file jpeg_accele_data.h
 * @author Product application department
 * @brief  JPEG 加速器测试数据头文件
 * @version V1.0
 * @date 2025-02-25
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 *
 */

#ifndef __JPEG_ACCELE_DATA_H
#define __JPEG_ACCELE_DATA_H

#ifdef __cplusplus
extern "C"
{
#endif

// 头文件包含
#include "type.h"

// 全局变量定义
#define PIC_672330_YUV420 0
#define PIC_480330_YUV420 0

#define PIC_1280_800 0
#define PIC_1280_720 0
#define PIC_1024_600 1
#define PIC_800_480 0
#define PIC_640_480 0
#define PIC_480_320 0
#define PIC_480_272 0

extern const uint8_t jpeg_image_data[];
extern uint32_t jpeg_image_data_len;

#ifdef __cplusplus
}
#endif

#endif
