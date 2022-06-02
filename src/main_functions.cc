/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "hx_drv_tflm.h"

#include "main_functions.h"
#include "detection_responder.h"
#include "image_provider.h"
#include "model_settings.h"
#include "model.h"
#include "test_samples.h"
#include "accelerometer_handler.h"
#include "examples/magic_wand/magic_wand_model_data.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
union float_c{
  float m_float;
  uint8_t m_bytes[sizeof(float)];
};
union int_c{
  float m_int;
  uint8_t m_bytes[sizeof(int)];
};
typedef struct
{
	uint8_t symbol;
	uint32_t int_part;
	uint32_t frac_part;
} accel_type;
union float_c myfloat;
union int_c myint;
// In order to use optimized tensorflow lite kernels, a signed int8_t quantized
// model is preferred over the legacy unsigned model format. This means that
// throughout this project, input images must be converted from unisgned to
// signed format. The easiest and quickest way to convert from unsigned to
// signed 8-bit integers is to subtract 128 from the unsigned value to get a
// signed value.

// An area of memory to use for input, output, and intermediate arrays.
constexpr int kTensorArenaSize = 50 * 1024;
alignas(16) uint8_t tensor_arena[kTensorArenaSize] = {0};
}  // namespace


// The name of this function is important for Arduino compatibility.
void setup() {
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(model_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }
  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  // tflite::AllOpsResolver resolver;
  // NOLINTNEXTLINE(runtime-global-variables)
  
  static tflite::MicroMutableOpResolver<20> micro_op_resolver;
  micro_op_resolver.AddAdd();
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddConcatenation();
  micro_op_resolver.AddFill();
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddLogistic();
  micro_op_resolver.AddMul();
  micro_op_resolver.AddReshape();
  micro_op_resolver.AddMaxPool2D();
  micro_op_resolver.AddPack();
  micro_op_resolver.AddRelu();
  micro_op_resolver.AddSub();
  micro_op_resolver.AddShape();
  micro_op_resolver.AddSplit();
  micro_op_resolver.AddSplitV();
  micro_op_resolver.AddSoftmax();
  micro_op_resolver.AddStridedSlice();
  micro_op_resolver.AddTranspose();
  micro_op_resolver.AddTanh();
  micro_op_resolver.AddUnpack();
  
  // Build an interpreter to run the model with.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Get information about the memory area to use for the model's input.
  input = interpreter->input(0);
  output = interpreter->output(0);

  // Obtain quantization parameters for result dequantization

}

// The name of this function is important for Arduino compatibility.

void loop_har(float x_data[3][300],int *signal_pass) {

  int32_t zero_point = input->params.zero_point;
  float scale = input->params.scale;
  float x_data_temp[3][50];
  hx_drv_uart_initial(UART_BR_115200);

  for (int i = 0 ;i< 300; i = i+6 )    //讀取三軸用的迴圈
  {
    x_data_temp[0][(i/6)] = (x_data[0][i] ) ;
    x_data_temp[1][(i/6)] = (x_data[1][i] ) ;
    x_data_temp[2][(i/6)] = (x_data[2][i] ) ;
  }

   // read accelerometer (accx, accy, accz)into model
  for (int i = 0; i < 50; i++) {  //  (int i = 0; i < 50; i=i++)(三軸輸入)        (int i = 0; i < 150; i=i+1)(temple.cc輸入)
    input->data.f[3*i] = x_data[0][i];// x
    input->data.f[3*i+1] =x_data[1][i];// y
    input->data.f[3*i+2] = x_data[2][i];// z
    //TF_LITE_REPORT_ERROR(error_reporter, "%f | %f | %f \n ", test_samples[j].image[i], test_samples[j].image[i+1], test_samples[j].image[i+2]);
    //input->data.f[i] = test_samples[j].image[i];
  }
  //TF_LITE_REPORT_ERROR(error_reporter, "Test sample[%d] Start Invoking\n", j);
  // Run the model on this input and make sure it succeeds.   !!!!!!!!!!!!!!!!
  if (kTfLiteOk != interpreter->Invoke()) {
    TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed.");
  }

  // Get max result from output array and calculate confidence
  float* results_ptr = output->data.f;
  int result = std::distance(results_ptr, std::max_element(results_ptr, results_ptr + 6));
  *signal_pass = result;   

  TF_LITE_REPORT_ERROR(error_reporter, 
  "Predicted %s \n", 
  kCategoryLabels[result]); 

}

// 用來測試temple.cc 測試資料， 檢查模型是否在晶片當中正常運作
// The name of this function is important for Arduino compatibility.
/*
void loop_har(float x_data[3][300],int *signal_pass) {
  int32_t test_cnt = 0;
  int32_t correct_cnt = 0;
  int32_t zero_point = input->params.zero_point;
  float scale = input->params.scale;

  hx_drv_uart_initial(UART_BR_115200);


  for (int j = 0; j < kNumSamples; j++){

    // read temple.cc and put into model
    for (int i = 0; i < kImageSize; i++) {
      input->data.f[i] = test_samples[j].image[i];
    }
    //TF_LITE_REPORT_ERROR(error_reporter, "Test sample[%d] Start Invoking\n", j);
    // Run the model on this input and make sure it succeeds.   !!!!!!!!!!!!!!!!
    if (kTfLiteOk != interpreter->Invoke()) {
      TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed.");
    }

    TF_LITE_REPORT_ERROR(error_reporter, "Test sample[%d] Start Finding Max Value\n", j);
    // Get max result from output array and calculate confidence
    float* results_ptr = output->data.f;

    int result = std::distance(results_ptr, std::max_element(results_ptr, results_ptr + 6));

    float confidence = ((results_ptr[result] - zero_point)*scale + 1) / 2;

    const char *status = result == test_samples[j].label ? "SUCCESS" : "FAIL";

    *signal_pass = result;
    
    if(result == test_samples[j].label)
      correct_cnt ++;
    test_cnt ++;

    TF_LITE_REPORT_ERROR(error_reporter, 
      "Test sample \"%s\":\n"
      "Predicted %s (%d%%) - %s\n",
      kCategoryLabels[test_samples[j].label], 
      kCategoryLabels[result], (int)(confidence * 100), status);    
  }
    TF_LITE_REPORT_ERROR(error_reporter, "Correct Rate = %d / %d\n\n", correct_cnt, test_cnt);
    while(1);
}
*/



