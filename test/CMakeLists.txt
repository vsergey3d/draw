set(PROJECT ${DRAW_LIB}_test)
project(${PROJECT} CXX)
include(GMock)

find_library(OPENGL opengl)
include_directories(${SRC_DIR})
include_directories(SYSTEM
    ${DEPS_DIR}/freetype2/include
    ${DEPS_DIR}/agg/include
	${DEPS_DIR}/glew/include
	${OPENGL_INCLUDE_DIR}
    ${DEPS_DIR}/bandit)
set(LIBS agg freetype)

add_definitions(-DGLEW_STATIC -D_CRT_SECURE_NO_WARNINGS -D_DRAW_TEST)
#Workaround for MinGW bug in math.h
if(MINGW)
    add_definitions(-D__NO_INLINE__)
endif()
if(UNIX OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif()
add_subdirectory(${DEPS_DIR}/agg ${CMAKE_BINARY_DIR}/agg EXCLUDE_FROM_ALL)
add_subdirectory(${DEPS_DIR}/freetype2 ${CMAKE_BINARY_DIR}/freetype2 EXCLUDE_FROM_ALL)

add_definitions(-D_UNICODE)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4244 /wd4512 /wd4100 /wd4702")
elseif(UNIX OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Wextra -pedantic -Wnon-virtual-dtor -Werror")
endif()

add_gmock_test(${PROJECT} ${SOURCE_FILES} ${TEST_FILES})
target_link_libraries(${PROJECT} ${LIBS})

find_package(codecov)
add_coverage(${PROJECT})
