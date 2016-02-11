#include "font.h"
#include <error.h>
#include <renderer.h>
#include <algorithm>
#include <agg_pixfmt_gray.h>
#include <agg_renderer_scanline.h>
#include <agg_font_cache_manager.h>
#include <agg_font_freetype.h>

namespace draw {

class Generator {

public:
    Generator() {

        static const wchar_t kMinCharCode = 32, kMaxCharCode = 126;

        alphabet_.reserve(kMaxCharCode - kMinCharCode);
        for (wchar_t c = kMinCharCode; c <= kMaxCharCode; ++c)
            alphabet_.push_back(c);
    }
    ~Generator() = default;
    Generator(Generator&) = delete;
    Generator& operator = (const Generator&) = delete;

    ImagePtr atlas(RendererImpl& rendererImpl, const std::string& filePath,
        uint32_t letterSize, Letters& letters) {

        static const auto kBorderWidth = 1u, kBorderSize = kBorderWidth * 2;
        static const auto kSpaceFactor = 0.7f;

        auto context = getContext(filePath, letterSize);
        if (!context)
            return ImagePtr();

        auto width = 0, height = 0, heightOffset = 0;
        auto x = (double)kBorderWidth, y = 0.0;

        if (!forEachGlyph(*context, alphabet_,
            [&x, &height, &heightOffset](wchar_t c, const Glyph& glyph) {
                if (c != kSpaceSymbol) {
                    height = std::max<int32_t>(height, glyph.bounds.y2 - glyph.bounds.y1);
                    heightOffset = std::min(heightOffset, glyph.bounds.y1);
                }
                x += (c == kSpaceSymbol ? kSpaceFactor : 1.0f) * glyph.advance_x + kBorderSize;
            })) {
            return ImagePtr();
        }
        width = (int32_t)ceil(x);
        height += -heightOffset + kBorderSize;

        auto renderer = makeRenderer((uint32_t)width, (uint32_t)height);
        x = kBorderWidth;
        y = -heightOffset + kBorderWidth;

        letters.clear();
        if (!forEachGlyph(*context, alphabet_,
            [&renderer, &context, &letters, &x, &y, &height](wchar_t c, const Glyph& glyph) {
                if (c != kSpaceSymbol)
                    renderer->renderGlyph(*context, glyph, x, y);
                auto w = (c == kSpaceSymbol ? kSpaceFactor : 1.0f) *
                    std::max(glyph.advance_x, (double)(glyph.bounds.x2 - glyph.bounds.x1));
                letters[c] = Rect((uint32_t)x, 0, (uint32_t)(x + w), height);
                x += w + kBorderSize;
            })) {
            return ImagePtr();
        }
        auto atlas = rendererImpl.makeImage({width, height}, Image::Format::A, true);
        atlas->upload(Image::Bytes(buffer_.data(), buffer_.size()));
        return atlas;
    }

private:
    using Glyph = agg::glyph_cache;

    struct Context {

        using Engine = agg::font_engine_freetype_int32;

        Engine engine;
        agg::font_cache_manager<Engine> manager;

        Context() : manager(engine) {}
    };

    class Renderer {

    public:
        Renderer(uint8_t* buffer, uint32_t width, uint32_t height) :
            renderBuffer(buffer, width, height, width),
            pixelFormat(renderBuffer),
            renderer(pixelFormat),
            glyphRenderer(renderer) {

            renderer.clear(0);
            glyphRenderer.color(255);
        }
        ~Renderer() = default;
        Renderer(Renderer&) = delete;
        Renderer& operator = (const Renderer&) = delete;

        void renderGlyph(Context& context, const Glyph& glyph, double x, double y) {

            context.manager.init_embedded_adaptors(&glyph, x, y);
            agg::render_scanlines(context.manager.gray8_adaptor(),
                context.manager.gray8_scanline(), glyphRenderer);
        }
    private:
        using AggPixelFormat = agg::pixfmt_gray8;
        using AggRenderer = agg::renderer_base<AggPixelFormat>;
        using AggGlyphRenderer = agg::renderer_scanline_aa_solid<AggRenderer>;

        agg::rendering_buffer renderBuffer;
        AggPixelFormat pixelFormat;
        AggRenderer renderer;
        AggGlyphRenderer glyphRenderer;
    };

    std::unique_ptr<Renderer> makeRenderer(uint32_t width, uint32_t height) {

        auto byteSize = (size_t)width * height; // * 1bpp
        if (buffer_.size() < byteSize) {
            buffer_.resize(byteSize);
        }
        return make_unique<Renderer>(buffer_.data(), width, height);
    }

    Context* getContext(const std::string& filePath, uint32_t letterSize) {

        auto& ctxt = cache_[filePath];
        if (!ctxt) {
            ctxt = make_unique<Context>();
            ASSERT(ctxt->engine.last_error() == 0);
            if (!ctxt->engine.load_font(filePath.c_str(), 0, agg::glyph_ren_native_gray8)) {
                setError(InvalidFontFile);
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

    template <typename Callable>
    bool forEachGlyph(Context& context, const std::wstring& text, Callable&& callable) {

        for (auto c = text.c_str(); *c; ++c) {
            context.manager.reset_last_glyph();
            auto glyph = context.manager.glyph((uint32_t)*c);
            if (!glyph) {
                setError(IncompleteFontFile);
                return false;
            }
            const auto& b = glyph->bounds;
            if (b.x1 != 0 || b.x2 != 0 || b.y1 != 0 || b.y2 != 0)
                callable(*c, *glyph);
        }
        return true;
    }

    std::wstring alphabet_;
    std::vector<uint8_t> buffer_;
    std::unordered_map<std::string, std::unique_ptr<Context>> cache_;
};

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
    static Generator generator;
    atlas_ = generator.atlas(renderer_, filePath_, letterSize_, letters_);
    return atlas_ != nullptr;
}

} //namespace draw
