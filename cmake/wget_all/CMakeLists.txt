cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(wget LANGUAGES NONE)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/../wget-custom")
# include_directories(${CMAKE_SOURCE_DIR}/../wget-custom)
# include_directories(/home/zizy/project/self/rubbish-bin/cmake/wget_custom)
include(dtu_wget)


# genrule(
#     name = "download_model",
#     outs = [
#         "data",
#     ],
#     cmd = "wget ftp://172.16.11.50/software/pretrained/pb/resnet50_v1.5.pb -P $(location data); wget ftp://172.16.11.50/software/pretrained/onnx/resnet50_v1.5.onnx -P $(location data)",
# )

dtu_wget(
    NAME  resnet50_v1.5_pb
    URI "ftp://172.16.11.50/software/pretrained/pb/resnet50_v1.5.pb"
    OUTPUT_DIR "${CMAKE_BINARY_DIR}/data/"
    # RENAME "1.pb"
    # PREPARE
)

dtu_wget(
    NAME resnet50_v1.5_onnx
    URI "ftp://172.16.11.50/software/pretrained/onnx/resnet50_v1.5.onnx"
    OUTPUT_DIR "${CMAKE_BINARY_DIR}/data/"
    # RENAME "1.pb"
    # PREPARE
)

add_custom_target(wet_all DEPENDS resnet50_v1.5_pb resnet50_v1.5_onnx)

