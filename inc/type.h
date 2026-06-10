/**
 * @file type.h
 * @author Product application department
 * @brief  类型定义头文件
 * @version V1.0
 * @date 2025-02-28
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 *
 */
#ifndef __TYPE_H__
#define __TYPE_H__

#ifdef __cplusplus
extern "C"
{
#endif

// 全局变量定义
#define __I volatile const
#define __O volatile
#define __IO volatile

#define TRUE 1
#define FALSE 0
#ifndef NULL
#define NULL ((void *)0)
#endif

#define MAX_TIMEOUT 0xFFFFFFFF

typedef unsigned char boolean;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef float float32_t;
typedef int32_t s32;
typedef unsigned short u16;
typedef unsigned char BOOL;

typedef union
{
    uint16_t size;
    uint8_t size_byte[2];
} uint16_8_union_t;

typedef union
{
    uint32_t size;
    uint16_t size_short[2];
    uint8_t size_byte[4];
} uint32_8_union_t;

typedef enum
{
    BIT_RESET = 0,
    BIT_SET = 1
} BitActionTypeDef;

typedef enum
{
    DISABLE = 0,
    ENABLE = 1
} functionalstate_t;

typedef enum
{
    RESET = 0,
    SET = 1
} flagstatus_t;

typedef enum
{
    GPIO_LOW = 0,
    GPIO_HIGH = 1
} gpio_pinstate_t;

#ifdef __cplusplus
}
#endif

#endif
