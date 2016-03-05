cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(DRAW_LIB draw)
set(PROJECT ${DRAW_LIB}_project)
project(${PROJECT} CXX)
enable_testing()

set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LIB_DIR ${ROOT_DIR}/lib/${LIB_PATH})
set(SRC_DIR ${ROOT_DIR}/src)
set(DEPS_DIR ${ROOT_DIR}/deps)
set(TEST_DIR ${ROOT_DIR}/test)
set(DRAW_H ${SRC_DIR}/draw.h)
set(DRAW_INCLUDE ${ROOT_DIR}/include)

set(SOURCE_FILES ${SRC_DIR}/draw.cpp
        ${SRC_DIR}/error.cpp
        ${SRC_DIR}/image.cpp
        ${SRC_DIR}/geometry.cpp
        ${SRC_DIR}/font.cpp
        ${SRC_DIR}/renderer.cpp
        ${SRC_DIR}/shape.cpp
        ${SRC_DIR}/text.cpp)

set(TEST_FILES ${TEST_DIR}/draw.cpp
        ${TEST_DIR}/renderer.cpp)

find_package(Doxygen REQUIRED)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/doc)
    set(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    configure_file(${doxyfile_in} ${DOXYGEN_INPUT} @ONLY)

    add_custom_command(
        OUTPUT ${DOXYGEN_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
        COMMAND ${CMAKE_COMMAND} -E echo "Done."
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${DOXYGEN_INPUT}
    )

    add_custom_target(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})

    add_custom_target(apidoc_forced
        COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
        COMMAND ${CMAKE_COMMAND} -E echo "Done."
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    message(FATAL_ERROR "doxygen not found")
endif(DOXYGEN_FOUND)

add_subdirectory(${SRC_DIR} ${CMAKE_BINARY_DIR}/${DRAW_LIB})
add_subdirectory(${TEST_DIR} ${CMAKE_BINARY_DIR}/${DRAW_LIB}_test)
