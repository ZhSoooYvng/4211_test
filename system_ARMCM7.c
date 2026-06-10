/**
 * @file     system_ARMCM7.c
 * @brief    CMSIS Device System Source File for
 *           ARMCM7 Device
 * @version  V1.0.1
 * @date     15. November 2019
******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include "libpsram.h"
#include "cpm_drv.h"
#include "chipconfig.h"
#if defined(ARMCM7)
#include "ARMCM7.h"
#elif defined(ARMCM7_SP)
#include "ARMCM7_SP.h"
#elif defined(ARMCM7_DP)
#include "ARMCM7_DP.h"
#else
#error device not specified!
#endif

///*----------------------------------------------------------------------------
//  Define clocks
// *----------------------------------------------------------------------------*/
// #define  XTAL            ( 1000000U)      /* Oscillator frequency             */

// #define  SYSTEM_CLOCK    (25 * XTAL)

///*----------------------------------------------------------------------------
//  System Core Clock Variable
// *----------------------------------------------------------------------------*/
// uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Core Clock Frequency      */

// void SystemCoreClockUpdate (void)
//{
//   SystemCoreClock = SYSTEM_CLOCK;
// }

extern void __Vectors(void);
extern void sys_init(void);
extern void drv_psram_openxip(uint32_t ssi_rx_sample_delay);
extern void drv_psram_closexip(void);
extern void mpu_config(void);
extern void hal_uart_reset(void);
static void cpu_cache_enable(void)
{
    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();
}
static void cpu_cache_disable(void)
{
    // 1. 禁用指令缓存前确保足够NOP
    __ISB(); // 指令同步屏障
    __DSB(); // 数据同步屏障   
    // 2. 禁用指令缓存
    SCB_DisableICache();
    __ISB(); // 指令同步屏障
    __DSB(); // 数据同步屏障   
    // 3. clean 数据缓存
    SCB_CleanDCache();
    __ISB(); // 指令同步屏障
    __DSB(); // 数据同步屏障  
    // 4. 无效化数据缓存
    SCB_CleanInvalidateDCache(); 	
    __ISB(); // 指令同步屏障
    __DSB(); // 数据同步屏障	

}
void pre_copy_code_init(void)
{
    extern uint8_t  Image$$RW_IRAM1$$Base;       // 执行域基地址
    extern uint8_t  Load$$RW_IRAM1$$Base ;       // 加载域基地址
    extern uint32_t Image$$RW_IRAM1$$Length;    // 长度

    volatile  uint8_t * dst = (volatile uint8_t *)&Image$$RW_IRAM1$$Base;
    volatile  uint8_t * src = (volatile uint8_t *)&Load$$RW_IRAM1$$Base;
    volatile  uint32_t len = (volatile uint32_t)&Image$$RW_IRAM1$$Length;
    memset((void *)dst, 0x0, len);
    for(uint32_t i = 0; i < len; i++)
    {
        dst[i] = src[i];
    }
		if( (SCB->CCR & 0x10000)==0x10000)
			{     
				SCB_CleanDCache();  // 同步数据
			}
}
void SystemInit(void)
{
    hal_uart_reset();
#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    //  SCB->VTOR = (uint32_t) &(__VECTOR_TABLE[0]);
    SCB->VTOR = ((unsigned int)__Vectors) & 0xFFFFFC00;
#endif

#if defined(__FPU_USED) && (__FPU_USED == 1U)
    SCB->CPACR |= ((3U << 10U * 2U) | /* enable CP10 Full Access */
                   (3U << 11U * 2U)); /* enable CP11 Full Access */
#endif
    if( (SCB->CCR & 0x10000)==0x10000)
    {      
        SCB_DisableICache();
        SCB_DisableDCache();      
    }

#if(CM4211_START_MODE == 2)

#else
    mpu_config(); // MPU 配置
    cpu_cache_enable();   
    sys_init();
    psram_deinit();
    #if (CCM4211_CHIP_PACKAGE == 0)
    psram_int(PSRAM_TYPE_3208X2, drv_cpm_getcoreclk()/2, PSRAM_DRIV_1);
    #elif(CCM4211_CHIP_PACKAGE == 1)
    psram_int(PSRAM_TYPE_128, drv_cpm_getcoreclk()/2, PSRAM_DRIV_0);
    #elif(CCM4211_CHIP_PACKAGE == 2)
    psram_int(PSRAM_TYPE_3208, drv_cpm_getcoreclk()/2, PSRAM_DRIV_1);
    #elif(CCM4211_CHIP_PACKAGE == 3)
    psram_int(PSRAM_TYPE_256, drv_cpm_getcoreclk()/2, PSRAM_DRIV_0);
    #elif(CCM4211_CHIP_PACKAGE == 4)
    psram_int(PSRAM_TYPE_128, drv_cpm_getcoreclk()/2, PSRAM_DRIV_0);
    #endif 
#endif
}

void LVD_EN(void)
{
    //    _bit_set(CPM->PWRCR,CPM_PWRCR_VCC_5V_LV_DETECT_EN);
    //    _bit_set(CPM->PWRCR,CPM_PWRCR_VCC_3V3_LV_DETECT_EN);
}

void IO_Latch_Clr(void)
{
    //    _bit_clr(CPM->PWRCR,CPM_PWRCR_VCC_IO_LATCH_SET_MASK);
    //    _bit_set(CPM->PWRCR,CPM_PWRCR_VCC_IO_LATCH_CLR_MASK);
    //    _bit_clr(CPM->PWRCR,CPM_PWRCR_VCC_IO_LATCH_CLR_MASK);
}
