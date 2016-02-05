#pragma once
#include <draw.h>
#include <GL/glew.h>

namespace draw {

class RendererImpl;

class GeometryImpl final : public Geometry {

public:
    GeometryImpl(RendererImpl& renderer, Geometry::Vertices vertices,
        Geometry::Indices indices, Geometry::Primitive primitive);
    virtual ~GeometryImpl();

    GeometryImpl(const GeometryImpl&) = delete;
    GeometryImpl& operator = (const GeometryImpl&) = delete;

    bool init();
    GLuint vb() { return vb_; }
    GLuint ib() { return ib_; }

    // Geometry

    virtual uint32_t vertexCount() const final { return vertices_.count; }
    virtual uint32_t indexCount() const final { return indices_.count; }
    virtual Primitive primitive() const final { return primitive_; }

private:
    RendererImpl& renderer_;
    Vertices vertices_;
    Indices indices_;
    Primitive primitive_ {Primitive::Triangle};
    GLuint vb_ {0};
    GLuint ib_ {0};
};

} // namespace draw
