/*------------------------------------------------------------------------
  draw: C++11 cross-platform GPU-accelerated pixel-space 2D drawing library
  Original location: https://github.com/vsergey3d/draw
  Distributed under the Simplified BSD License:
  https://github.com/vsergey3d/draw/blob/master/LICENSE
------------------------------------------------------------------------*/
#pragma once
#include <stdint.h>
#include <memory>

#ifndef ASSERT
#include <cassert>
#define ASSERT(e) assert(e)
#endif

#define DRAW_NO_EXCEPTIONS
#ifndef DRAW_NO_EXCEPTIONS
#include <exception>
#endif

#ifndef SHARED_PTR
#define SHARED_PTR std::shared_ptr
#define MAKE_SHARED_PTR std::make_shared
#endif

namespace draw {

//! An error code which library can generate.
enum ErrorCode {

    NoError = 0, /*!< there are no errors */
    InvalidArgument, /*!< argument is invalid */
    OpenGLAbsentFeature, /*!< some OpenGL features are not supported by your hardware */
    OpenGLOutOfMemory, /*!< memory is not enough to create OpenGL resource */
    InvalidFontFile, /*!< a file which contains font glyphs is not found or corrupted */
    IncompleteFontFile /*!< some alphabet's glyphs are not contained in the font file */
};

#ifndef DRAW_NO_EXCEPTIONS

//! An error exception.
class Error : public std::exception {

public:
    virtual ~Error() = default;

    //! return error code
    virtual ErrorCode code() const = 0;
};

#else
//! return last error and set the value to draw::NoError
ErrorCode getLastError();
#endif

//! A point in pixel space.
struct Point {

    int32_t x {0};
    int32_t y {0};

    //! construct from zero values
    Point() = default;

    //! constructs from parameters
    Point(int32_t x, int32_t y) :
        x(x), y(y) {};
};

//! checks if two points are equal
bool operator == (const Point& left, const Point& right);

//! checks if two points are not equal
bool operator != (const Point& left, const Point& right);

//! A size in pixel space.
struct Size {

    uint32_t width {1};
    uint32_t height {1};

    //! Constructs from unit values.
    Size() = default;
    //! Constructs from parameters.
    Size(uint32_t width, uint32_t height) :
        width(width), height(height) {};
};

//! checks if two sizes are equal
bool operator == (const Size& left, const Size& right);

//! checks if two sizes are not equal
bool operator != (const Size& left, const Size& right);

//! A rect in pixel space.
struct Rect {

    int32_t left {0};
    int32_t bottom {0};
    int32_t right {0};
    int32_t top {0};

    //! constructs from zero values
    Rect() = default;
    //! constructs from parameters
    Rect(int32_t left, int32_t bottom, int32_t right, int32_t top) :
        left(left), bottom(bottom), right(right), top(top) {};
};

//! checks if two rects are equal
bool operator == (const Rect& left, const Rect& right);

//! checks if two rects are not equal
bool operator != (const Rect& left, const Rect& right);

//! Floating-point 2D-vector.
struct Vector2 {

    float x {0.0f};
    float y {0.0f};

    //! construct from zero values
    Vector2() = default;
    //! construct from parameters
    Vector2(float x, float y) :
        x(x), y(y) {};
};

//! RGBA floating-point color
struct Color {

    //! red component
    float r {0.0f};
    //! green component
    float g {0.0f};
    //! blue component
    float b {0.0f};
    //! alpha component
    float a {1.0f};

    //! construct from zero values
    Color() = default;
    //! construct from parameters
    Color(float r, float g, float b, float a = 1.0f) :
        r(r), g(g), b(b), a(a) {};
};

//! Contiguous sequence of elements.
template <typename T>
struct Span {

    //! pointer to the first element
    const T* ptr;
    //! elements count
    uint32_t count;

    //! constructs from parameters
    Span(const T* ptr, uint32_t count) :
        ptr(ptr), count(count) {}
};

//! Set of indexed 2D-primitives.
/*!
  You should define your vertices in [0.0, 1.0] space as if it's fit to one pixel.
  Then you can set appropriate pixel size via Shape::size(const Size& size).
  To create an object of this type use Renderer::makeGeometry function.
 */
class Geometry {

public:
    //! maximum available vertex count
    static const uint32_t kMaxVertexCount = 65535;//std::numeric_limits<Index>::max();
    //! 2D-vertex.
    struct Vertex {
        //! 2D-position in [0.0, 1.0] space
        Vector2 position;
        //! UV-coordinates in [0.0, 1.0] space
        Vector2 uv;
        //! constructs from zero values
        Vertex() = default;
        //! constructs from parameters
        Vertex(const Vector2& position, const Vector2& uv) :
            position(position), uv(uv) {};
    };
    //! Sequence of vertices.
    using Vertices = Span<Vertex>;
    //! An index of a vertex.
    using Index = uint16_t;
    //! Sequence of indices.
    using Indices = Span<Index>;
    //! A type of primitive.
    enum class Primitive {

        Point = 1, /*!< consumes 1 index per primitive */
        Line, /*!< consumes 2 index per primitive */
        Triangle /*!< consumes 3 index per primitive */
    };
    virtual ~Geometry() = default;
    //! return vertex count
    virtual uint32_t vertexCount() const = 0;
    //! return index count
    virtual uint32_t indexCount() const = 0;
    //! return primitive type
    virtual Primitive primitive() const = 0;
};

using GeometryPtr = SHARED_PTR<Geometry>;

//! 2D-array of pixels specified format.
/*! To create an object of this type use Renderer::makeImage function. */
#ifdef DRAW_NO_EXCEPTIONS
/*!
  NOTE: exceptions are disabled. 'Exceptions' section lists expected errors
  which you can check via draw::getLastError.
*/
#endif
class Image {

public:
    //! maximum available size of image's width and height
    static const uint32_t kMaxSize = 4096;
    //! Sequence of bytes.
    using Bytes = Span<uint8_t>;
    //! A pixel format.
    enum class Format {

        A, /*!< 1 byte per pixel (alpha only) */
        RGB, /*!< 3 bytes per pixel (1-red, 1-green, 1-blue) */
        RGBA /*!< 4 bytes per pixel (1-red, 1-green, 1-blue, 1-alpha) */
    };
    virtual ~Image() = default;

    //! return size
    virtual const Size& size() const = 0;
    //! return format
    virtual Format format() const = 0;
    //! check if bilinear filtering is enabled or not
    virtual bool filter() const = 0;
    //! upload bytes to the image
    /*!
      \throw draw::InvalidArgument if bytes.data is invalid
      \throw draw::InvalidArgument if bytes.count != width * height * (bytes per pixel)
    */
    virtual void upload(Bytes bytes) = 0;
};

using ImagePtr = SHARED_PTR<Image>;

//! Base interface class of any visual object.
class Visual {

public:
    virtual ~Visual() = default;
    //! enable/disable visibility
    virtual void visibility(bool enable) = 0;
    //! check if visibility is enabled or not
    virtual bool visibility() const = 0;
    //! set order of drawing (first 0, then 1, ...)
    virtual void order(uint32_t order) = 0;
    //! return order of drawing
    virtual uint32_t order() const = 0;
    //! set position
    virtual void position(const Point& position) = 0;
    //! return position
    virtual const Point& position() const = 0;
    //! return bounding rect
    virtual const Rect& bounds() const = 0;
};

using VisualPtr = SHARED_PTR<Visual>;

//! A visual shape.
/*! To create an object of this type use Renderer::makeShape function. */
class Shape : public Visual {

public:
    virtual ~Shape() = default;
    //! set size
    virtual void size(const Size& size) = 0;
    //! return size
    virtual const Size& size() const = 0;
    //! set RGBA-color
    virtual void color(const Color& color) = 0;
    //! return RGBA-color
    virtual const Color& color() const = 0;
    //! enable/disable transparency (alpha = image.a * color.a)
    virtual void transparency(bool value) = 0;
    //! check if transparency is enabled or not
    virtual bool transparency() const = 0;
    //! set geometry
    virtual void geometry(const GeometryPtr& geometry) = 0;
    //! return geometry
    virtual GeometryPtr geometry() const = 0;
    //! set image
    virtual void image(const ImagePtr& image) = 0;
    //! set image with tile factor (repetition count)
    virtual void image(const ImagePtr& image, const Vector2& tile) = 0;
    //! return image
    virtual ImagePtr image() const = 0;
    //! set a rectangular element of image atlas
    virtual void image(const ImagePtr& atlas, const Rect& element) = 0;
    //! return image atlas and rectangular part of it
    virtual ImagePtr image(Rect& element) const = 0;
};

using ShapePtr = SHARED_PTR<Shape>;

//! Set of characters of the same style and size.
/*! To create an object of this type use Renderer::makeFont function. */
class Font {

public:
    //! minimum available letter size
    static const uint32_t kMinLetterSize = 5;

    virtual ~Font() = default;
    //! return file path
    virtual const char* filePath() const = 0;
    //! return letter size
    virtual uint32_t letterSize() const = 0;
};

using FontPtr = SHARED_PTR<Font>;

//! A text object.
/*! To create an object of this type use Renderer::makeText function. */
class Text : public Visual {

public:
    virtual ~Text() = default;
    //! set Font object
    virtual void font(const FontPtr& font) = 0;
    //! return Font object
    virtual FontPtr font() const = 0;
    //! set text string
    virtual void text(const wchar_t* text) = 0;
    //! return text string
    virtual const wchar_t* text() const = 0;
    //! set color
    virtual void color(const Color& color) = 0;
    //! return color
    virtual const Color& color() const = 0;
    //! Horizontal alignment.
    enum class HorizAlign {

        Left,
        Center,
        Right
    };
    //! set horizontal alignment
    virtual void horizAlign(HorizAlign alignment) = 0;
    //! return horizontal alignment
    virtual HorizAlign horizAlign() const = 0;
    //! Vertical alignment.
    enum class VertAlign {

        Top,
        Middle,
        Bottom
    };
    //! set vertical alignment
    virtual void vertAlign(VertAlign alignment) = 0;
    //! return vertical alignment
    virtual VertAlign vertAlign() const = 0;
};

using TextPtr = SHARED_PTR<Text>;

//! Factory and context owner.
/*! To create an object of this type use draw::makeRenderer function. */
#ifdef DRAW_NO_EXCEPTIONS
/*!
  NOTE: exceptions are disabled. 'Exceptions' section lists expected errors
  which you can obtain via draw::getLastError.
*/
#endif
class Renderer {

public:
    //! destruct renderer and owned context
    /*! Note: all created objects must be destroyed before the renderer object */
    virtual ~Renderer() = default;
    //! make Geometry object
    /*!
      \throw draw::InvalidArgument if vertices.data is invalid
      \throw draw::InvalidArgument if vertices.count is zero or > Geometry::kMaxVertexCount
      \throw draw::InvalidArgument if indices.data is invalid
      \throw draw::InvalidArgument if indices.count is zero
      \throw draw::OpenGLOutOfMemory if is not enough memory to create internal OpenGL resources
    */
    virtual GeometryPtr makeGeometry(Geometry::Vertices vertices,
        Geometry::Indices indices, Geometry::Primitive primitive) = 0;
    //! make Image object
    /*!
      \param size
      \param format
      \param filter use bilinear filtering or not
      \throw draw::InvalidArgument if size.width is zero or > Image::kMaxSize
      \throw draw::InvalidArgument if size.height is zero or > Image::kMaxSize
      \throw draw::OpenGLOutOfMemory if is not enough memory to create internal OpenGL resources
    */
    virtual ImagePtr makeImage(const Size& size, Image::Format format, bool filter) = 0;
    //! make Font object
    /*!
      \throw draw::InvalidArgument if filePath is invalid
      \throw draw::InvalidArgument if letterSize is less than Font::kMinLetterSize
      \throw draw::InvalidFontFile if font file is not exist or corrupted
      \throw draw::IncompleteFontFile if some alphabet's glyphs is not contained in the font file
      \throw draw::OpenGLOutOfMemory if is not enough memory to create internal OpenGL resources
    */
    virtual FontPtr makeFont(const char* filePath, uint32_t letterSize) = 0;
    //! make Shape object with rectangle Geometry
    /*!
      \throw draw::OpenGLOutOfMemory if is not enough memory to create internal OpenGL resources
    */
    virtual ShapePtr makeRect() = 0;
    //! make Shape object
    /*!
      \throw draw::OpenGLOutOfMemory if is not enough memory to create internal OpenGL resources
    */
    virtual ShapePtr makeShape() = 0;
    //! make Text object
    /*!
      \throw draw::OpenGLOutOfMemory if is not enough memory to create internal OpenGL resources
    */
    virtual TextPtr makeText() = 0;
    //! clear the screen and repaint all visible objects
    /*!
      \return drawn objects count
    */
    virtual uint32_t draw(const Color& clear) = 0;
    //! set a new screen size
    virtual void resize(const Size& size) = 0;
};

using RendererPtr = SHARED_PTR<Renderer>;

//! OpenGL rendering context base interface class.
/*!
  You should inherit from this interface class and implement Context::setCurrent()
  for your platform. It's recommended to move ownership of the context immediately to
  the renderer: makeRenderer(std::move(make_unique<ContextImpl>())).
 */
class Context {

public:
    virtual ~Context() = default;
    //! set context as current
    virtual void setCurrent() = 0;
};

using ContextPtr = std::unique_ptr<Context>;

//! make Renderer object
#ifdef DRAW_NO_EXCEPTIONS
/*!
  NOTE: exceptions are disabled. 'Exceptions' section lists expected errors
  which you can obtain via draw::getLastError.
*/
#endif
/*!
  \param context It's recommended to move ownership of the context immediately to
  the renderer: makeRenderer(std::move(make_unique<ContextImpl>())).
  \throw draw::InvalidArgument if context is invalid
  \throw draw::OpenGLAbsentFeature if OpenGL 2.0 or ARB_draw_instanced are not supported
  \throw draw::OpenGLOutOfMemory if is not enough memory to create internal OpenGL resources
*/
RendererPtr makeRenderer(ContextPtr context);

} // namespace draw
