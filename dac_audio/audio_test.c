/**
 * @file audio_test.c
 * @author Product application department
 * @brief audio 测试用例，仅供参考
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

// 头文件包含
#include "audio_test.h"

// 全局变量定义
uint8_t audio_flag = 0;

// 函数定义
/**
 * @brief AUDIO 播放完成回调函数
 *
 * @param number
 */
void callback_print(uint32_t number)
{
    printf("Play Done\n");
    audioserviceplay(TEST_AUDIO_NUMBER, UNBREAK);
    audio_flag = 1;
    delayms(2000);
}
/**
 * @brief AUDIO 测试示例函数
 *
 */
void audio_demo(void)
{
    uint8_t i = 0;

    audioserviceinit(callback_print);

    audioserviceplay(TEST_AUDIO_NUMBER, UNBREAK);

    while (1)
    {
        /*此处根据回调函数里的flag，判断是否播放下一句话,测试使用*/
        if (audio_flag == 1)
        {
            audioserviceplay(TEST_AUDIO_NUMBER, UNBREAK);
            i++;
        }
        audio_flag = 0;
        // printf("run\r\n");
        audioservicerouting();
    }
}

/** @} */  // 结束 DACAUDIO 模块分组
/** @} */  // 结束外设模块分组
