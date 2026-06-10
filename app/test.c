#include "test.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define PACKET_SIZE 132
#define HEADER_SIZE 2
#define TEMP_DATA_SIZE 130
#define GRID_SIZE 8
#define MAX_PEAKS 10
#define DETECTION_THRESHOLD 0.5f  // 默认检测阈值
#define ARRAY_SIZE 8
#define BG_RANGE 3.5f  
#define UART_DATA_SIZE 6

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;


const int INVALID_POSITIONS[4][2] = {
    {0, 0}, {0, 7}, {7, 0}, {7, 7}
};

// 峰值点结构体
typedef struct {
    int row;
    int col;
    float value;
} Peak;

// 检测结果结构体
typedef struct {
    Peak peaks[MAX_PEAKS];
    int peak_count;
} DetectionResult;

// 匹配结果结构体
typedef struct {
    int true_positives;
    int false_positives;
    int false_negatives;
    int match_count;
} MatchingResult;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
void calculate_and_send_centroid(float temp_array[ARRAY_SIZE][ARRAY_SIZE], 
                                void (*uart_send_func)(uint8_t*, uint32_t));
void process_temperature_data(void);
void send_data_via_usart1(uint8_t* data, uint32_t size);
int __io_putchar(int ch);
int ParseTemperatureData(uint8_t* rx_data, float output[8][8]);
int detect_peaks(float heatmap[GRID_SIZE][GRID_SIZE], float threshold, Peak peaks[]);
int is_invalid_position(int row, int col);
int calculate_manhattan_distance(int row1, int col1, int row2, int col2);
MatchingResult strict_bidirectional_matching(Peak true_peaks[], int true_count, Peak pred_peaks[], int pred_count);
float find_min_temperature(float temp_array[ARRAY_SIZE][ARRAY_SIZE]);
void format_detection_result(DetectionResult* result, char* buffer, int buffer_size);
void process_and_send_output(float* heatmap, DetectionResult* result);
/* USER CODE END PFP */
uint8_t rx_buffer[PACKET_SIZE];
uint8_t temp_data_buffer[TEMP_DATA_SIZE];
uint32_t packet_count = 0;
uint8_t system_ready = 0;
uint8_t temp_data_received = 1;

// 背景温度范围，可根据实际调整

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE BEGIN 0 */
volatile float g_detection_threshold = DETECTION_THRESHOLD;  // 可动态修改的阈值
uint8_t uart1_rx_buffer[4];                                  // 接收 UART1 命令的缓冲区
/* USER CODE END 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /*硬件初始化和AI初始化*/
//     HAL_Init();
//     SystemClock_Config();
//     MX_GPIO_Init();
//     //设置PB0为低电平
//     MX_USART1_UART_Init();
// 	HAL_UART_Receive_IT(&huart1, uart1_rx_buffer, 4);
//     MX_USART2_UART_Init();
//   //MX_USB_OTG_HS_HCD_Init();
//     MX_X_CUBE_AI_Init();
//     HAL_Delay(1000);
    printf(" __      __  __  __   _____   __  __   ____ \r\n");
	printf(" \\ \\    / / |  \\/  | | ____| |  \\/  | / ___| \r\n");
	printf("  \\ \\  / /  | |\\/| | |  _|   | |\\/| | \\___ \\ \r\n");
	printf("   \\ \\/ /   | |  | | | |___  | |  | |  ___) | \r\n");
	printf("    \\__/    |_|  |_| |_____| |_|  |_| |____/ \r\n");
	printf("\r\n");
	printf("         __      __     _      ___  \r\n");
	printf("         \\ \\    / /    / \\    |_ _| \r\n");
	printf("          \\ \\  / /    / _ \\    | |  \r\n");
	printf("           \\ \\/ /    / ___ \\   | |  \r\n");
	printf("            \\__/    /_/   \\_\\ |___|  \r\n");
    printf("System Initialized - Starting Main Loop\r\n");
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    //启动UART1接收
    /*清空缓冲区*/
	memset(rx_buffer, 0, PACKET_SIZE);
    /*接收串口数据*/
    HAL_UART_Receive_IT(&huart2, rx_buffer, 132);  // 先接收包头
	//printf("DATA_HEAD_ALREADY\r\n");
    system_ready = 1;
while (1) {
			//printf("DATA_HEAD_ALREADY\r\n");
		  //MX_X_CUBE_AI_Process();
      // 检查是否收到温度数据
        if (temp_data_received) {
            float Data[8][8] = {
                {25.00,25.00,25.00,25.00,25.00,25.00,25.00,25.00},
                {25.00,25.00,25.00,25.00,25.00,25.00,25.00,25.00},
                {25.00,25.00,25.00,25.00,25.00,25.00,25.00,25.00},
                {25.00,25.00,25.00,25.00,26.00,25.00,25.00,25.00},
                {25.00,25.00,25.00,26.00,27.00,26.00,25.00,25.00},
                {25.00,25.00,25.00,25.00,26.00,25.00,25.00,25.00},
                {25.00,25.00,25.00,25.00,25.00,25.00,25.00,25.00},
                {25.00,25.00,25.00,25.00,25.00,25.00,25.00,25.00}
            };
            //if (ParseTemperatureData(rx_buffer, Data) == 0) {
            //NetPrint();  // 打印输入输出形状
            // 正确传递数据
            PrepareInput((float*)Data);
                        // 打印输入数据（示例）
//						printf("Input data sample:\r\n");
//						for(int i=0; i<8; i++) {
//								for(int j=0; j<8; j++) {
//										printf("%.2f ", Data[i][j]);
//								}
//								printf("\r\n");
//						}
//						printf("...\r\n");					
            // 获取预测结果
            float* output = Predict();
//						printf("Output data:\r\n");
//						for(int i=0; i<8; i++) {
//								for(int j=0; j<8; j++) {
//										printf("%.2f ", output[i*8 + j]);
//								}
//								printf("\r\n");
//						}
            float heatmap[8][8];
                for (int i = 0; i < 8; i++) {
                        for (int j = 0; j < 8; j++) {
                                heatmap[i][j] = output[i * 8 + j];
                    }		
            }
            DetectionResult detection_result;
            detection_result.peak_count = detect_peaks(heatmap, g_detection_threshold, detection_result.peaks);
            // 格式化并输出检测结果
            char result_text[256];
            format_detection_result(&detection_result, result_text, sizeof(result_text));
            printf("Detection Result:\r\n%s\r\n", result_text);
            // printf("Temperature data received, capturing camera image...\r\n");
            process_and_send_output(output, &detection_result);
            // 处理温度数据
            //process_temperature_data();
            //}
            // 重置标志
            temp_data_received = 1;
            memset(rx_buffer, 0, PACKET_SIZE);
    //					HAL_UART_Receive_IT(&huart2, rx_buffer, 132);
    //          HAL_Delay(100);
        }
      // 重新开始接收温度数据
    }
  /* USER CODE END 3 */
}




// 配置参数


// 查找8x8阵列中的最低温度
//float find_min_temperature(float temp_array[ARRAY_SIZE][ARRAY_SIZE]) {
//    float min_temp = temp_array[0][0];
//    
//    for (int i = 0; i < ARRAY_SIZE; i++) {
//        for (int j = 0; j < ARRAY_SIZE; j++) {
//							printf("%f  ",temp_array[i][j]);
//            if (temp_array[i][j] < min_temp) {
//                min_temp = temp_array[i][j];

//            }

//        }
//					        printf("\r\n");
//    }
//    return min_temp;
//}

// 计算热目标质心并打包数据通过串口发送
//void calculate_and_send_centroid(float temp_array[ARRAY_SIZE][ARRAY_SIZE], 
//                                 void (*uart_send_func)(uint8_t*, uint32_t)) {
//    // 1. 查找最低温度
//    float min_temp = find_min_temperature(temp_array);
//       // printf("%f\r\n",min_temp);
//    // 2. 计算背景阈值
//    float threshold = min_temp + BG_RANGE;
//    //printf("%f\r\n",threshold);
//    // 3. 计算加权质心
//    float sum_x = 0.0f;
//    float sum_y = 0.0f;
//    float weight_sum = 0.0f;
//    
//    for (int i = 0; i < ARRAY_SIZE; i++) {
//        for (int j = 0; j < ARRAY_SIZE; j++) {
//            float temp = temp_array[i][j];
//            
//            if (temp > threshold) {
//                float weight = temp - threshold;
//                
//                // 转换为坐标系，中心为原点
//                float x = j - (ARRAY_SIZE - 1) / 2.0f;
//                float y = (ARRAY_SIZE - 1) / 2.0f - i;  // Y轴向上为正
//                
//                sum_x += weight * x;
//                sum_y += weight * y;
//                weight_sum += weight;
//            }
//        }
//    }
//       printf("%f\r\n",weight_sum);
//    // 4. 计算质心坐标
//    float centroid_x, centroid_y;
//    if (weight_sum > 0.0f) {
//        centroid_x = - (sum_x / weight_sum);
//        centroid_y = - (sum_y / weight_sum);
//    } else {
//        // 无热目标，返回中心点
//        centroid_x = 0.0f;
//        centroid_y = 0.0f;
//    }
//    printf("%f,%f\r\n",centroid_x, centroid_y);
//    
//    // 5. 转换为像素坐标系（原点在左上角）
//    float pixel_x = centroid_x + (ARRAY_SIZE - 1) / 2.0f;
//    float pixel_y = (ARRAY_SIZE - 1) / 2.0f - centroid_y;
//    
//    // 6. 将浮点坐标映射到16位整数范围 (0-65535)
//    // 确保坐标在有效范围内
//    if (pixel_x < 0.0f) pixel_x = 0.0f;
//    if (pixel_x > (ARRAY_SIZE - 1)) pixel_x = (ARRAY_SIZE - 1);
//    if (pixel_y < 0.0f) pixel_y = 0.0f;
//    if (pixel_y > (ARRAY_SIZE - 1)) pixel_y = (ARRAY_SIZE - 1);
//    printf("%f,%f\r\n",pixel_x, pixel_y);
//    // 映射到16位整数范围 (0-65535)
//    uint16_t x_coord = (uint16_t)((pixel_x / (ARRAY_SIZE - 1)) * 65535.0f);
//    uint16_t y_coord = (uint16_t)((pixel_y / (ARRAY_SIZE - 1)) * 65535.0f);
//    
//    // 7. 创建数据包
//    uint8_t data_packet[UART_DATA_SIZE];
//    
//    // 设置起始字节
//    data_packet[0] = 0xAA;  // 起始字节1
//    data_packet[1] = 0xBB;  // 起始字节2
//    
//    // 设置X坐标（大端字节序）
//    data_packet[4] = (uint8_t)((x_coord >> 8) & 0xFF);  // X高字节
//    data_packet[5] = (uint8_t)(x_coord & 0xFF);         // X低字节
//    
//    // 设置Y坐标（大端字节序）
//    data_packet[2] = (uint8_t)((y_coord >> 8) & 0xFF);  // Y高字节
//    data_packet[3] = (uint8_t)(y_coord & 0xFF);         // Y低字节
//    
//    // 8. 通过串口发送数据
//    uart_send_func(data_packet, UART_DATA_SIZE);
//}
/**
  * @brief System Clock Configuration
  * @retval None
  */
//void process_temperature_data(void) {
//    // 这里可以添加温度数据的处理逻辑
//    printf("Processing temperature data...\r\n");
//		send_data_via_usart2(rx_buffer, 132);
//    // 实际应用中这里会解析温度数据
//}

void send_data_via_usart1(uint8_t* data, uint32_t size) {
    //uint8_t header[4] = {0xAA, 0xBB, (size >> 8) & 0xFF, size & 0xFF};

    // 发送包头
    //HAL_UART_Transmit(&huart1, header, 4, 1000);

    // 发送数据（分块发送）
    uint32_t sent = 0;
    while (sent < size) {
        uint32_t chunk_size = (size - sent > 100) ? 100 : (size - sent);
        HAL_UART_Transmit(&huart1, data + sent, chunk_size, 1000);
        sent += chunk_size;
    }

    //printf("Data sent via USART1: %lu bytes\r\n", size);
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//    if (huart->Instance == USART2) {
//        // 打印接收到的原始数据包（调试用）
//        printf("Received %d bytes: ", PACKET_SIZE);
//        for (int i = 0; i < PACKET_SIZE; i++) {
//            printf("%02X ", rx_buffer[i]);
//            if ((i + 1) % 16 == 0) printf("\r\n\t"); // 每16字节换行，便于查看
//        }
//        printf("\r\n");

//        // 查找 AA BB 包头的位置
//        int offset = 0;
//        while (offset < PACKET_SIZE - 1) {
//            if (rx_buffer[offset] == 0xAA && rx_buffer[offset+1] == 0xBB)
//                break;
//            offset++;
//        }
//        if (offset < PACKET_SIZE - 1) {
//            // 如果包头不在起始位置，将有效数据移到缓冲区开头
//            if (offset > 0) {
//                memmove(rx_buffer, rx_buffer + offset, PACKET_SIZE - offset);
//                // 注意：移动后缓冲区大小仍是 PACKET_SIZE，但有效数据长度变短
//                // 这里简单置标志，由上层解析时注意实际长度
//            }
//            temp_data_received = 1;
//        } else {
//            // 未找到合法包头，可选择性打印警告
//            printf("Warning: No AA BB header found in this packet\r\n");
//        }

//        // 重新启动下一次接收
//        HAL_UART_Receive_IT(&huart2, rx_buffer, PACKET_SIZE);
//    }
//}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//    if (huart->Instance == USART2) {
//        // 可选：调试打印（建议关闭，仅调试时开启）
//        // printf("UART2 received\r\n");
//        
//        // 直接设置标志，由主循环解析（解析函数会自行搜索包头）
//        temp_data_received = 1;
//        
//        // 重新启动下一次接收
//        HAL_UART_Receive_IT(&huart2, rx_buffer, PACKET_SIZE);
//    }
//}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        temp_data_received = 1;
        HAL_UART_Receive_IT(&huart2, rx_buffer, PACKET_SIZE);
    }
    else if (huart->Instance == USART1) {
        // 检查命令包头是否为 AA BB
        if (uart1_rx_buffer[0] == 0xAA && uart1_rx_buffer[1] == 0xBB) {
            // 解析阈值：高字节在前，低字节在后
            uint16_t raw_th = (uart1_rx_buffer[2] << 8) | uart1_rx_buffer[3];
            float new_th = (float)raw_th / 65535.0f;
            // 可选：限制范围 [0,1]
            if (new_th < 0.0f) new_th = 0.0f;
            if (new_th > 1.0f) new_th = 1.0f;
            g_detection_threshold = new_th;
            printf("Threshold updated to %.4f (raw=%d)\r\n", g_detection_threshold, raw_th);
        } else {
            // 可选：打印错误提示
            // printf("Invalid command header: 0x%02X 0x%02X\r\n", uart1_rx_buffer[0], uart1_rx_buffer[1]);
        }
        // 重新启动下一次接收
        HAL_UART_Receive_IT(&huart1, uart1_rx_buffer, 4);
    }
}
//int ParseTemperatureData(uint8_t* rx_data, float output[8][8])
//{
//    // 检查起始符
//    if (rx_data[0] != 0xAA || rx_data[1] != 0xBB) {
//        printf("Invalid header: 0x%02X 0x%02X\r\n", rx_data[0], rx_data[1]);
//        return -1;
//    }
//    
//    // 解析128字节有效数据（64个16位数据）
//    uint8_t* data_ptr = &rx_data[2]; // 跳过起始符
//    
//    for (int i = 0; i < 8; i++) {
//        for (int j = 0; j < 8; j++) {
//            // 每两个字节组成一个16位数据，先低位后高位
//            int index = (i * 8 + j) * 2;
//            uint16_t raw_value = (data_ptr[index + 1] << 8) | data_ptr[index];
//            
//            // 转换为float并除以100
//            output[i][j] = (float)raw_value / 100.0f;
//        }
//    }
//    
//    return 0;
//}
int ParseTemperatureData(uint8_t* rx_data, float output[8][8])
{
    // 搜索 AA BB 包头
    int offset = 0;
    while (offset < PACKET_SIZE - 1) {
        if (rx_data[offset] == 0xAA && rx_data[offset+1] == 0xBB)
            break;
        offset++;
    }
    if (offset >= PACKET_SIZE - 1) {
        printf("Header AA BB not found\r\n");
        return -1;
    }
    
    // 有效数据起始位置
    uint8_t* data_ptr = &rx_data[offset + 2];
    
    // 确保剩余数据足够 130 字节
//    if (PACKET_SIZE - (offset + 2) < 130) {
//        printf("Insufficient data after header\r\n");
//        return -1;
//    }
    
    // 解析 65 个 uint16_t（低位在前）
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int index = (i * 8 + j) * 2;
            uint16_t raw_value = (data_ptr[index + 1] << 8) | data_ptr[index];
            output[i][j] = (float)raw_value / 100.0f;
        }
    }
    return 0;
}

/**
 * @brief 检测热力图中的峰值点，过滤角落区域的假阳性
 * @param heatmap 8x8热力图数据
 * @param threshold 检测阈值
 * @param peaks 输出峰值数组
 * @return 检测到的峰值数量
 */
int detect_peaks(float heatmap[GRID_SIZE][GRID_SIZE], float threshold, Peak peaks[]) {
    int peak_count = 0;
    
    // 第一遍：寻找所有超过阈值的点
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            // 检查是否超过阈值
            if (heatmap[i][j] >= threshold) {
                int is_local_max = 1;
                
                // 检查3x3邻域内的局部最大值
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        int ni = i + di;
                        int nj = j + dj;
                        
                        // 跳过自身和边界外的点
                        if (ni < 0 || ni >= GRID_SIZE || nj < 0 || nj >= GRID_SIZE) {
                            continue;
                        }
                        
                        // 如果邻域内有更大的值，则不是局部最大值
                        if (heatmap[ni][nj] > heatmap[i][j]) {
                            is_local_max = 0;
                            break;
                        }
                    }
                    if (!is_local_max) break;
                }
                
                // 如果是局部最大值且不在无效位置，则添加到峰值列表
                if (is_local_max && !is_invalid_position(i, j) && peak_count < MAX_PEAKS) {
                    peaks[peak_count].row = i;
                    peaks[peak_count].col = j;
                    peaks[peak_count].value = heatmap[i][j];
                    peak_count++;
                }
            }
        }
    }
    
    // 按峰值强度排序（降序）
    for (int i = 0; i < peak_count - 1; i++) {
        for (int j = 0; j < peak_count - i - 1; j++) {
            if (peaks[j].value < peaks[j + 1].value) {
                Peak temp = peaks[j];
                peaks[j] = peaks[j + 1];
                peaks[j + 1] = temp;
            }
        }
    }
    
    return peak_count;
}

/**
 * @brief 检查位置是否在无效区域（角落）
 * @param row 行坐标
 * @param col 列坐标
 * @return 1表示无效位置，0表示有效位置
 */
int is_invalid_position(int row, int col) {
    for (int i = 0; i < 4; i++) {
        if (row == INVALID_POSITIONS[i][0] && col == INVALID_POSITIONS[i][1]) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief 计算两点之间的曼哈顿距离
 * @param row1, col1 第一个点的坐标
 * @param row2, col2 第二个点的坐标
 * @return 曼哈顿距离
 */
int calculate_manhattan_distance(int row1, int col1, int row2, int col2) {
    return abs(row1 - row2) + abs(col1 - col2);
}

/**
 * @brief 严格双向匹配策略
 * @param true_peaks 真实峰值数组
 * @param true_count 真实峰值数量
 * @param pred_peaks 预测峰值数组
 * @param pred_count 预测峰值数量
 * @return 匹配结果
 */
MatchingResult strict_bidirectional_matching(Peak true_peaks[], int true_count, Peak pred_peaks[], int pred_count) {
    MatchingResult result = {0};
    int true_matched[MAX_PEAKS] = {0};
    int pred_matched[MAX_PEAKS] = {0};
    
    // 第一阶段：为每个真实热点寻找最近的预测点
    for (int i = 0; i < true_count; i++) {
        int min_distance = 9999;
        int best_match = -1;
        
        for (int j = 0; j < pred_count; j++) {
            if (pred_matched[j]) {
                continue;  // 预测点已被匹配
            }
            
            int distance = calculate_manhattan_distance(
                true_peaks[i].row, true_peaks[i].col,
                pred_peaks[j].row, pred_peaks[j].col
            );
            
            if (distance <= 1 && distance < min_distance) {  // max_distance=1
                min_distance = distance;
                best_match = j;
            }
        }
        
        if (best_match != -1) {
            result.match_count++;
            true_matched[i] = 1;
            pred_matched[best_match] = 1;
        }
    }
    
    result.true_positives = result.match_count;
    
    // 计算误报（未被匹配的预测点）
    for (int j = 0; j < pred_count; j++) {
        if (!pred_matched[j]) {
            result.false_positives++;
        }
    }
    
    // 计算漏报（未被匹配的真实点）
    for (int i = 0; i < true_count; i++) {
        if (!true_matched[i]) {
            result.false_negatives++;
        }
    }
    
    return result;
}

/**
 * @brief 格式化检测结果为文本
 * @param result 检测结果
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 */
//void format_detection_result(DetectionResult* result, char* buffer, int buffer_size) {
//    char temp[256] = "";
//    char line[64];

//    for (int i = 0; i < result->peak_count; i++) {
//        snprintf(line, sizeof(line), "person%d [position: %d, %d]; ", 
//                 i + 1, result->peaks[i].col, result->peaks[i].row);
//        strcat(temp, line);
//    }
//    
//    if (result->peak_count > 0) {
//        snprintf(line, sizeof(line), "\ndetected: %d people.", result->peak_count);
//        strcat(temp, line);
//    }
//    
//    strncpy(buffer, temp, buffer_size - 1);
//    buffer[buffer_size - 1] = '\0';
//}

void format_detection_result(DetectionResult* result, char* buffer, int buffer_size) {
    // 检查峰值数量是否大于8，如果是则直接跳出函数
    if (result->peak_count > 8) {
        printf("[WARNING] Too many peaks (%d > 8), truncating\r\n", result->peak_count);
        snprintf(buffer, buffer_size, "Error: Too many peaks (%d > 8)", result->peak_count);
        return; // 直接跳出函数
    }
    
    char temp[256] = "";
    char line[64];
    
    for (int i = 0; i < result->peak_count; i++) {
        snprintf(line, sizeof(line), "person%d [position: %d, %d]; ", 
                 i + 1, result->peaks[i].col, result->peaks[i].row);
        strcat(temp, line);
    }
    
    if (result->peak_count > 0) {
        snprintf(line, sizeof(line), "\ndetected: %d people.", result->peak_count);
        strcat(temp, line);
    }
    
    strncpy(buffer, temp, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
}

/**
 * @brief 处理AI输出热力图：阈值过滤、归一化、峰值强制为1，然后打包通过USART1发送
 * @param heatmap 指向8x8热力图数据的指针（按行优先存储）
 * @param result 峰值检测结果
 */
void process_and_send_output(float* heatmap, DetectionResult* result) {
    // 使用全局阈值 g_detection_threshold
    float threshold = g_detection_threshold;
    float processed[GRID_SIZE][GRID_SIZE];
    float max_val = 0.0f;
    int i, j;
    
    // 1. 查找全局最大值（仅用于归一化分母，可包含小于阈值的点）
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            float val = heatmap[i * GRID_SIZE + j];
            if (val > max_val) max_val = val;
        }
    }
    
    // 防止分母为零
    float denom = max_val - threshold;
    if (denom <= 0.0f) denom = 1.0f;  // 如果阈值大于等于最大值，则所有点直接置0
    
    // 2. 阈值过滤 + 归一化
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            float val = heatmap[i * GRID_SIZE + j];
            if (val < threshold) {
                processed[i][j] = 0.0f;
            } else {
                processed[i][j] = (val - threshold) / denom;
                // 限制在[0,1]内
                if (processed[i][j] > 1.0f) processed[i][j] = 1.0f;
            }
        }
    }
    
    // 3. 峰值点强制设为1
    for (i = 0; i < result->peak_count; i++) {
        int row = result->peaks[i].row;
        int col = result->peaks[i].col;
        if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE) {
            processed[row][col] = 1.0f;
        }
    }
    
    // 4. 量化：乘以100，转为uint16_t（0~100）
    uint16_t quantized[GRID_SIZE][GRID_SIZE];
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            uint16_t val = (uint16_t)(processed[i][j] * 100.0f + 0.5f);  // 四舍五入
            if (val > 100) val = 100;
            quantized[i][j] = val;
        }
    }
    
    // 5. 打包：AA BB + 128字节（64个uint16_t，低位在前）
    uint8_t tx_buffer[2 + 128];  // 130字节
    tx_buffer[0] = 0xAA;
    tx_buffer[1] = 0xBB;
    
    int idx = 2;
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            uint16_t value = quantized[i][j];
            tx_buffer[idx++] = (uint8_t)(value & 0xFF);      // 低字节
            tx_buffer[idx++] = (uint8_t)((value >> 8) & 0xFF); // 高字节
        }
    }
    
    // 6. 通过USART1发送
    send_data_via_usart1(tx_buffer, sizeof(tx_buffer));
    printf("Processed heatmap sent via USART1 (130 bytes)\r\n");
}
//void format_detection_result(DetectionResult* result, char* buffer, int buffer_size) {
//    char temp[256] = "";
//    char line[64];
//    
//    // 调试信息1：输入参数
//    printf("[DEBUG] format_detection_result called:\r\n");
//    printf("  result address: %p\r\n", result);
//    printf("  result->peak_count: %d\r\n", result->peak_count);
//    printf("  buffer address: %p\r\n", buffer);
//    printf("  buffer_size: %d\r\n", buffer_size);
//    
//    // 调试信息2：遍历前的信息
//    printf("  Starting to process %d peaks...\r\n", result->peak_count);
//    
//    for (int i = 0; i < result->peak_count; i++) {
//        // 调试信息3：每个peak的详细信息
//        printf("  Processing peak %d:\r\n", i);
//        printf("    col: %d, row: %d\r\n", result->peaks[i].col, result->peaks[i].row);
//        
//        snprintf(line, sizeof(line), "person%d [position: %d, %d]; ", 
//                 i + 1, result->peaks[i].col, result->peaks[i].row);
//        
//        // 调试信息4：拼接后的line内容
//        printf("    line content: %s\r\n", line);
//        
//        strcat(temp, line);
//        
//        // 调试信息5：当前的temp内容
//        printf("    current temp: %s\r\n", temp);
//    }
//    
//    if (result->peak_count > 0) {
//        snprintf(line, sizeof(line), "\ndetected: %d people.", result->peak_count);
//        printf("  Adding summary line: %s\r\n", line);
//        strcat(temp, line);
//    }
//    
//    // 调试信息6：最终temp内容
//    printf("  Final temp before copy: %s\r\n", temp);
//    printf("  temp length: %d\r\n", strlen(temp));
//    
//    strncpy(buffer, temp, buffer_size - 1);
//    buffer[buffer_size - 1] = '\0';
//    
//    // 调试信息7：最终buffer内容
//    printf("  Final buffer after copy: %s\r\n", buffer);
//    printf("  buffer length: %d\r\n", strlen(buffer));
//    printf("[DEBUG] format_detection_result finished.\r\n\r\n");
//}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  /* USER CODE BEGIN MX_GPIO_Init_2 */
  __HAL_RCC_GPIOB_CLK_ENABLE();
//    
//    // 初始化PB0
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_4;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	    // 初始化PB0

	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  /* USER CODE END MX_GPIO_Init_2 */
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

/* USER CODE BEGIN 4 */
int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 10);
    return ch;
}

void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit( &huart1,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
  
}

int fputc(int ch, FILE *f)
{

	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);	
	
	return (ch);
}


int fgetc(FILE *f)
{
		
	int ch;
	HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}
	
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}

#endif /* USE_FULL_ASSERT */
