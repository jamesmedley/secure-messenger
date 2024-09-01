#include "anti_phishing.h"

const OrtApi* OrtApiHandle = NULL;

// Define a function to preprocess input data
OrtStatus* prepare_input(OrtAllocator* allocator, const char** texts, size_t texts_count, OrtValue** input_tensor) {
    size_t input_dim[] = {texts_count, 1};
    OrtMemoryInfo* memory_info;
    OrtStatus* status = OrtApiHandle->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &memory_info);
    if (status != NULL) return status;

    // Create a tensor of shape [texts_count, 1]
    char** input_data = malloc(texts_count * sizeof(char*));
    for (size_t i = 0; i < texts_count; i++) {
        input_data[i] = strdup(texts[i]);
    }

    status = OrtApiHandle->CreateTensorWithDataAsOrtValue(memory_info, input_data, texts_count * sizeof(char*), input_dim, 2, ONNX_TENSOR_ELEMENT_DATA_TYPE_STRING, input_tensor);

    OrtApiHandle->ReleaseMemoryInfo(memory_info);
    return status;
}

int main() {
    // Initialize ONNX Runtime
    OrtEnv* env;
    OrtApiHandle->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "test", &env);

    // Create a session
    OrtSession* session;
    OrtSessionOptions* session_options;
    OrtApiHandle->CreateSessionOptions(&session_options);
    OrtApiHandle->SetIntraOpNumThreads(session_options, 1);

    const wchar_t* model_path = "python/model/spam_classifier.onnx";
    OrtApiHandle->CreateSession(env, model_path, session_options, &session);

    // Prepare the input
    const char* texts[] = {"XXXMobileMovieClub: To use your credit, click the WAP link in the next txt message or click here>> http://wap.xxxmobilemovieclub.com?n=QJKGIGHJJGCBL"};
    size_t texts_count = sizeof(texts) / sizeof(texts[0]);
    OrtAllocator* allocator;
    OrtApiHandle->GetAllocatorWithDefaultOptions(&allocator);
    OrtValue* input_tensor = NULL;
    OrtStatus* status = prepare_input(allocator, texts, texts_count, &input_tensor);
    if (status != NULL) {
        const char* msg = OrtApiHandle->GetErrorMessage(status);
        fprintf(stderr, "Error preparing input: %s\n", msg);
        OrtApiHandle->ReleaseStatus(status);
        return 1;
    }

    // Run inference
    const char* input_names[] = {"input"};
    const char* output_names[] = {"label", "probabilities"};
    OrtValue* output_tensors[2];
    status = OrtApiHandle->Run(session, NULL, input_names, (const OrtValue* const*)&input_tensor, 1, output_names, 2, output_tensors);
    if (status != NULL) {
        const char* msg = OrtApiHandle->GetErrorMessage(status);
        fprintf(stderr, "Error running inference: %s\n", msg);
        OrtApiHandle->ReleaseStatus(status);
        return 1;
    }

    // Process the outputs
    OrtTensorTypeAndShapeInfo* label_info;
    OrtApiHandle->GetTensorTypeAndShape(output_tensors[0], &label_info);
    size_t label_dims;
    OrtApiHandle->GetDimensionsCount(label_info, &label_dims);
    size_t* label_shape = malloc(label_dims * sizeof(size_t));
    OrtApiHandle->GetDimensions(label_info, label_shape, label_dims);

    int64_t* labels;
    OrtApiHandle->GetTensorMutableData(output_tensors[0], (void**)&labels);
    for (size_t i = 0; i < label_shape[0]; i++) {
        printf("Label: %lld\n", labels[i]);
    }

    OrtTensorTypeAndShapeInfo* prob_info;
    OrtApiHandle->GetTensorTypeAndShape(output_tensors[1], &prob_info);
    size_t prob_dims;
    OrtApiHandle->GetDimensionsCount(prob_info, &prob_dims);
    size_t* prob_shape = malloc(prob_dims * sizeof(size_t));
    OrtApiHandle->GetDimensions(prob_info, prob_shape, prob_dims);

    float* probabilities;
    OrtApiHandle->GetTensorMutableData(output_tensors[1], (void**)&probabilities);
    for (size_t i = 0; i < prob_shape[0]; i++) {
        printf("Probabilities: ");
        for (size_t j = 0; j < prob_shape[1]; j++) {
            printf("%f ", probabilities[i * prob_shape[1] + j]);
        }
        printf("\n");
    }

    // Cleanup
    OrtApiHandle->ReleaseValue(input_tensor);
    OrtApiHandle->ReleaseValue(output_tensors[0]);
    OrtApiHandle->ReleaseValue(output_tensors[1]);
    OrtApiHandle->ReleaseTensorTypeAndShapeInfo(label_info);
    OrtApiHandle->ReleaseTensorTypeAndShapeInfo(prob_info);
    OrtApiHandle->ReleaseSession(session);
    OrtApiHandle->ReleaseSessionOptions(session_options);
    OrtApiHandle->ReleaseEnv(env);

    return 0;
}