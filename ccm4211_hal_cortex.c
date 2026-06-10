/**
 * @file ccm4211_hal_cortex.c
 * @author ccore
 * @brief cc4211 hal cortex source file
 * @version 0.1
 * @date 2025-03-03
 *
 * @copyright Copyright (c) 2025 China Core Co. Ltd. All Rights Reserved
 *
 */
/* Includes ------------------------------------------------------*/
#include "ccm4211_hal_cortex.h"

/* Private macros ------------------------------------------------*/

/* Private variables ---------------------------------------------*/

/* Private function prototypes -----------------------------------*/

/* Exported functions --------------------------------------------*/
#if (__MPU_PRESENT == 1)
/**
 * @brief  Initializes and configures the Region and the memory to be protected.
 * @param  MPU_Init: Pointer to a MPU_Region_InitTypeDef structure that contains
 *                the initialization and configuration information.
 * @retval None
 */
void HAL_MPU_ConfigRegion(MPU_Region_InitTypeDef *MPU_Init)
{
    /* Check the parameters */
    //  assert_param(IS_MPU_REGION_NUMBER(MPU_Init->Number));
    //  assert_param(IS_MPU_REGION_ENABLE(MPU_Init->Enable));

    /* Set the Region number */
    MPU->RNR = MPU_Init->Number;

    if ((MPU_Init->Enable) != RESET)
    {
        /* Check the parameters */
        //    assert_param(IS_MPU_INSTRUCTION_ACCESS(MPU_Init->DisableExec));
        //    assert_param(IS_MPU_REGION_PERMISSION_ATTRIBUTE(MPU_Init->AccessPermission));
        //    assert_param(IS_MPU_TEX_LEVEL(MPU_Init->TypeExtField));
        //    assert_param(IS_MPU_ACCESS_SHAREABLE(MPU_Init->IsShareable));
        //    assert_param(IS_MPU_ACCESS_CACHEABLE(MPU_Init->IsCacheable));
        //    assert_param(IS_MPU_ACCESS_BUFFERABLE(MPU_Init->IsBufferable));
        //    assert_param(IS_MPU_SUB_REGION_DISABLE(MPU_Init->SubRegionDisable));
        //    assert_param(IS_MPU_REGION_SIZE(MPU_Init->Size));

        MPU->RBAR = MPU_Init->BaseAddress;
        MPU->RASR = ((uint32_t)MPU_Init->DisableExec << MPU_RASR_XN_Pos) |
                    ((uint32_t)MPU_Init->AccessPermission << MPU_RASR_AP_Pos) |
                    ((uint32_t)MPU_Init->TypeExtField << MPU_RASR_TEX_Pos) |
                    ((uint32_t)MPU_Init->IsShareable << MPU_RASR_S_Pos) |
                    ((uint32_t)MPU_Init->IsCacheable << MPU_RASR_C_Pos) |
                    ((uint32_t)MPU_Init->IsBufferable << MPU_RASR_B_Pos) |
                    ((uint32_t)MPU_Init->SubRegionDisable << MPU_RASR_SRD_Pos) |
                    ((uint32_t)MPU_Init->Size << MPU_RASR_SIZE_Pos) |
                    ((uint32_t)MPU_Init->Enable << MPU_RASR_ENABLE_Pos);
    }
    else
    {
        MPU->RBAR = 0x00;
        MPU->RASR = 0x00;
    }
}

/* |TypeExtField | IsCacheable | IsBufferable |           Description              |
 * |     000     |       1     |       0      | Write through, no write allocate   |
 * |     000     |       1     |       1      | Write-back, no write allocate      |
 * |     001     |       0     |       0      | Non-cacheable                      |
 * |     001     |       1     |       1      | Write-back, write and read allocate|
 **/
void mpu_config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct;

    /* Disable the MPU */
    HAL_MPU_Disable();

    /* Configure the MPU attributes as "Write through, no write allocate" for spiflash1 */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x10000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU attributes as "Write through, no write allocate" for spiflash2 */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x12000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER1;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU attributes as "Write through, no write allocate" for spiflash3 */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x14000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER2;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU attributes as "Write through, no write allocate" for PSRAM */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x1C000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER3;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    // /* Configure the MPU attributes as "Non-cacheable */
    // MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    // MPU_InitStruct.BaseAddress = 0x1C000000;
    // MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
    // MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    // MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    // MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    // MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    // MPU_InitStruct.Number = MPU_REGION_NUMBER3;
    // MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    // MPU_InitStruct.SubRegionDisable = 0x00;
    // MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    // HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU attributes as "Write-back, no write allocate*/
    // MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    // MPU_InitStruct.BaseAddress = 0x1C000000;
    // MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
    // MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    // MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    // MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    // MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    // MPU_InitStruct.Number = MPU_REGION_NUMBER3;
    // MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    // MPU_InitStruct.SubRegionDisable = 0x00;
    // MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    // HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU attributes as "Write-back, write and read allocate" for PSRAM */
    // MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    // MPU_InitStruct.BaseAddress = 0x1C000000;
    // MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
    // MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    // MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    // MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    // MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    // MPU_InitStruct.Number = MPU_REGION_NUMBER3;
    // MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    // MPU_InitStruct.SubRegionDisable = 0x00;
    // MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    // HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU attributes as "Non-cacheable" for SRAM */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x20000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER4;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Enable the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
#endif /* __MPU_PRESENT */
