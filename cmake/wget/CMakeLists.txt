cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(wget LANGUAGES NONE)

find_package(Wget)

function(dtu_wget)
    set(oneValueArgs URI OUTPUT_DIR RENAME)
    set(options )
    set(multiValueArgs )

    cmake_parse_arguments(DTU_WGET
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    message(STATUS "dtu_wget param: ${DTU_WGET_URI} ${DTU_WGET_OUTPUT_DIR}")

    if (NOT DTU_WGET_URI)
        message(FATAL_ERROR "Missing dtu_wget param: URI")
    endif()
    
    if ("${DTU_WGET_OUTPUT_DIR}" STREQUAL "")
        set (OUT_DIR ${CMAKE_BINARY_DIR})
    else()
        set (OUT_DIR ${DTU_WGET_OUTPUT_DIR})
    endif()
    if (DTU_WGET_RENAME)
        set (TARGET ${OUT_DIR}/${DTU_WGET_RENAME})
    else()
        get_filename_component(_T ${DTU_WGET_URI} NAME)
        set (TARGET ${OUT_DIR}/${_T})
    endif()
    
    if (NOT EXISTS ${TARGET})
        if (NOT EXISTS ${OUT_DIR})
            execute_process(COMMAND "mkdir" "-p" ${OUT_DIR})
        endif()
        execute_process(
            COMMAND ${WGET_EXECUTABLE} ${DTU_WGET_URI} "-O" ${TARGET}
            RESULT_VARIABLE _result
            ERROR_VARIABLE _stderr
        )
        if (_result)
            message(FATAL_ERROR "Download ${DUT_WGET_URI} failed ${_stderr}")
        endif()
    endif()
endfunction(dtu_wget)

dtu_wget(
    # URI "ftp://172.16.11.50/software/pretrained/pb/resnet50_v1.5.pbsssss"
    URI "ftp://172.16.11.50/software/pretrained/pb/resnet50_v1.5.pb"
    # OUTPUT_DIR "${CMAKE_BINARY_DIR}/data"
    RENAME "1.pb"
)

# execute_process(
#     # COMMAND "mkdir" "-p" "$(CMAKE_BINARY_DIR)/data"
#     # COMMAND "wget" "ftp://172.16.11.50/software/pretrained/pb/resnet50_v1.5.pb" "-P" ${CMAKE_BINARY_DIR}/data
#     COMMAND "wget" "ftp://172.16.11.50/software/pretrained/pb/resnet50_v1.5.pbsssss" "-P" ${CMAKE_BINARY_DIR}/data
#     # COMMAND ${PYTHON_EXECUTABLE} "-c" "import ${_module_name}; print($ {_module_name}.__version__)"
#     RESULT_VARIABLE _result
#     OUTPUT_VARIABLE _stdout
#     ERROR_VARIABLE _stderr
#     OUTPUT_STRIP_TRAILING_WHITESPACE
#     ERROR_STRIP_TRAILING_WHITESPACE
# )

# # message(STATUS ${_stdout})
# # message(STATUS ${_stderr})
# message(STATUS ${_result})

# if(_result)
#     message(FATAL_ERROR "Download falied ${_stderror}")
# else()
#     message(STATUS "download complete")
# endif()
