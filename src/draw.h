#pragma once
#include <memory>
#include <exception>

#ifndef ASSERT
#include <cassert>
#define ASSERT(e) assert(e)
#endif

//#define THREAD_CHECK

//#define SHARED_PTR
//#define MAKE_SHARED

namespace draw {

class Error : public std::exception {

public:
    enum class Code {

		NoError = 0,
		UnsupportedOpenGLFeature,
        InvalidImageWidthHeight,
		IncorrectImageDataSize,
        InvalidBatcherVertexData,
        InvalidBatcherIndexData,
        InvalidBatcherVertexShader,
		InvalidBatcherFragmentShader,
		InvalidBatcherProgram,
		InvalidFontFile,
		UnsupportedFontGlyph
    };

	virtual ~Error() = 0;

	virtual const char* desc() const = 0; //noexcept
	virtual Code code() const = 0; //noexcept
};

class Context {

public:
	virtual ~Context() = 0;

	virtual void setCurrent() = 0;
};

void init(Context& context);

struct Point {

	int32_t x {0};
	int32_t y {0};

	Point() = default;
	Point(int32_t x, int32_t y) :
		x(x), y(y) {};
};

bool operator == (const Point& left, const Point& right);
bool operator != (const Point& left, const Point& right);

struct Size {

	int32_t width {0};
	int32_t height {0};

	Size() = default;
	Size(int32_t width, int32_t height) :
		width(width), height(height) {};
};

bool operator == (const Size& left, const Size& right);
bool operator != (const Size& left, const Size& right);

struct Rect {

	int32_t left {0};
	int32_t bottom {0};
	int32_t right {0};
	int32_t top {0};

	Rect() = default;
	Rect(int32_t left, int32_t bottom, int32_t right, int32_t top) :
		left(left), bottom(bottom), right(right), top(top) {};
};

bool operator == (const Rect& left, const Rect& right);
bool operator != (const Rect& left, const Rect& right);

struct Vector2 {

	float x {0.0f};
	float y {0.0f};

	Vector2() = default;
	Vector2(float x, float y) :
		x(x), y(y) {};
};

bool operator == (const Vector2& left, const Vector2& right);
bool operator != (const Vector2& left, const Vector2& right);

struct Color {

	float r {0.0f};
	float g {0.0f};
	float b {0.0f};
	float a {1.0f};

	Color() = default;
	Color(float r, float g, float b, float a = 1.0f) :
		r(r), g(g), b(b), a(a) {};
};

bool operator == (const Color& left, const Color& right);
bool operator != (const Color& left, const Color& right);

struct Vertex {

	Vector2 position;
	Vector2 uv;

	Vertex() = default;
	Vertex(const Vector2& position, const Vector2& uv) :
		position(position), uv(uv) {};
};

bool operator == (const Vertex& left, const Vertex& right);
bool operator != (const Vertex& left, const Vertex& right);

using Index = uint16_t;

class Image {

public:
	static const uint32_t kMaxSize = 4096;

	enum class Format {

		A,
		RGB,
		RGBA
	};

	virtual ~Image() = 0;

	virtual Format format() const = 0; //nothrow
	virtual uint32_t width() const = 0; //nothrow
	virtual uint32_t height() const = 0; //nothrow

	virtual void upload(const uint8_t* data, uint32_t byteSize) = 0;
	virtual bool upload(const uint8_t* data, uint32_t byteSize,
		Error::Code& errorCode) = 0; //nothrow
};

using ImagePtr = std::shared_ptr<Image>;

ImagePtr makeImage(uint32_t width, uint32_t height, Image::Format format, bool filter);
ImagePtr makeImage(uint32_t width, uint32_t height, Image::Format format, bool filter,
	Error::Code& errorCode); //nothrow

class Batcher {

public:
	virtual ~Batcher() = 0;

	virtual void screenSize(uint32_t width, uint32_t height) = 0; //nothrow
	virtual uint32_t draw() = 0; //nothrow
};

using BatcherPtr = std::shared_ptr<Batcher>;

struct InstanceData {

	const Vertex* vertices {nullptr};
	uint32_t vertexCount {0};
	const Index* indices {nullptr};
	uint32_t indexCount {0};
	//! fillMode ?
	const char* vertexShader {nullptr};
	const char* fragmentShader {nullptr};
	bool blending {false};
};

BatcherPtr makeBatcher(const InstanceData& data);
BatcherPtr makeBatcher(const InstanceData& data,
	Error::Code& errorCode, std::string& errorDesc); //nothrow

class Visual {

public:
	virtual ~Visual() = 0;

	virtual void visibility(bool enable) = 0; //nothrow
	virtual bool visibility() const = 0; //nothrow

	virtual void order(uint32_t order) = 0; //nothrow
	virtual uint32_t order() const = 0; //nothrow

	virtual void position(const Point& position) = 0; //nothrow
	virtual const Point& position() const = 0; //nothrow

	virtual const Rect& bounds() const = 0; //nothrow
};

using VisualPtr = std::shared_ptr<Visual>;

class Shape : public Visual {

public:
	virtual ~Shape() = 0;

	virtual void size(const Size& size) = 0; //nothrow
	virtual const Size& size() const = 0; //nothrow

	virtual void color(const Color& color) = 0; //nothrow
	virtual const Color& color() const = 0; //nothrow

	virtual void image(ImagePtr image) = 0; //nothrow
	virtual void image(ImagePtr image, const Vector2& tile) = 0; //nothrow
	virtual ImagePtr image() const = 0; //nothrow

	virtual void image(ImagePtr atlas, const Rect& element) = 0; //nothrow
	virtual ImagePtr image(Rect& element) const = 0; //nothrow
};

using ShapePtr = std::shared_ptr<Shape>;

ShapePtr makeShape(BatcherPtr batcher); //nothrow

class Font {

public:
	virtual ~Font() = 0;

	virtual const char* filePath() const = 0; //nothrow
	virtual uint32_t letterSize() const = 0; //nothrow
};

using FontPtr = std::shared_ptr<Font>;

FontPtr makeFont(const char* filePath, uint32_t letterSize);
FontPtr makeFont(const char* filePath, uint32_t letterSize,
	Error::Code& errorCode, std::string& errorDesc); //nothrow

class Text : public Visual {

public:
	virtual ~Text() = 0;

	virtual void font(FontPtr font) = 0; //nothrow
	virtual FontPtr font() const = 0; //nothrow

	virtual void text(const wchar_t* text) = 0; //nothrow
	virtual const wchar_t* text() const = 0; //nothrow

	virtual void color(const Color& color) = 0; //nothrow
	virtual const Color& color() const = 0; //nothrow

	enum class HorizAlign {

		Left,
		Center,
		Right
	};

	virtual void horizAlign(HorizAlign alignment) = 0; //nothrow
	virtual HorizAlign horizAlign() const = 0; //nothrow

	enum class VertAlign {

		Top,
		Middle,
		Bottom
	};

	virtual void vertAlign(VertAlign alignment) = 0; //nothrow
	virtual VertAlign vertAlign() const = 0; //nothrow
};

using TextPtr = std::shared_ptr<Text>;

TextPtr makeText(BatcherPtr batcher); //nothrow

} // namespace draw
