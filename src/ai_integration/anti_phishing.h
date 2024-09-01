#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "onnxruntime_c_api.h"
#include <onnxruntime_run_options_config_keys.h>

OrtStatus* prepare_input(OrtAllocator* allocator, const char** texts, size_t texts_count, OrtValue** input_tensor);