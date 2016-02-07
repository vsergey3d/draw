cmake_minimum_required(VERSION 3.1)

set(AGG_LIB agg)
project(${AGG_LIB} C CXX)

if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(LIB_TYPE mingw_lib)
	
elseif(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

    set(LIB_TYPE msvc_lib)
endif()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LIB_DIR ${ROOT_DIR}/lib)
set(INCLUDE_DIR ${ROOT_DIR}/include)
set(SRC_DIR ${ROOT_DIR}/src)

set(SOURCE_FILES ${SRC_DIR}/agg_arc.cpp
    ${SRC_DIR}/agg_arrowhead.cpp
    ${SRC_DIR}/agg_bezier_arc.cpp
    ${SRC_DIR}/agg_bspline.cpp
    ${SRC_DIR}/agg_curves.cpp
    ${SRC_DIR}/agg_embedded_raster_fonts.cpp
    ${SRC_DIR}/agg_gsv_text.cpp
    ${SRC_DIR}/agg_image_filters.cpp
    ${SRC_DIR}/agg_line_aa_basics.cpp
    ${SRC_DIR}/agg_line_profile_aa.cpp
    ${SRC_DIR}/agg_rounded_rect.cpp
    ${SRC_DIR}/agg_sqrt_tables.cpp
    ${SRC_DIR}/agg_trans_affine.cpp
    ${SRC_DIR}/agg_trans_double_path.cpp
    ${SRC_DIR}/agg_trans_single_path.cpp
    ${SRC_DIR}/agg_trans_warp_magnifier.cpp
    ${SRC_DIR}/agg_vcgen_bspline.cpp
    ${SRC_DIR}/agg_vcgen_contour.cpp
    ${SRC_DIR}/agg_vcgen_dash.cpp
    ${SRC_DIR}/agg_vcgen_markers_term.cpp
    ${SRC_DIR}/agg_vcgen_smooth_poly1.cpp
    ${SRC_DIR}/agg_vcgen_stroke.cpp
    ${SRC_DIR}/agg_vpgen_clip_polygon.cpp
    ${SRC_DIR}/agg_vpgen_clip_polyline.cpp
    ${SRC_DIR}/agg_vpgen_segmentator.cpp
    ${ROOT_DIR}/gpc/gpc.c
    ${ROOT_DIR}/font_freetype/agg_font_freetype.cpp
    ${ROOT_DIR}/font_win32_tt/agg_font_win32_tt.cpp)

include_directories(${INCLUDE_DIR} ${ROOT_DIR}/../freetype2/include)

add_library(${AGG_LIB} STATIC ${SOURCE_FILES})

set_target_properties(${AGG_LIB} PROPERTIES DEBUG_POSTFIX d)

set(FONT_FILES ${ROOT_DIR}/font_freetype/agg_font_freetype.h
        ${ROOT_DIR}/font_win32_tt/agg_font_win32_tt.h)
set(GPC_H  ${ROOT_DIR}/gpc/gpc.h)
file(COPY ${FONT_FILES} ${GPC_H} DESTINATION ${INCLUDE_DIR})

install(TARGETS ${AGG_LIB} DESTINATION ${LIB_DIR})
install(FILES ${FONT_FILES} ${GPC_H} DESTINATION ${INCLUDE_DIR})