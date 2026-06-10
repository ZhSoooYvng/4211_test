#include "tflm_test.h"

float g_detection_threshold = DETECTION_THRESHOLD;  // 可动态修改的阈值

const int INVALID_POSITIONS[4][2] = {
    {0, 0}, {0, 7}, {7, 0}, {7, 7}
};

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
    for (int j = 0; j < pred_count; j++){
        if (!pred_matched[j]){
            result.false_positives++;
        }
    }
    // 计算漏报（未被匹配的真实点）
    for (int i = 0; i < true_count; i++){
        if (!true_matched[i]){
            result.false_negatives++;
        }
    }   
    return result;
}
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
        snprintf(line, sizeof(line), "person%d [position: %d, %d]; ", i + 1, result->peaks[i].col, result->peaks[i].row);
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
void process_and_send_output(float heatmap[8][8], DetectionResult* result){
    // 使用全局阈值 g_detection_threshold
    float threshold = g_detection_threshold;
    float processed[GRID_SIZE][GRID_SIZE];
    float max_val = 0.0f;
    int i, j;
    
    // 1. 查找全局最大值（仅用于归一化分母，可包含小于阈值的点）
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            float val = heatmap[i][j];
            if (val > max_val) max_val = val;
        }
    }
    
    // 防止分母为零
    float denom = max_val - threshold;
    if (denom <= 0.0f) denom = 1.0f;  // 如果阈值大于等于最大值，则所有点直接置0
    
    // 2. 阈值过滤 + 归一化
    for (i = 0; i < GRID_SIZE; i++) {
        for (j = 0; j < GRID_SIZE; j++) {
            float val = heatmap[i][j];
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
    //send_data_via_usart1(tx_buffer, sizeof(tx_buffer));
    printf("Processed heatmap sent via USART1 (130 bytes)\r\n");
}


/*
 ====================================================================
  以下为新增的 Pre-Predict 极差分析与滑动平均 drift 补偿模块
==================================================================== */
// 预判相关阈值配置
#define PRE_PREDICT_DIFF_THRESHOLD 2.0f  // 预判触发的温差阈值(℃)
#define DRIFT_WINDOW_SIZE 10             // 滑动平均窗口大小

// 滑动窗口缓冲区
static float drift_window[DRIFT_WINDOW_SIZE] = {0};
static int drift_index = 0;
static int drift_count = 0;
static float current_drift = 0.0f;

/**
 * @brief 更新无人状态下的温差滑动平均 (drift)
 * @param diff 当前帧的有效极差
 */
static void update_drift(float diff) {
    drift_window[drift_index] = diff;
    drift_index = (drift_index + 1) % DRIFT_WINDOW_SIZE;
    if (drift_count < DRIFT_WINDOW_SIZE) {
        drift_count++;
    }
    
    float sum = 0.0f;
    for (int i = 0; i < drift_count; i++) {
        sum += drift_window[i];
    }
    current_drift = sum / drift_count;
}

/**
 * @brief 分析原始热堆数据，计算有效区域的温度极差
 */
float analyze_raw_thermal(float raw[GRID_SIZE][GRID_SIZE]) {
    float max_val = -999.0f;
    float min_val =  999.0f;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (is_invalid_position(i, j)) continue;
            float v = raw[i][j];
            if (v > max_val) max_val = v;
            if (v < min_val) min_val = v;
        }
    }
    return (max_val > -999.0f) ? (max_val - min_val) : 0.0f;
}

/**
 * @brief 综合判断系统状态（包含 drift 补偿逻辑）
 * @param raw 原始热堆数据
 * @param heatmap 神经网络输出热力图
 * @param result 检测结果指针
 * @param out_delta_t 输出：补偿后的 ΔT
 * @return SystemState 状态枚举
 */
SystemState get_system_state(float raw[GRID_SIZE][GRID_SIZE], 
                             float heatmap[GRID_SIZE][GRID_SIZE], 
                             DetectionResult* result, 
                             float* out_delta_t) 
{
    // 1. 获取当前原始温差 diff
    float diff = analyze_raw_thermal(raw);
    
    // 2. 获取神经网络峰值检测结果
    result->peak_count = detect_peaks(heatmap, g_detection_threshold, result->peaks);
    
    // 3. 状态判定与 drift 更新
    if (result->peak_count > 0) {
        // 检测到人，不更新 drift
        // 仍计算个 ΔT 用于日志显示
        *out_delta_t = diff - current_drift;
        return STATE_DETECTED;
    } else {
        // 未检测到人，计算补偿后的 ΔT = diff - drift
        float delta_t = diff - current_drift;
        *out_delta_t = delta_t;
        
        if (delta_t >= PRE_PREDICT_DIFF_THRESHOLD) {
            // 预判状态，可能是人刚进来，此时停止更新背景 drift
            return STATE_PRE_PREDICT;
        } else {
            // 纯无人状态，更新背景 drift
            update_drift(diff);
            return STATE_IDLE;
        }
    }
}


