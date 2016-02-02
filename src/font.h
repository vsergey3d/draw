#pragma once
#include <draw.h>
#include <unordered_map>

namespace draw {

static const wchar_t kSpaceSymbol = L' ';

using Letters = std::unordered_map<wchar_t, Rect>;

class FontImpl final : public Font {

public:
	FontImpl(const char* filePath, uint32_t letterSize) :
		filePath_(filePath), letterSize_(letterSize) {}
	virtual ~FontImpl() = default;

	FontImpl(const FontImpl&) = delete;
	FontImpl& operator = (const FontImpl&) = delete;

	bool init(Error::Code& errorCode, std::string& errorDesc);

	ImagePtr atlas() const { return atlas_; }
	const Letters& letters() const { return letters_; }

	// Font

	virtual const char* filePath() const final { return filePath_.c_str(); }
	virtual uint32_t letterSize() const final { return letterSize_; }

private:
	std::string filePath_;
	uint32_t letterSize_ {0};

	ImagePtr atlas_;
	Letters letters_;
};

} // namespace draw
