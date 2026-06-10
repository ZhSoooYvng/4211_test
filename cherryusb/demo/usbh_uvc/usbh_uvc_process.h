/**
 * @file usbh_uvc_process.h
 * @author Product application department
 * @brief  UVC摄像头 处理头文件，仅供参考
 * @version V1.0
 * @date 2025-07-30
 * 
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 * 
 */
#ifndef __USBH_UVC_USBH_UVC_PROCESS_H_
#define __USBH_UVC_USBH_UVC_PROCESS_H_
#ifdef __cplusplus
extern "C"{
#endif

// 头文件包含
#include "main.h"
#include "usbh_core.h"
#include "usbh_hub.h"
#include "usb_musb_reg.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "usbh_video.h"
#include "axidma_hal.h"

//全局变量定义
#define elog_raw(...)		printf(__VA_ARGS__)

//USBH_VIDEO_FORMAT_MJPEG
//USBH_VIDEO_FORMAT_UNCOMPRESSED
/* 设置USB 摄像头的输出格式  YUV / MJPEG   */
#define OUT_FORMAT						USBH_VIDEO_FORMAT_MJPEG

/* 设置摄像头输出图像的 宽 和 高，一定要写为 摄像头自身支持的分辨率 */
#define UVC_TARGET_WIDTH                (1024)
#define UVC_TARGET_HEIGHT               (768)

/* 设定UVC摄像头的输出 帧率 ，用户应根据实际摄像头支持的帧率进行设定
    一般情况下 Mjpeg 格式输出的帧率可以设定为 30Fps ,25Fps ,20FPs , 15Fps, 10Fps, 7.5Fps, 5Fps
    由于YUV数据量较大，越大的分辨率支持的帧率越低，用户可以根据实际情况进行设定,用户可以使用
    PC 软件 UsbTreeView 查看摄像头在指定输出格式下,特定分辨率下的帧率，主要查看 adwFrameInterval 字段
 */
#define UVC_TARGET_FPS			        (30) //(30) 


/* 设置 buffer 队列中 帧图像的 缓存个数 , 根据设置的输出格式, 分辨率，以及可用的DDR 内存决定, 用户根据自身实际需要进行增大或者减小，最少为  1 */
#define FRAME_NUM_PACKETS			    (2)


/*USB 摄像头输出YUV数据的帧大小,以字节为单位  */
#define UVC_UNCOMP_FRAME_SIZE           (UVC_TARGET_WIDTH * UVC_TARGET_HEIGHT * 2)

/* USB 摄像头输出 MJPEG 数据的帧大小,以字节为单位;大小一般是 YUV数据的1/5 ~ 1/10 之间，用户根据实际测试设定，以降低 DDR占用空间*/
#define UVC_MJPEG_FRAME_SIZE           (UVC_UNCOMP_FRAME_SIZE / 5)


#if OUT_FORMAT == USBH_VIDEO_FORMAT_UNCOMPRESSED

#define UVC_MAX_FRAME_SIZE              UVC_UNCOMP_FRAME_SIZE

#elif OUT_FORMAT == USBH_VIDEO_FORMAT_MJPEG

#define UVC_MAX_FRAME_SIZE              UVC_MJPEG_FRAME_SIZE

#endif

/* 摄像头输出YUV / Mjpeg 数据 转换成 RGB888 占用空间的大小, 以为字节为单位 */
#define UVC_FRAME_RGB888_SIZE            (UVC_TARGET_WIDTH * UVC_TARGET_HEIGHT * 3)



#define UVC_HEADER_SIZE_POS             0
#define UVC_HEADER_BIT_FIELD_POS        1

#define UVC_HEADER_FID_BIT              (1 << 0)
#define UVC_HEADER_EOF_BIT              (1 << 1)
#define UVC_HEADER_SIZE                 12

#define UVC_HEADER_ERR_BIT              (1 << 6)

// 函数声明
void video_iso_process(struct usbh_bus *bus, uint8_t ep_idx ,struct usbh_urb * urb ,uint32_t data_len);

#ifdef __cplusplus
}
#endif
#endif /* __USBH_UVC_USBH_UVC_PROCESS_H_ */
