#include "sys.h"
#include <stdint.h>
#include <stdio.h>
#include "dwt.h"

#define DWT_MAX_RECORD_CNT  (1000)

extern volatile uint32_t g_core_clk;  /* System Core Clock Frequency */ 


static uint32_t start_cyc[DWT_MAX_RECORD_CNT];
static uint32_t end_cyc[DWT_MAX_RECORD_CNT];
static uint32_t current = 0;
// 启用DWT
void DWT_Init()
{
    current = 0;
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

// 停止DWT
void DWT_DeInit()
{
    DWT->CTRL = 0; // 禁用DWT
	current = 0;
}

// 测量函数执行时间
uint32_t DWT_GetCycles()
{
    return DWT->CYCCNT;
}

// 计数清0
void DWT_ClearCycles()
{
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t DWT_Record_Start()
{
    if (current >= DWT_MAX_RECORD_CNT)
    {
        current = 0;
        printf("[warnning]: DWT_Record_Start overflow.\n");
    }
    // DWT_ClearCycles();
    start_cyc[current] = DWT_GetCycles();
	end_cyc[current] = start_cyc[current] - 1;	// in case user forgot use "DWT_Record_End"?, ref:micro_profiler.cc
    return current++;
}

float DWT_Record_End(char *tag, uint32_t event_index)
{
	float time_ms;
	if (event_index >= DWT_MAX_RECORD_CNT)
	{
		printf("[Error]: event_index out of bound.\n");
	}
    end_cyc[event_index] = DWT_GetCycles();
	time_ms = (end_cyc[event_index] - start_cyc[event_index]) / (float)g_core_clk * 1000.0f;
    // if (tag != NULL)
    //     printf("%s %.2f ms\n", tag, time_ms);
	return time_ms;
}

/*
 * Return:      void
 * Parameters:  Time: 延时时间
 * Description: 微秒延时
 */
void vDWTDelayUs(float fTime)
{
	//g_sys_clk  系统时钟频率
    volatile uint32_t uiTimeStop = 0u, uiTimeStart = 0u;
 
    uiTimeStart = DWT->CYCCNT;
    uiTimeStop  = (uint32_t)((g_core_clk / 1000000u) * fTime) + uiTimeStart;
 
    if(uiTimeStop >= uiTimeStart)
        while((DWT->CYCCNT > uiTimeStart) && (DWT->CYCCNT < uiTimeStop));
    else
        while(!((DWT->CYCCNT > uiTimeStop) && (DWT->CYCCNT < uiTimeStart)));
}

void vDWTDelayMs(uint32_t fTime)
{
	uint32_t i;
	for (i = 0; i < fTime; i++)
	{
		vDWTDelayUs(1000.f);
	}
	
}
