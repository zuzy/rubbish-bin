cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(tar LANGUAGES NONE)


#
# Function to make a tar package
#
# dtu_pkg_tar PARAM:
# dtu_pkg_tar(
#     NAME                  - The package name, will create {NAME}.tar
#     SRCS                  - Files to package.
#     [PACKAGE_DIR]         - Prefix dir.
#     [MODE]                - Mode of files to package
#     [APPEND]              - If APPEND is true, files will append to the old
#

function(dtu_pkg_tar)
    set(oneValueArgs NAME PACKAGE_DIR MODE)
    set(options APPEND)
    set(multiValueArgs REGEX_COUPLES SRCS DIRS)

    cmake_parse_arguments(DTU_PKG_TAR
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    # Check if tar works
    execute_process(
        COMMAND "which" "tar" 
        OUTPUT_VARIABLE _TAR
        RESULT_VARIABLE _result
        ERROR_VARIABLE _stderr
    )
    if (_result)
        message(FATAL_ERROR "Not tar progress here!")
    endif()
    string(STRIP "${_TAR}" TAR_EXECUTABLE)

    if (DTU_PKG_TAR_APPEND)
    set(_FLAG "rf")
    else()
    set(_FLAG "cvf")
    endif()

    if ("${DTU_PKG_TAR_NAME}" STREQUAL "")
        message(FATAL_ERROR "Missing dut_pkg_tar param: NAME")
    endif()

    # set (_TARGET ${DTU_PKG_TAR_NAME}.tar)
    set (_TARGET ${CMAKE_CURRENT_BINARY_DIR}/${DTU_PKG_TAR_NAME}.tar)
    message(STATUS ${_TARGET})

    if (NOT "${DTU_PKG_TAR_MODE}" STREQUAL "")
        set (_MODE --mode=${DTU_PKG_TAR_MODE})
    endif()
    
    if (DTU_PKG_TAR_SRCS_GLOB)
        file(GLOB_RECURSE _SRCS ${DTU_PKG_TAR_SRCS_GLOB})
    endif()
    list(APPEND _SRCS ${DTU_PKG_TAR_SRCS})

    if (DTU_PKG_TAR_EXCLUDES_GLOB)
        file(GLOB_RECURSE _EXCLUDES  ${DTU_PKG_TAR_EXCLUDES_GLOB})
    endif()
    list(APPEND _EXCLUDES ${DTU_PKG_TAR_EXCLUDES})

    foreach (src_exclude IN LISTS _EXCLUDES)
        list(REMOVE_ITEM _SRCS ${src_exclude})
    endforeach()
    
    string (REPLACE ";" " " _SRC_STR "${_SRCS}")
    

    if (NOT "${DTU_PKG_TAR_PACKAGE_DIR}" STREQUAL "")
        message(STATUS "${DTU_PKG_TAR_PACKAGE_DIR}")
        set (_REPLACE "s,^,${DTU_PKG_TAR_PACKAGE_DIR},;")
    endif()
    foreach (_couple IN LISTS DTU_PKG_TAR_REGEX_COUPLES)
        set (_REPLACE "${_REPLACE};${_couple};")
    endforeach()
    if (NOT "${_REPLACE}" STREQUAL "")
        set (_REPLACE --transform="${_REPLACE}")
    endif()
    message(STATUS "${_REPLACE}")

    message(STATUS "${TAR_EXECUTABLE} cvf ${_TARGET} ${_SRC_STR} ${_REPLACE} ${_MODE}")

    set(GENDIR ${CMAKE_CURRENT_BINARY_DIR}/ztest_dir/target)

    add_custom_command(
        OUTPUT ${GENDIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${GENDIR}
    )

    add_custom_command(
        OUTPUT ${_TARGET}      
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DTU_PKG_TAR_DIRS} ${GENDIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_SRCS} ${GENDIR}
        COMMAND pwd > path
        COMMAND ${TAR_EXECUTABLE} ${_FLAG} ${_TARGET} * --overwrite --force-local "${_REPLACE}" ${_MODE}
        # COMMAND ${CMAKE_COMMAND} -E remove
        WORKING_DIRECTORY ${GENDIR}
        DEPENDS ${GENDIR}
        # COMMAND cp -r ${_SRCS} ${GENDIR}
        
        # COMMAND ${TAR_EXECUTABLE} "${_FLAG}" ${_TARGET} ${_SRCS} "--overwrite" "--force-local" "${_REPLACE}" ${_MODE}
    )
    add_custom_target(${DTU_PKG_TAR_NAME} ALL DEPENDS ${_TARGET})
    # add_custom_target(${_TARGET} ALL DEPENDS ${PYPI_ARG_OUTPUTPATH}/${whl_file_name} ${PYPI_ARG_DEPENDS})
    # execute_process(
    #     COMMAND ${TAR_EXECUTABLE} "${_FLAG}" ${_TARGET} ${_SRCS} "--overwrite" "--force-local" "${_REPLACE}" ${_MODE}
    #     OUTPUT_VARIABLE _stdout
    #     RESULT_VARIABLE _result
    #     ERROR_VARIABLE _stderr
    # )
endfunction(dtu_pkg_tar)


dtu_pkg_tar(
    NAME test
    # SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../**/CMakeLists.txt"
    SRCS "${CMAKE_SOURCE_DIR}/CMakeLists.txt" "${CMAKE_SOURCE_DIR}/"
    # DIRS ""
    DIRS "${CMAKE_SOURCE_DIR}/1"
    # SRCS_GLOB "./**"
    # EXCLUDE_GLOB "./build/CMakeLists/**"
    # REMAP_COUPLES "tar,TAR" "wget,Wget" "cmake,CMAKE"
    REGEX_COUPLES "s,build,B/BUILD," "s,path,CMAKE,g"
    PACKAGE_DIR "usr/bin/"
    MODE "0755"
    # APPEND
)

