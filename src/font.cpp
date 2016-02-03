#include "font.h"
#include <error.h>
#include <algorithm>
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

using Glyph = agg::glyph_cache;

struct FontContext {

	using FontEngine = agg::font_engine_freetype_int32;
	using FontManager = agg::font_cache_manager<FontEngine>;

	FontEngine engine;
	FontManager manager;

	FontContext() : manager(engine) {}
};

template <typename Callable>
bool forEachGlyph(FontContext& context, const std::wstring& text, Callable&& callable,
	Error::Code& errorCode) {

	const wchar_t* c = text.c_str();
	while (*c) {
		context.manager.reset_last_glyph();

		const Glyph* glyph = context.manager.glyph((uint32_t)*c);
		if (!glyph) {
			errorCode = Code::UnsupportedFontGlyph;
			return false;
		}
		if (!(glyph->bounds.x1 == 0 && glyph->bounds.x2 == 0 &&
			  glyph->bounds.y1 == 0 && glyph->bounds.y2 == 0)) {

			callable(*c, *glyph);
		}
		++c;
	}
	return true;
}

class Renderer {

public:
	Renderer(uint32_t width, uint32_t height) :
		width_(width), height_(height) {

		auto size = width_ * height_;
		if (buffer_.size() < size) {
			buffer_.resize(size);
		}
		impl_ = make_unique<Impl>(buffer_.data(), width_, height_);
	}
	~Renderer() = default;

	Renderer(Renderer&) = delete;
	Renderer& operator = (const Renderer&) = delete;

	void renderGlyph(FontContext& context, const Glyph& glyph, double x, double y) {

		context.manager.init_embedded_adaptors(&glyph, x, y);
		agg::render_scanlines(context.manager.gray8_adaptor(),
			context.manager.gray8_scanline(), impl_->glyphRenderer);
	}

	ImagePtr makeAtlas() {

		auto atlas = makeImage(width_, height_, Image::Format::A, true);
		atlas->upload(buffer_.data(), buffer_.size());
		return atlas;
	}

private:
	struct Impl {

		using AggPixelFormat = agg::pixfmt_gray8;
		using AggRenderer = agg::renderer_base<AggPixelFormat>;
		using AggGlyphRenderer = agg::renderer_scanline_aa_solid<AggRenderer>;

		agg::rendering_buffer buffer;
		AggPixelFormat pixelFormat;
		AggRenderer renderer;
		AggGlyphRenderer glyphRenderer;

		Impl(uint8_t* tmpBuffer, uint32_t width, uint32_t height) :
			buffer(tmpBuffer, width, height, width),
			pixelFormat(buffer),
			renderer(pixelFormat),
			glyphRenderer(renderer) {

			renderer.clear(0);
			glyphRenderer.color(255);
		}
	};

	uint32_t width_ {0};
	uint32_t height_ {0};
	std::unique_ptr<Impl> impl_;
	static std::vector<uint8_t> buffer_;
};

std::vector<uint8_t> Renderer::buffer_{};

class Generator {

public:
	Generator() {

		static const auto kMinCharCode = 32u;
		static const auto kMaxCharCode = 126u;

		alphabet_.reserve(kMaxCharCode - kMinCharCode);
		for (wchar_t c = kMinCharCode; c <= kMaxCharCode; ++c) {
			alphabet_.push_back(c);
		}
	}
	~Generator() = default;

	Generator(Generator&) = delete;
	Generator& operator = (const Generator&) = delete;

	ImagePtr atlas(const std::string& filePath, uint32_t letterSize,
		Letters& letters, Error::Code& errorCode) {

		static const auto kBorderWidth = 1u, kBorderSize = kBorderWidth * 2;
		static const auto kSpaceFactor = 0.7f;

		errorCode = Code::NoError;
		letters.clear();

		auto context = getContext(filePath, letterSize, errorCode);
		if (!context)
			return ImagePtr();

		auto width = 0, height = 0, heightOffset = 0;
		auto x = (double)kBorderWidth, y = 0.0;

		if (!forEachGlyph(*context, alphabet_,
			[&x, &height, &heightOffset](wchar_t c, const Glyph& glyph){
				if (c != kSpaceSymbol) {
					height = std::max<int32_t>(height, glyph.bounds.y2 - glyph.bounds.y1);
					heightOffset = std::min(heightOffset, glyph.bounds.y1);
				}
				x += (c == kSpaceSymbol ? kSpaceFactor : 1.0f) * glyph.advance_x + kBorderSize;
			}, errorCode)) {
			return ImagePtr();
		}
		width = (int32_t)ceil(x);
		height += -heightOffset + kBorderSize;

		Renderer renderer((uint32_t)width, (uint32_t)height);
		x = kBorderWidth;
		y = -heightOffset + kBorderWidth;

		if (!forEachGlyph(*context, alphabet_,
			[&renderer, &context, &letters, &x, &y, &height](wchar_t c, const Glyph& glyph){
				if (c != kSpaceSymbol) {
					renderer.renderGlyph(*context, glyph, x, y);
				}
				auto w = std::max(glyph.advance_x, (double)(glyph.bounds.x2 - glyph.bounds.x1));
				if (c == kSpaceSymbol) w *= kSpaceFactor;
				letters[c] = Rect((uint32_t)x, 0, (uint32_t)(x + w), height);
				x += w + kBorderSize;
			}, errorCode)) {
			return ImagePtr();
		}
		return renderer.makeAtlas();
	}

private:
	FontContext* getContext(const std::string& filePath, uint32_t letterSize,
		Error::Code& errorCode) {

		auto& ctxt = cache_[filePath];
		if (!ctxt) {
			ctxt = make_unique<FontContext>();
			ASSERT(ctxt->engine.last_error() == 0);

			if (!ctxt->engine.load_font(filePath.c_str(), 0, agg::glyph_ren_native_gray8)) {
				errorCode = Code::InvalidFontFile;
				return nullptr;
			}
		}
		ctxt->engine.hinting(false);

		if (ctxt->engine.height() != letterSize ||
			ctxt->engine.width() != letterSize) {
			ctxt->engine.height(letterSize);
			ctxt->engine.width(letterSize);
		}
		return ctxt.get();
	}

	std::unordered_map<std::string, std::unique_ptr<FontContext>> cache_;
	std::wstring alphabet_;
};

bool FontImpl::init(Error::Code& errorCode) {

	static Generator generator;
	atlas_ = generator.atlas(filePath_, letterSize_, letters_, errorCode);
	return atlas_ != nullptr;
}

FontPtr makeFont(const char* filePath, uint32_t letterSize, Error::Code& errorCode) {

	auto font = std::make_shared<FontImpl>(filePath, letterSize);
	if (!font->init(errorCode)) {
		return FontPtr();
	}
	return font;
}

FontPtr makeFont(const char* filePath, uint32_t letterSize) {

	auto errorCode = Code::NoError;
	auto font = makeFont(filePath, letterSize, errorCode);
	if (!font) {
		throw error(errorCode);
	}
	return font;
}

} //namespace draw
