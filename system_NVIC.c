/**
  **********************************************************************************
             Copyright(c) 2020 China Core Co. Ltd.
                      All Rights Reserved
  **********************************************************************************
  * @file    system_NVIC.c
  * @author  Product Application Department
  * @version V1.0
  * @date    2020.03.25
  * @brief   NIIC接口
  * @note none
  *
  @verbatim
  ===================================================================================
                                  ##### 更新 #####
  ===================================================================================
  [..] 更新时间:
  [..] 更新人:
  [..] 更新内容:
        1. nothing.
  @endverbatim
  */
#define __SYSTEM_NVIC__
#ifdef __SYSTEM_NVIC__

#include "system_NVIC.h"

/*** 全局变量定义 ***************************************************************/

/*** volatile **********/

/*** @ static **********/

/*** 常量定义 *******************************************************************/

/*** 函数定义 *******************************************************************/

// 设置向量表偏移地址
// NVIC_VectTab:基址
// Offset:偏移量
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset)
{
    SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0xFFFFFE00); // 设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留。
}
#if 1
// 设置NVIC分组
// NVIC_Group:NVIC分组 0~4 总共5组
void NVIC_PriorityGroupConfig(uint8_t NVIC_Group)
{
    uint32_t temp, temp1;
    temp1 = (~NVIC_Group) & 0x07; // 取后三位
    temp1 <<= 8;
    temp = SCB->AIRCR;  // 读取先前的设置
    temp &= 0X0000F8FF; // 清空先前分组
    temp |= 0X05FA0000; // 写入钥匙
    temp |= temp1;
    SCB->AIRCR = temp; // 设置分组
}
// 设置NVIC
// NVIC_PreemptionPriority:抢占优先级
// NVIC_SubPriority       :响应优先级
// NVIC_Channel           :中断编号
// NVIC_Group             :中断分组 0~4
// 注意优先级不能超过设定的组的范围!否则会有意想不到的错误
// 组划分:
// 组0:0位抢占优先级,3位响应优先级
// 组1:1位抢占优先级,2位响应优先级
// 组2:2位抢占优先级,1位响应优先级
// 组3:3位抢占优先级,0位响应优先级

// NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先
void NVIC_Init(uint8_t NVIC_PreemptionPriority,
                uint8_t NVIC_SubPriority,
                uint8_t NVIC_Channel,
                uint8_t NVIC_Group)
{
    uint32_t temp;
    NVIC_PriorityGroupConfig(NVIC_Group); // 设置分组
    temp = NVIC_PreemptionPriority << (3 - NVIC_Group);
    temp |= NVIC_SubPriority & (0x07 >> NVIC_Group);
    temp &= 0x7;                                             // 取低三位
    NVIC->ISER[NVIC_Channel / 32] |= 1 << NVIC_Channel % 32; // 使能中断位(要清除的话,设置ICER对应位为1即可)
    NVIC->IPR[NVIC_Channel] = temp << 5;                     // 设置响应优先级和抢断优先级
}

void NVIC_Clear(uint8_t NVIC_Channel)
{
    NVIC->ICER[NVIC_Channel / 32] = 1 << NVIC_Channel % 32; /*清除中断位*/
}

// //外部中断配置函数
// //只针对GPIOA~I;不包括PVD,RTC,USB_OTG,USB_HS,以太网唤醒等
// //参数:
// //GPIOx:0~8,代表GPIOA~I
// //BITx:需要使能的位;
// //TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
// //该函数一次只能配置1个IO口,多个IO口,需多次调用
// //该函数会自动开启对应中断,以及屏蔽线
// void Ex_NVIC_Config(uint8_t GPIOx,uint8_t BITx,uint8_t TRIM)
// {
//     uint8_t EXTOFFSET=(BITx%4)*4;
//     RCC->APB2ENR|=1<<14;                          //使能SYSCFG时钟
//     SYSCFG->EXTICR[BITx/4]&=~(0x000F<<EXTOFFSET);//清除原来设置！！！
//     SYSCFG->EXTICR[BITx/4]|=GPIOx<<EXTOFFSET;    //EXTI.BITx映射到GPIOx.BITx
//     //自动设置
//     EXTI->IMR|=1<<BITx;                    //开启line BITx上的中断(如果要禁止中断，则反操作即可)
//     if(TRIM&0x01)EXTI->FTSR|=1<<BITx;    //line BITx上事件下降沿触发
//     if(TRIM&0x02)EXTI->RTSR|=1<<BITx;    //line BITx上事件上升降沿触发
// }
#else

/**
 * @brief  Set the priority grouping field (pre-emption priority and subpriority)
 *         using the required unlock sequence.
 * @param  PriorityGroup: The priority grouping bits length.
 *         This parameter can be one of the following values:
 *         @arg NVIC_PRIORITYGROUP_0: 0 bit  for pre-emption priority,
 *                                    4 bits for subpriority
 *         @arg NVIC_PRIORITYGROUP_1: 1 bit  for pre-emption priority,
 *                                    3 bits for subpriority
 *         @arg NVIC_PRIORITYGROUP_2: 2 bits for pre-emption priority,
 *                                    2 bits for subpriority
 *         @arg NVIC_PRIORITYGROUP_3: 3 bits for pre-emption priority,
 *                                    1 bit  for subpriority
 *         @arg NVIC_PRIORITYGROUP_4: 4 bits for pre-emption priority,
 *                                    0 bit  for subpriority
 * @note   When the NVIC_PriorityGroup_0 is selected, IRQ pre-emption is no more possible.
 *         The pending IRQ priority will be managed only by the subpriority.
 * @retval None
 */
void HAL_NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
    /* Set the PRIGROUP[10:8] bits according to the PriorityGroup parameter value */
    NVIC_SetPriorityGrouping(PriorityGroup);
}
/**
 * @brief  Set the priority of an interrupt.
 * @param  IRQn: External interrupt number.
 *         This parameter can be an enumerator of IRQn_Type enumeration
 *         (For the complete STM32 Devices IRQ Channels list, please refer to the appropriate CMSIS device file (stm32l4xxxx.h))
 * @param  PreemptPriority: The pre-emption priority for the IRQn channel.
 *         This parameter can be a value between 0 and 15
 *         A lower priority value indicates a higher priority
 * @param  SubPriority: the subpriority level for the IRQ channel.
 *         This parameter can be a value between 0 and 15
 *         A lower priority value indicates a higher priority.
 * @retval None
 */
void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
    uint32_t prioritygroup = 0x00;

    /* Check the parameters */

    prioritygroup = NVIC_GetPriorityGrouping();

    NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
}

/**
 * @brief  Enable a device specific interrupt in the NVIC interrupt controller.
 * @note   To configure interrupts priority correctly, the NVIC_PriorityGroupConfig()
 *         function should be called before.
 * @param  IRQn External interrupt number.
 *         This parameter can be an enumerator of IRQn_Type enumeration
 *         (For the complete STM32 Devices IRQ Channels list, please refer to the appropriate CMSIS device file (stm32l4xxxx.h))
 * @retval None
 */
void HAL_NVIC_EnableIRQ(IRQn_Type IRQn)
{
    /* Enable interrupt */
    NVIC_EnableIRQ(IRQn);
}

/**
 * @brief  Disable a device specific interrupt in the NVIC interrupt controller.
 * @param  IRQn External interrupt number.
 *         This parameter can be an enumerator of IRQn_Type enumeration
 *         (For the complete STM32 Devices IRQ Channels list, please refer to the appropriate CMSIS device file (stm32l4xxxx.h))
 * @retval None
 */
void HAL_NVIC_DisableIRQ(IRQn_Type IRQn)
{
    /* Disable interrupt */
    NVIC_DisableIRQ(IRQn);
}
#endif

// 关闭所有中断(但是不包括fault和NMI中断)
void INTX_DISABLE(void)
{
    __ASM volatile("cpsid i");
}
// 开启所有中断
void INTX_ENABLE(void)
{
    __ASM volatile("cpsie i");
}

#endif /* end __HAL__ */
