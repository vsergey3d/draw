#pragma once
#include <draw.h>
#include <string>
#include <unordered_map>

namespace draw {

static const wchar_t kSpaceSymbol = L' ';

using Letters = std::unordered_map<wchar_t, Rect>;

class RendererImpl;

class FontImpl final : public Font {

public:
    FontImpl(RendererImpl& renderer, const char* filePath, uint32_t letterSize);
    virtual ~FontImpl() = default;

    FontImpl(const FontImpl&) = delete;
    FontImpl& operator = (const FontImpl&) = delete;

    bool init();
    ImagePtr atlas() const { return atlas_; }
    const Letters& letters() const { return letters_; }

    // Font

    virtual const char* filePath() const final { return filePath_.c_str(); }
    virtual uint32_t letterSize() const final { return letterSize_; }

private:
	RendererImpl& renderer_;
	std::string filePath_;
    uint32_t letterSize_ {0};
	Letters letters_;
	ImagePtr atlas_;
};

} // namespace draw
