/**
 * @file usbh_uvc_process.c
 * @author Product application department
 * @brief  UVC摄像头 处理文件，仅供参考
 * @version V1.0
 * @date 2025-07-30
 * 
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 * 
 */

// 头文件包含
#include "usbh_uvc_process.h"
#include "usbh_dma_memcpy.h"

#ifdef USBH_UVC_DEMO

//函数定义
/**
 * @brief usbh_uvc_process.c video_iso_process
 *
 * @param [in] bus     usb host bus
 * @param [in] ep_idx  endpoint index
 * @param [in] urb     usb urb
 * @param [in] buffer_len  buffer length
 *
 * @details
 *    This function is a callback when usbh receive a iso packet.\n
 *    It handle the packet and copy the data to the buffer.\n
 *    When a frame is completed, it send the index of the buffer to the queue.\n
 *    The queue is used to notify the application that a frame is completed.\n
 */
void video_iso_process(struct usbh_bus *bus, uint8_t ep_idx ,struct usbh_urb *urb ,uint32_t buffer_len)
{ 
    volatile uint16_t real_data_len = 0;
    static uint32_t index = 0;
    static uint8_t firstflag = 1;
    static volatile uint8_t last_incomplete_packet_transmitting = 0;
    static volatile uint8_t uvc_prev_packet_eof = 1;
    static volatile uint8_t uvc_prev_fid_state = 0;
    static volatile uint32_t uvc_pack_error_cnt = 0;
    volatile uint8_t masked_fid = 0;
    if(urb->transfer_buffer[0] != 0x0c)
    {
        elog_raw("packet format  recv error \r\n");
        for(uint8_t i = 0 ; i< 12 ;i++)
        {
            elog_raw("%02x ",urb->transfer_buffer[i]);
        }
        elog_raw("\r\n");
        return;//while(1);       
    }
    // 小于等于 12 字节的数据包 不拷贝数据
    if (buffer_len < UVC_HEADER_SIZE)
    {
        // ACK 头
        //uvc_header_cnt++;
    }else
    {
        if(buffer_len == UVC_HEADER_SIZE)
        {
            if(firstflag == 1)
            {
                firstflag = 0;
                uvc_prev_fid_state = (urb->transfer_buffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_FID_BIT);
                uvc_prev_fid_state^=1;
            }
            if((urb->transfer_buffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_EOF_BIT) == 0)
            {
                //普通的 ACK 直接返回无需关注
                return ;
            }else
            {
                if(last_incomplete_packet_transmitting == 1)
                {
                    if(urb->transfer_buffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_EOF_BIT)
                    {
                        masked_fid = (urb->transfer_buffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_FID_BIT); 
                        uvc_prev_fid_state = masked_fid;
                        last_incomplete_packet_transmitting = 0;
                    }
                    return ;
                }
            }
        }else
        {
        }

       //Get FID bit state   
        if(urb->transfer_buffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_ERR_BIT)
        {
            elog_raw("packet recv error current recv cnt %d \r\n",urb->iso_packet[index].actual_length);

            for(uint8_t i = 0 ; i< 12 ;i++)
            {
                elog_raw("%02x ",urb->transfer_buffer[i]);
            }
            elog_raw("\r\n");
            uvc_pack_error_cnt++;
            //while(1);
        }


        masked_fid = (urb->transfer_buffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_FID_BIT);  // bit 0 
        if((masked_fid != uvc_prev_fid_state)&&(urb->transfer_buffer[0]==0x0c) &&(uvc_prev_packet_eof == 1))
        {
            if(xQueueIsQueueFullFromISR((QueueHandle_t)urb->arg) == pdTRUE) // 判断消息队列是否满
            {
                last_incomplete_packet_transmitting = 1;
                if(urb->transfer_buffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_EOF_BIT)
                {
                    uvc_prev_fid_state = masked_fid;
                    last_incomplete_packet_transmitting = 0;
                }
                return;
            }else
            {
                /**  fix bug 
                 * While a frame of data is being discarded, if the message queue is not empty at this moment, 
                 * it will enter this branch and treat it as a new frame, but in fact,
                 *  this data is the latter part of the frame data that is currently being discarded.
                 */
                if(last_incomplete_packet_transmitting == 1)
                {
                    if(urb->transfer_buffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_EOF_BIT)
                    {
                        uvc_prev_fid_state = masked_fid;
                        last_incomplete_packet_transmitting = 0;
                    }
                    return ;
                }
            }
        }
        uvc_prev_fid_state = masked_fid;
        real_data_len = buffer_len - UVC_HEADER_SIZE;  // 得到除去header的实际的图像数据长度
        if((urb->iso_packet[index].actual_length + real_data_len) > UVC_MAX_FRAME_SIZE)
        {
            elog_raw("size is over flow index is %x lenth is %d \r\n",index,urb->iso_packet[index].actual_length);
            for(uint32_t cnt = 0; cnt < 12; cnt++)
            {
                elog_raw("%02x ",urb->iso_packet[index].transfer_buffer[cnt]);
            }
            elog_raw("\r\n");
            while(1);
        }
        
        if(real_data_len != 0)
        {
            memcpy(&urb->iso_packet[index].transfer_buffer[urb->iso_packet[index].actual_length], &urb->transfer_buffer[UVC_HEADER_SIZE], real_data_len);
//            Fast_DMA_Memcpy(1,(uint8_t *)&urb->iso_packet[index].transfer_buffer[urb->iso_packet[index].actual_length], &urb->transfer_buffer[UVC_HEADER_SIZE], real_data_len);
			
//			//cache sync
//			extern void cc_SCB_InvalidateDCache_by_Addr (uint32_t *addr, int32_t dsize);
//			cc_SCB_InvalidateDCache_by_Addr((uint32_t *)(uint8_t *)&urb->iso_packet[index].transfer_buffer[urb->iso_packet[index].actual_length], real_data_len);
			
            urb->iso_packet[index].actual_length += real_data_len;           
        }
        if(urb->transfer_buffer[UVC_HEADER_BIT_FIELD_POS] & UVC_HEADER_EOF_BIT) // 尾包
        {
            uvc_prev_packet_eof= 1;
            // 一帧结束
            if(uvc_pack_error_cnt != 0)
            {
                elog_raw("err packet num is %x \r\n",uvc_pack_error_cnt);
            }
            uvc_pack_error_cnt = 0;
            if(xQueueSendFromISR((QueueHandle_t)urb->arg, &index, NULL) != pdPASS)
            {
                elog_raw("send error %d \r\n",index);while(1);
            }else
            {
                index = (index + 1) % FRAME_NUM_PACKETS;
            }
        }else{
            uvc_prev_packet_eof = 0;
        }
    }
}

#endif