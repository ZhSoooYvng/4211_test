/**
 * @file usbh_uvc_test.h
 * @author Product application department
 * @brief  UVC摄像头 测试用例头文件，仅供参考
 * @version V1.0
 * @date 2025-07-30
 * 
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 * 
 */

#ifndef __USBH_USBH_UVC_USBH_UVC_DEMO_H_
#define __USBH_USBH_UVC_USBH_UVC_DEMO_H_


 /**
 * @defgroup ThirdPart 第三方模块
 * @{
 *
 * @defgroup UVC UVC模块
 * @ingroup ThirdPart
 * @{
 */
#ifdef __cplusplus
extern "C"{
#endif


// 头文件包含
#include "main.h"
#include "user_config.h"
#include "usbh_core.h"
#include "usbh_uvc_process.h"

#ifdef USBH_UVC_DEMO
// 函数声明
void usbh_uvc_demo(void);

#endif

#ifdef __cplusplus
}
#endif
#endif /* __USBH_USBH_UVC_USBH_UVC_DEMO_H_ */

/** @} */  // 结束 UVC 模块分组
/** @} */  // 结束外设模块分组