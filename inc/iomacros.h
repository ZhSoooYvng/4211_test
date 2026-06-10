/**
 * @file iomacros.h
 * @author Product application department
 * @brief 移位类型定义头文件
 * @version V1.0
 * @date 2025-02-28
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 *
 */

#ifndef __IOMACROS_H__
#define __IOMACROS_H__

#ifdef __cplusplus
extern "C"
{
#endif

// 头文件包含
#include "type.h"
#include <string.h>

// 全局变量定义
#define _bit(x) ((uint32_t)1 << (x))
#define _bit_set(value, bit) ((value) |= (bit))
#define _bit_clr(value, bit) ((value) &= ~(bit))
#define _bit_get(value, bit) ((value) & (bit))
#define _reg_chk(value, bit) ((value) & (bit))
#define _bit_and(value, bit) ((value) &= (bit))

#define _reg_write(reg, val) ((reg) = (val))
#define _reg_read(reg) ((reg))
#define _reg_clear(reg) ((reg) = (0x0))
#define _reg_modify(reg, clearmask, setmask) _reg_write((reg), (((_reg_read(reg)) & (clearmask)) | (setmask)))

#define _io_read8(p) (*(volatile uint8_t *)(p))
#define _io_write8(p, v) (*((volatile uint8_t *)(p)) = ((uint8_t)(v)))
#define _io_read16(p) (*(volatile uint16_t *)(p))
#define _io_write16(p, v) (*((volatile uint16_t *)(p)) = (uint16_t)(v))
#define _io_read32(p) (*(volatile uint32_t *)(p))
#define _io_write32(p, v) (*((volatile uint32_t *)(p)) = (uint32_t)(v))

#define _byte_low(x) ((uint8_t)x)
#define _byte_high(x) ((uint8_t)(x >> 8))

#define _memcpy memcpy
#define _memset memset

#ifdef __cplusplus
}
#endif

#endif
