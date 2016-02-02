cmake_minimum_required(VERSION 3.3)

set(DRAW_LIB draw)
project(${DRAW_LIB} CXX)

add_definitions(-D_UNICODE -DGLEW_STATIC)
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic -Wnon-virtual-dtor -Werror")
    set(LIB_TYPE mingw_lib)
elseif(MSVC)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /WX")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

    set(LIB_TYPE msvc_lib)
endif()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LIB_DIR ${ROOT_DIR}/lib/${LIB_TYPE})
set(SRC_DIR ${ROOT_DIR}/src)
set(DEPS_DIR ${ROOT_DIR}/deps)
set(DRAW_H ${SRC_DIR}/draw.h)
set(DRAW_INCLUDE ${ROOT_DIR}/include)

set(SOURCE_FILES ${SRC_DIR}/gl.cpp
        ${SRC_DIR}/draw.cpp
        ${SRC_DIR}/image.cpp
        ${SRC_DIR}/font.cpp
        ${SRC_DIR}/batcher.cpp
        ${SRC_DIR}/shape.cpp
        ${SRC_DIR}/text.cpp)

find_library(OPENGL opengl)

set(INCLUDE_DIR
        ${DEPS_DIR}/freetype2/include
        ${DEPS_DIR}/glew/include
        ${DEPS_DIR}/agg/include
        {$OPENGL_INCLUDE_DIR}
)
include_directories(${SRC_DIR})
include_directories(SYSTEM ${INCLUDE_DIR})

add_library(${DRAW_LIB} STATIC ${SOURCE_FILES})
set_target_properties(${DRAW_LIB} PROPERTIES DEBUG_POSTFIX d)

file(COPY ${DRAW_H} DESTINATION ${DRAW_INCLUDE})

install(TARGETS ${DRAW_LIB} DESTINATION ${LIB_DIR})
install(FILES ${DRAW_H} DESTINATION ${DRAW_INCLUDE})

