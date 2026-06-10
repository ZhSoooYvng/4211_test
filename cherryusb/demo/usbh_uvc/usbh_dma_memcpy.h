/*
 * usbh_dma_memcpy.h
 *
 *  Created on: 2024年11月22日
 *      Author: yqzhuang
 */

#ifndef __USBH_USBH_UVC_USBH_DMA_MEMCPY_H_
#define __USBH_USBH_UVC_USBH_DMA_MEMCPY_H_
#ifdef __cplusplus
extern "C"{
#endif

#include "dma_drv.h"

#define DST_TRANSFER_WIDTH_8    (0<<1)
#define DST_TRANSFER_WIDTH_16   (1<<1)
#define DST_TRANSFER_WIDTH_32   (2<<1)
#define DST_TRANSFER_WIDTH_64   (3<<1)
#define DST_TRANSFER_WIDTH_128  (4<<1)
#define DST_TRANSFER_WIDTH_256  (5<<1)

#define SRC_TRANSFER_WIDTH_8    (0<<4)
#define SRC_TRANSFER_WIDTH_16   (1<<4)
#define SRC_TRANSFER_WIDTH_32   (2<<4)
#define SRC_TRANSFER_WIDTH_64   (3<<4)
#define SRC_TRANSFER_WIDTH_128  (4<<4)
#define SRC_TRANSFER_WIDTH_256  (5<<4)

#define DST_ADDR_INC            (0<<7)
#define DST_ADDR_DEC            (1<<7)
#define DST_ADDR_NO_INC         (2<<7)

#define SRC_ADDR_INC            (0<<9)
#define SRC_ADDR_DEC            (1<<9)
#define SRC_ADDR_NO_INC         (2<<9)

#define DST_BURST_LENGHT_1      (0<<11)
#define DST_BURST_LENGHT_4      (1<<11)
#define DST_BURST_LENGHT_8      (2<<11)
#define DST_BURST_LENGHT_16     (3<<11)
#define DST_BURST_LENGHT_32     (4<<11)
#define DST_BURST_LENGHT_64     (5<<11)
#define DST_BURST_LENGHT_128    (6<<11)
#define DST_BURST_LENGHT_256    (7<<11)


#define SRC_BURST_LENGHT_1      (0<<14)
#define SRC_BURST_LENGHT_4      (1<<14)
#define SRC_BURST_LENGHT_8      (2<<14)
#define SRC_BURST_LENGHT_16     (3<<14)
#define SRC_BURST_LENGHT_32     (4<<14)
#define SRC_BURST_LENGHT_64     (5<<14)
#define SRC_BURST_LENGHT_128    (6<<14)
#define SRC_BURST_LENGHT_256    (7<<14)

typedef enum
{
	dma_channel_0 = 0,
	dma_channel_1,
	dma_channel_2,
	dma_channel_3
}DMA_CHANNEL_NUM;






static inline  uint8_t Fast_DMA_Memcpy(DMA_CHANNEL_NUM channel, uint8_t *dest, const uint8_t *src, uint32_t len)
{
    /* 复位DMA */
    _dmac_ch_dis(DMAC2,channel);
    _dmac_clr_dmac_ctrln(DMAC2,channel);
    _dmac_clr_raw_interrupt_status(DMAC2,channel);
    _dmac_clr_block_interrupt_status(DMAC2,channel);

    /* 初始化DMA配置 */
    //DRV_DMAC_CtrlRegConfig(DMAC4, channel, 0, 4, 4, 0, 0, 1, 1);
    DMAC2->CH[channel].CTRL =   DST_TRANSFER_WIDTH_32	|\
                                SRC_TRANSFER_WIDTH_8    | \
                                DST_ADDR_INC            | \
                                SRC_ADDR_INC            | \
                                DST_BURST_LENGHT_32     | \
                                SRC_BURST_LENGHT_32     | \
                                (1<<25);
    /* 设置DMA传输参数 */
    _dmac_set_transfer_length(DMAC2,channel, len);
    _dmac_set_source_address(DMAC2,channel, (uint32_t)src);
    _dmac_set_destination_address(DMAC2,channel, (uint32_t)dest);
    /* 握手 */
    _dmac_src_hs_soft(DMAC2,channel);
    _dmac_dst_hs_soft(DMAC2,channel);
    /* 启动DMA */
    _dmac_en(DMAC2);
    _dmac_ch_dis(DMAC2,channel);

    DMAC2->MASKTFR &= ~(((1 << channel) << 8 | (1 << channel)));

    // FV_MODE == 1
    DMAC2->CH[channel].CONFIG_HIGH |= DMAC_FC_MODE;
    _dmac_ch_en(DMAC2,channel);
    while((_dmac_get_raw_interrupt_status(DMAC2,channel)&(1<<channel)) == 0);
    _dmac_clr_raw_interrupt_status(DMAC2,channel);
    return 0;
}







#ifdef __cplusplus
}
#endif

#endif /* SRC_DEMO_USBH_USBH_UVC_USBH_DMA_MEMCPY_H_ */
