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
#                           - pkg.
#     [HDRS]                - List of header files to compile
#     [SRCS_GLOB]           - The globbing-expressions to glob source files, the results will append to SRCS list
#     [HDRS_GLOB]           - The globbing-expressions to glob header files, the results will append to HDRS list
#     [SRCS_EXCLUDE]        - The source files that need to be exclude from SRCS list.
#     [DEPS]                - The dependent libaries or other targets (the dependent lib and targets needs to be defined
#                             first due to the limitation of cmake gramma. If the dependents can't be defined first,
#                             use LINK_DEPS and UTIL_DEPS instead)
#     [IMPLEMENTATION_DEPS] - The list of other libraries that the library target depends on. Unlike with DEPS and LINK_DEPS,
#                             the headers and include paths of these libraries (and all their transitive deps) are only
#                             used for compilation of this library, and not libraries that depend on it.
#     [LINK_DEPS]           - The dependent libaries to be linked.
#     [UTIL_DEPS]           - The dependent utiliy targets that need to build first.
#     [INCLUDES]            - List of include dirs to be added to the compile line. These flags are added for this target
#                             and every targets that depends on it.
#     [DEFINES]             - List of defines to add to the compile line. Each string added to the compile command line to
#                             this target, as well as to every target that depends on it.
#     [LOCAL_DEFINES]       - List of defines to add to the compile line. Each string only added to the compile command line
#                             for this target, but not to its dependents.
#     [COPTS]               - List of options to the C++ compilation command.The flags take effect only for compiling this
#                             target, not its dependents.
#     [LINKOPTS]            - List of flags to add to the C++ linker command.
# )
# GLOBAL Variables:
#     DTU_COMMON_INCLUDE_DIRS       - common include directories for dtu cc libraries
#     DTU_CC_LIB_COMMON_DEPS        - common dependencies for dtu cc libraries (only add the dependenices to make sure the build sequence,
#                                     will not add dependencies to the link list)
#     DTU_CC_LIB_COMMON_LINK_LIBS   - common dependent libraries to link to dtu cc libraries
#     DTU_COMMON_INCLUDE_TENSORFLOW115_DIRS -common include directories for  libraries dependent on tensorflow 1.15.0 #CAI_E2:Added
#     DTU_COMMON_INCLUDE_TENSORFLOW114_DIRS -common include directories for  libraries dependent on tensorflow 1.14.0 #CAI_E2:Added
#     DTU_COMMON_INCLUDE_TENSORFLOW210_DIRS -common include diretories for  libraries dependent on tensorflow 2.10 #CAI_E2:Added
#

function(dtu_pkg_tar)
    set(oneValueArgs NAME PACKAGE_DIR MODE)
    set(options APPEND)
    # set(multiValueArgs REGEX_COUPLES SRCS SRCS_GLOB EXCLUDES EXCLUDES_GLOB)
    set(multiValueArgs REGEX_COUPLES SRCS)

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

    
    # message(STATUS ${TAR_EXECUTABLE})
    # execute_process(
    #     COMMAND "${TAR_EXECUTABLE}" "--version"
    #     OUTPUT_VARIABLE _stdout
    #     RESULT_VARIABLE _result
    #     ERROR_VARIABLE _stderr
    # )
    # message(STATUS ${_stdout})

    if ("${DTU_PKG_TAR_NAME}" STREQUAL "")
        message(FATAL_ERROR "Missing dut_pkg_tar param: NAME")
    endif()

    set (_TARGET ${DTU_PKG_TAR_NAME}.tar)
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
        set (_REPLACE --transform=${_REPLACE})
    endif()
    message(STATUS "${_REPLACE}")

    message(STATUS "${TAR_EXECUTABLE} cvf ${_TARGET} ${_SRC_STR} ${_REPLACE} ${_MODE}")

    execute_process(
        # COMMAND "${TAR_EXECUTABLE} cvf ${_TARGET} ${_SRC_STR} ${_REPLACE} ${MODE}"
        # COMMAND ${TAR_EXECUTABLE} "cvf" ${_TARGET} ${_SRC_STR} ${_REPLACE} ${MODE}
        # COMMAND ${TAR_EXECUTABLE} "-cvf" ${_TARGET} ${_SRCS} ${_REPLACE} ${MODE}
        # COMMAND ${TAR_EXECUTABLE} "-cvf" ${_TARGET} ${_SRCS} "--overwrite" "--force-local" "${_REPLACE}" ${_MODE}
        COMMAND ${TAR_EXECUTABLE} "${_FLAG}" ${_TARGET} ${_SRCS} "--overwrite" "--force-local" "${_REPLACE}" ${_MODE}
        # COMMAND ${TAR_EXECUTABLE} "${_FLAG}" ${_TARGET} ${_SRCS} "--force-local" "${_REPLACE}" ${_MODE}
        OUTPUT_VARIABLE _stdout
        RESULT_VARIABLE _result
        ERROR_VARIABLE _stderr
    )
    # message (STATUS "${_stdout}")
    # message (STATUS "${_stderr}")

endfunction(dtu_pkg_tar)


dtu_pkg_tar(
    NAME test
    # SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../**/CMakeLists.txt"
    SRCS "CMakeLists.txt" "build"
    # SRCS_GLOB "./**"
    # EXCLUDE_GLOB "./build/CMakeLists/**"
    # REMAP_COUPLES "tar,TAR" "wget,Wget" "cmake,CMAKE"
    REGEX_COUPLES "s,build,B/BUILD," "s,cmake,CMAKE,g"
    PACKAGE_DIR "usr/bin/"
    MODE "0755"
    # APPEND
)

