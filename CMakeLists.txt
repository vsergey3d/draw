cmake_minimum_required(VERSION 3.3)

set(DRAW_LIB draw)
project(${DRAW_LIB} CXX)

add_definitions(-D_UNICODE -DGLEW_STATIC)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI /MTd")
    set(LIB_PATH msvc_lib)    
	
elseif(UNIX OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic -Wnon-virtual-dtor -Werror")

	set(LIB_PATH unx_lib)
	if(MINGW)
		set(LIB_PATH mingw_lib)
	elseif(APPLE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -mmacosx-version-min=10.7")
        set(LIB_PATH osx_lib)
    endif()
endif()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LIB_DIR ${ROOT_DIR}/lib/${LIB_PATH})
set(SRC_DIR ${ROOT_DIR}/src)
set(DEPS_DIR ${ROOT_DIR}/deps)
set(DRAW_H ${SRC_DIR}/draw.h)
set(DRAW_INCLUDE ${ROOT_DIR}/include)

set(SOURCE_FILES ${SRC_DIR}/draw.cpp
        ${SRC_DIR}/image.cpp
        ${SRC_DIR}/geometry.cpp
        ${SRC_DIR}/font.cpp
        ${SRC_DIR}/renderer.cpp
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

