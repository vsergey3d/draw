#pragma once
#if defined(_MSC_VER)
#pragma warning(disable: 4244 4512 4100 4996 4267)
#endif
#include <agg_pixfmt_gray.h>
#include <agg_renderer_scanline.h>
#include <agg_font_cache_manager.h>
#include <agg_font_freetype.h>
#if defined(_MSC_VER)
#pragma warning(default: 4244 4512 4100 4996 4267)
#endif

namespace draw {
namespace aggdef {

// agg definitions

using PixelFormat = agg::pixfmt_gray8;
using Renderer = agg::renderer_base<PixelFormat>;
using Color = Renderer::color_type;

// font definitions

using FontEngine = agg::font_engine_freetype_int32;
using FontManager = agg::font_cache_manager<FontEngine>;

struct FontContext {

	FontEngine engine;
	FontManager manager;

	FontContext() : manager(engine) {}
};
using FontContextPtr = std::unique_ptr<FontContext>;
 
using FontScanline = FontManager::gray8_scanline_type;
using FontAdaptor = FontManager::gray8_adaptor_type;

// FreeType2 error definitions
    
#undef __FTERRORS_H__
#define FT_ERRORDEF(e, v, s) {e, s},
#define FT_ERROR_START_LIST {
#define FT_ERROR_END_LIST {0, 0}};

static const struct {

	int err_code;
	const char* err_msg;

} ft_errors[] =
#include FT_ERRORS_H

inline std::string freeTypeError(int32_t errorCode) {

	if (errorCode < 0 || sizeof(ft_errors) <= (uint32_t)errorCode) {
		return std::string();
	}
	static const char FreeTypeDesc[] = "FreeType2 error: ";
	return FreeTypeDesc + std::string(ft_errors[errorCode].err_msg);
}

} //namespace aggdef
} //namespace draw