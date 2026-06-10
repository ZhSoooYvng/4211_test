# 4211_test (CCM4211 Application Test)

本项目是基于 **C*CORE CCM4211 (ARM Cortex-M7)** 核心的综合应用测试与固件开发框架。项目集成了丰富的硬件外设驱动测试、第三方中间件（如 CherryUSB、FatFS、FreeRTOS）以及端侧 AI 推理（TensorFlow Lite for Microcontrollers, TFLM）的演示。

## 硬件与系统平台

- **微控制器**: C*CORE CCM4211 (ARM Cortex-M7)
- **核心频率**: 可配置（启动时通过 UART 打印核心、系统和 IPS 时钟）
- **Cache 支持**: 支持 I-Cache 与 D-Cache，并提供底层 Cache 一致性维护接口 (`cache_util.h`)。
- **系统滴答定时器**: 基于 DWT（Data Watchpoint and Trigger）的微秒/毫秒级高精度延时与执行周期统计 (`dwt.h`)。

## 核心特性

1. **全面的外设测试框架**
   `main.h` 中预定义了大量外设的测试宏（如 `UART_DEMO`, `ADC_DEMO`, `SPI_DEMO`, `LCDC_DEMO`, `TOUCH_DEMO` 等）。开发者可通过打开/关闭对应宏定义，快速编译和验证指定外设功能。

2. **端侧 AI 推理 (TFLM)**
   集成了 TensorFlow Lite for Microcontrollers 框架，包含以下测试用例：
   - `hello_world` 模型（支持 Float 和 INT8 量化模型）
   - 热成像/红外阵列数据处理算法（`tflm_test.cpp` 实现了基于 8x8 热力图数据的局部最大值/峰值检测算法）。
   - 推理耗时与 CPU 周期统计（基于 DWT 计数器）。

3. **丰富的中间件集成**
   - **CherryUSB**: 轻量级 USB 主机/设备协议栈，支持 CDC, HID, MSC 等类设备测试。
   - **FatFS**: 通用 FAT 文件系统，支持对 SD 卡或 NandFlash 等存储介质的文件操作，包含字典和 INI 解析器工具。
   - **音视频处理**: 包含 `jpeg_accelerator` (JPEG 硬件加速解码) 和 `dac_audio` (音频 DAC 输出与环形缓冲处理)。

## 目录结构说明

```text
├── app/                  # 应用层与 AI 模型代码
│   ├── models/           # TFLM 模型数据 (hello_world, test_model 等)
│   ├── tflm_app.cpp      # TFLM 推理入口与应用层封装
│   └── tflm_test.cpp     # 包含 8x8 热力图峰值检测等算法测试
├── cherryusb/            # CherryUSB 协议栈源码与配置
├── dac_audio/            # 音频 DAC 输出与测试代码
├── fatfs/                # FatFS 文件系统源码及 INI 解析器
├── inc/                  # 全局通用头文件 (类型定义与 IO 宏)
├── jpeg_accelerator/     # JPEG 硬件加速器驱动与测试
├── middle/               # 通用中间件 (如 circular_queue 环形队列实现)
├── usbd/                 # USB Device 测试应用 (CDC, HID, MSC)
├── usbh/                 # USB Host 测试应用
├── main.cpp/h            # 主程序入口、全局宏定义与时钟初始化
├── cache_util.h          # Cortex-M7 D-Cache/I-Cache 一致性维护工具
├── dwt.c/h               # DWT 周期计数器与高精度延时驱动
└── startup_ARMCM7.s      # 芯片启动文件
```

## 编译与运行

1. **环境配置**: 推荐使用 Keil MDK 或基于 GCC 的交叉编译工具链（需支持 ARM Cortex-M7 架构）。
2. **测试切换**: 在 `main.h` 中，取消注释你想运行的模块宏（例如 `#define UART_DEMO` 或 `#define ALGO_DEMO`），然后重新编译。
3. **系统启动**:
   - 上电后，系统将通过 `UART2` (115200 bps 默认配置) 打印固件版本、编译时间以及当前时钟频率。
   - 若拉低特定引脚（如 `EPORT6_PIN3`），系统可触发 `reback_boot()` 进入出厂/ROM 启动模式。

## 关键模块：热力图峰值检测 (Peak Detection)

在 `app/tflm_test.cpp` 中，实现了一个用于 8x8 热力图传感器的数据处理算法：
- `detect_peaks()`: 通过 3x3 邻域比较，提取热力图中的局部最高温极值点。
- 结合阈值过滤和无效坐标屏蔽，可用于低分辨率红外阵列传感器的人体/热源追踪。

## 许可证

本项目遵循各自模块（如 CherryUSB, FatFS, TensorFlow）的开源协议。业务代码及 C*CORE 硬件抽象层归属中国芯（China Core Co. Ltd）及原作者所有。
