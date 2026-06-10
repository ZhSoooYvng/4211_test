/**
 * @file jpeg_accelerator_test.h
 * @author Product application department
 * @brief  JPEG 加速器配置头文件
 * @version V1.0
 * @date 2025-02-25
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

#ifndef __JPEG_ACCELERATOR_H
#define __JPEG_ACCELERATOR_H

#ifdef __cplusplus
extern "C"
{
#endif

// 头文件包含
#include "image_decoder_test.h"
#include "jpeg_accele_data.h"
#include "axidma_drv.h"

// 全局变量定义

/**
 * @brief JEPG 模块加速器模式调用AXIDMA的LLP链表参数结构体定义
 * 
 */

 typedef struct
 {
    uint32_t dbg_ch;
    uint32_t dbg_transWidth;
    uint32_t dbg_transLen;
    uint32_t dbg_mSize;
    uint32_t dbg_llp_size;
} axidma_transfer_param_t;

 /**
  * @brief JEPG 模块加速器模式相关基本参数结构体定义
  * 
  */
 typedef struct
 {
    uint32_t pic_w;
    uint32_t pic_h;
    uint32_t jpeg_data_width;
    uint32_t jpeg_w16_cnt;
    uint32_t jpeg_w16_remian_cnt;
    uint32_t jpeg_h16_cnt;
    uint32_t jpeg_h16_remian_cnt;
    uint32_t jpeg_axi_len_1;
    uint32_t jpeg_axi_len_2;
    uint32_t jpeg_axi_w_offset;
    uint32_t jpeg_axi_start_src_addr_1;
    uint32_t jpeg_axi_start_src_addr_2;
    uint32_t jpeg_axi_start_src_addr;
    uint32_t jpeg_axi_start_dst_addr;
    uint32_t jpeg_s2p_src_addr[16];
    uint32_t jpeg_s2p_dst_addr[16];
    uint8_t jpeg_w_irq;
    uint8_t jpeg_h_irq;
    uint32_t jpeg_sram_fixed_len;
    uint32_t jpeg_fixed_datatype_len;
    uint32_t jpeg_arraylen;
} jpeg_axidma_llp_t;

/**
 * @brief JEPG 模块加速器模式图片格式结构体定义
 * 
 */
typedef struct
{
    uint8_t is_rgb;                          // 0: YUV  1: RGB
    pp_output_format_t out_yuv_datatype;     // 输出YUV格式
    pp_output_rgb_format_t out_rgb_datatype; // 输出RGB格式
    uint32_t image_addr;                     // 图像地址
    uint32_t image_len;                      // 图像长度
    uint32_t out_addr0;                      // 输出地址0（Y）/（RGB）
    uint32_t out_addr1;                      // 输出地址1（U）
    uint32_t out_addr2;                      // 输出地址2（V）
    uint8_t alpha;                           // 透明度
    uint8_t xip_src;                         // 解码图像源地址；0：SPIFLASH  1：PSRAM
    image_deocoder_callback_t jpeg_irq_cb;
} jpeg_accelerator_initstructure_t;

// 函数声明
void jpeg_accelerate_demo(void);
void jpeg_accelerate_axidma_init_para(jfif_info_t *jfif_info, jpeg_outset_t *jpeg_outset, uint8_t data_type);
void jpeg_axidma_setparam(axi_dma_t *pdmac, axidma_transfer_param_t dbg_para);
void jpeg_axidma_calculate(void);

#ifdef __cplusplus
}
#endif

/** @} */  // 结束 ACCELERATOR 模块分组
/** @} */  // 结束外设模块分组

#endif
