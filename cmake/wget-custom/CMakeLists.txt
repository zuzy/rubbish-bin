cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(wget LANGUAGES NONE)

find_package(Wget)

function(dtu_wget)
    set(oneValueArgs NAME URI OUTPUT_DIR RENAME)
    set(options PREPARE)
    set(multiValueArgs )
    cmake_parse_arguments(DTU_WGET
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )
    
    if ("${DTU_WGET_NAME}" STREQUAL "")
    message(FATAL_ERROR "Missing dtu_wget param: NAME")
    endif()
    if ("${DTU_WGET_URI}" STREQUAL "")
    message(FATAL_ERROR "Missing dtu_wget param: URI")
    endif()
    message(STATUS "dtu_wget param: ${DTU_WGET_URI} ${DTU_WGET_OUTPUT_DIR}")
    
    if (DTU_WGET_RENAME)
        set (TARGET ${DTU_WGET_RENAME})
    else()
        get_filename_component(TARGET ${DTU_WGET_URI} NAME)
    endif()

    if ("${DTU_WGET_OUTPUT_DIR}" STREQUAL "")
        set (OUT_DIR ${CMAKE_BINARY_DIR})
    else()
        set (OUT_DIR ${DTU_WGET_OUTPUT_DIR})
    endif()
    if (DTU_WGET_PREPARE)
        if (NOT EXISTS ${OUT_DIR}/${TARGET})
            if (NOT EXISTS ${OUT_DIR})
                execute_process(COMMAND "mkdir" "-p" ${OUT_DIR})
            endif()
            execute_process(
                COMMAND ${WGET_EXECUTABLE} ${DTU_WGET_URI} "-O" ${OUT_DIR}/${TARGET}
                RESULT_VARIABLE _result
                ERROR_VARIABLE _stderr
            )
            if (_result)
                message(FATAL_ERROR "Download ${DUT_WGET_URI} failed ${_stderr}")
            endif()
        endif()
    else()
        if (NOT EXISTS ${OUT_DIR})
            add_custom_command(
                OUTPUT ${OUT_DIR}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${OUT_DIR}
            )
            list(APPEND ADDITIONAL_MAKE_CLEAN_FILES ${OUT_DIR})
        endif()

        add_custom_command(
            OUTPUT ${TARGET}
            COMMAND ${WGET_EXECUTABLE} ${DTU_WGET_URI} "-O" ${TARGET}
            WORKING_DIRECTORY ${OUT_DIR}
            DEPENDS ${OUT_DIR}
        )
    endif()

    list(APPEND ADDITIONAL_MAKE_CLEAN_FILES ${OUT_DIR}/${TARGET})
    add_custom_target(${DTU_WGET_NAME} DEPENDS ${TARGET})
endfunction(dtu_wget)

dtu_wget(
    NAME wget_pb
    URI "ftp://172.16.11.50/software/pretrained/pb/resnet50_v1.5.pbsssss"
    # URI "ftp://172.16.11.50/software/pretrained/pb/resnet50_v1.5.pb"
    OUTPUT_DIR "${CMAKE_BINARY_DIR}/data/dat/da"
    RENAME "1.pb"
    # PREPARE
)

