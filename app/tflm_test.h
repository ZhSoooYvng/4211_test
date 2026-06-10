#ifndef __TFLM_TEST_H__
#define __TFLM_TEST_H__

#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DETECTION_THRESHOLD 0.37f  // 默认检测阈值

extern float g_detection_threshold;

#define GRID_SIZE 8
#define MAX_PEAKS 10


typedef struct {
    int row;
    int col;
    float value;
} Peak;


typedef struct {
    Peak peaks[MAX_PEAKS];
    int peak_count;
} DetectionResult;


typedef struct {
    int true_positives;
    int false_positives;
    int false_negatives;
    int match_count;
} MatchingResult;


typedef enum {
STATE_IDLE        = 0,  // 无人
STATE_PRE_PREDICT = 1,  // 预判（可能有人）
STATE_DETECTED    = 2   // 确认有人
} SystemState;
// 声明新增的综合判断函数
SystemState get_system_state(float raw[GRID_SIZE][GRID_SIZE], 
float heatmap[GRID_SIZE][GRID_SIZE], 
DetectionResult* result, 
float* out_delta_t);

extern void format_detection_result(DetectionResult* result, char* buffer, int buffer_size); 
extern MatchingResult strict_bidirectional_matching(Peak true_peaks[], int true_count, Peak pred_peaks[], int pred_count);
extern int detect_peaks(float heatmap[GRID_SIZE][GRID_SIZE], float threshold, Peak peaks[]);
extern void process_and_send_output(float heatmap[8][8], DetectionResult* result);

#endif
