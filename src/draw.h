#pragma once
#include <memory>
#include <exception>

//#define THREAD_CHECK
//#define SHARED_PTR
#define DRAW_NO_EXCEPTIONS

#ifndef ASSERT
#include <cassert>
#define ASSERT(e) assert(e)
#endif

#if defined(__clang__)
#if __has_feature(cxx_noexcept)
#define HAS_NOEXCEPT
#endif
#elif !defined(__clang__) && \
    defined(__GXX_EXPERIMENTAL_CXX0X__) && __GNUC__ * 10 + __GNUC_MINOR__ >= 46 || \
    defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 190023026
#define HAS_NOEXCEPT
#endif
#ifdef HAS_NOEXCEPT
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

namespace draw {

class Error : public std::exception {

public:
    enum class Code {

        NoError = 0,
        OpenGLAbsentFeature,
        OpenGLOutOfMemory,
        InvalidArgument,
        InvalidFontFile,
        AbsentFontGlyph
    };
    virtual ~Error() = 0;
    virtual Code code() const = 0; //noexcept
};

#ifdef DRAW_NO_EXCEPTIONS
Error::Code getLastError();
#endif

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

template <typename T>
struct Span {

    const T* data;
    uint32_t count;

    Span(const T* data, uint32_t count) :
        data(data), count(count) {}
};

using Bytes = Span<uint8_t>;

class Geometry {

public:
    struct Vertex {

        Vector2 position;
        Vector2 uv;

        Vertex() = default;
        Vertex(const Vector2& position, const Vector2& uv) :
            position(position), uv(uv) {};
    };
    using Vertices = Span<Vertex>;

    using Index = uint16_t;
    using Indices = Span<Index>;

    enum class Primitive {

        Point = 1,
        Line,
        Triangle
    };
    virtual ~Geometry() = 0;

    virtual uint32_t vertexCount() const = 0; //nothrow
    virtual uint32_t indexCount() const = 0; //nothrow
    virtual Primitive primitive() const = 0; //nothrow
};

using GeometryPtr = std::shared_ptr<Geometry>;

class Image {

public:
    static const int32_t kMaxSize = 4096;

    enum class Format {

        A,
        RGB,
        RGBA
    };
    virtual ~Image() = 0;

    virtual const Size& size() const = 0; //nothrow
    virtual Format format() const = 0; //nothrow
    virtual bool filter() const = 0; //nothrow

    virtual bool upload(Bytes data) = 0; //nothrow
};

using ImagePtr = std::shared_ptr<Image>;

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

class Font {

public:
    virtual ~Font() = 0;

    virtual const char* filePath() const = 0; //nothrow
    virtual uint32_t letterSize() const = 0; //nothrow
};

using FontPtr = std::shared_ptr<Font>;

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

class Renderer {

public:
    virtual ~Renderer() = 0;

    virtual GeometryPtr makeGeometry(Geometry::Vertices vertices,
        Geometry::Indices indices, Geometry::Primitive primitive) = 0;
    virtual ImagePtr makeImage(const Size& size, Image::Format format, bool filter) = 0;
    virtual FontPtr makeFont(const char* filePath, uint32_t letterSize) = 0;

    virtual ShapePtr makeRect(bool transparent) = 0;
    virtual ShapePtr makeShape(GeometryPtr geometry, bool transparent) = 0;
    virtual TextPtr makeText() = 0;

    virtual uint32_t draw(const Size& screen, const Color& clearColor) = 0; //nothrow
};

using RendererPtr = std::shared_ptr<Renderer>;

class Context {

public:
    virtual ~Context() = 0;
    virtual void setCurrent() = 0;
};

using ContextPtr = std::unique_ptr<Context>;

RendererPtr makeRenderer(ContextPtr context);

} // namespace draw
