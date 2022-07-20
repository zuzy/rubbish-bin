
#
# Function to add a dtu cc library
#
# dtu_cc_library PARAM:
# dtu_cc_library(
#     NAME                  - The library name
#     [TENSORFLOW114]       - If set,tensorflow version 1.14.0 headers are
#                              used by the library #CAI_E2:Added
#     [TENSORFLOW115]       - If set,tensorflow version 1.15.0 headers are
#                              used by the library #CAI_E2:Added
#     [HIDE_SYMBOLS]        - If set, hide symbols from export(NOT implemented)
#     [SRCS]                - List of source files to compile
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
function(dtu_cc_library)
    set(oneValueArgs NAME)
    # set(multiValueArgs HDRS SRCS COPTS DEFINES LOCAL_DEFINES LINKOPTS DEPS LINK_DEPS UTIL_DEPS IMPLEMENTATION_DEPS INCLUDES SRCS_GLOB HDRS_GLOB SRCS_EXCLUDE)
    set(multiValueArgs SRCS_GLOB HDRS_GLOB)

    cmake_parse_arguments(DTU_CC_LIB
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )
    if (NOT DTU_CC_LIB_NAME)
        message(FATAL_ERROR "Missing dtu_cc_library param: NAME")
    endif()

    if (DTU_CC_LIB_SRCS_GLOB)
        file(GLOB DTU_CC_GLOB_SRCS CONFIGURE_DEPENDS ${DTU_CC_LIB_SRCS_GLOB})
        if (CONFIG_DEBUG)
            foreach(lib_src IN LISTS DTU_CC_GLOB_SRCS)
                message("dtu_cc_library ${DTU_CC_LIB_NAME} glob src: " ${lib_src})
            endforeach ()
        endif ()
        list(APPEND DTU_CC_LIB_SRCS ${DTU_CC_GLOB_SRCS})
        foreach(lib_src_exclude IN LISTS DTU_CC_LIB_SRCS_EXCLUDE)
            if (CONFIG_DEBUG)
                message("Excluding ${lib_src_exclude}")
            endif ()
            get_filename_component(src_exclude ${lib_src_exclude} ABSOLUTE)
            list(REMOVE_ITEM DTU_CC_LIB_SRCS ${src_exclude})
        endforeach()
    endif()

    if (DTU_CC_LIB_HDRS_GLOB)
        file(GLOB DTU_CC_GLOB_HDRS CONFIGURE_DEPENDS ${DTU_CC_LIB_HDRS_GLOB})
        if (CONFIG_DEBUG)
            foreach(lib_inc IN LISTS DTU_CC_GLOB_HDRS)
                message("dtu_cc_library ${DTU_CC_LIB_NAME} glob hdrs: " ${lib_inc})
            endforeach ()
        endif ()
        list(APPEND DTU_CC_LIB_HDRS ${DTU_CC_GLOB_HDRS})
    endif()

    list(APPEND DTU_CC_LIB_LINK_DEPS ${DTU_CC_LIB_DEPS})
    foreach(deps_lib_name IN LISTS DTU_CC_LIB_DEPS)
        get_target_property(target_type ${deps_lib_name} TYPE)
        if ("${target_type}" STREQUAL "UTILITY")
            list(APPEND DTU_CC_LIB_UTIL_DEPS ${deps_lib_name})
            list(REMOVE_ITEM DTU_CC_LIB_LINK_DEPS ${deps_lib_name})
        endif ()
    endforeach()

    set(_NAME "${DTU_CC_LIB_NAME}")
    # Check if this is a header-only library
    set(DTU_CC_SRCS "${DTU_CC_LIB_SRCS}")
    foreach(src_file IN LISTS DTU_CC_SRCS)
      if(${src_file} MATCHES ".*\\.(h|hpp)$")
        list(REMOVE_ITEM DTU_CC_SRCS "${src_file}")
      endif()
    endforeach()
    if("${DTU_CC_SRCS}" STREQUAL "")
        set(DTU_CC_LIB_IS_INTERFACE 1)
    else()
        set(DTU_CC_LIB_IS_INTERFACE 0)
    endif()

    if(NOT DTU_CC_LIB_IS_INTERFACE)
        add_library(${_NAME} OBJECT "")
        if (CONFIG_DEBUG)
            message("add dtu_cc_library: " ${_NAME})
        endif ()
        target_sources(${_NAME} PRIVATE ${DTU_CC_LIB_SRCS} ${DTU_CC_LIB_HDRS})
#CAI_E2:Added conditions for using headers of different versions of tensorflow
        if(DTU_CC_LIB_TENSORFLOW114)
            target_include_directories(${_NAME}
            PUBLIC
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_DIRS}>"
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_TENSORFLOW114_DIRS}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                "$<BUILD_INTERFACE:${DTU_CC_LIB_INCLUDES}>"
            )
        elseif(DTU_CC_LIB_TENSORFLOW115)
            target_include_directories(${_NAME}
            PUBLIC
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_DIRS}>"
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_TENSORFLOW115_DIRS}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                "$<BUILD_INTERFACE:${DTU_CC_LIB_INCLUDES}>"
            )
        else()
            target_include_directories(${_NAME}
            PUBLIC
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_DIRS}>"
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_TENSORFLOW210_DIRS}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                "$<BUILD_INTERFACE:${DTU_CC_LIB_INCLUDES}>"
            )
        endif ()
        target_compile_options(${_NAME}
            PRIVATE ${DTU_CC_LIB_COPTS}
        )
        target_link_libraries(${_NAME}
            PUBLIC
                ${DTU_CC_LIB_LINK_DEPS}
                ${DTU_CC_LIB_COMMON_LINK_LIBS}
            PRIVATE
                ${DTU_CC_LIB_IMPLEMENTATION_DEPS}
        )
        target_link_options(${_NAME}
            PRIVATE
                ${DTU_CC_LIB_LINKOPTS}
                ${DTU_DEFAULT_LINKOPTS}
        )
        target_compile_definitions(${_NAME} PUBLIC ${DTU_CC_LIB_DEFINES}
                                            PRIVATE ${DTU_CC_LIB_LOCAL_DEFINES})

        # INTERFACE libraries can't have the CXX_STANDARD property set
        set_property(TARGET ${_NAME} PROPERTY CXX_STANDARD ${DTU_CXX_STANDARD})
        set_property(TARGET ${_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

    else()
        # Generating header-only library
        add_library(${_NAME} INTERFACE)
        target_include_directories(${_NAME}
            INTERFACE
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_DIRS}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                "$<BUILD_INTERFACE:${DTU_CC_LIB_INCLUDES}>"
        )
        target_link_libraries(${_NAME}
            INTERFACE
                ${DTU_CC_LIB_LINK_DEPS}
        )
        target_compile_definitions(${_NAME} INTERFACE ${DTU_CC_LIB_DEFINES})
    endif()

    if (NOT "${DTU_CC_LIB_UTIL_DEPS}" STREQUAL "")
        add_dependencies(${_NAME} ${DTU_CC_LIB_UTIL_DEPS})
        if (CONFIG_DEBUG)
            message("${_NAME} add utility depencies: " ${DTU_CC_LIB_UTIL_DEPS})
        endif ()
    endif()

    if (NOT "${DTU_CC_LIB_COMMON_DEPS}" STREQUAL "")
        add_dependencies(${_NAME} ${DTU_CC_LIB_COMMON_DEPS})
        if (CONFIG_DEBUG)
            message("${_NAME} add common depencies: " ${DTU_CC_LIB_COMMON_DEPS})
        endif ()
    endif()

    add_library(dtu::${DTU_CC_LIB_NAME} ALIAS ${_NAME})
endfunction() # dtu_cc_library


function(glob_fun)

    set(oneValueArgs NAME)
    set(multiValueArgs HDRS SRCS COPTS DEFINES LOCAL_DEFINES LINKOPTS DEPS LINK_DEPS UTIL_DEPS IMPLEMENTATION_DEPS INCLUDES SRCS_GLOB HDRS_GLOB SRCS_EXCLUDE)

    cmake_parse_arguments(DTU_CC_LIB
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )
    if (NOT DTU_CC_LIB_NAME)
        message(FATAL_ERROR "Missing dtu_cc_library param: NAME")
    endif()

    if (DTU_CC_LIB_SRCS_GLOB)
        file(GLOB DTU_CC_GLOB_SRCS CONFIGURE_DEPENDS ${DTU_CC_LIB_SRCS_GLOB})
        if (CONFIG_DEBUG)
            foreach(lib_src IN LISTS DTU_CC_GLOB_SRCS)
                message("dtu_cc_library ${DTU_CC_LIB_NAME} glob src: " ${lib_src})
            endforeach ()
        endif ()
        list(APPEND DTU_CC_LIB_SRCS ${DTU_CC_GLOB_SRCS})
        foreach(lib_src_exclude IN LISTS DTU_CC_LIB_SRCS_EXCLUDE)
            if (CONFIG_DEBUG)
                message("Excluding ${lib_src_exclude}")
            endif ()
            get_filename_component(src_exclude ${lib_src_exclude} ABSOLUTE)
            list(REMOVE_ITEM DTU_CC_LIB_SRCS ${src_exclude})
        endforeach()
    endif()

    if (DTU_CC_LIB_HDRS_GLOB)
        file(GLOB DTU_CC_GLOB_HDRS CONFIGURE_DEPENDS ${DTU_CC_LIB_HDRS_GLOB})
        if (CONFIG_DEBUG)
            foreach(lib_inc IN LISTS DTU_CC_GLOB_HDRS)
                message("dtu_cc_library ${DTU_CC_LIB_NAME} glob hdrs: " ${lib_inc})
            endforeach ()
        endif ()
        list(APPEND DTU_CC_LIB_HDRS ${DTU_CC_GLOB_HDRS})
    endif()

    list(APPEND DTU_CC_LIB_LINK_DEPS ${DTU_CC_LIB_DEPS})
    foreach(deps_lib_name IN LISTS DTU_CC_LIB_DEPS)
        get_target_property(target_type ${deps_lib_name} TYPE)
        if ("${target_type}" STREQUAL "UTILITY")
            list(APPEND DTU_CC_LIB_UTIL_DEPS ${deps_lib_name})
            list(REMOVE_ITEM DTU_CC_LIB_LINK_DEPS ${deps_lib_name})
        endif ()
    endforeach()

    set(_NAME "${DTU_CC_LIB_NAME}")
    # Check if this is a header-only library
    set(DTU_CC_SRCS "${DTU_CC_LIB_SRCS}")
    foreach(src_file IN LISTS DTU_CC_SRCS)
      if(${src_file} MATCHES ".*\\.(h|hpp)$")
        list(REMOVE_ITEM DTU_CC_SRCS "${src_file}")
      endif()
    endforeach()
    if("${DTU_CC_SRCS}" STREQUAL "")
        set(DTU_CC_LIB_IS_INTERFACE 1)
    else()
        set(DTU_CC_LIB_IS_INTERFACE 0)
    endif()

    if(NOT DTU_CC_LIB_IS_INTERFACE)
        add_library(${_NAME} OBJECT "")
        if (CONFIG_DEBUG)
            message("add dtu_cc_library: " ${_NAME})
        endif ()
        target_sources(${_NAME} PRIVATE ${DTU_CC_LIB_SRCS} ${DTU_CC_LIB_HDRS})
#CAI_E2:Added conditions for using headers of different versions of tensorflow
        if(DTU_CC_LIB_TENSORFLOW114)
            target_include_directories(${_NAME}
            PUBLIC
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_DIRS}>"
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_TENSORFLOW114_DIRS}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                "$<BUILD_INTERFACE:${DTU_CC_LIB_INCLUDES}>"
            )
        elseif(DTU_CC_LIB_TENSORFLOW115)
            target_include_directories(${_NAME}
            PUBLIC
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_DIRS}>"
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_TENSORFLOW115_DIRS}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                "$<BUILD_INTERFACE:${DTU_CC_LIB_INCLUDES}>"
            )
        else()
            target_include_directories(${_NAME}
            PUBLIC
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_DIRS}>"
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_TENSORFLOW210_DIRS}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                "$<BUILD_INTERFACE:${DTU_CC_LIB_INCLUDES}>"
            )
        endif ()
        target_compile_options(${_NAME}
            PRIVATE ${DTU_CC_LIB_COPTS}
        )
        target_link_libraries(${_NAME}
            PUBLIC
                ${DTU_CC_LIB_LINK_DEPS}
                ${DTU_CC_LIB_COMMON_LINK_LIBS}
            PRIVATE
                ${DTU_CC_LIB_IMPLEMENTATION_DEPS}
        )
        target_link_options(${_NAME}
            PRIVATE
                ${DTU_CC_LIB_LINKOPTS}
                ${DTU_DEFAULT_LINKOPTS}
        )
        target_compile_definitions(${_NAME} PUBLIC ${DTU_CC_LIB_DEFINES}
                                            PRIVATE ${DTU_CC_LIB_LOCAL_DEFINES})

        # INTERFACE libraries can't have the CXX_STANDARD property set
        set_property(TARGET ${_NAME} PROPERTY CXX_STANDARD ${DTU_CXX_STANDARD})
        set_property(TARGET ${_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

    else()
        # Generating header-only library
        add_library(${_NAME} INTERFACE)
        target_include_directories(${_NAME}
            INTERFACE
                "$<BUILD_INTERFACE:${DTU_COMMON_INCLUDE_DIRS}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                "$<BUILD_INTERFACE:${DTU_CC_LIB_INCLUDES}>"
        )
        target_link_libraries(${_NAME}
            INTERFACE
                ${DTU_CC_LIB_LINK_DEPS}
        )
        target_compile_definitions(${_NAME} INTERFACE ${DTU_CC_LIB_DEFINES})
    endif()

    if (NOT "${DTU_CC_LIB_UTIL_DEPS}" STREQUAL "")
        add_dependencies(${_NAME} ${DTU_CC_LIB_UTIL_DEPS})
        if (CONFIG_DEBUG)
            message("${_NAME} add utility depencies: " ${DTU_CC_LIB_UTIL_DEPS})
        endif ()
    endif()

    if (NOT "${DTU_CC_LIB_COMMON_DEPS}" STREQUAL "")
        add_dependencies(${_NAME} ${DTU_CC_LIB_COMMON_DEPS})
        if (CONFIG_DEBUG)
            message("${_NAME} add common depencies: " ${DTU_CC_LIB_COMMON_DEPS})
        endif ()
    endif()

    add_library(dtu::${DTU_CC_LIB_NAME} ALIAS ${_NAME})
endfunction() # dtu_cc_library

