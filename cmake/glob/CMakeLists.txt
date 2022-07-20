cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(glob LANGUAGES NONE)

function(dtu_glob)
    set(oneValueArgs )
    set(options )
    set(multiValueArgs SRCS_GLOB SRCS EXCLUDES EXCLUDES_GLOB)
    cmake_parse_arguments(DTU_GLOB
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if (DTU_GLOB_SRCS_GLOB)
        file(GLOB_RECURSE _SRCS ${DTU_GLOB_SRCS_GLOB})
    endif()
    list(APPEND _SRCS ${DTU_GLOB_SRCS})

    if (DTU_GLOB_EXCLUDES_GLOB)
        file(GLOB_RECURSE _EXCLUDES  ${DTU_GLOB_EXCLUDES_GLOB})
    endif()
    list(APPEND _EXCLUDES ${DTU_GLOB_EXCLUDES})

    foreach (src_exclude IN LISTS _EXCLUDES)
        list(REMOVE_ITEM _SRCS ${src_exclude})
    endforeach()

    message(STATUS "SRC:${_SRCS}")    
    # message(STATUS "SRC:${_SRCS} || EXCLUDE:${_EXCLUDES}")
endfunction(dtu_glob)

dtu_glob(
    SRCS "/home/zizy/project/self/rubbish-bin/cmake/glob/CMakeLists.txt"
    SRCS_GLOB "/home/zizy/project/self/rubbish-bin/**/CMakeLists.txt"
    EXCLUDES "/home/zizy/project/self/rubbish-bin/cmake/appendStr/CMakeLists.txt"
    EXCLUDES_GLOB "/home/zizy/project/self/rubbish-bin/c/**/CMakeLists.txt"
)
