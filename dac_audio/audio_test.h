/**
 * @file audio_test.h
 * @author Product application department
 * @brief audio 测试用例头文件，仅供参考
 * @version V1.0
 * @date 2025-02-17
 * 
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 * 
 */

/**
 * @defgroup ThirdPart 第三方模块
 * @{
 *
 * @defgroup DACAUDIO DACAUDIO模块
 * @ingroup ThirdPart
 * @{
 */

#ifndef __AUDIO_DEMO_H__
#define __AUDIO_DEMO_H__

// 头文件包含
#include "debug.h"
#include "systick_drv.h"
#include "audio_hal.h"
#include "audio_service.h"

// 函数声明
void audio_demo(void);


/** @} */  // 结束 DACAUDIO 模块分组
/** @} */  // 结束外设模块分组

#endif
