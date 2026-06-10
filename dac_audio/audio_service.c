
/**
 * @file audio_service.c
 * @author Product application department
 * @brief audio service驱动，供应用层调用，仅供参考
 * @version V1.0
 * @date 2025-02-17
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved.
 *
 */
// 头文件包含
#include "audio_service.h"


// 全局变量定义
/* 计数器 */
static volatile int xTickCount = 0U;

#define Calc_PastTime(currentTime, lastTime) ((currentTime >= lastTime) ? (currentTime - lastTime) : (UINT32_MAX - lastTime + currentTime))

// 计算环形缓存里面可以读的数据个数
#define RbCanRead(rb) ((rb->read <= rb->write) ? ((rb->write - rb->read) / rb->size) : (rb->len - ((rb->read - rb->write) / rb->size)))

// 计算环形缓存里面可以写的数据个数
#define RbCanWrite(rb) (rb->len - RbCanRead(rb))

typedef void *ringbufferhandle_t;

// 创建环形缓存，保存待播放的语音
static ringbufferhandle_t rbHandle = NULL;

/* 定义缓存大小 */
#define AUDIO_RING_BUFFER_LEN (40)
#define AUDIO_RING_BUFFER_SIZE (sizeof(uint16_t))

static AudioServiceCallback_t pAdCallBack = NULL;
static uint16_t activeAudioNumber = 0;

/**
 * @brief 环形缓存数据结构
 *
 */
typedef struct
{
    uint16_t len;  // 缓存长度（数据个数）
    uint16_t size; // 每个数据的大小
    volatile uint8_t *write;
    volatile uint8_t *read;
    volatile uint8_t buff[];
} ringbuffer_t;

typedef struct
{
    uint32_t tickStart; // 事件启动时间
    uint32_t tickDelay; // 延时启动时间
    BOOL bEffective;    // 事件是否有效
    errorstatus (*pFunc)();
} event_t;

event_t mAudioEvent[2] = {0};

// 函数定义

/**
 * @brief  AUDIO 创建环形缓存函数
 * @note
 * @param  buffLength：缓存长度
 * @param  itemSize：条目大小
 *
 * @return 返回缓存句柄，错误返回NULL
 */
ringbufferhandle_t ringbuffercreate(uint16_t buffLength, uint8_t itemSize)
{
    ringbuffer_t *pRB;

    if (buffLength == 0 || itemSize == 0)
    {
        return NULL;
    }
    buffLength = buffLength + 1;
    pRB = (ringbuffer_t *)malloc(sizeof(ringbuffer_t) + buffLength * itemSize);

    if (pRB != NULL)
    {
        pRB->len = buffLength;
        pRB->size = itemSize;
        pRB->read = pRB->buff;
        pRB->write = pRB->buff;
    }
    else
    {
        // 分配失败，可以重启芯片
        // TO DO
    }

    return (ringbufferhandle_t)pRB;
}

/**
 * @brief  AUDIO 往环形缓存写入一个数据函数
 * @note   该函数允许中断调用（一般是中断收到数据，就调用该函数将数据写入缓存）
 *
 * @param  handle：缓存句柄
 * @param  pData：数据首地址
 *
 * @return SUCCESS   ERROR
 */
errorstatus ringbufferwrite(ringbufferhandle_t handle, void *pData)
{
    ringbuffer_t *pRB = (ringbuffer_t *)handle;

    if (pRB == NULL || pData == NULL)
    {
        return _ERROR;
    }

    if (RbCanWrite(pRB) <= 1)
    {
        return _ERROR;
    }

    memcpy((void *)(pRB->write), pData, pRB->size);

    pRB->write += pRB->size;
    if (pRB->write == (pRB->buff + pRB->len * pRB->size))
    {
        pRB->write = pRB->buff;
    }
    return _SUCCESS;
}

/**
 * @brief  AUDIO 从环形缓存里面读出读取有效数据的长度函数
 * @note
 * @param  handle：缓存句柄
 *
 * @return 返回数据长度（RB_BUF里面的有效字节数）
 */
uint32_t ringbuffergetvalidsize(ringbufferhandle_t handle)
{
    ringbuffer_t *pRB = (ringbuffer_t *)handle;

    if (pRB == NULL)
    {
        return 0;
    }
    return RbCanRead(pRB);
}

/**
 * @brief  AUDIO 从环形缓存里面读出一个数据函数
 * @note
 * @param  handle：缓存句柄
 * @param  pData：数据首地址
 *
 * @return SUCCESS   ERROR
 */
errorstatus ringbufferread(ringbufferhandle_t handle, void *pData)
{
    ringbuffer_t *pRB = (ringbuffer_t *)handle;

    if (pRB == NULL || pData == NULL)
    {
        return _ERROR;
    }

    if (RbCanRead(pRB) == 0)
    {
        return _ERROR;
    }

    memcpy(pData, (void *)(pRB->read), pRB->size);

    pRB->read += pRB->size;
    if (pRB->read == (pRB->buff + pRB->len * pRB->size))
    {
        pRB->read = pRB->buff;
    }
    return _SUCCESS;
}

/**
 * @brief  AUDIO 复位环形缓存函数
 * @note   清空缓存里面的所有数据
 *
 * @param  handle：缓存句柄
 *
 * @return SUCCESS   ERROR
 */
errorstatus ringbufferreset(ringbufferhandle_t handle)
{
    ringbuffer_t *pRB = (ringbuffer_t *)handle;

    if (pRB != NULL)
    {
        pRB->read = pRB->buff;
        pRB->write = pRB->buff;
        return _SUCCESS;
    }
    return _ERROR;
}

/**
 * @brief  AUDIO 软定时器计数函数
 *
 * @return NONE
 */
void audiosystickhandler()
{
    xTickCount++;
}

/**
 * @brief AUDIO 语音事件函数
 * @param num 播放语音段
 * @param bSet 是否播放下一段

 */
void audiosetevent(uint8_t num, BOOL bSet)
{
    mAudioEvent[num].bEffective = bSet;
    mAudioEvent[num].tickStart = bSet ? xTickCount : 0;
    mAudioEvent[num].tickDelay = 0;
    if (bSet)
        mAudioEvent[num].tickDelay = num ? 50 : 1;
}

/**
 * @brief AUDIO 播放下一个音频函数
 *
 * @retval NONE
 */
void audioserviceplaynext()
{
    uint16_t audioNumber;

    if (activeAudioNumber == 0)
    {
        if (ringbufferread(rbHandle, &audioNumber) == _SUCCESS)
        {
            hal_audiosetplaynumber(audioNumber);
            activeAudioNumber = audioNumber;

            audiosetevent(0, TRUE);
            audiosetevent(1, TRUE);
        }
        else
        {
            audiosetevent(0, FALSE);
            audiosetevent(1, FALSE);
        }
    }
}

/**
 * @brief AUDIO 扫描当前音频服务工作状态，执行回调函数
 *
 * @retval 当前状态
 */
errorstatus audioservicescan()
{
    errorstatus status = hal_audiogetstatus();
    if (status == _SUCCESS)
    {
        activeAudioNumber = 0;
        audioserviceplaynext();

        if (pAdCallBack != NULL)
        {
            pAdCallBack(activeAudioNumber);
        }
    }

    return status;
}

/**
 * @brief AUDIO 初始化音频服务函数
 *
 * @retval NONE
 */
errorstatus audioserviceinit(AudioServiceCallback_t mCallback)
{
    hal_audioinit();

    if (mCallback == NULL)
        return _ERROR;

    pAdCallBack = mCallback;

    // 初始化音频服务所需资源
    rbHandle = ringbuffercreate(AUDIO_RING_BUFFER_LEN, AUDIO_RING_BUFFER_SIZE);
    memset(mAudioEvent, 0, sizeof(event_t) * 2);

    mAudioEvent[0].pFunc = hal_audioplay;
    mAudioEvent[1].pFunc = audioservicescan;

    return _SUCCESS;
}

/**
 * @brief AUDIO 音频服务函数
 *
 * @param[in] number
 *         需要播放的语言文件编号
 * @param[in] mode
 *         音频播放模式，是否需要打断当前音频，0:打断当前音频，并立即播放,1:等待播放结束后再播放
 * @retval NONE
 */
void audioserviceplay(uint16_t playNumber, playmode_t mode)
{
    uint16_t audioNumber = playNumber;

    if (BREAK == mode)
    {
        activeAudioNumber = 0;
        ringbufferreset(rbHandle); // reset Ringbuffer
        hal_audiostop();
        hal_audioinit();
    }

    ringbufferwrite(rbHandle, &audioNumber);
    audioserviceplaynext();
}

/**
 * @brief AUDIO 音频服务设置音量大小功能函数（客户可自行增加音量++,--功能，仅作示例，不增加接口）
 *
 * @param volume 音量参数取值范围0~最大值
 * @retval NONE
 */
void audioservicesetvolume(uint8_t volume)
{
    uint8_t uMaxVolume = 0;

    (void)uMaxVolume;

    uMaxVolume = hal_audiogetmaxvolume();

    hal_audiosetvolume(volume); // 设置音量
                                // AudioHalSetVolume(AudioHalGetCurVolume()+1); //音量++
                                // AudioHalSetVolume(AudioHalGetCurVolume()-1); //音量--
}

/**
 * @brief AUDIO 音频服务例行进程函数，系统主循环调用即可
 *
 * @retval NONE
 */
void audioservicerouting()
{
    uint8_t i = 0;

    for (; i < 2; i++)
    {
        if (mAudioEvent[i].bEffective)
        {

            if (mAudioEvent[i].pFunc() == _SUCCESS)
            {
                mAudioEvent[i].bEffective = FALSE;
            }
        }

        //        if(mAudioEvent[i].tickStart != 0 && mAudioEvent->tickDelay <= Calc_PastTime(xTickCount,mAudioEvent[i].tickStart))
        //        {
        //            mAudioEvent[i].tickStart = xTickCount;
        mAudioEvent[i].bEffective = TRUE;
        // }
    }
}
