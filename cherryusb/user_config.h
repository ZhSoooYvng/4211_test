#ifndef  __USER_CONFIG_H
#define  __USER_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ============================== USER edit start ============================== */
/* -------- SDK相关软件配置 -------- */
/* 是否启用FreeRTOS，使用NPU必须开启此选项 */
#define CONFIG_USE_FreeRTOS                 1

/* 是否启用MODBUS，MODBUS可参考src/demo/modbus */
#define CONFIG_USE_MODBUS                   0


#ifdef __cplusplus
}
#endif

#endif

