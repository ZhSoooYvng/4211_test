/**
 * @file usb_uvc_lcd.c
 * @author Product application department
 * @brief  UVC摄像头 图像显示文件，仅供参考
 * @version V1.0
 * @date 2025-07-30
 * 
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 * 
 */

// 头文件包含
#include "usb_uvc_lcd.h"
#include "image_decoder_hal.h"
#include "lcd_interface.h"
#include "tp_interface.h"
#include "usbh_uvc_process.h"
#include "dma2d_reg.h"

// 全局变量定义
extern lcd_handle_t lcd_dev;
__attribute__((section("NO_INIT"))) uint8_t g_clcd_bg_buf[LCD_DISP_HOR_RES*LCD_DISP_VER_RES*2] = {0};	

#define writew(addr, value) (*(volatile unsigned short *)(addr))=((unsigned short)(value))


// 函数定义

/**
 * @brief CPU刷屏初始化数据函数
 * 
 * @param buf_addr 刷图地址
 * @param width 刷图宽度
 * @param height 刷图长度
 * @param Color 刷图数据
 * @return int 成功
 */

static void color_rgb565_init(uint32_t buf_addr, uint32_t width, uint32_t height,uint32_t Color)
{
    unsigned int i, j;
    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        writew((buf_addr+(i*width+j)*2), Color);
    }
}
/**
 * @brief CPU刷屏函数
 * 
 * @param buf_addr 刷图地址
 * @param width 刷图宽度
 * @param height 刷图长度
 * @param Color 刷图数据
 * @return int 成功
 */
int color_rgb565_write(uint32_t buf_addr, uint16_t width, uint16_t height, uint16_t *Color)
{
    unsigned int i, j;
    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {			
            writew((buf_addr+(1024*i+j)*2), Color[i*width+j]);
        }
    }
    return 0;
}


/**
 * @brief DMA2D搬运函数
 * 
 * @param pSrc 源地址
 * @param pDst 目标地址
 * @param sx 起始x坐标
 * @param sy 起始y坐标
 * @param ex 终止x坐标
 * @param ey 终止y坐标
 * @param offsetline 刷图行数
 */
static void lcd_rgb_dma2d_fillimage(void* pSrc, void* pDst, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t offsetline)
{
    uint32_t width  = ex - sx;
    uint32_t height = ey - sy;	
    uint32_t OffLineSrc = 0;
    uint32_t OffLineDst = offsetline - width;
    uint32_t addrOffset = 2 * (offsetline * sy + sx);
    /* DMA2D config M2M */
    DMA2D->CR&=~(1<<0);										//先停止DMA2D
    DMA2D->CR = 0x00000000UL;								//存储器到存储器模式 
    DMA2D->FG_ROTA_LAYER = (uint32_t)pSrc;//						//设置源数据的首地址
    DMA2D->OUT_ROTC_LAYER = (uint32_t)pDst+addrOffset;				//设置目标数据的首地址
    DMA2D->FGOR = OffLineSrc;								//设置源数据行偏移量
    DMA2D->OOR = OffLineDst;								//设置输出的行偏移量
    DMA2D->BGOR = OffLineDst; 
    DMA2D->NLR_ROTA_SIZE = (width << 16) | height;					//设定行数寄存器	
    DMA2D->FG_ROTA_CTRL = 0x02;									//设置源数据的颜色格式, 0x01 RGB888, 0x02 RGB565
    DMA2D->BG_ROTB_CTRL  = 0x02;
    DMA2D->OUT_ROTC_CTRL = 0x02;									//设置输出数据的颜色格式
    /* start transmit */
    DMA2D->CR |= ((1<<0) | (1<<16));
    /* wait send complete */
    while (DMA2D->CR & 1<<0) {

    }
}

/**
 * @brief lcd update address with no cache
 * 
 */
static void lcd_rgb_draw(uint32_t *clcd_addr)
{
    lcd_write_dots(&lcd_dev, 0, 0, LCD_DISP_HOR_RES, LCD_DISP_VER_RES, (uint8_t *)clcd_addr);    
}


/**
 * @brief 屏幕初始化函数
 * 
 */
void uvc_lcd_rgb_init(void)
{
    lcd_dev.lcd_itf_type = LCD_RGB;
    if (CCM4211_CHIP_PACKAGE == 2)
    lcd_dev.lcd_ic = RGB_480272;
    else
    lcd_dev.lcd_ic = RGB_1024600;	
    lcd_dev.lcd_size.x = LCD_DISP_HOR_RES;
    lcd_dev.lcd_size.y = LCD_DISP_VER_RES;

    lcd_init(&lcd_dev);	 
    lcd_rgb_draw((uint32_t*)g_clcd_bg_buf);

}
#include "cpm_drv.h"
/**
 * @brief jpeg解码函数
 * 
 * @param img_addr 图像地址
 * @param image_len 图像长度
 * @param out_addr 解码图像输出地址
 */
void uvc_lcd_jpeg_decode(uint32_t img_addr, uint32_t image_len, uint32_t out_addr)
{
	
    uint32_t timeout = 0xffff;
    status_t ret;
    uint8_t *jpeg_image_sdr;
    uint32_t i = 0;
    volatile uint32_t imagedecoder_clcdflg = 0;
    image_decoder_initstructure_t image_decoder_init;

    image_decoder_init.image_type = IMAGE_JPEG;
    image_decoder_init.output_type = JPEG_OUTPUT_RGB565;
    image_decoder_init.image_addr = (uint32_t)img_addr; // 地址必须是8字节对齐
    image_decoder_init.image_len = image_len;
    image_decoder_init.out_addr0 = out_addr;
    image_decoder_init.out_addr1 = 0;
    image_decoder_init.out_addr2 = 0;
    image_decoder_init.alpha = 0; // 输出ARGB才会生效
    image_decoder_init.dither_en = 1;
    image_decoder_init.crop_en = 0;
    image_decoder_init.crop_width = 0;
    image_decoder_init.crop_height = 0;
    image_decoder_init.decoder_irq_en = 0;
    image_decoder_init.image_decoder_irq_cb = NULL;

    ret = hal_imagedecoder_init(&image_decoder_init);
      if (ret != STATUS_OK)
    {
        printf("image decoder error, ret = %d\n", ret);
        return;
    }
    if (image_decoder_init.decoder_irq_en)
    {
        while (imagedecoder_clcdflg == 0)
            ;
        imagedecoder_clcdflg = 0;
    }
    else
    {
        while ((hal_get_imagedecoder_status() != STATUS_OK)&&(timeout != 0))
                {
                    timeout--;
                }
        hal_imagedecoder_clr_status();
    }
  
}

/**
 * @brief lcd dma2d draw 
 * 
 * @param psrc RGB565 or RGB888 source address
 * @param sx   camera X start pixel
 * @param sy   camera Y start pixel
 * @param ex   camera X end pixel
 * @param ey   camera Y end pixel
 * @param offsetline lcd size of horizon 
 */
void uvc_lcd_rgb_dma2d_draw(uint32_t *psrc, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t offsetline)
{
    lcd_rgb_dma2d_fillimage(psrc, g_clcd_bg_buf, sx, sy, ex, ey, offsetline);
    // uvc_axidma_transfer((uint32_t)g_clcd_bg_buf,(uint32_t)image_buf0);
    lcd_rgb_draw((uint32_t*)g_clcd_bg_buf);

}

