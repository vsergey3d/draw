#include "geometry.h"
#include <renderer.h>
#include <error.h>
#include <limits>

namespace draw {

GeometryImpl::GeometryImpl(RendererImpl& renderer, Geometry::Vertices vertices,
    Geometry::Indices indices, Geometry::Primitive primitive) :
    renderer_(renderer),
    vertices_(vertices),
    indices_(indices),
    primitive_(primitive) {
}

GeometryImpl::~GeometryImpl() {

    renderer_.setContext();

    glDeleteBuffers(1, &vb_);
    glDeleteBuffers(1, &ib_);
}

bool GeometryImpl::init() {

    if (!vertices_.data || vertices_.count <= 0 ||
        vertices_.count >= std::numeric_limits<Index>::max() ||
        !indices_.data || indices_.count <= 0) {
        setError(InvalidArgument);
        return false;
    }
    renderer_.setContext();

    glGenBuffers(1, &vb_);
    glBindBuffer(GL_ARRAY_BUFFER, vb_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_.count,
        vertices_.data, GL_STATIC_DRAW);

    glGenBuffers(1, &ib_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index) * indices_.count,
        indices_.data, GL_STATIC_DRAW);

    if (glGetError() == GL_OUT_OF_MEMORY) {
        setError(OpenGLOutOfMemory);
        return false;
    }
    ASSERT(glGetError() == GL_NO_ERROR);
    return true;
}

} // namespace draw
