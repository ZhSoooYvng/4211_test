/**
 * @file jpeg_accele_table.h
 * @author Product application department
 * @brief JPEG 加速器配置表文件
 * @version 0.1
 * @date 2025-08-19
 * 
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 * 
 */
/**
 * @defgroup ThirdPart 第三方模块
 * @{
 *
 * @defgroup ACCELERATOR ACCELERATOR
 * @ingroup ThirdPart
 * @{
 */

#ifndef __JPEG_ACCELERATOR_TABLE_H
#define __JPEG_ACCELERATOR_TABLE_H

#ifdef __cplusplus
extern "C"
{ 
#endif

// 头文件包含
#include "jpeg_accelerator_test.h"

// 全局变量定义

extern const uint32_t jpeg_fixed_src_sram[2][16];


/* 固化的参数结构体 */
extern const jpeg_axidma_llp_t jpeg_axidma_llp_480272_RGB888;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_480272_RGB565;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_480320_RGB888;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_480320_RGB565;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_640480_RGB888;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_640480_RGB565;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_800480_RGB888;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_800480_RGB565;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_1024600_RGB888;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_1024600_RGB565;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_1280720_RGB888;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_1280720_RGB565;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_1280800_RGB888;
extern const jpeg_axidma_llp_t jpeg_axidma_llp_1280800_RGB565;
/* 固化的 jpeg_fixed_dst 数组 */
extern const uint32_t jpeg_fixed_dst_480272_RGB888[34][16];
extern const uint32_t jpeg_fixed_dst_480272_RGB565[17][16];
extern const uint32_t jpeg_fixed_dst_480320_RGB888[40][16];
extern const uint32_t jpeg_fixed_dst_480320_RGB565[20][16];
extern const uint32_t jpeg_fixed_dst_640480_RGB888[60][16];
extern const uint32_t jpeg_fixed_dst_640480_RGB565[60][16];
extern const uint32_t jpeg_fixed_dst_800480_RGB888[90][16];
extern const uint32_t jpeg_fixed_dst_800480_RGB565[60][16];
extern const uint32_t jpeg_fixed_dst_1024600_RGB888[152][16];
extern const uint32_t jpeg_fixed_dst_1024600_RGB565[76][16];
extern const uint32_t jpeg_fixed_dst_1280720_RGB888[180][16];
extern const uint32_t jpeg_fixed_dst_1280720_RGB565[135][16];
extern const uint32_t jpeg_fixed_dst_1280800_RGB888[200][16];
extern const uint32_t jpeg_fixed_dst_1280800_RGB565[150][16];

#ifdef __cplusplus
}
#endif

/** @} */  // 结束 ACCELERATOR 模块分组
/** @} */  // 结束外设模块分组

#endif // __JPEG_ACCELERATOR_TABLE_H



