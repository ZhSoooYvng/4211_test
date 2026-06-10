#ifndef CACHE_UTIL_H_
#define CACHE_UTIL_H_

#include <stdint.h>
#include <stddef.h>
#include "core_cm7.h"

#ifndef DCACHE_LINE_SIZE
#define DCACHE_LINE_SIZE   32U
#endif

#ifndef ICACHE_LINE_SIZE
#define ICACHE_LINE_SIZE   32U
#endif

#ifndef CACHE_ALIGN_DOWN
#define CACHE_ALIGN_DOWN(addr, size)  ((uintptr_t)(addr) & ~((uintptr_t)(size) - 1U))
#endif
#ifndef CACHE_ALIGN_UP
#define CACHE_ALIGN_UP(addr, size)    (((uintptr_t)(addr) + ((size) - 1U)) & ~((uintptr_t)(size) - 1U))
#endif

/**
 * @brief 清除指定范围内的 D-Cache 行
 * 
 * @param addr 指向要清除的内存区域的指针
 * @param len 要清除的内存区域的长度
 * @return __STATIC_FORCEINLINE
 */
__STATIC_FORCEINLINE void dcache_clean_range(const void *addr, size_t len)
{
#if (__DCACHE_PRESENT == 1U)
    if (len == 0U) return;
    uintptr_t start = CACHE_ALIGN_DOWN(addr, DCACHE_LINE_SIZE);
    uintptr_t end   = CACHE_ALIGN_UP((uintptr_t)addr + len, DCACHE_LINE_SIZE);
    SCB_CleanDCache_by_Addr((uint32_t *)start, (int32_t)(end - start));
    __DSB();
#endif
}
/**
 * @brief 清除指定范围内的 D-Cache 行并失效对应的 D-Cache 行
 *
 * @param addr 指向要清除的内存区域的指针
 * @param len 要清除的内存区域的长度
 * @return __STATIC_FORCEINLINE
 */
__STATIC_FORCEINLINE void dcache_invalidate_range(const void *addr, size_t len)
{
#if (__DCACHE_PRESENT == 1U)
    if (len == 0U) return;
    uintptr_t start = CACHE_ALIGN_DOWN(addr, DCACHE_LINE_SIZE);
    uintptr_t end   = CACHE_ALIGN_UP((uintptr_t)addr + len, DCACHE_LINE_SIZE);
    SCB_InvalidateDCache_by_Addr((uint32_t *)start, (int32_t)(end - start));
    __DSB();
#endif
}
/**
 * @brief 清除指定范围内的 D-Cache 行并失效对应的 D-Cache 行
 * 
 * @param addr 指向要清除的内存区域的指针
 * @param len 要清除的内存区域的长度
 * @return __STATIC_FORCEINLINE
 */
__STATIC_FORCEINLINE void dcache_clean_invalidate_range(const void *addr, size_t len)
{
#if (__DCACHE_PRESENT == 1U)
    if (len == 0U) return;
    uintptr_t start = CACHE_ALIGN_DOWN(addr, DCACHE_LINE_SIZE);
    uintptr_t end   = CACHE_ALIGN_UP((uintptr_t)addr + len, DCACHE_LINE_SIZE);
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)start, (int32_t)(end - start));
    __DSB();
#endif
}
/**
 * @brief 清除指定范围内的 I-Cache 行
 * 
 * @param addr 指向要清除的内存区域的指针
 * @param len 要清除的内存区域的长度
 * @return __STATIC_FORCEINLINE
 */
__STATIC_FORCEINLINE void icache_invalidate_range(const void *addr, size_t len)
{
#if (__ICACHE_PRESENT == 1U)
    if (len == 0U) return;
    uintptr_t start = CACHE_ALIGN_DOWN(addr, ICACHE_LINE_SIZE);
    uintptr_t end   = CACHE_ALIGN_UP((uintptr_t)addr + len, ICACHE_LINE_SIZE);

#if defined(SCB_InvalidateICache_by_Addr)
    SCB_InvalidateICache_by_Addr((uint32_t *)start, (int32_t)(end - start));
#else
    (void)start; (void)end;
    SCB_InvalidateICache();
#endif
    __DSB(); __ISB();
#else
    (void)addr; (void)len;
#endif
}

/**
 * @brief 清除指定范围内的 I-Cache 行，并失效对应的 D-Cache 行
 * 先 Clean D‑Cache：把脏行写回主存，让主存里是真正的新代码。再 Invalidate I‑Cache：让取指不再命中旧的 I‑Cache 行，转去主存拿到刚刚刷下去的新代码。
 * 
 * @param addr 指向要清除的内存区域的指针
 * @param len 要清除的内存区域的长度
 * @return __STATIC_FORCEINLINE
 */
__STATIC_FORCEINLINE void icache_clean_invalidate_range(const void *addr, size_t len)
{
#if (__ICACHE_PRESENT == 1U) && (__DCACHE_PRESENT == 1U)
    if (len == 0U) return;
    uintptr_t start = CACHE_ALIGN_DOWN(addr, ICACHE_LINE_SIZE);
    uintptr_t end   = CACHE_ALIGN_UP((uintptr_t)addr + len, ICACHE_LINE_SIZE);

    SCB_CleanDCache_by_Addr((uint32_t *)start, (int32_t)(end - start));
    __DSB();

#if defined(SCB_InvalidateICache_by_Addr)
    SCB_InvalidateICache_by_Addr((uint32_t *)start, (int32_t)(end - start));
#else
    SCB_InvalidateICache();
#endif
    __DSB(); __ISB();
#else
    (void)addr; (void)len;
#endif
}

#endif /* CACHE_UTIL_H_ */
