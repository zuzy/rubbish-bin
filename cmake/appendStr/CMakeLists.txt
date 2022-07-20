cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(appendStr LANGUAGES NONE)

function(appendStr)
    set(oneValueArgs PREFIX MODE)
    set(options )
    set(multiValueArgs COUPLES)
    cmake_parse_arguments(DTU_APPEND
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if (NOT "${DTU_APPEND_PREFIX}" STREQUAL "")
    message(STATUS "${DTU_APPEND_PREFIX}")
    set (REPLACE "s,^,${DTU_APPEND_PREFIX},;")
    else()
    message(STATUS "none")
    endif()

    if (NOT "${DTU_APPEND_MODE}" STREQUAL "")
    set (_MODE --mode="${DTU_APPEND_MODE}")
    endif()
    message(STATUS "MODE: ${_MODE}")


    foreach (_couple IN LISTS DTU_APPEND_COUPLES)
        set (REPLACE "${REPLACE}s,${_couple},;")
    endforeach()

    message(STATUS "${REPLACE}")    
    # message(STATUS "SRC:${_SRCS} || EXCLUDE:${_EXCLUDES}")
endfunction(appendStr)

appendStr(
    # COUPLES "home,/hh/home" "ss,sd/ddd/s"
    PREFIX "SSS"
    MODE "0644"
)
message(STATUS "${REPLACE}")