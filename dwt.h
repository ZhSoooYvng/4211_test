#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void DWT_Init(void);

void DWT_DeInit(void);

uint32_t DWT_GetCycles(void);

void DWT_ClearCycles(void);

uint32_t DWT_Record_Start(void);

float DWT_Record_End(char *tag, uint32_t event_index);

void vDWTDelayMs(uint32_t fTime);

void vDWTDelayUs(float fTime);

#ifdef __cplusplus
}
#endif

