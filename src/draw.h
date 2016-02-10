#pragma once
#include <memory>

#ifndef ASSERT
#include <cassert>
#define ASSERT(e) assert(e)
#endif

#ifndef DRAW_NO_EXCEPTIONS
#include <exception>
#endif

namespace draw {

//! An error code.
/*! Describes an error which library can generate. */
enum ErrorCode {

    NoError = 0, /*!< there are no errors */
    OpenGLAbsentFeature, /*!< some OpenGL features is not supported on your system */
    OpenGLOutOfMemory, /*!< memory is not enough to create OpenGL resource */
    InvalidArgument, /*!< argument is invalid */
    InvalidFontFile, /*!< a file which contains font glyphs is not found or corrupted */
    AbsentFontGlyph /*!< a glyph is not supported by the font */
};

#ifndef DRAW_NO_EXCEPTIONS

//! An error exception.
/*! Encapsulates error code. Only available if DRAW_NO_EXCEPTIONS is set. */
class Error : public std::exception {

public:
    virtual ~Error() = 0;

    //! Returns error code.
    /*!
      \return error code
    */
    virtual ErrorCode code() const = 0;
};

#else
//! Returns last error.
/*!
  \return error code
*/
ErrorCode getLastError();
#endif

//! Point in pixel space.
struct Point {

    int32_t x {0};
    int32_t y {0};

    //! Constructs from zero values.
    Point() = default;

    //! Constructs from parameters.
    Point(int32_t x, int32_t y) :
        x(x), y(y) {};
};

//! Checks if two points are equal.
bool operator == (const Point& left, const Point& right);

//! Checks if two points are not equal.
bool operator != (const Point& left, const Point& right);

//! Size in pixel space.
struct Size {

    int32_t width {1};
    int32_t height {1};

    //! Constructs from unit values.
    Size() = default;
    //! Constructs from parameters.
    Size(int32_t width, int32_t height) :
        width(width), height(height) {};
};

//! Checks if two sizes are equal.
bool operator == (const Size& left, const Size& right);

//! Checks if two sizes are not equal.
bool operator != (const Size& left, const Size& right);

//! Rect in pixel space.
struct Rect {

    int32_t left {0};
    int32_t bottom {0};
    int32_t right {0};
    int32_t top {0};

    //! Constructs from zero values.
    Rect() = default;
    //! Constructs from parameters.
    Rect(int32_t left, int32_t bottom, int32_t right, int32_t top) :
        left(left), bottom(bottom), right(right), top(top) {};
};

//! Checks if two rects are equal.
bool operator == (const Rect& left, const Rect& right);

//! Checks if two rects are not equal.
bool operator != (const Rect& left, const Rect& right);

//! Floating-point 2D-vector.
struct Vector2 {

    float x {0.0f};
    float y {0.0f};

    //! Constructs from zero values.
    Vector2() = default;
    //! Constructs from parameters.
    Vector2(float x, float y) :
        x(x), y(y) {};
};

//! RGBA floating-point color.
struct Color {

    float r {0.0f};
    float g {0.0f};
    float b {0.0f};
    float a {1.0f};

    //! Constructs from zero values.
    Color() = default;
    //! Constructs from parameters.
    Color(float r, float g, float b, float a = 1.0f) :
        r(r), g(g), b(b), a(a) {};
};

//! Continuous data representation.
template <typename T>
struct Span {

    //! Data pointer.
    const T* data;
    //! Elements count.
    uint32_t count;

    //! Constructs from parameters.
    Span(const T* data, uint32_t count) :
        data(data), count(count) {}
};

//! Geometry vertex.
/*!
 * Represents an indexed 2D-geometry.
 * You should define your vertices in [0.0, 1.0] space as if it have fit to one pixel.
 * Then you can set appropriate size via Shape object.
 */
class Geometry {

public:
    //! Geometry vertex.
    struct Vertex {

        //! 2D-position in [0.0, 1.0] space.
        Vector2 position;
        //! UV-coordinates in [0.0, 1.0] space.
        Vector2 uv;

        //! Constructs from zero values.
        Vertex() = default;
        //! Constructs from parameters.
        Vertex(const Vector2& position, const Vector2& uv) :
            position(position), uv(uv) {};
    };
    //! Continuous array of vertices.
    using Vertices = Span<Vertex>;

    //! Index of geometry vertex.
    using Index = uint16_t;

    //! Continuous array of indices.
    using Indices = Span<Index>;

    //! A type of primitive.
    enum class Primitive {

        Point = 1, /*!< consumes 1 index per primitive */
        Line, /*!< consumes 2 index per primitive */
        Triangle /*!< consumes 3 index per primitive */
    };
    virtual ~Geometry() = 0;

    //! Returns vertex count.
    /*!
      \return vertex count
    */
    virtual uint32_t vertexCount() const = 0;

    //! Returns index count.
    /*!
      \return index count
    */
    virtual uint32_t indexCount() const = 0;

    //! Returns primitive type.
    /*!
      \return primitive type
    */
    virtual Primitive primitive() const = 0;
};

//! Geometry shared-pointer type.
using GeometryPtr = std::shared_ptr<Geometry>;

//! Image.
/*!
 * Represents an image.
 */
class Image {

public:
    static const int32_t kMaxSize = 4096;

    //! Continuous array of bytes.
    using Bytes = Span<uint8_t>;

    enum class Format {

        A,
        RGB,
        RGBA
    };
    virtual ~Image() = 0;

    virtual const Size& size() const = 0;
    virtual Format format() const = 0;
    virtual bool filter() const = 0;

    virtual void upload(Bytes data) = 0;
};

//! Image shared-pointer type.
using ImagePtr = std::shared_ptr<Image>;

class Visual {

public:
    virtual ~Visual() = 0;

    virtual void visibility(bool enable) = 0;
    virtual bool visibility() const = 0;

    virtual void order(uint32_t order) = 0;
    virtual uint32_t order() const = 0;

    virtual void position(const Point& position) = 0;
    virtual const Point& position() const = 0;

    virtual const Rect& bounds() const = 0;
};

//! Visual shared-pointer type.
using VisualPtr = std::shared_ptr<Visual>;

class Shape : public Visual {

public:
    virtual ~Shape() = 0;

    virtual void size(const Size& size) = 0;
    virtual const Size& size() const = 0;

    virtual void color(const Color& color) = 0;
    virtual const Color& color() const = 0;

    virtual void transparency(bool value) = 0;
    virtual bool transparency() const = 0;

    virtual void geometry(GeometryPtr geometry) = 0;
    virtual GeometryPtr geometry() const = 0;

    virtual void image(ImagePtr image) = 0;
    virtual void image(ImagePtr image, const Vector2& tile) = 0;
    virtual ImagePtr image() const = 0;

    virtual void image(ImagePtr atlas, const Rect& element) = 0;
    virtual ImagePtr image(Rect& element) const = 0;
};

//! Shape shared-pointer type.
using ShapePtr = std::shared_ptr<Shape>;

class Font {

public:
    virtual ~Font() = 0;

    virtual const char* filePath() const = 0;
    virtual uint32_t letterSize() const = 0;
};

//! Font shared-pointer type.
using FontPtr = std::shared_ptr<Font>;

class Text : public Visual {

public:
    virtual ~Text() = 0;

    virtual void font(FontPtr font) = 0;
    virtual FontPtr font() const = 0;

    virtual void text(const wchar_t* text) = 0;
    virtual const wchar_t* text() const = 0;

    virtual void color(const Color& color) = 0;
    virtual const Color& color() const = 0;

    enum class HorizAlign {

        Left,
        Center,
        Right
    };

    virtual void horizAlign(HorizAlign alignment) = 0;
    virtual HorizAlign horizAlign() const = 0;

    enum class VertAlign {

        Top,
        Middle,
        Bottom
    };

    virtual void vertAlign(VertAlign alignment) = 0;
    virtual VertAlign vertAlign() const = 0;
};

//! Text shared-pointer type.
using TextPtr = std::shared_ptr<Text>;

class Renderer {

public:
    virtual ~Renderer() = 0;

    virtual GeometryPtr makeGeometry(Geometry::Vertices vertices,
        Geometry::Indices indices, Geometry::Primitive primitive) = 0;
    virtual ImagePtr makeImage(const Size& size, Image::Format format, bool filter) = 0;
    virtual FontPtr makeFont(const char* filePath, uint32_t letterSize) = 0;

    virtual ShapePtr makeRect() = 0;
    virtual ShapePtr makeShape() = 0;
    virtual TextPtr makeText() = 0;

    virtual uint32_t draw(const Size& screen, const Color& clear) = 0;
};

using RendererPtr = std::shared_ptr<Renderer>;

class Context {

public:
    virtual ~Context() = 0;
    virtual void setCurrent() = 0;
};

//! Context pointer type.
using ContextPtr = std::unique_ptr<Context>;

RendererPtr makeRenderer(ContextPtr context);

} // namespace draw
