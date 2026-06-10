/**
 * @file audio_service.h
 * @author Product application department
 * @brief audio service驱动头文件，供应用层调用，仅供参考
 * @version V1.0
 * @date 2025-02-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __AUDIO_SERVICE_H_
#define __AUDIO_SERVICE_H_

// 头文件包含
#include "audio_hal.h"
#include "stdlib.h"
#include "string.h"

// 全局变量定义
typedef enum
{
    BREAK = 0, // 打断
    UNBREAK,   // 不打断
} playmode_t;

// 函数声明
typedef void (*AudioServiceCallback_t)(uint32_t result);
void audioserviceplay(uint16_t number, playmode_t mode);
void audioservicerouting(void);
void audioservicesetvolume(uint8_t volume);
errorstatus audioserviceinit(AudioServiceCallback_t mCallback);

void audiosystickhandler(void);

#endif
