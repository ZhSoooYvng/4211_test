/**
 * @file jpeg_accelerator_test.c
 * @author Product application department
 * @brief  JPEG 加速器配置文件
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

// 头文件包含
#include "jpeg_accelerator_test.h"
#include "jpeg_accele_table.h"
#include "dwt.h"
// 全局变量定义
// #define JPEG_ACCELERATE_TEST

#ifdef JPEG_ACCELERATE_TEST
#define JPEG_LOG_EN (0)

#if (JPEG_LOG_EN)
#define DBG_LOG_JPEG(FORMAT, ...) printf(FORMAT, ##__VA_ARGS__)
#else
#define DBG_LOG_JPEG(FORMAT, ...)
#endif

#define JPEG_USE_TABLE (1)

#define JPEG_AC_SRC_ADDR 0X1C000000
#define JPEG_AC_OUT_ADDR 0X1C400000

#define DWORD_LEN 8
#define JPEG_OUT_MCU_LEN 16

static jfif_info_t jpeg_info = {0};
volatile uint8_t jpeg_axi_dma_inrtFlag = 0;

static axidma_transfer_param_t m_s2p_1_para_t;
static axidma_transfer_param_t m_s2p_2_para_t;
static jpeg_axidma_llp_t jpeg_axi_dma_llp_param_t = {0};
static axidma_handle_t jpeg_axidma_handle;

static __attribute__((aligned(64))) axi_dma_llp_t jpeg_axidma_llp[16] = {0};
static uint32_t jpeg_fixed_src[2][16] = {0};
/*width方向，以RGB888测算，1024/3/16=21;21*16=336;width以336为界，1280需要4次传输
 *width方向，以RGB565测算，1024/2/16=32;32*16=512;width以512为界，1280需要3次传输
 *height方向，800/16=50;
 *所以数组最大为4*50=200；
 */
static uint32_t jpeg_fixed_dst[200][16] = {0};

static uint32_t decoder_time_id=0;
static float decoder_time;

#define WH_KEY(w, h) (((uint64_t)(w) << 32) | (uint64_t)(h))

// 函数定义

/**
 * @brief JEPG 模块加速器使用AXIDMA传输的下一次LLP参数并调用AXIDMA传输函数
 *
 */
void jpeg_axidma_calculate(void)
{
    int i;
    // 给axidma此次传输的llp地址赋值
    for (i = 0; i < 16; i++)
    {
        jpeg_axi_dma_llp_param_t.jpeg_s2p_src_addr[i] = jpeg_fixed_src[jpeg_axi_dma_llp_param_t.jpeg_w_irq % 2][i]; // 轮切SRAM0/1
        jpeg_axi_dma_llp_param_t.jpeg_s2p_dst_addr[i] = jpeg_fixed_dst[jpeg_axi_dma_llp_param_t.jpeg_w_irq][i];     // DST地址向后推
        // DBG_LOG_JPEG("SRC ADDR[%d] -> DST ADDR[%d]:  0X%X -> 0X%X\r\n",i,i,jpeg_axi_dma_llp_param_t.jpeg_s2p_src_addr[i],jpeg_axi_dma_llp_param_t.jpeg_s2p_dst_addr[i]);
    }
    // 判定传输的包axidma_len和llp_len
    if (jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt == 0)
    {
        // width方向无尾包,axidma_len恒定不变，只需要传输m_s2p_1_paraT
        if (jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt != 0)
        {
            // height方向有尾包，最后一行的llp_len需要改变，等于jpeg_h16_remian_cnt
            if (jpeg_axi_dma_llp_param_t.jpeg_w_irq >= (jpeg_axi_dma_llp_param_t.jpeg_arraylen - jpeg_axi_dma_llp_param_t.jpeg_w16_cnt))
                m_s2p_1_para_t.dbg_llp_size = m_s2p_2_para_t.dbg_llp_size = jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt;
        }
        // DBG_LOG_JPEG("m_s2p_1_para_t.dbg_llp_size = %d\r\n",m_s2p_1_para_t.dbg_llp_size);
        // 配置axidma，唤起新一轮传输
        jpeg_axidma_setparam(AXI_DMAC1, m_s2p_1_para_t);
    }
    else
    {
        // width方向有尾包，固定包走m_s2p_1_paraT，尾包走m_s2p_2_paraT
        if (jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt != 0)
        {
            // height方向有尾包，最后一行的llp_len需要改变，等于jpeg_h16_remian_cnt
            if (jpeg_axi_dma_llp_param_t.jpeg_w_irq >= (jpeg_axi_dma_llp_param_t.jpeg_arraylen - (jpeg_axi_dma_llp_param_t.jpeg_w16_cnt + 1)))
                m_s2p_1_para_t.dbg_llp_size = m_s2p_2_para_t.dbg_llp_size = jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt;
        }
        // DBG_LOG_JPEG("m_s2p_1_para_t.dbg_llp_size = %d\r\n",m_s2p_1_para_t.dbg_llp_size);
        // 配置axidma，唤起新一轮传输
        if ((jpeg_axi_dma_llp_param_t.jpeg_w_irq + 1) % (jpeg_axi_dma_llp_param_t.jpeg_w16_cnt + 1))
        {
            // DBG_LOG_JPEG("trans 1\r\n");
            jpeg_axidma_setparam(AXI_DMAC1, m_s2p_1_para_t);
        }
        else
        {
            // DBG_LOG_JPEG("trans 2\r\n");
            jpeg_axidma_setparam(AXI_DMAC1, m_s2p_2_para_t);
        }
    }

    // 推jpeg_w_irq
    jpeg_axi_dma_llp_param_t.jpeg_w_irq++;
    return;
}

/**
 * @brief JPEG 模块加速器模式下，axidma的中断处理函数
 *
 */
void jpeg_axidma_llp_isr()
{
    volatile uint32_t reg;
    volatile uint32_t intStat;
    static uint8_t irq_cnt = 0;
    // DBG_LOG_JPEG("axidma llp mode isr = %d\r\n",irq_cnt++);
    // DBG_LOG_JPEG("jpeg_w_irq = %d\r\n",jpeg_axi_dma_llp_param_t.jpeg_w_irq);
    reg = _reg_read(AXI_DMAC1->COMMONREG_INTSTATUS_L);
    _reg_write(AXI_DMAC1->COMMONREG_INTCLEAR_L, reg);
    intStat = _reg_read(AXI_DMAC1->INT_STATUS_L);

    if (intStat & (1 << 0))
    {
        reg = _reg_read(AXI_DMAC1->CH[0].INT_STATUS_L);
        if (reg & AXI_DMAC_INTBIT_DMA_TFR_DONE)
        {
            _reg_write(AXI_DMAC1->CH[0].INT_CLEAR_L, reg);
            // wirq推至最后一包传输完成，则结束，不再唤起新的axidma传输
            if (jpeg_axi_dma_llp_param_t.jpeg_w_irq >= jpeg_axi_dma_llp_param_t.jpeg_arraylen)
            {
                jpeg_axi_dma_inrtFlag = 1;
                return;
            }
            // 清除JPEG ACCELERATOR中断
            while (!(JPEG_PIC_SR & (0x01)))
                ;
            JPEG_PIC_SR |= 0X01;
            // 计算下一次传输的llp地址并唤起axidma传输
            jpeg_axidma_calculate();
        }
    }
}

/**
 * @brief JEPG 模块配置加速器的AXIDMA传输参数并启动传输函数
 *
 * @param pdmac
 * @param dbg_para
 */
void jpeg_axidma_setparam(axi_dma_t *pdmac, axidma_transfer_param_t dbg_para)
{
    int i;
#if 0
     jpeg_axidma_handle.instance = pdmac;
     jpeg_axidma_handle.channel_num = dbg_para.dbg_ch;
     jpeg_axidma_handle.src_msize = dbg_para.dbg_mSize;//burst size 
     jpeg_axidma_handle.dst_msize = dbg_para.dbg_mSize;//burst size 
     jpeg_axidma_handle.src_tr_width = dbg_para.dbg_transWidth;//transfer width 8 bits
     jpeg_axidma_handle.dst_tr_width = dbg_para.dbg_transWidth;//transfer width 8 bits
     jpeg_axidma_handle.src_inc = AXI_DMAC_SINC_INC;
     jpeg_axidma_handle.dst_inc = AXI_DMAC_DINC_INC;
     jpeg_axidma_handle.src_multblk_type = AXI_DMAC_MULTIBLK_SRC_LINKEDLIST;
     jpeg_axidma_handle.dst_multblk_type = AXI_DMAC_MULTIBLK_DST_LINKEDLIST;
     jpeg_axidma_handle.transfer_type = AXI_DMAC_TRANSFERTYPE_M2M_CONTROLLER_DMA;
     
     jpeg_axidma_handle.src_addr = jpeg_axi_dma_llp_param_t.jpeg_s2p_src_addr[0];    
     jpeg_axidma_handle.dst_addr = jpeg_axi_dma_llp_param_t.jpeg_s2p_dst_addr[0];
     jpeg_axidma_handle.nBlocks = dbg_para.dbg_transLen-1;
     jpeg_axidma_handle.irq_en = 1;
     jpeg_axidma_handle.blk_tfr_en = 0;  
     jpeg_axidma_handle.axidma_cb = jpeg_axidma_llp_isr;    
 
     //config linklist
     for(i=0;i<(dbg_para.dbg_llp_size-1);i++)
     {
         jpeg_axidma_llp[i].SRC_ADDR_L = jpeg_axi_dma_llp_param_t.jpeg_s2p_src_addr[i];
         jpeg_axidma_llp[i].SRC_ADDR_H = 0;
         jpeg_axidma_llp[i].DST_ADDR_L = jpeg_axi_dma_llp_param_t.jpeg_s2p_dst_addr[i];
         jpeg_axidma_llp[i].DST_ADDR_H = 0;
         jpeg_axidma_llp[i].BLOCK_TS   = jpeg_axidma_handle.nBlocks;
         jpeg_axidma_llp[i].CHx_CTL_L  = AXI_DMAC1->CH[jpeg_axidma_handle.channel_num].CTL_L;
         jpeg_axidma_llp[i].CHx_CTL_H  = (AXI_DMAC_LLI_VALID_MASK | AXI_DMAC_LLI_IOC_BLK_MASK);
         jpeg_axidma_llp[i].NEXT_LLI_L = (uint32_t)&jpeg_axidma_llp[i+1];
         jpeg_axidma_llp[i].NEXT_LLI_H = 0;
     }
 
     jpeg_axidma_llp[dbg_para.dbg_llp_size-1].SRC_ADDR_L = jpeg_axi_dma_llp_param_t.jpeg_s2p_src_addr[dbg_para.dbg_llp_size-1];
     jpeg_axidma_llp[dbg_para.dbg_llp_size-1].SRC_ADDR_H = 0;
     jpeg_axidma_llp[dbg_para.dbg_llp_size-1].DST_ADDR_L = jpeg_axi_dma_llp_param_t.jpeg_s2p_dst_addr[dbg_para.dbg_llp_size-1];
     jpeg_axidma_llp[dbg_para.dbg_llp_size-1].DST_ADDR_H = 0;
     jpeg_axidma_llp[dbg_para.dbg_llp_size-1].BLOCK_TS   = jpeg_axidma_handle.nBlocks;
     jpeg_axidma_llp[dbg_para.dbg_llp_size-1].CHx_CTL_L  = AXI_DMAC1->CH[jpeg_axidma_handle.channel_num].CTL_L;
 
     jpeg_axidma_llp[dbg_para.dbg_llp_size-1].CHx_CTL_H  = (AXI_DMAC_LLI_VALID_MASK | AXI_DMAC_LLI_LAST_MASK | AXI_DMAC_LLI_IOC_BLK_MASK);
     jpeg_axidma_llp[dbg_para.dbg_llp_size-1].NEXT_LLI_L = 0;
     jpeg_axidma_llp[dbg_para.dbg_llp_size-1].NEXT_LLI_H = 0;
 
     hal_axidma_init(&jpeg_axidma_handle);
     if((hal_axidma_config_llp(&jpeg_axidma_handle,(uint32_t)&jpeg_axidma_llp[0]) )!=STATUS_OK)
     {
         DBG_LOG_JPEG("axidma llp config error\r\n");
         return;
     }
     jpeg_axi_dma_inrtFlag = 0;
 
     hal_axidma_start(&jpeg_axidma_handle);
     return;
#else
    uint32_t cfg_l_val = 0, cfg_h_val = 0, ctl_l_val = 0;

    // reset AXI DMA
    _reg_set(AXI_DMAC1->RESET_L, AXI_DMA_RESET);
    while (_reg_chk(AXI_DMAC1->RESET_L, AXI_DMA_RESET))
        ;

    // 使能AXI模块
    _reg_set(AXI_DMAC1->CFG_L, AXI_DMA_CFG_EN);

    // 配置MultiBlock传输类型
    cfg_l_val = _reg_read(AXI_DMAC1->CH[0].CFG_L);
    cfg_l_val &= (~(AXI_DMAC_SRC_MULTBLK_MASK | AXI_DMAC_DST_MULTBLK_MASK));

    cfg_l_val |= (((AXI_DMAC_MULTIBLK_SRC_LINKEDLIST << AXI_DMAC_SRC_MULTBLK_SHIFT_MASK) & (AXI_DMAC_SRC_MULTBLK_MASK)) |
                  ((AXI_DMAC_MULTIBLK_DST_LINKEDLIST << AXI_DMAC_DST_MULTBLK_SHIFT_MASK) & (AXI_DMAC_DST_MULTBLK_MASK)));

    _reg_write(AXI_DMAC1->CH[0].CFG_L, cfg_l_val);

    // 配置设备类型、握手
    cfg_h_val = _reg_read(AXI_DMAC1->CH[0].CFG_H);
    cfg_h_val &= (~(AXI_DMAC_TT_FC_MASK | AXI_DMAC_DST_PER_MASK | AXI_DMAC_SRC_PER_MASK |
                    AXI_DMAC_SRC_HS_SEL_MASK | AXI_DMAC_DST_HS_SEL_MASK));
    cfg_h_val |= ((AXI_DMAC_TRANSFERTYPE_M2M_CONTROLLER_DMA << AXI_DMAC_TT_FC_SHIFT_MASK) & (AXI_DMAC_TT_FC_MASK));

    _reg_write(AXI_DMAC1->CH[0].CFG_H, cfg_h_val);

    // CTL寄存器
    ctl_l_val = _reg_read(AXI_DMAC1->CH[0].CTL_L);
    ctl_l_val &= (~(AXI_DMAC_SINC_INC_MASK | AXI_DMAC_DINC_INC_MASK | AXI_DMAC_SRC_TR_WIDTH_MASK |
                    AXI_DMAC_DST_TR_WIDTH_MASK | AXI_DMAC_SRC_MSIZE_MASK | AXI_DMAC_DST_MSIZE_MASK));

    ctl_l_val = ((0 << 0) |
                 (0 << 2) |
                 ((AXI_DMAC_SINC_INC << AXI_DMAC_SINC_INC_SHIFT_MASK) & (AXI_DMAC_SINC_INC_MASK)) |
                 ((AXI_DMAC_DINC_INC << AXI_DMAC_DINC_INC_SHIFT_MASK) & (AXI_DMAC_DINC_INC_MASK)) |
                 ((dbg_para.dbg_transWidth << AXI_DMAC_SRC_TR_WIDTH_SHIFT_MASK) & (AXI_DMAC_SRC_TR_WIDTH_MASK)) |
                 ((dbg_para.dbg_transWidth << AXI_DMAC_DST_TR_WIDTH_SHIFT_MASK) & (AXI_DMAC_DST_TR_WIDTH_MASK)) |
                 ((dbg_para.dbg_mSize << AXI_DMAC_SRC_MSIZE_SHIFT_MASK) & (AXI_DMAC_SRC_MSIZE_MASK)) |
                 ((dbg_para.dbg_mSize << AXI_DMAC_DST_MSIZE_SHIFT_MASK) & (AXI_DMAC_DST_MSIZE_MASK)));

    _reg_write(AXI_DMAC1->CH[0].CTL_L, ctl_l_val);

    _reg_write(AXI_DMAC1->CH[0].BLOCK_TS_L, dbg_para.dbg_transLen - 1);

    jpeg_axidma_handle.axidma_cb = jpeg_axidma_llp_isr;

    AXIDMA_IRQHandler_Callback = jpeg_axidma_handle.axidma_cb;

    // config linklist
    for (i = 0; i < (dbg_para.dbg_llp_size - 1); i++)
    {
        jpeg_axidma_llp[i].SRC_ADDR_L = jpeg_axi_dma_llp_param_t.jpeg_s2p_src_addr[i];
        jpeg_axidma_llp[i].SRC_ADDR_H = 0;
        jpeg_axidma_llp[i].DST_ADDR_L = jpeg_axi_dma_llp_param_t.jpeg_s2p_dst_addr[i];
        jpeg_axidma_llp[i].DST_ADDR_H = 0;
        jpeg_axidma_llp[i].BLOCK_TS = dbg_para.dbg_transLen - 1;
        jpeg_axidma_llp[i].CHx_CTL_L = AXI_DMAC1->CH[0].CTL_L;
        jpeg_axidma_llp[i].CHx_CTL_H = (AXI_DMAC_LLI_VALID_MASK | AXI_DMAC_LLI_IOC_BLK_MASK);
        jpeg_axidma_llp[i].NEXT_LLI_L = (uint32_t)&jpeg_axidma_llp[i + 1];
        jpeg_axidma_llp[i].NEXT_LLI_H = 0;
        // DBG_LOG_JPEG("lli %d set\r\n",i);
        // DBG_LOG_JPEG("SRC ADDR[%d] -> DST ADDR[%d]:  0X%X -> 0X%X\r\n",i,i,jpeg_axidma_llp[i].SRC_ADDR_L,jpeg_axidma_llp[i].DST_ADDR_L);
    }

    jpeg_axidma_llp[dbg_para.dbg_llp_size - 1].SRC_ADDR_L = jpeg_axi_dma_llp_param_t.jpeg_s2p_src_addr[dbg_para.dbg_llp_size - 1];
    jpeg_axidma_llp[dbg_para.dbg_llp_size - 1].SRC_ADDR_H = 0;
    jpeg_axidma_llp[dbg_para.dbg_llp_size - 1].DST_ADDR_L = jpeg_axi_dma_llp_param_t.jpeg_s2p_dst_addr[dbg_para.dbg_llp_size - 1];
    jpeg_axidma_llp[dbg_para.dbg_llp_size - 1].DST_ADDR_H = 0;
    jpeg_axidma_llp[dbg_para.dbg_llp_size - 1].BLOCK_TS = dbg_para.dbg_transLen - 1;
    jpeg_axidma_llp[dbg_para.dbg_llp_size - 1].CHx_CTL_L = AXI_DMAC1->CH[0].CTL_L;

    jpeg_axidma_llp[dbg_para.dbg_llp_size - 1].CHx_CTL_H = (AXI_DMAC_LLI_VALID_MASK | AXI_DMAC_LLI_LAST_MASK | AXI_DMAC_LLI_IOC_BLK_MASK);
    jpeg_axidma_llp[dbg_para.dbg_llp_size - 1].NEXT_LLI_L = 0;
    jpeg_axidma_llp[dbg_para.dbg_llp_size - 1].NEXT_LLI_H = 0;
    // DBG_LOG_JPEG("lli %d set\r\n",dbg_para.dbg_llp_size-1);
    // DBG_LOG_JPEG("SRC ADDR[%d] -> DST ADDR[%d]:  0X%X -> 0X%X\r\n",dbg_para.dbg_llp_size-1,dbg_para.dbg_llp_size-1,jpeg_axidma_llp[dbg_para.dbg_llp_size-1].SRC_ADDR_L,jpeg_axidma_llp[dbg_para.dbg_llp_size-1].DST_ADDR_L);

    AXI_DMAC1->CH[0].LLP_L = (uint32_t)&jpeg_axidma_llp[0];
    AXI_DMAC1->CH[0].LLP_H = 0;

    _reg_write(AXI_DMAC1->CH[0].INT_STATUS_ENABLE_L, AXI_DMAC_INTBIT_DMA_TFR_DONE);
    _reg_set(AXI_DMAC1->CH[0].CTL_H, AXI_DMA_CHx_CTL_IOC_BLKTFR);

    // 使能AXI DMA中断
    _reg_set(AXI_DMAC1->CFG_L, AXI_DMA_CFG_INT_EN);

    // 使能AXI DMA通道
    _reg_set(AXI_DMAC1->CH_EN_L, AXI_DMA_CH_EN_WE(0) | AXI_DMA_CH_EN(0));

    // TODO:NVIC INIT
    NVIC_Init(6, 0, AXIDMA_IRQn, 3);
    return;
#endif
}

/**
 * @brief 32K SRAM,划分为2个16K，用于JPEG加速器；单片16K SRAM中，划分为16个1K，用于AXIDMA，每个1K对应AXIDMA llp的一个节点；
 * AXIDMA传输必须是DWORD对齐，所以单行（1K）内，根据RGB格式占用的字节数不同，传输的实际AXIDMA长度不同；
 * *888：336 -> [21]
 * *565：512 -> [32]
 * @param jfif_info
 * @param jpeg_outset
 * @param data_type
 */
void jpeg_accelerate_axidma_init_para(jfif_info_t *jfif_info, jpeg_outset_t *jpeg_outset, uint8_t data_type)
{
    int i, j, k;
    int comp_cnt;
    uint32_t jpeg_dst_temp_addr;
    // pic param

    jpeg_axi_dma_llp_param_t.pic_w = jfif_info->width;
    jpeg_axi_dma_llp_param_t.pic_h = jfif_info->height;

    if ((data_type == PP_OUTPUT_RGB565) || (data_type == PP_OUTPUT_BGR565))
        jpeg_axi_dma_llp_param_t.jpeg_data_width = 2;
    else if ((data_type == PP_OUTPUT_RGB888) || (data_type == PP_OUTPUT_BGR888))
        jpeg_axi_dma_llp_param_t.jpeg_data_width = 3;
    else if ((data_type == PP_OUTPUT_ABGR888) || (data_type == PP_OUTPUT_ARGB888) || (data_type == PP_OUTPUT_BGRA888) || (data_type == PP_OUTPUT_RGBA888))
        jpeg_axi_dma_llp_param_t.jpeg_data_width = 4;

    // 1K的固定长度
    jpeg_axi_dma_llp_param_t.jpeg_sram_fixed_len = 1024;

    // 根据RGB格式占用的字节数不同，实际传输一行能够使用的长度不同；由于JPEG输出MCU有固定单元长度，这里计算对应格式下传输MCU的上限；
    jpeg_axi_dma_llp_param_t.jpeg_fixed_datatype_len = jpeg_axi_dma_llp_param_t.jpeg_sram_fixed_len / jpeg_axi_dma_llp_param_t.jpeg_data_width / JPEG_OUT_MCU_LEN;
    DBG_LOG_JPEG("jpeg_fixed_datatype_len = %d\r\n", jpeg_axi_dma_llp_param_t.jpeg_fixed_datatype_len);

    // 使用上述得到的对应格式固定MCU上限长度，计算一行的传输次数(单位MCU*DATA WIDTH)，和末尾剩余的传输长度（单位width）
    jpeg_axi_dma_llp_param_t.jpeg_w16_cnt = jpeg_axi_dma_llp_param_t.pic_w / (jpeg_axi_dma_llp_param_t.jpeg_fixed_datatype_len * JPEG_OUT_MCU_LEN);
    jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt = jpeg_axi_dma_llp_param_t.pic_w % (jpeg_axi_dma_llp_param_t.jpeg_fixed_datatype_len * JPEG_OUT_MCU_LEN);

    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_w16_cnt = %d\r\n", jpeg_axi_dma_llp_param_t.jpeg_w16_cnt);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt = %d\r\n", jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt);

    // height方向，由于IP特性，以JPEG_OUT_MCU_LEN为单位计算传输次数
    jpeg_axi_dma_llp_param_t.jpeg_h16_cnt = jpeg_axi_dma_llp_param_t.pic_h / JPEG_OUT_MCU_LEN;
    jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt = jpeg_axi_dma_llp_param_t.pic_h % JPEG_OUT_MCU_LEN;

    // 计算总的传输次数
    if (jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt == 0)
    {
        // width方向没有单独的尾包，只有固定包
        if (jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt == 0)
            jpeg_axi_dma_llp_param_t.jpeg_arraylen = jpeg_axi_dma_llp_param_t.jpeg_w16_cnt * jpeg_axi_dma_llp_param_t.jpeg_h16_cnt; // height方向无尾包
        else
            jpeg_axi_dma_llp_param_t.jpeg_arraylen = jpeg_axi_dma_llp_param_t.jpeg_w16_cnt * (jpeg_axi_dma_llp_param_t.jpeg_h16_cnt + 1); // height方向有尾包
    }
    else
    {
        // width方向有单独的尾包
        if (jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt == 0)
            jpeg_axi_dma_llp_param_t.jpeg_arraylen = (jpeg_axi_dma_llp_param_t.jpeg_w16_cnt + 1) * jpeg_axi_dma_llp_param_t.jpeg_h16_cnt; // height方向无尾包
        else
            jpeg_axi_dma_llp_param_t.jpeg_arraylen = (jpeg_axi_dma_llp_param_t.jpeg_w16_cnt + 1) * (jpeg_axi_dma_llp_param_t.jpeg_h16_cnt + 1); // height方向有尾包
    }

    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_arraylen =%d\r\n", jpeg_axi_dma_llp_param_t.jpeg_arraylen);

    if (jpeg_axi_dma_llp_param_t.jpeg_w16_cnt > 0)
    {
        // width方向无法单次传输完成，需要分两次传输
        // 计算固定传输AXIDMA的长度，单位DWORD
        jpeg_axi_dma_llp_param_t.jpeg_axi_len_1 = (jpeg_axi_dma_llp_param_t.jpeg_fixed_datatype_len * JPEG_OUT_MCU_LEN * jpeg_axi_dma_llp_param_t.jpeg_data_width) / DWORD_LEN;
        // 确认一行的尾包AXIDMA传输是否和DWORD对齐，非对齐则需要多传输一次DWORD
        if ((jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt * jpeg_axi_dma_llp_param_t.jpeg_data_width) % DWORD_LEN == 0)
            jpeg_axi_dma_llp_param_t.jpeg_axi_len_2 = jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt * jpeg_axi_dma_llp_param_t.jpeg_data_width / DWORD_LEN;
        else
            jpeg_axi_dma_llp_param_t.jpeg_axi_len_2 = jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt * jpeg_axi_dma_llp_param_t.jpeg_data_width / DWORD_LEN + 1;
    }
    else
    {
        // width方向单次即可传完，确认一包AXIDMA传输是否和DWORD对齐，非对齐则需要多传输一次DWORD（此处只有尾包，所以是jpeg_axi_len_2）
        if ((jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt * jpeg_axi_dma_llp_param_t.jpeg_data_width) % DWORD_LEN == 0)
            jpeg_axi_dma_llp_param_t.jpeg_axi_len_2 = jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt * jpeg_axi_dma_llp_param_t.jpeg_data_width / DWORD_LEN;
        else
            jpeg_axi_dma_llp_param_t.jpeg_axi_len_2 = jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt * jpeg_axi_dma_llp_param_t.jpeg_data_width / DWORD_LEN + 1;
        jpeg_axi_dma_llp_param_t.jpeg_axi_len_1 = 0;
    }
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_axi_len_1 = %d\r\n", jpeg_axi_dma_llp_param_t.jpeg_axi_len_1);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_axi_len_2 = %d\r\n", jpeg_axi_dma_llp_param_t.jpeg_axi_len_2);
    // 行偏移，单位BYTE
    jpeg_axi_dma_llp_param_t.jpeg_axi_w_offset = jpeg_axi_dma_llp_param_t.pic_w * jpeg_axi_dma_llp_param_t.jpeg_data_width;
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_axi_w_offset = %d\r\n", jpeg_axi_dma_llp_param_t.jpeg_axi_w_offset);

    // 32K SRAM分为两块，各自的首地址；（固定）
    jpeg_axi_dma_llp_param_t.jpeg_axi_start_src_addr_1 = 0x20010000;
    jpeg_axi_dma_llp_param_t.jpeg_axi_start_src_addr_2 = 0x20014000;
    // 输出目标的首地址（动态变化）
    jpeg_axi_dma_llp_param_t.jpeg_axi_start_dst_addr = jpeg_outset->rgb_addr;
    jpeg_dst_temp_addr = jpeg_axi_dma_llp_param_t.jpeg_axi_start_dst_addr;
    // AXIDMA中断次数，记录行和列的中断次数
    jpeg_axi_dma_llp_param_t.jpeg_w_irq = 0;
    jpeg_axi_dma_llp_param_t.jpeg_h_irq = 0;

    // axi param set
    // m_s2p_1_paraT作为完整包传输的参数设置或每行单包传输的传输参数设置
    m_s2p_1_para_t.dbg_ch = 0;
    m_s2p_1_para_t.dbg_transWidth = AXI_DMAC_TRANSWIDTH_64_BITS;           // DWORD
    m_s2p_1_para_t.dbg_mSize = AXI_DMAC_MIZE_16;                           // BURST
    m_s2p_1_para_t.dbg_transLen = jpeg_axi_dma_llp_param_t.jpeg_axi_len_1; // 第一次传输的长度固定使用jpeg_axi_len_1
    if (jpeg_axi_dma_llp_param_t.jpeg_h16_cnt > 0)
        m_s2p_1_para_t.dbg_llp_size = 16;
    else
        m_s2p_1_para_t.dbg_llp_size = jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt;

    // m_s2p_2_paraT作为每行尾包传输的传输参数设置
    if (jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt > 0)
    {
        m_s2p_2_para_t.dbg_ch = 0;
        m_s2p_2_para_t.dbg_transWidth = AXI_DMAC_TRANSWIDTH_64_BITS;
        m_s2p_2_para_t.dbg_mSize = AXI_DMAC_MIZE_16;
        m_s2p_2_para_t.dbg_transLen = jpeg_axi_dma_llp_param_t.jpeg_axi_len_2;
        if (jpeg_axi_dma_llp_param_t.jpeg_h16_cnt > 0)
            m_s2p_2_para_t.dbg_llp_size = 16;
        else
            m_s2p_2_para_t.dbg_llp_size = jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt;
    }

    // SRAM1首地址配置
    for (i = 0; i < 16; i++)
        jpeg_fixed_src[0][i] = jpeg_axi_dma_llp_param_t.jpeg_axi_start_src_addr_1 + jpeg_axi_dma_llp_param_t.jpeg_sram_fixed_len * i;
    // SRAM2首地址配置
    for (i = 0; i < 16; i++)
        jpeg_fixed_src[1][i] = jpeg_axi_dma_llp_param_t.jpeg_axi_start_src_addr_2 + jpeg_axi_dma_llp_param_t.jpeg_sram_fixed_len * i;

    // DST PSRAM首地址配置

    if (jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt == 0)
    {
        for (i = 0; i < jpeg_axi_dma_llp_param_t.jpeg_arraylen; i++)
        {
            if (i % (jpeg_axi_dma_llp_param_t.jpeg_w16_cnt) == 0)
                jpeg_dst_temp_addr = jpeg_axi_dma_llp_param_t.jpeg_axi_start_dst_addr + 16 * jpeg_axi_dma_llp_param_t.jpeg_axi_w_offset * (i / (jpeg_axi_dma_llp_param_t.jpeg_w16_cnt));

            for (j = 0; j < 16; j++)
            {
                jpeg_fixed_dst[i][j] = jpeg_dst_temp_addr + jpeg_axi_dma_llp_param_t.jpeg_axi_len_1 * DWORD_LEN * (i % (jpeg_axi_dma_llp_param_t.jpeg_w16_cnt)) + jpeg_axi_dma_llp_param_t.jpeg_axi_w_offset * j;
            }
        }
    }
    else
    {
        for (i = 0; i < jpeg_axi_dma_llp_param_t.jpeg_arraylen; i++)
        {
            if (i % (jpeg_axi_dma_llp_param_t.jpeg_w16_cnt + 1) == 0)
                jpeg_dst_temp_addr = jpeg_axi_dma_llp_param_t.jpeg_axi_start_dst_addr + 16 * jpeg_axi_dma_llp_param_t.jpeg_axi_w_offset * (i / (jpeg_axi_dma_llp_param_t.jpeg_w16_cnt + 1));

            for (j = 0; j < 16; j++)
            {
                jpeg_fixed_dst[i][j] = jpeg_dst_temp_addr + jpeg_axi_dma_llp_param_t.jpeg_axi_len_1 * DWORD_LEN * (i % (jpeg_axi_dma_llp_param_t.jpeg_w16_cnt + 1)) + jpeg_axi_dma_llp_param_t.jpeg_axi_w_offset * j;
            }
        }
    }
#if 0
    DBG_LOG_JPEG("_______________________jpeg_axi param setting_______________________\r\n");
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.pic_w = 0x%x\r\n", jpeg_axi_dma_llp_param_t.pic_w);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.pic_h = 0x%x\r\n", jpeg_axi_dma_llp_param_t.pic_h);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_data_width = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_data_width);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_w16_cnt = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_w16_cnt);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_h16_cnt = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_h16_cnt);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_axi_len_1 = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_axi_len_1);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_axi_len_2 = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_axi_len_2);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_axi_w_offset = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_axi_w_offset);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_axi_start_src_addr_1 = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_axi_start_src_addr_1);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_axi_start_src_addr_2 = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_axi_start_src_addr_2);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_axi_start_src_addr = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_axi_start_src_addr);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_axi_start_dst_addr = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_axi_start_dst_addr);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_w_irq = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_w_irq);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_h_irq = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_h_irq);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_sram_fixed_len = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_sram_fixed_len);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_fixed_datatype_len = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_fixed_datatype_len);
    DBG_LOG_JPEG("jpeg_axi_dma_llp_param_t.jpeg_arraylen = 0x%x\r\n", jpeg_axi_dma_llp_param_t.jpeg_arraylen);

    DBG_LOG_JPEG("_______________________m_s2p_1_para_t param setting_______________________\r\n");
    DBG_LOG_JPEG("m_s2p_1_para_t.dbg_ch = 0x%x\r\n", m_s2p_1_para_t.dbg_ch);
    DBG_LOG_JPEG("m_s2p_1_para_t.dbg_transWidth = 0x%x\r\n", m_s2p_1_para_t.dbg_transWidth);
    DBG_LOG_JPEG("m_s2p_1_para_t.dbg_mSize = 0x%x\r\n", m_s2p_1_para_t.dbg_mSize);
    DBG_LOG_JPEG("m_s2p_1_para_t.dbg_transLen = 0x%x\r\n", m_s2p_1_para_t.dbg_transLen);
    DBG_LOG_JPEG("m_s2p_1_para_t.dbg_llp_size = 0x%x\r\n", m_s2p_1_para_t.dbg_llp_size);

    DBG_LOG_JPEG("_______________________m_s2p_2_para_t param setting_______________________\r\n");
    DBG_LOG_JPEG("m_s2p_2_para_t.dbg_ch = 0x%x\r\n", m_s2p_2_para_t.dbg_ch);
    DBG_LOG_JPEG("m_s2p_2_para_t.dbg_transWidth = 0x%x\r\n", m_s2p_2_para_t.dbg_transWidth);
    DBG_LOG_JPEG("m_s2p_2_para_t.dbg_mSize = 0x%x\r\n", m_s2p_2_para_t.dbg_mSize);
    DBG_LOG_JPEG("m_s2p_2_para_t.dbg_transLen = 0x%x\r\n", m_s2p_2_para_t.dbg_transLen);
    DBG_LOG_JPEG("m_s2p_2_para_t.dbg_llp_size = 0x%x\r\n", m_s2p_2_para_t.dbg_llp_size);

    DBG_LOG_JPEG("_______________________jpeg_w_fixed_src param setting_______________________\r\n");
    for (i = 0; i < 16; i++)
    {
        DBG_LOG_JPEG("jpeg_fixed_src[0][%d] = 0x%x\r\n", i, jpeg_fixed_src[0][i]);
    }
    for (i = 0; i < 16; i++)
    {
        DBG_LOG_JPEG("jpeg_fixed_src[1][%d] = 0x%x\r\n", i, jpeg_fixed_src[1][i]);
    }

    DBG_LOG_JPEG("_______________________jpeg_h_fixed_dst param setting_______________________\r\n");
    for (i = 0; i < jpeg_axi_dma_llp_param_t.jpeg_arraylen; i++)
    {
        DBG_LOG_JPEG("array %d\r\n", i);
        for (j = 0; j < 16; j++)
        {
            DBG_LOG_JPEG("jpeg_fixed_dst[%d][%d] = 0X%X\r\n", i, j, jpeg_fixed_dst[i][j]);
        }
    }
    while (1)
        ;
#endif
}

static inline void jpeg_apply_llp(const jpeg_axidma_llp_t *cfg) {
    jpeg_axi_dma_llp_param_t = *cfg;
}


void jpeg_ac_param_set(jfif_info_t *jfif_info, jpeg_outset_t *jpeg_outset)
{
    int i,j;

    uint16_t width= jfif_info->width;
    uint16_t height= jfif_info->height;
    uint8_t data_type = jpeg_outset->format;
    uint32_t addr = jpeg_outset->rgb_addr;

    DBG_LOG_JPEG("w = %d, h = %d, data type = 0x%x,addr = 0x%x\r\n", width, height, data_type, addr);

    switch (WH_KEY(width, height))
    {
    case WH_KEY(480, 272):
        if ((data_type == PP_OUTPUT_RGB565) || (data_type == PP_OUTPUT_BGR565))
        {
            DBG_LOG_JPEG("480272 RGB565\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_480272_RGB565);
            for(i=0;i<17;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_480272_RGB565[i][j];
                }   
            }
        }
        else if ((data_type == PP_OUTPUT_RGB888) || (data_type == PP_OUTPUT_BGR888))
        {
            DBG_LOG_JPEG("480272 RGB888\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_480272_RGB888);
            for(i=0;i<34;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_480272_RGB888[i][j];
                }   
            }
        }
        break;
    case WH_KEY(480,320):
        if ((data_type == PP_OUTPUT_RGB565) || (data_type == PP_OUTPUT_BGR565))
        {
            DBG_LOG_JPEG("480320 RGB565\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_480320_RGB565);
            for(i=0;i<20;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_480320_RGB565[i][j];
                }   
            }
        }
        else if ((data_type == PP_OUTPUT_RGB888) || (data_type == PP_OUTPUT_BGR888))
        {
            DBG_LOG_JPEG("480320 RGB888\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_480320_RGB888);
            for(i=0;i<40;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_480320_RGB888[i][j];
                }   
            }
        }
        break;
    case WH_KEY(640,480):
        if ((data_type == PP_OUTPUT_RGB565) || (data_type == PP_OUTPUT_BGR565))
        {
            DBG_LOG_JPEG("640480 RGB565\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_640480_RGB565);
            for(i=0;i<60;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_640480_RGB565[i][j];
                }   
            }
        }
        else if ((data_type == PP_OUTPUT_RGB888) || (data_type == PP_OUTPUT_BGR888))
        {
            DBG_LOG_JPEG("640480 RGB888\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_640480_RGB888);
            for(i=0;i<60;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_640480_RGB888[i][j];
                }   
            }
        }
        break;
    case WH_KEY(800,480):
        if ((data_type == PP_OUTPUT_RGB565) || (data_type == PP_OUTPUT_BGR565))
        {
            DBG_LOG_JPEG("800480 RGB565\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_800480_RGB565);
            for(i=0;i<60;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_800480_RGB565[i][j];
                }   
            }
        }
        else if ((data_type == PP_OUTPUT_RGB888) || (data_type == PP_OUTPUT_BGR888))
        {
            DBG_LOG_JPEG("800480 RGB888\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_800480_RGB888);
            for(i=0;i<90;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_800480_RGB888[i][j];
                }   
            }
        }
        break;
    case WH_KEY(1024,600):
        if ((data_type == PP_OUTPUT_RGB565) || (data_type == PP_OUTPUT_BGR565))
        {
            DBG_LOG_JPEG("1024600 RGB565\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_1024600_RGB565);
            for(i=0;i<76;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_1024600_RGB565[i][j];
                }   
            }
        }
        else if ((data_type == PP_OUTPUT_RGB888) || (data_type == PP_OUTPUT_BGR888))
        {
            DBG_LOG_JPEG("1024600 RGB888\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_1024600_RGB888);
            for(i=0;i<152;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_1024600_RGB888[i][j];
                }   
            }
        }
        break;
    case WH_KEY(1280,720):
        if ((data_type == PP_OUTPUT_RGB565) || (data_type == PP_OUTPUT_BGR565))
        {
            DBG_LOG_JPEG("1280720 RGB565\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_1280720_RGB565);
            for(i=0;i<135;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_1280720_RGB565[i][j];
                }   
            }
        }
        else if ((data_type == PP_OUTPUT_RGB888) || (data_type == PP_OUTPUT_BGR888))
        {
            DBG_LOG_JPEG("1280720 RGB888\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_1280720_RGB888);
            for(i=0;i<180;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_1280720_RGB888[i][j];
                }   
            }
        }
        break;
    case WH_KEY(1280,800):
        if ((data_type == PP_OUTPUT_RGB565) || (data_type == PP_OUTPUT_BGR565))
        {
            DBG_LOG_JPEG("1280800 RGB565\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_1280800_RGB565);
            for(i=0;i<150;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_1280800_RGB565[i][j];
                }   
            }
        }
        else if ((data_type == PP_OUTPUT_RGB888) || (data_type == PP_OUTPUT_BGR888))
        {
            DBG_LOG_JPEG("1280800 RGB888\r\n");
            jpeg_apply_llp(&jpeg_axidma_llp_1280800_RGB888);
            for(i=0;i<200;i++)
            {
                for(j=0;j<16;j++)
                {
                    jpeg_fixed_dst[i][j] = addr + jpeg_fixed_dst_1280800_RGB888[i][j];
                }   
            }
        }
        break;
    default:
        DBG_LOG_JPEG("wh no param,need add\r\n");
        // 其他
        break;
    }

    // m_s2p_1_paraT作为完整包传输的参数设置或每行单包传输的传输参数设置
    m_s2p_1_para_t.dbg_ch = 0;
    m_s2p_1_para_t.dbg_transWidth = AXI_DMAC_TRANSWIDTH_64_BITS;           // DWORD
    m_s2p_1_para_t.dbg_mSize = AXI_DMAC_MIZE_16;                           // BURST
    m_s2p_1_para_t.dbg_transLen = jpeg_axi_dma_llp_param_t.jpeg_axi_len_1; // 第一次传输的长度固定使用jpeg_axi_len_1
    if (jpeg_axi_dma_llp_param_t.jpeg_h16_cnt > 0)
        m_s2p_1_para_t.dbg_llp_size = 16;
    else
        m_s2p_1_para_t.dbg_llp_size = jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt;

    // m_s2p_2_paraT作为每行尾包传输的传输参数设置
    if (jpeg_axi_dma_llp_param_t.jpeg_w16_remian_cnt > 0)
    {
        m_s2p_2_para_t.dbg_ch = 0;
        m_s2p_2_para_t.dbg_transWidth = AXI_DMAC_TRANSWIDTH_64_BITS;
        m_s2p_2_para_t.dbg_mSize = AXI_DMAC_MIZE_16;
        m_s2p_2_para_t.dbg_transLen = jpeg_axi_dma_llp_param_t.jpeg_axi_len_2;
        if (jpeg_axi_dma_llp_param_t.jpeg_h16_cnt > 0)
            m_s2p_2_para_t.dbg_llp_size = 16;
        else
            m_s2p_2_para_t.dbg_llp_size = jpeg_axi_dma_llp_param_t.jpeg_h16_remian_cnt;
    }

    memcpy(jpeg_fixed_src,
           jpeg_fixed_src_sram,
           sizeof(jpeg_fixed_src_sram));
}

/**
 * @brief JPEG 模块加速器模式下，解码jpeg图片函数
 *
 * @param jpeg_decoder_init jpeg解码器初始化结构体
 */
void jpeg_ac_decode(jpeg_accelerator_initstructure_t *jpeg_decoder_init)
{
    uint8_t *jpeg_image_sdr;
    jpeg_outset_t jpeg_outset;

    int i;
    if (jpeg_decoder_init->xip_src)
    {
        jpeg_image_sdr = (uint8_t *)jpeg_decoder_init->image_addr;
    }
    else
    {
        jpeg_image_sdr = (uint8_t *)JPEG_AC_SRC_ADDR;
        memcpy(jpeg_image_sdr, (uint8_t *)jpeg_decoder_init->image_addr, jpeg_decoder_init->image_len);
        for (i = 0; i < jpeg_decoder_init->image_len; i++)
        {
            if (jpeg_image_sdr[i] != *(uint8_t *)(jpeg_decoder_init->image_addr + i))
            {
                DBG_LOG_JPEG("memcpy data error\r\n");
                while (1)
                    ;
            }
        }
    }
    DBG_LOG_JPEG("JPEG DATA ADDR = 0X%X\r\n", (uint32_t)jpeg_image_sdr);
    if (hal_jpeg_parse(jpeg_image_sdr, jpeg_decoder_init->image_len, &jpeg_info) != JFIF_RES_OK)
    {
        DBG_LOG_JPEG("format error\r\n");
        while (1)
            ;
    }

    if (jpeg_decoder_init->is_rgb)
    {
        jpeg_outset.output_rgb = 1;
        jpeg_outset.format = jpeg_decoder_init->out_rgb_datatype;
        jpeg_outset.rgb_addr = jpeg_decoder_init->out_addr0;
    }
    else
    {
        jpeg_outset.output_rgb = 0;
        jpeg_outset.format = jpeg_decoder_init->out_yuv_datatype;
        jpeg_outset.y_addr = jpeg_decoder_init->out_addr0;
        jpeg_outset.u_addr = jpeg_decoder_init->out_addr1;
        jpeg_outset.v_addr = jpeg_decoder_init->out_addr2;
    }

    jpeg_outset.crop_enable = DISABLE;
    jpeg_outset.crop_width = 0;
    jpeg_outset.crop_height = 0;
    jpeg_outset.output_alpha = jpeg_decoder_init->alpha;
#if JPEG_USE_TABLE
    jpeg_ac_param_set(&jpeg_info, &jpeg_outset);
#else
    jpeg_accelerate_axidma_init_para(&jpeg_info, &jpeg_outset, jpeg_outset.format);
#endif
    // 复位jpeg加速器
    hal_jpeg_accelerate_reset();

    // 初始化jpeg解码器

    hal_jpeg_init(0);
    hal_jpeg_decode_set(&jpeg_info, &jpeg_outset);

    // jpeg加速中断必须打开，以触发配置axidma第一次搬运
    hal_jpeg_accelerate_set(&jpeg_info, &jpeg_outset);
    hal_jpeg_accelerate_start();

    // 等待axidma全部完成标志
    while (!jpeg_axi_dma_inrtFlag)
        ;
    jpeg_axi_dma_inrtFlag = 0;

    // 等待jpeg模块解码完成
    while (hal_get_imagedecoder_status() != STATUS_OK)
        ;
    hal_imagedecoder_clr_status();
    // 复位jpeg加速器
    hal_jpeg_accelerate_reset();

    // printf("!!!!!!!!!!!!!!!!!!!!!!jpeg decode end!!!!!!!!!!!!!!!!!!!!!!\r\n");
}

void JPEG_ACCELERATOR_IRQHandler(void)
{
    // clr irq status,disable jpeg accelerator irq
    JPEG_PIC_SR |= 0X01;
    JPEG_PIC_CONFIG &= ~(0x01 << 1);
    // 配置第一次axidma以tiggeraxidma中断，之后数据搬运由axidma中断自行触发
    jpeg_axidma_calculate();
    return;
}

/**
 * @brief JEPG 模块加速解码初始化函数
 *
 */
void jpeg_app_accelerate_decode(void)
{
    DWT_Init(); // DWT初始化
    decoder_time_id = DWT_Record_Start();
    jpeg_accelerator_initstructure_t jpeg_decoder_init;
    jpeg_decoder_init.is_rgb = 1;
    jpeg_decoder_init.xip_src = 1;
    jpeg_decoder_init.out_rgb_datatype = PP_OUTPUT_RGB565; // PP_OUTPUT_RGB565 PP_OUTPUT_RGB888
    jpeg_decoder_init.image_addr = (uint32_t)jpeg_image_data;
    jpeg_decoder_init.image_len = jpeg_image_data_len;
    jpeg_decoder_init.out_addr0 = JPEG_AC_OUT_ADDR;
    jpeg_ac_decode(&jpeg_decoder_init);
    decoder_time = DWT_Record_End("DECODER:", decoder_time_id);
    printf("DECODER: %f ms\r\n", decoder_time);
}

/**
 * @brief JEPG 模块加速解码测试函数
 *
 */
void jpeg_accelerate_demo(void)
{
    jpeg_app_accelerate_decode();
}
#endif

/** @} */ // 结束 ACCELERATOR 模块分组
/** @} */ // 结束外设模块分组
