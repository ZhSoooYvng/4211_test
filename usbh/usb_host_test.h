/**
 * @file usb_device_test.h
 * @author Product application department
 * @brief  USB 主机模块测试用例头文件，仅供参考
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
 * @defgroup USBHOST USBHOST模块
 * @ingroup ThirdPart
 * @{
 */


#ifndef _USB_HOST_TEST_H
#define _USB_HOST_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

// 头文件包含
#include "type.h"
#include "usb_host_setup.h"

// 全局变量定义
#define DELAY_VALUE 0xFFFFFF

// 函数声明 
void ReadAndWriteTest(void);
void usb_host_demo(void);

#ifdef __cplusplus
}
#endif

#endif  /* _USB_HOST_TEST_H */

/** @} */  // 结束 USBHOST 模块分组
/** @} */  // 结束外设模块分组