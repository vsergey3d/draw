#include "font.h"
#include <error.h>
#include <renderer.h>
#include <algorithm>
#include <agg_pixfmt_gray.h>
#include <agg_renderer_scanline.h>
#include <agg_font_cache_manager.h>
#include <agg_font_freetype.h>

namespace draw {

namespace details {

using Glyph = agg::glyph_cache;

class Renderer {

public:
    using Buffer = agg::rendering_buffer;
    using PixelFormat = agg::pixfmt_gray8;
    using BaseRenderer = agg::renderer_base<PixelFormat>;
    using Scanline = agg::renderer_scanline_aa_solid<BaseRenderer>;

    Renderer(uint32_t width, uint32_t height) :
        buffer_((size_t)width * height/* * 1bpp*/),
        renderBuffer_(buffer_.data(), width, height, width),
        pixelFormat_(renderBuffer_),
        baseRenderer_(pixelFormat_),
        scanline_(baseRenderer_) {

        baseRenderer_.clear(0);
        scanline_.color(255);
    }
    ~Renderer() = default;
    Renderer(Renderer&) = delete;
    Renderer& operator = (const Renderer&) = delete;

    Scanline& scanline() { return scanline_; }
    const uint8_t* data() { return buffer_.data(); }
    size_t size() const { return buffer_.size(); }

private:
    std::vector<uint8_t> buffer_;
    Buffer renderBuffer_;
    PixelFormat pixelFormat_;
    BaseRenderer baseRenderer_;
    Scanline scanline_;
};

class Context {

public:
    Context() : manager_(engine_) { }
    ~Context() = default;
    Context(Context &) = delete;
    Context &operator=(const Context &) = delete;

    bool loadFont(const std::string &filePath, uint32_t letterSize);
    const Glyph* getGlyph(wchar_t character);
    void renderGlyph(Renderer& renderer, const Glyph* glyph, double x, double y);

private:
    using Engine = agg::font_engine_freetype_int32;
    using Manager = agg::font_cache_manager<Engine>;

    Engine engine_;
    Manager manager_;
};

const std::wstring& getAlphabet() {

    const wchar_t kMinCharCode = 32, kMaxCharCode = 126;

    static std::wstring alphabet;
    if (alphabet.empty()) {
        alphabet.reserve(kMaxCharCode - kMinCharCode);
        for (wchar_t c = kMinCharCode; c <= kMaxCharCode; ++c) {
            alphabet.push_back(c);
        }
    }
    return alphabet;
}

bool Context::loadFont(const std::string &filePath, uint32_t letterSize) {

    ASSERT(engine_.last_error() == 0);

    if (!engine_.load_font(filePath.c_str(), 0, agg::glyph_ren_native_gray8)) {
        setError(InvalidFontFile);
        return false;
    }
    engine_.hinting(false);
    if (engine_.height() != letterSize ||
        engine_.width() != letterSize) {
        engine_.height(letterSize);
        engine_.width(letterSize);
    }
    for (auto c : getAlphabet()) {
        manager_.reset_last_glyph();
        if (!manager_.glyph((uint32_t)c)) {
            setError(IncompleteFontFile);
            return false;
        }
    }
    return true;
}

const Glyph* Context::getGlyph(wchar_t character) {

    manager_.reset_last_glyph();
    auto glyph = manager_.glyph((uint32_t)character);
    const auto& b = glyph->bounds;
    if (b.x1 != 0 || b.x2 != 0 || b.y1 != 0 || b.y2 != 0) {
        return glyph;
    }
    return nullptr;
}

void Context::renderGlyph(Renderer& renderer, const Glyph* glyph, double x, double y) {

    manager_.init_embedded_adaptors(glyph, x, y);

    agg::render_scanlines(manager_.gray8_adaptor(),
        manager_.gray8_scanline(), renderer.scanline());
}

ImagePtr generateAtlas(RendererImpl& rendererImpl, const std::string& filePath,
    uint32_t letterSize, Letters& letters) {

    static const auto kBorderWidth = 1u, kBorderSize = kBorderWidth * 2;
    static const auto kSpaceFactor = 0.7f;

    Context context;
    if (!context.loadFont(filePath, letterSize))
        return ImagePtr();

    auto width = 0, height = 0, heightOffset = 0;
    auto x = (double)kBorderWidth, y = 0.0;

    for (auto c : getAlphabet()) {
        auto glyph = context.getGlyph(c);
        if (!glyph)
            continue;
        if (c != kSpaceSymbol) {
            height = std::max<int32_t>(height, glyph->bounds.y2 - glyph->bounds.y1);
            heightOffset = std::min(heightOffset, glyph->bounds.y1);
        }
        x += (c == kSpaceSymbol ? kSpaceFactor : 1.0f) * glyph->advance_x + kBorderSize;
    }
    width = (int32_t)ceil(x);
    height += -heightOffset + kBorderSize;
    x = kBorderWidth;
    y = -heightOffset + kBorderWidth;

    letters.clear();
    Renderer renderer((uint32_t)width, (uint32_t)height);

    for (auto c : getAlphabet()) {
        auto glyph = context.getGlyph(c);
        if (!glyph)
            continue;
        if (c != kSpaceSymbol)
            context.renderGlyph(renderer, glyph, x, y);
        auto w = (c == kSpaceSymbol ? kSpaceFactor : 1.0f) *
             std::max(glyph->advance_x, (double)(glyph->bounds.x2 - glyph->bounds.x1));
        letters[c] = Rect((int32_t)x, 0, (int32_t)(x + w), height);
        x += w + kBorderSize;
    }
    auto atlas = rendererImpl.makeImage(
        {(uint32_t)width, (uint32_t)height}, Image::Format::A, true);
    atlas->upload(Image::Bytes(renderer.data(), renderer.size()));
    return atlas;
}

} // namespace details

FontImpl::FontImpl(RendererImpl& renderer, const char* filePath, uint32_t letterSize) :
    renderer_(renderer),
    filePath_(filePath),
    letterSize_(letterSize) {
}

bool FontImpl::init() {

    if (letterSize_ < kMinLetterSize) {
        setError(InvalidArgument);
        return false;
    }
    atlas_ = details::generateAtlas(renderer_, filePath_, letterSize_, letters_);
    return atlas_ != nullptr;
}

} //namespace draw
