#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    // std::string onnx_model_file = 
    //     "tops/system_test/tops_inference_test/py_test/inputs/ \
    //     models/generate_file/network_7.onnx";
    const char* onnx_model_file = 
        "tops/system_test/tops_inference_test/py_test/inputs/"
        "models/generate_file/network_7.onnx";
    std::cout << onnx_model_file << std::endl;
    return 0;
}