/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <math.h>
#include "tflm_test.h"

#include "tensorflow/lite/core/c/common.h"
// #include "models/hello_world_float_model_data.h"
// #include "models/hello_world_int8_model_data.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/recording_micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/cortex_m_generic/debug_log_callback.h"
#include "tflm_app.h"
#include "dwt.h"
#include "models/test_model_data.h"


// Macro to control printing of inference output
#define TFLM_DEBUG_PRINT 1

extern "C" volatile uint32_t g_core_clk;

namespace {
using HelloWorldOpResolver = tflite::MicroMutableOpResolver<1>;

TfLiteStatus RegisterOps(HelloWorldOpResolver &op_resolver)
{
    TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
    return kTfLiteOk;
}

// A larger op resolver with more common ops for generic models.
// Increase the template parameter if your model needs more ops.
using GenericOpResolver = tflite::MicroMutableOpResolver<18>;
TfLiteStatus RegisterGenericOps(GenericOpResolver &op_resolver) {
    TF_LITE_ENSURE_STATUS(op_resolver.AddMul());
    TF_LITE_ENSURE_STATUS(op_resolver.AddTranspose());
    TF_LITE_ENSURE_STATUS(op_resolver.AddReshape());
    TF_LITE_ENSURE_STATUS(op_resolver.AddConv2D());
    TF_LITE_ENSURE_STATUS(op_resolver.AddLeakyRelu());
    TF_LITE_ENSURE_STATUS(op_resolver.AddConcatenation());
    TF_LITE_ENSURE_STATUS(op_resolver.AddLogistic());
    TF_LITE_ENSURE_STATUS(op_resolver.AddAdd());
    TF_LITE_ENSURE_STATUS(op_resolver.AddSpaceToBatchNd());
    TF_LITE_ENSURE_STATUS(op_resolver.AddBatchToSpaceNd());
    return kTfLiteOk;
}

// TfLiteStatus RegisterGenericOps(GenericOpResolver &op_resolver) {
//     TF_LITE_ENSURE_STATUS(op_resolver.AddPad());
//     TF_LITE_ENSURE_STATUS(op_resolver.AddConv2D(tflite::Register_CONV_2D_INT8()));
//     TF_LITE_ENSURE_STATUS(op_resolver.AddDepthwiseConv2D(tflite::Register_DEPTHWISE_CONV_2D_INT8()));
//     TF_LITE_ENSURE_STATUS(op_resolver.AddStridedSlice());
//     TF_LITE_ENSURE_STATUS(op_resolver.AddMul());
//     TF_LITE_ENSURE_STATUS(op_resolver.AddAdd(tflite::Register_ADD_INT8()));
//     TF_LITE_ENSURE_STATUS(op_resolver.AddRelu());
//     TF_LITE_ENSURE_STATUS(op_resolver.AddReshape());
//     TF_LITE_ENSURE_STATUS(op_resolver.AddShape());
//     TF_LITE_ENSURE_STATUS(op_resolver.AddPack());
//     TF_LITE_ENSURE_STATUS(op_resolver.AddConcatenation());
//     TF_LITE_ENSURE_STATUS(op_resolver.AddSoftmax(tflite::Register_SOFTMAX_INT8()));
//     return kTfLiteOk;
// }

} // namespace

TfLiteStatus ProfileMemoryAndLatency()
{
    tflite::MicroProfiler profiler;
    HelloWorldOpResolver op_resolver;
    TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

    // Arena size just a round number. The exact arena usage can be determined
    // using the RecordingMicroInterpreter.
    constexpr int kTensorArenaSize = 3000;
    uint8_t tensor_arena[kTensorArenaSize];
    constexpr int kNumResourceVariables = 24;

    tflite::RecordingMicroAllocator *allocator(tflite::RecordingMicroAllocator::Create(tensor_arena, kTensorArenaSize));
    tflite::RecordingMicroInterpreter interpreter(
    tflite::GetModel(NULL), op_resolver, allocator, // g_hello_world_float_model_data
    tflite::MicroResourceVariables::Create(allocator, kNumResourceVariables), &profiler);

    TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());
    TFLITE_CHECK_EQ(interpreter.inputs_size(), 1);
    interpreter.input(0)->data.f[0] = 1.f;
    TF_LITE_ENSURE_STATUS(interpreter.Invoke());

    MicroPrintf(""); // Print an empty new line
    profiler.LogTicksPerTagCsv();

    MicroPrintf(""); // Print an empty new line
    interpreter.GetMicroAllocator().PrintAllocations();
    return kTfLiteOk;
}

TfLiteStatus LoadQuantModelAndPerformInference()
{
    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    const tflite::Model *model = ::tflite::GetModel(test_model_data);  // g_hello_world_int8_model_data
    TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

    HelloWorldOpResolver op_resolver;
    TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

    // Arena size just a round number. The exact arena usage can be determined
    // using the RecordingMicroInterpreter.
    constexpr int kTensorArenaSize = 3000;
    uint8_t tensor_arena[kTensorArenaSize];

    tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena, kTensorArenaSize);

    TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());

    TfLiteTensor *input = interpreter.input(0);
    TFLITE_CHECK_NE(input, nullptr);

    TfLiteTensor *output = interpreter.output(0);
    TFLITE_CHECK_NE(output, nullptr);

    float output_scale = output->params.scale;
    int output_zero_point = output->params.zero_point;

    // Check if the predicted output is within a small range of the
    // expected output
    float epsilon = 0.05;

    constexpr int kNumTestValues = 4;
    float golden_inputs_float[kNumTestValues] = {0.77, 1.57, 2.3, 3.14};

    // The int8 values are calculated using the following formula
    // (golden_inputs_float[i] / input->params.scale + input->params.zero_point)
    int8_t golden_inputs_int8[kNumTestValues] = {-96, -63, -34, 0};

    for (int i = 0; i < kNumTestValues; ++i) {
        input->data.int8[0] = golden_inputs_int8[i];
        TF_LITE_ENSURE_STATUS(interpreter.Invoke());
        float y_pred = (output->data.int8[0] - output_zero_point) * output_scale;
        TFLITE_CHECK_LE(abs(sin(golden_inputs_float[i]) - y_pred), epsilon);
    }

    return kTfLiteOk;
}

// Arena size. This needs to be big enough for the model.
// We can increase this if needed.
constexpr uint32_t kTensorArenaSize = 2 * 1024 * 1024; // 100KB, a reasonable starting point
uint8_t tensor_arena[kTensorArenaSize];

/*热堆模拟数据*/
float test_data[8][8] = {
{25.00,25.00,25.00,25.00,25.00,25.00,25.00,25.00},
{25.00,25.00,25.00,25.00,25.00,25.00,25.00,25.00},
{25.00,25.00,25.00,25.00,25.00,25.00,25.00,25.00},
{25.00,25.00,25.00,25.00,26.00,25.00,25.00,25.00},
{25.00,25.00,25.00,26.00,27.00,26.00,25.00,25.00},
{25.00,25.00,25.00,25.00,26.00,25.00,25.00,25.00},
{25.00,25.00,25.00,25.00,25.00,25.00,25.00,25.00},
{25.00,25.00,25.00,25.00,25.00,25.00,25.00,25.00}
};

TfLiteStatus RunInference(const unsigned char* model_data)
{
    // Map the model into a usable data structure.
    const tflite::Model* model = tflite::GetModel(model_data);
    TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

    // Use the generic op resolver
    GenericOpResolver op_resolver;
    TF_LITE_ENSURE_STATUS(RegisterGenericOps(op_resolver));

    tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena, kTensorArenaSize);
    TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());

    // Get information about input tensor.
    TfLiteTensor* input = interpreter.input(0);
    TFLITE_CHECK_NE(input, nullptr);
	MicroPrintf("=====input_bytes=%d=====\r\n",input->bytes);
    // input->data.f[0]= 1.0f;
	// MicroPrintf("=====input_bytes=%f=====\r\n",input->data.f[0]);
    for (uint8_t i=0; i<8;i++)
    {
        for(uint8_t j=0;j<8;j++){
            input->data.f[8*i+j] = test_data[i][j];
        }
    }
    // Fill input tensor with dummy data (zeros).
    //memset(input->data.raw, 1, input->bytes);
    MicroPrintf("Input tensor has been filled with zeros.");
    
    // Run inference and time it.
    MicroPrintf("Running inference...");
    DWT_ClearCycles();
    TF_LITE_ENSURE_STATUS(interpreter.Invoke());
    uint32_t cycles = DWT_GetCycles();
    float time_ms = (float)cycles / (float)g_core_clk * 1000.0f;
    MicroPrintf("Inference done in %lu cycles, %.3f ms.", cycles, time_ms);


#if TFLM_DEBUG_PRINT
    // Print output tensor.
    TfLiteTensor* output = interpreter.output(0);
    /*检测output不为空*/
    TFLITE_CHECK_NE(output, nullptr);
    MicroPrintf("Output tensor shape:");
    // for (int i = 0; i < output->dims->size; ++i) {
    //     MicroPrintf("  dim %d: %d", i, output->dims->data[i]);
    // }
    // MicroPrintf("Output tensor data:");
    // // Handle different data types for printing
    // if (output->type == kTfLiteFloat32) {
    //     for (uint32_t i = 0; i < output->bytes / sizeof(float); i++) {
    //         MicroPrintf("  [%d]: %f", i, output->data.f[i]);
    //     }
    // } else if (output->type == kTfLiteInt8) {
    //     for (uint32_t i = 0; i < output->bytes; i++) {
    //         MicroPrintf("  [%d]: %d", i, output->data.int8[i]);
    //     }
    // } else if (output->type == kTfLiteUInt8) {
    //     for (uint32_t i = 0; i < output->bytes; i++) {
    //         MicroPrintf("  [%d]: %u", i, output->data.uint8[i]);
    //     }
    // }
    // else {
    //     MicroPrintf("  Output tensor type %s not handled for printing.", TfLiteTypeGetName(output->type));
    // }
    for (uint8_t i=0;i<8;i++){
        for(uint8_t j=0;j<8;j++){
            printf("%f    ",output->data.f[8*i+j]);
        }
        printf("\r\n");
    }
#endif // TFLM_DEBUG_PRINT

    MicroPrintf(""); // New line for separation
    return kTfLiteOk;
}


void debug_log_printf(const char* s)
{
    printf("%s", s);
}

// int hello_world_test()
// {
//     RegisterDebugLogCallback(debug_log_printf);
		
//     tflite::InitializeTarget();
//     /*内核定时器初始化，用于较准确的输出时间*/
//     DWT_Init(); // Initialize DWT for timing

//     // --- Original tests are preserved ---
//     // TF_LITE_ENSURE_STATUS(ProfileMemoryAndLatency());
//     // TF_LITE_ENSURE_STATUS(LoadFloatModelAndPerformInference());
//     // TF_LITE_ENSURE_STATUS(LoadQuantModelAndPerformInference());

//     // --- Run new generic inference function ---
//     MicroPrintf("\n--- Running generic inference on model ---");
//     TF_LITE_ENSURE_STATUS(RunInference(test_model_data));
	
//     MicroPrintf("~~~ALL TESTS PASSED~~~\n");
//     return kTfLiteOk;
// }


TfLiteStatus RunInference_test(const unsigned char* model_data)
{
    // Map the model into a usable data structure.
    const tflite::Model* model = tflite::GetModel(model_data);
    TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

    // Use the generic op resolver
    GenericOpResolver op_resolver;
    TF_LITE_ENSURE_STATUS(RegisterGenericOps(op_resolver));

    tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena, kTensorArenaSize);
    TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());

    // Get information about input tensor.
    TfLiteTensor* input = interpreter.input(0);
    TFLITE_CHECK_NE(input, nullptr);
	MicroPrintf("=====input_bytes=%d=====\r\n",input->bytes);
    // input->data.f[0]= 1.0f;
	// MicroPrintf("=====input_bytes=%f=====\r\n",input->data.f[0]);
    for (uint8_t i=0; i<8;i++)
    {
        for(uint8_t j=0;j<8;j++){
            input->data.f[8*i+j] = thermopile_data[i][j];
        }
    }
    // Fill input tensor with dummy data (zeros).
    //memset(input->data.raw, 1, input->bytes);
    //MicroPrintf("Input tensor has been filled with zeros.");
    
    // Run inference and time it.
    MicroPrintf("Running inference...");
    DWT_ClearCycles();
    TF_LITE_ENSURE_STATUS(interpreter.Invoke());
    uint32_t cycles = DWT_GetCycles();
    float time_ms = (float)cycles / (float)g_core_clk * 1000.0f;
    MicroPrintf("Inference done in %lu cycles, %.3f ms.", cycles, time_ms);


#if TFLM_DEBUG_PRINT
    // Print output tensor.
    TfLiteTensor* output = interpreter.output(0);
    /*检测output不为空*/
    TFLITE_CHECK_NE(output, nullptr);
    MicroPrintf("Output tensor shape:");
    // for (int i = 0; i < output->dims->size; ++i) {
    //     MicroPrintf("  dim %d: %d", i, output->dims->data[i]);
    // }
    // MicroPrintf("Output tensor data:");
    // // Handle different data types for printing
    // if (output->type == kTfLiteFloat32) {
    //     for (uint32_t i = 0; i < output->bytes / sizeof(float); i++) {
    //         MicroPrintf("  [%d]: %f", i, output->data.f[i]);
    //     }
    // } else if (output->type == kTfLiteInt8) {
    //     for (uint32_t i = 0; i < output->bytes; i++) {
    //         MicroPrintf("  [%d]: %d", i, output->data.int8[i]);
    //     }
    // } else if (output->type == kTfLiteUInt8) {
    //     for (uint32_t i = 0; i < output->bytes; i++) {
    //         MicroPrintf("  [%d]: %u", i, output->data.uint8[i]);
    //     }
    // }
    // else {
    //     MicroPrintf("  Output tensor type %s not handled for printing.", TfLiteTypeGetName(output->type));
    // }
    float heatmap[8][8];
    for (uint8_t i=0;i<8;i++){
        for(uint8_t j=0;j<8;j++){
            printf("%f    ",output->data.f[8*i+j]);
            heatmap[i][j] = output->data.f[8*i+j];
        }
				if(i!=7){
					printf("\r\n");
				}	
    }
		/*热堆数据处理（含 Pre-Predict 和 Drift 补偿）*/
		DetectionResult detection_result;
		float delta_t = 0.0f;
		// 调用综合判断函数，传入原始热堆数据 thermopile_data 和网络输出 heatmap
		SystemState state = get_system_state(thermopile_data, heatmap, &detection_result,&delta_t);
		printf("%.2f    \r\n",delta_t);
		switch (state) {
			case STATE_DETECTED: {
				char result_text[256];
				format_detection_result(&detection_result, result_text, sizeof(result_text));
				printf("Detection Result:\r\n%s\r\n", result_text);
				printf("Temperature data received, capturing camera image...\r\n");
				process_and_send_output(heatmap, &detection_result);
				break;
			}
			case STATE_PRE_PREDICT: {
				// 预判状态：可能有人，输出提示和当前的补偿温差
				printf("[PRE-PREDICT] Anomaly detected! Delta_T = %.2f (diff - drift)",delta_t);
				// 预判状态下是否要捕获摄像头或发送数据，您可以根据需求决定
				// process_and_send_output(heatmap, &detection_result); 
				break;
			}
			case STATE_IDLE: {
				// 纯无人状态
				printf("[IDLE] Background updating... Delta_T = %.2f\r\n", delta_t);
				break;
			}
			default:
				
				break;
		}

#endif // TFLM_DEBUG_PRINT

    MicroPrintf(""); // New line for separation
    // MicroPrintf("=====input_bytes=%d=====\r\n",input->bytes);
    // // input->data.f[0]= 1.0f;
	// // MicroPrintf("=====input_bytes=%f=====\r\n",input->data.f[0]);
    // for (uint8_t i=0; i<8;i++)
    // {
    //     for(uint8_t j=0;j<8;j++){
    //         input->data.f[8*i+j] = test_data[i][j];
    //     }
    // }
    // // Fill input tensor with dummy data (zeros).
    // //memset(input->data.raw, 1, input->bytes);
    // MicroPrintf("Input tensor has been filled with zeros.");
    
    // // Run inference and time it.
    // MicroPrintf("Running inference...");
    // DWT_ClearCycles();
    // TF_LITE_ENSURE_STATUS(interpreter.Invoke());
    // cycles = DWT_GetCycles();
    // time_ms = (float)cycles / (float)g_core_clk * 1000.0f;
    // MicroPrintf("Inference done in %lu cycles, %.3f ms.", cycles, time_ms);
    // MicroPrintf("Output tensor shape:");
    // for (uint8_t i=0;i<8;i++){
    //     for(uint8_t j=0;j<8;j++){
    //         printf("%f    ",output->data.f[8*i+j]);
    //         heatmap[i][j] = output->data.f[8*i+j];
    //     }
    //     printf("\r\n");
    // }
    return kTfLiteOk;
}


int hello_world_test(void)
{
    RegisterDebugLogCallback(debug_log_printf);
	
    tflite::InitializeTarget();

    /*内核定时器初始化，用于较准确的输出时间*/
    DWT_Init(); // Initialize DWT for timing

    /*模拟数据处理*/
    // --- Run new generic inference function ---
    MicroPrintf("\n--- Running generic inference on model ---");
    TF_LITE_ENSURE_STATUS(RunInference_test(test_model_data));

    MicroPrintf("~~~ALL TESTS PASSED~~~\n");
    return kTfLiteOk;
}