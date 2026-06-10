/**
 * @file main.c
 * @author Product Application Department
 * @brief  主程序文件
 * @version V1.0
 * @date 2025-03-04
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved.
 *
 */
// 头文件包含
#include "sys.h"
#include "main.h"
#include "env_vir.h"
#include "circular_queue.h"
#include "libpsram.h"
#include "pinswap_drv.h"
#include "eport_hal.h"
#include "app/tflm_app.h"

uint8_t uart_rx1_buf[UART_BUFFER_SIZE];
uint8_t uart_ringbuff1[UART_BUFFER_SIZE] = {0};

uint8_t uart_rx2_buf[UART_BUFFER_SIZE];
uint8_t uart_ringbuff2[UART_BUFFER_SIZE] = {0};

/*接收环形缓冲区的132个数据*/
char usart_data_receive[256]={0};
/*温度矩阵为8*8共64个温度数据*/
float thermopile_data[8][8] = {0};

float env_temperature = 0.0f;

void UART4_DMA_INIT()
{
    uint8_t dma_channel;
    uint16_t ret;
		uart_handle_t g_uart_handle;
    g_uart_handle.Init.BaudRate = 115200;
    g_uart_handle.Init.IPSFreq = g_ips_clk;
    g_uart_handle.Init.Parity = UART_PARITY_MODE_NONE;
    g_uart_handle.Init.DatabitsLength = UART_DATA_BITS_LENGTH_8;
    g_uart_handle.Init.StopbitsLength = UART_STOP_BITS_LENGTH_1;
    g_uart_handle.Instance = UART4;
    g_uart_handle.Init.IdleEn = ENABLE;
    g_uart_handle.Init.RxFifoEn = ENABLE;
    g_uart_handle.Init.TxFifoEn = DISABLE;
    g_uart_handle.Init.RxFifoFull = UART_FIFO_1_8;
    hal_uart_init(&g_uart_handle);
    hal_uart_setrxfifotiemout(UART4, 15);
    hal_uart_enrxdma(UART4);
    hal_uart_receiveit(&g_uart_handle, uart_rx1_buf, UART_BUFFER_SIZE);
    hal_uart_itinit();
    dma_channel = 1;
    ret = hal_uart_dmareceive_ringbuf_init(UART4, dma_channel, uart_ringbuff1, UART_BUFFER_SIZE);
}

void UART2_DMA_INIT()
{
    uint8_t dma_channel;
    uint16_t ret;
		uart_handle_t g_uart_handle;
    g_uart_handle.Init.BaudRate = 115200;
    g_uart_handle.Init.IPSFreq = g_ips_clk;
    g_uart_handle.Init.Parity = UART_PARITY_MODE_NONE;
    g_uart_handle.Init.DatabitsLength = UART_DATA_BITS_LENGTH_8;
    g_uart_handle.Init.StopbitsLength = UART_STOP_BITS_LENGTH_1;
    g_uart_handle.Instance = UART2;
    g_uart_handle.Init.IdleEn = ENABLE;
    g_uart_handle.Init.RxFifoEn = ENABLE;
    g_uart_handle.Init.TxFifoEn = DISABLE;
    g_uart_handle.Init.RxFifoFull = UART_FIFO_1_8;
    hal_uart_init(&g_uart_handle);
    hal_uart_setrxfifotiemout(UART2, 15);
    hal_uart_enrxdma(UART2);		
    hal_uart_receiveit(&g_uart_handle, uart_rx2_buf, UART_BUFFER_SIZE);
    hal_uart_itinit();
    dma_channel = 0;
    ret = hal_uart_dmareceive_ringbuf_init(UART2, dma_channel, uart_ringbuff2, UART_BUFFER_SIZE);
}

// 函数定义
/**
 * @brief 恢复芯片出厂模式接口函数
 *
 */
static void reback_boot(void)
{
    uint8_t value_level;
    SwitchPinFunction(WAKEUP_FUN, EPORT6_PIN3_FUN);
    hal_eport_input(EPORT6, EPORT6_PIN3, PS_PULL_UP);
    value_level = hal_eport_readpinslevel(EPORT6, EPORT6_PIN3);
    if (value_level == BIT_RESET)
    {
        sys_startupconfig(Startup_From_ROM, ENABLE);
        while (1)
            ;
    }
}

/**
 * @brief 打印固件版本信息函数
 *
 */
static void printfversion(void)
{

    printf("/***************************************************/\r\n");
    printf(" date = %s,time = %s    \r\n", (char *)__DATE__, (char *)__TIME__);
    printf(" C*CORE CCM4211 FW V1.1.0  \r\n");
    printf(" core clk    = %dMHz         \r\n", g_core_clk / 1000000);
    printf(" system clk = %dMHz         \r\n", g_sys_clk / 1000000);
    printf(" ips clk    = %dMHz         \r\n", g_ips_clk / 1000000);
    printf("/***************************************************/\r\n");
}

/**
 * @brief SDK 测试用例接口函数
 *
 */
void app_demo(void)
{

#ifdef ALGO_DEMO
    algo_demo();
#endif

#ifdef UART_DEMO
    uart_demo();
#endif

#ifdef CAN_DEMO
    can_demo();
#endif

#ifdef PWMT_DEMO
    pwmt_demo();
#endif

#ifdef PIT_DEMO
    pit32_demo();
#endif

#ifdef TC_DEMO
    tc_demo();
#endif

#ifdef WDT_DEMO
    wdt_demo();
#endif

#ifdef EPORT_DEMO
    eport_demo();
#endif

#ifdef SPI_DEMO
    spi_demo();
#endif

#ifdef ADC_DEMO
    adc_demo();
#endif

#ifdef DAC_DEMO
    dac_demo();
#endif

#ifdef RTC_DEMO
    rtc_demo();
#endif

#ifdef I2C_DEMO
    i2c_demo();
#endif

#ifdef I2S_DEMO
    i2s_demo();
#endif

#ifdef RESET_DEMO
    reset_demo();
#endif

#ifdef LOWPOWER_DEMO
    lowerpower_demo();
#endif

#ifdef SSI_DEMO
    ssi_demo();
#endif

#ifdef SIDO_SD_TEST
    sdio_sd_test();
    // sdio_fatfs_test();
#endif

#ifdef ETH_DEMO
    eth_demo();
#endif

#ifdef FREERTOS_DEMO
    freertos_demo();
#endif

#ifdef IMGDEC_DEMO
    image_decoder_demo();
#endif

#ifdef AXIDMA_DEMO
    axidma_demo();
#endif

#ifdef I8080_DEMO
    i8080_demo();
#endif

#ifdef LCDC_DEMO
    lcdc_demo();
#endif

#ifdef TOUCH_DEMO
    touch_demo();
#endif

#ifdef DMA2D_DEMO
    dma2d_demo();
#endif

#ifdef JPEG_ACCELERATE_DEMO
    jpeg_accelerate_demo();
#endif

#ifdef USB_DEVICE_DEMO
    usb_device_demo();
#endif
#ifdef USB_HOST_DEMO
    usb_host_demo();
#endif

#ifdef DAC_AUDIO_DEMO
    audio_demo();
#endif

#ifdef NAND_FLASH_DEMO
    nandflash_demo();
#endif

#ifdef IAP_DEMO
    iap_demo();
#endif

#ifdef LVGL_DEMO
    lvgl_demo();
#endif

#ifdef USBH_UVC_DEMO
    usbh_uvc_demo();
#endif

#ifdef FREETYPE_DEMO
    freetype_demo();
#endif
}

/*环形队列相关变量声明*/
QueueType_t circular_queue;
uint8_t queue_buf[CIRCULAR_LEN];

void char_send(uint8_t ch){
		while ((UART4->SR1 & UART_SR1_TDRE_MASK) == 0x00);
		UART4->DRL = ch;
		while ((UART4->SR1 & UART_SR1_TC_MASK) == 0x00);
}

/*对温度矩阵进行转置*/
/*并将数据顺序和上位机对齐*/
static void temp_data_exc(float source_data[8][8]){
	/*设置置换缓冲区*/
	float temp_data_exc_buffer[8][8]={0};
	for(uint8_t i=0;i<8;i++){
		for(uint8_t j=0;j<8;j++){
			temp_data_exc_buffer[i][j] = source_data[i][j];
		}
	}
	/*温度矩阵转置*/
	/*第一行*/
	for(uint8_t k=0;k<8;k++){
		source_data[0][k] = temp_data_exc_buffer[k][7];
	}
	for(uint8_t k=0;k<8;k++){
		source_data[1][k] = temp_data_exc_buffer[k][6];
	}
	for(uint8_t k=0;k<8;k++){
		source_data[2][k] = temp_data_exc_buffer[k][5];
	}
	for(uint8_t k=0;k<8;k++){
		source_data[3][k] = temp_data_exc_buffer[k][4];
	}
	for(uint8_t k=0;k<8;k++){
		source_data[4][k] = temp_data_exc_buffer[k][3];
	}
	for(uint8_t k=0;k<8;k++){
		source_data[5][k] = temp_data_exc_buffer[k][2];
	}
	for(uint8_t k=0;k<8;k++){
		source_data[6][k] = temp_data_exc_buffer[k][1];
	}
	for(uint8_t k=0;k<8;k++){
		source_data[7][k] = temp_data_exc_buffer[k][0];
	}
}


/*data_store为环形队列数据接收缓冲区,类型为char*/
/*temp_data为温度矩阵,类型为float*/
static void thermopile_data_solve(char data_store[132],float temp_data[8][8]){
	/*代表当前环形队列无数据*/
	if(QueueCount(&circular_queue)==0){
		//printf("data_buffer_clear\r\n");
		return;
	}else{
		/*从环形队列取出数据*/
		QueuePopArray(&circular_queue,data_store,132);
		/*将环形队列中获取的数据处理为温度数据*/
		/*首先前两个数据为帧头0xaa 0xbb*/
		for(uint8_t i=0;i<2;i++){
			if(data_store[0]!=0xaa){
				printf("frame_head_error\r\n");
				return;
			}
			if(data_store[1]!=0xbb){
				printf("frame_head_error\r\n");
				return;
			}
		}
		/*其次是128个温度数据*/
		for(uint8_t i=2;i<130;i+=2){
			temp_data[((i-2)/2)/8][((i-2)/2)%8] = ((((uint16_t)data_store[i])&0x00ff) + ((data_store[i+1]<<8)&0xff00))/100.0f;
//			printf("data_source[%d]=%x,data_sourcep[%d]=%x\r\n",i,data_store[i],i+1,data_store[i+1]);
//			printf("temp_data=%.2f\r\n",temp_data[((i-2)/2)/8][((i-2)/2)%8]);
		}
		/*最后两个数据为环境温度*/
		env_temperature = ((((uint16_t)data_store[130])&0x00ff) + (data_store[131]<<8&0xff00))/100.0f;
		/*对矩阵进行转置*/
		temp_data_exc(temp_data);
		printf("\r\n");
		/*将温度矩阵通过串口输出*/
		for(uint8_t i=0;i<8;i++){
			for(uint8_t j=0;j<8;j++){
				printf("%.2f    ",temp_data[i][j]);
				if(j==7){
					printf("\r\n");
				}
			}
		}
		printf("env_temperature=%.2f\r\n",env_temperature);
	}
	hello_world_test();
//	char_send(0xaa);
//	char_send(0xbb);
////	temp_data[0][0]=0;
////	temp_data[0][1]=0;
//	for(uint8_t i=0;i<8;i++){
//		for(uint8_t j=0;j<8;j++){
////			char_send((uint8_t)((uint16_t)roundf((temp_data[i][j]*100))&0x00ff));
////			char_send((uint8_t)(((uint16_t)roundf((temp_data[i][j]*100))&0xff00)>>8));
//			/*上位机方向测试*/
//			char_send((uint8_t)((uint16_t)roundf(((8*i+j)*100))&0x00ff));
//			char_send((uint8_t)(((uint16_t)roundf(((8*i+j)*100))&0xff00)>>8));
//		}
//	}
//	char_send(((uint8_t)(2500&0x00ff)));
//	char_send((uint8_t)((2500&0xff00)>>8));
	
}


/**
 * @brief 主函数入口
 *
 * @return int
 */
int main(void)
{
    // 获取系统时钟
    sys_cpm_getclk();
    // 恢复boot接口，若调整函数接口位置，请仿真调试没问题后再调整
		/*板子不同导致该地方可能会卡住，需要更改判断中的电平状态*/
    reback_boot();
    // 初始化UART1, 默认波特率115200，8个有效数据位，无校验
    sys_uartdebuginit(UART4, g_ips_clk, 115200);
    delayms(1000);
    /*环形队列初始化*/
    QueueInit(&circular_queue,queue_buf,CIRCULAR_LEN);
    /*切换引脚功能*/
    SwitchPinFunction(CLCD_DATA19_FUN,EPORT9_PIN3_FUN);	
    hal_eport_output(EPORT9, EPORT9_PIN3);
		hal_eport_writepinlevel(EPORT9, EPORT9_PIN3, GPIO_HIGH);
    //UART4_DMA_INIT();
    //UART2_DMA_INIT();
    //SDK模块测试用例
		printfversion();
    /*UART初始化*/
    app_demo();

    //hello_world_test();
    while (1)
    {
			/*在此对数据进行处理*/
			thermopile_data_solve(&usart_data_receive[0],thermopile_data);
    };

    return 0;
}
