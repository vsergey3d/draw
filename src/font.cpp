#include "font.h"
#include <agg.h>
#include <error.h>
#include <map>
#include <algorithm>

namespace draw {

class Generator {

public:
	Generator();
	~Generator() = default;

	Generator(Generator&) = delete;
	Generator& operator = (const Generator&) = delete;

	ImagePtr atlas(const std::string& filePath, uint32_t letterSize,
		Letters& letters, Error::Code& errorCode, std::string& errorDesc);

private:
	static const uint32_t kMinCharCode = 32;
	static const uint32_t kMaxCharCode = 126;
	static const uint32_t kCharCount = kMaxCharCode - kMinCharCode;
	static const uint32_t kBPP = 1;
	static const uint32_t kBorderWidth = 1;
	static const uint32_t kBorderSize = kBorderWidth * 2;
	static const float kSpaceFactor;

	struct Key {

		std::string filePath;
		uint32_t letterSize;

		Key(const std::string& filePath, uint32_t letterSize) :
			filePath(filePath), letterSize(letterSize) {}

		bool operator < (const Key& other) const;
	};

	aggdef::FontContext* initFont(const Key& key,
		Error::Code& errorCode, std::string& errorDesc);

	void renderFont(aggdef::FontContext& fontCntx, void* buff, uint32_t width,
		uint32_t height, uint32_t stride, int32_t heightOffset, Letters& letters);

	void calcAtlasParams(const std::wstring &text, aggdef::FontContext &fontCntx,
		uint32_t &width, uint32_t &height, int32_t &heightOffset);

	std::unordered_map<std::string, aggdef::FontContextPtr> fontCache_;
	std::vector<unsigned char> buff_;
	std::wstring alphabet_;
};

const float Generator::kSpaceFactor = 0.7f;

bool Generator::Key::operator < (const Key& other) const {

	if (std::lexicographical_compare(filePath.begin(), filePath.end(),
		other.filePath.begin(), other.filePath.end())) {
		return true;
	}
	else if (std::lexicographical_compare(other.filePath.begin(),
		other.filePath.end(), filePath.begin(), filePath.end())) {
		return false;
	}
	if (letterSize < other.letterSize) {
		return true;
	}
	else if (other.letterSize < letterSize) {
		return false;
	}
	return false;
}

Generator::Generator() {

	alphabet_.reserve(kCharCount);
	for (wchar_t wchar = kMinCharCode; wchar <= kMaxCharCode; ++wchar) {
		alphabet_.push_back(wchar);
	}
}

ImagePtr Generator::atlas(const std::string& filePath, uint32_t letterSize,
	Letters& letters, Error::Code& errorCode, std::string& errorDesc) {

	errorCode = Code::NoError;
	errorDesc.clear();

	auto context = initFont(Key(filePath, letterSize), errorCode, errorDesc);
	if (context == nullptr)
		return false;

	auto width = 0u, height = 0u;
	auto heightOffset = 0;
	calcAtlasParams(alphabet_, *context, width, height, heightOffset);

	auto stride = width * kBPP;
	auto size = stride * height;

	if (buff_.empty() || buff_.size() < size) {
		buff_.reserve(size);
		buff_.resize(size);
	}
	renderFont(*context, &buff_[0], width, height, stride, heightOffset, letters);

	auto atlas = makeImage(width, height, Image::Format::A, true);
	atlas->upload(&buff_[0], size);
	return atlas;
}

aggdef::FontContext* Generator::initFont(const Key& key,
	Error::Code& errorCode, std::string& errorDesc) {

	const auto& path = key.filePath;

	auto it = fontCache_.find(path);
	if (it == fontCache_.end()) {
		auto context = std::unique_ptr<aggdef::FontContext>(new aggdef::FontContext());
		ASSERT(context->engine.last_error() == 0);

		if (!context->engine.load_font(path.c_str(), 0, agg::glyph_ren_native_gray8)) {
			errorCode = Code::InvalidFontFile;
			errorDesc = aggdef::freeTypeError(context->engine.last_error());
			return nullptr;
		}
		it = fontCache_.emplace(path, std::move(context)).first;
	}
	auto context = it->second.get();
	context->engine.hinting(false);

	if (context->engine.height() != key.letterSize ||
		context->engine.width() != key.letterSize) {

		context->engine.height(key.letterSize);
		context->engine.width(key.letterSize);
	}
	return context;
}

void Generator::calcAtlasParams(const std::wstring &text, aggdef::FontContext &fontCntx,
	uint32_t &width, uint32_t &height, int32_t &heightOffset) {

	fontCntx.manager.reset_last_glyph();

	width = 0; height = 0;
	heightOffset = std::numeric_limits<int32_t>::max();

	double x = kBorderWidth;
	const wchar_t* p = text.c_str();
	while (*p) {

		fontCntx.manager.reset_last_glyph();
		const agg::glyph_cache* glyph = fontCntx.manager.glyph(*p);
		if (glyph == 0) {
			throw std::runtime_error("Cannot load glyph: " +
				aggdef::freeTypeError(fontCntx.engine.last_error()));
		}
		if (!(glyph->bounds.x1 == 0 && glyph->bounds.x2 == 0 &&
			  glyph->bounds.y1 == 0 && glyph->bounds.y2 == 0)) {

			if (*p != kSpaceSymbol) {
				height = std::max<uint32_t>(height, glyph->bounds.y2 - glyph->bounds.y1);
				heightOffset = std::min(heightOffset, glyph->bounds.y1);
			}
			x += (*p == kSpaceSymbol ? kSpaceFactor : 1.0f) * glyph->advance_x + kBorderSize;
		}
		++p;
	}
	width = (uint32_t)ceil(x);
	height += -heightOffset + kBorderSize;
}

void Generator::renderFont(aggdef::FontContext& fontCntx, void* buff, uint32_t width,
	uint32_t height, uint32_t stride, int32_t heightOffset, Letters& letters) {

	letters.clear();

	agg::rendering_buffer buffer((unsigned char*)buff, width, height, stride);
	aggdef::PixelFormat pxlFormat(buffer);
	aggdef::Renderer renderer(pxlFormat);
	renderer.clear(0);

	agg::renderer_scanline_aa_solid<aggdef::Renderer> glyphRenderer(renderer);
	glyphRenderer.color(255);

	aggdef::FontScanline& sl = fontCntx.manager.gray8_scanline();
	aggdef::FontAdaptor& adaptor = fontCntx.manager.gray8_adaptor();

	double x = kBorderWidth, y = -heightOffset + kBorderWidth, gWidth = 0.0;
	const wchar_t* p = alphabet_.c_str();
	while (*p) {

		fontCntx.manager.reset_last_glyph();
		const agg::glyph_cache* glyph = fontCntx.manager.glyph((uint32_t)*p);
		if (glyph == 0) {
			throw std::runtime_error("Cannot load glyph: " +
				aggdef::freeTypeError(fontCntx.engine.last_error())); //! ASSERT
		}

		if (!(glyph->bounds.x1 == 0 && glyph->bounds.x2 == 0 &&
			glyph->bounds.y1 == 0 && glyph->bounds.y2 == 0)) {

			if (*p != kSpaceSymbol) {
				fontCntx.manager.init_embedded_adaptors(glyph, x, y);
				agg::render_scanlines(adaptor, sl, glyphRenderer);
			}

			gWidth = std::max(glyph->advance_x,
				(double)(glyph->bounds.x2 - glyph->bounds.x1));
			if (*p == kSpaceSymbol) {
				gWidth *= kSpaceFactor;
			}
			letters[*p] = Rect((uint32_t)x, 0, (uint32_t)(x + gWidth), height);
			x += gWidth + kBorderSize;
		}
		++p;
	}
}

bool FontImpl::init(Error::Code& errorCode, std::string& errorDesc) {

	static Generator generator;
	atlas_ = generator.atlas(filePath_, letterSize_, letters_, errorCode, errorDesc);
	return atlas_ != nullptr;
}

FontPtr makeFont(const char* filePath, uint32_t letterSize,
	Error::Code& errorCode, std::string& errorDesc) {

	auto font = std::make_shared<FontImpl>(filePath, letterSize);
	if (!font->init(errorCode, errorDesc)) {
		return FontPtr();
	}
	return font;
}

FontPtr makeFont(const char* filePath, uint32_t letterSize) {

	auto errorCode = Code::NoError;
	std::string errorDesc;
	auto font = makeFont(filePath, letterSize, errorCode, errorDesc);
	if (!font) {
		throw error(errorCode, errorDesc);
	}
	return font;
}

} //namespace draw
