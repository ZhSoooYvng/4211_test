#ifndef __ENV_VIR_H__
#define __ENV_VIR_H__

#include "main.h"

typedef struct
{
	uint32_t head;        //数组下标，指向队头
	uint32_t tail;        //数组下标，指向队尾
	uint32_t size;        //队列缓存长度（初始化时赋值）
	uint8_t *buffer;      //队列缓存数组（初始化时赋值）
	
}QueueType_t;

typedef enum
{
	QUEUE_OK = 0,       //队列正常
	QUEUE_ERROR,        //队列错误
	QUEUE_OVERLOAD,     //队列已满
	QUEUE_EMPTY         //队列已空
} QueueStatus_t;

#endif