/**
 * @file usb_device_test.h
 * @author Product application department
 * @brief  USB 从机模块测试用例头文件，仅供参考
 * @version V1.0
 * @date 2025-03-02
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 *
 */


/**
 * @defgroup ThirdPart 第三方模块
 * @{
 *
 * @defgroup USBDEVICE USBDEVICE模块
 * @ingroup ThirdPart
 * @{
 */

#ifndef _USB_TEST_H
#define _USB_TEST_H

#ifdef __cplusplus
extern "C"
{
#endif

// 头文件包含
#include "usb_drv.h"
#include "usb_ms_protocol.h"
#include "usb_cdc_protocol.h"
#include "usb_hid_protocol.h"

// 函数声明
void usb_device_demo(void);

#ifdef __cplusplus
}
#endif

/** @} */  // 结束 USBDEVICE 模块分组
/** @} */  // 结束外设模块分组

#endif
