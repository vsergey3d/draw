#include "renderer.h"
#include <error.h>
#include <geometry.h>
#include <image.h>
#include <font.h>
#include <shape.h>
#include <text.h>
#include <algorithm>
#include <cstring>

namespace draw {

inline GLuint getAttribLocation(GLuint program, const GLchar* name) {

    auto location = glGetAttribLocation(program, name);
    ASSERT(location >= 0 && "missing shader attribute");
    return (GLuint)location;
}

inline GLuint getUniformLocation(GLuint program, const GLchar* name) {

    auto location = glGetUniformLocation(program, name);
    ASSERT(location >= 0 && "missing shader uniform");
    return (GLuint)location;
}

inline bool checkShader(GLuint shader) {

    GLint param = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &param);
    return (param == GL_TRUE);
}

inline bool checkProgram(GLuint program) {

    GLint param = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &param);
    return (param == GL_TRUE);
}

class Program {

public:
    struct Attributes {

        GLuint pos {0};
        GLuint uv {0};
        GLuint posFrame {0};
        GLuint uvFrame {0};
        GLuint color {0};
    };

    struct Uniforms {

        GLuint image {0};
        GLuint screenFrame {0};
    };

    Program(RendererImpl& renderer, const char* vs, const char* fs);
    ~Program();

    Program(const Program&) = delete;
    Program& operator = (const Program&) = delete;

    GLuint handle() { return handle_; }
    const Attributes& attributes() const { return attributes_; }
    const Uniforms& uniforms() const { return uniforms_; }

private:
    RendererImpl& renderer_;
    Attributes attributes_;
    Uniforms uniforms_;
    GLuint vs_ {0};
    GLuint fs_ {0};
    GLuint handle_ {0};
};

Program::Program(RendererImpl& renderer, const char* vs, const char* fs):
    renderer_(renderer) {

    renderer_.setContext();

    GLint length = strlen(vs);
    vs_ = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs_, 1, &vs, &length);
    glCompileShader(vs_);
    ASSERT(checkShader(vs_));

    length = strlen(fs);
    fs_ = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs_, 1, &fs, &length);
    glCompileShader(fs_);
    ASSERT(checkShader(fs_));

    handle_ = glCreateProgram();
    glAttachShader(handle_, vs_);
    glAttachShader(handle_, fs_);
    glLinkProgram(handle_);
    ASSERT(checkProgram(handle_));

    attributes_.pos = getAttribLocation(handle_, "pos");
    attributes_.uv = getAttribLocation(handle_, "uv");
    attributes_.posFrame = getAttribLocation(handle_, "posFrame");
    attributes_.uvFrame = getAttribLocation(handle_, "uvFrame");
    attributes_.color = getAttribLocation(handle_, "color");
    uniforms_.screenFrame = getUniformLocation(handle_, "screenFrame");
    uniforms_.image = getUniformLocation(handle_, "image");

    ASSERT(glGetError() == GL_NO_ERROR);
}

Program::~Program() {

    renderer_.setContext();

    glDeleteShader(vs_);
    glDeleteShader(fs_);
    glDeleteProgram(handle_);
}

inline uint32_t typeByteSize(GLenum type) {

    switch (type) {
    case GL_UNSIGNED_BYTE: return 1;
    case GL_FLOAT: return 4;
    }
    return 0;
}

namespace shaders {

static const char* kVS = R"(

    precision highp float;
    attribute vec2 pos;
    attribute vec2 uv;
    attribute vec4 posFrame;
    attribute vec4 uvFrame;
    attribute vec4 color;
    varying vec2 vUV;
    varying vec4 vColor;
    uniform vec2 screenFrame;

    void main() {
        gl_Position = vec4((pos * posFrame.zw + posFrame.xy) *
            screenFrame - vec2(1.0), 0.0, 1.0);
        vUV = uv * uvFrame.zw + uvFrame.xy;
        vColor = color;
    }
)";

static const char* kGeomFS = R"(

    precision highp float;
    uniform sampler2D image;
    varying vec2 vUV;
    varying vec4 vColor;

    void main() {
        gl_FragColor = vColor * texture2D(image, vUV);
    }
)";

static const char* kFontFS = R"(

    precision highp float;
    uniform sampler2D image;
    varying vec2 vUV;
    varying vec4 vColor;

    void main() {
        gl_FragColor = vec4(vColor.rgb, vColor.a * texture2D(image, vUV).a);
    }
)";

} // namespace shaders

RendererImpl::RendererImpl(ContextPtr context) :
    context_(std::move(context)) {
}

RendererImpl::~RendererImpl() {

    ASSERT(batches_.empty() &&
        "all renderer's objects must be destroyed before the renderer itself");

    setContext();
    glDeleteBuffers(1, &glBuffer_);
}

bool RendererImpl::init() {

    if (!context_) {
        setError(InvalidArgument);
        return false;
    }
    setContext();
    if (glewInit() != GLEW_OK ||
        !glewIsSupported("GL_VERSION_2_0  GL_ARB_draw_instanced")) {
        setError(OpenGLAbsentFeature);
        return false;
    }
    glDisable(GL_DITHER);
    glDisable(GL_STENCIL_TEST);

    glGenBuffers(1, &glBuffer_);
    resizeDataBuffer(kDataInitCapacity);

    static const Geometry::Vertex kQuadVertices[] = {
        {{0.0f, 0.0f}, {0.0f, 0.0f}},
        {{1.0f, 0.0f}, {1.0f, 0.0f}},
        {{1.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.0f, 1.0f}, {0.0f, 1.0f}}
    };
    static const Geometry::Index kQuadIndices[] = {0, 1, 2, 2, 3, 0};
    rectGeometry_ = makeGeometry(
        {kQuadVertices, sizeof(kQuadVertices) / sizeof(Geometry::Vertex)},
        {kQuadIndices, sizeof(kQuadIndices) / sizeof(Geometry::Index)},
        Geometry::Primitive::Triangle);

    static const uint8_t kWhiteTexel[] = {0xFF, 0xFF, 0xFF, 0xFF};
    stubImage_ = makeImage({1, 1}, Image::Format::RGBA, false);
    stubImage_->upload({kWhiteTexel, sizeof(kWhiteTexel)});

    using namespace shaders;
    geometryProgram_ = make_unique<Program>(*this, kVS, kGeomFS);
    fontProgram_ = make_unique<Program>(*this, kVS, kFontFS);

    if (glGetError() == GL_OUT_OF_MEMORY) {
        setError(OpenGLOutOfMemory);
        return false;
    }
    ASSERT(glGetError() == GL_NO_ERROR);
    return true;
}

void RendererImpl::resizeDataBuffer(uint32_t size) {

    setContext();

    glBindBuffer(GL_ARRAY_BUFFER, glBuffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Instance) * size, nullptr, GL_DYNAMIC_DRAW);
    ASSERT(glGetError() == GL_NO_ERROR);

    if (glGetError() == GL_OUT_OF_MEMORY) {
        setError(OpenGLOutOfMemory);
        return;
    }
    dataBuffer_.resize(size);
}

Instance* RendererImpl::add(const Key& key) {

    auto& batch = batches_[key];
    batch.emplace_back(make_unique<Instance>());
    if (batch.size() > dataBuffer_.size())
        resizeDataBuffer(dataBuffer_.size() * kDataGrowthFactor);

    return batch.back().get();
}

void RendererImpl::remove(const Key& key, Instance* instance) {

    auto it = batches_.find(key);
    ASSERT(it != batches_.end() && "batch is not exist");

    auto& batch = it->second;
    auto itInst = std::find_if(std::begin(batch), std::end(batch),
        [&instance](const InstancePtr& e) { return instance == e.get(); });
    ASSERT(itInst != batch.end() && "instance is not exist");

    auto index = std::distance(std::begin(batch), itInst);
    batch[index] = std::move(batch[batch.size() - 1]);
    batch.resize(batch.size() - 1);

    if (batch.empty())
        batches_.erase(it);
}

Program* RendererImpl::getProgram(FillMode fillMode) {

    switch (fillMode) {
    case FillMode::Solid:
    case FillMode::Transparent:
        return geometryProgram_.get();
    case FillMode::Font:
        return fontProgram_.get();
    }
    return nullptr;
}

inline void setupFillMode(FillMode fillMode) {

    switch (fillMode) {
    case FillMode::Solid:
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        break;
    case FillMode::Transparent:
    case FillMode::Font:
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    }
}

inline void bindProgram(Program* program, const Vector2& screenFrame) {

    glUseProgram(program->handle());
    glUniform2fv(program->uniforms().screenFrame, 1, &screenFrame.x);
}

inline void bindImage(Program* program, ImageImpl* image) {

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image->handle());
    glUniform1i(program->uniforms().image, 0);
}

inline void bindAttribute(GLuint location, GLenum type, bool normalized,
    uint32_t size, uint32_t stride, uint32_t& offset, bool perInstance = false) {

    glEnableVertexAttribArray(location);

    auto norm = GLboolean(normalized ? GL_TRUE : GL_FALSE);
    glVertexAttribPointer(location, size, type, norm, stride, (char*)0 + offset);
    offset += typeByteSize(type) * size;

    if (perInstance)
        glVertexAttribDivisor(location, 1);
}

inline void bindGeometry(Program* program, GeometryImpl* geometry) {

    glBindBuffer(GL_ARRAY_BUFFER, geometry->vb());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->ib());

    const auto& attributes = program->attributes();
    uint32_t offset = 0u, stride = sizeof(Geometry::Vertex);
    bindAttribute(attributes.pos, GL_FLOAT, false, 2, stride, offset);
    bindAttribute(attributes.uv, GL_FLOAT, false, 2, stride, offset);
}

inline GLuint glPrimitive(Geometry::Primitive primitive) {

    switch (primitive) {
    case Geometry::Primitive::Point:
        return GL_POINTS;
    case Geometry::Primitive::Line:
        return GL_LINES;
    case Geometry::Primitive::Triangle:
        return GL_TRIANGLES;
    }
    return 0;
}

inline void setupScreen(const Size& screen, const Color& clear) {

    glViewport(0, 0, screen.width, screen.height);

    glClearColor(clear.r, clear.g, clear.b, clear.a);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

uint32_t RendererImpl::draw(const Size& screen, const Color& clear) {

    setContext();
    setupScreen(screen, clear);

    GeometryImpl* lastGeometry = nullptr;
    ImageImpl* lastImage = nullptr;
    Program* lastProgram = nullptr;
    Vector2 frame(2.0f / std::max(1u, screen.width), 2.0f / std::max(1u, screen.height));
    auto total = 0u;

    for (const auto& pair : batches_) {
        const auto& key = pair.first;
        setupFillMode(key.fillMode);

        auto* program = getProgram(key.fillMode);
        if (lastProgram != program) {
            bindProgram(program, frame);
            lastProgram = program;
        }
        auto* image = static_cast<ImageImpl*>(key.image ? key.image : stubImage_.get());
        if (lastImage != image || lastProgram != program) {
            bindImage(program, image);
            lastImage = image;
        }
        auto* geometry = static_cast<GeometryImpl*>(key.geometry);
        if (geometry) {
            if (lastGeometry != geometry) {
                bindGeometry(program, geometry);
                lastGeometry = geometry;
            }
            auto count = bindBatch(program, pair.second);
            glDrawElementsInstanced(glPrimitive(geometry->primitive()),
                geometry->indexCount(), GL_UNSIGNED_SHORT, 0, count);
            total += count;
        }
    }
    ASSERT(glGetError() == GL_NO_ERROR);
    return total;
}

uint32_t RendererImpl::bindBatch(Program* program, const Batch& batch) {

    auto count = 0u;
    for (auto& instance : batch)
        dataBuffer_[count++] = *instance;

    glBindBuffer(GL_ARRAY_BUFFER, glBuffer_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Instance) * count, &dataBuffer_[0]);

    const auto& attributes = program->attributes();
    uint32_t offset = 0u, stride = sizeof(Instance);
    bindAttribute(attributes.posFrame, GL_FLOAT, false, 4, stride, offset, true);
    bindAttribute(attributes.uvFrame, GL_FLOAT, false, 4, stride, offset, true);
    bindAttribute(attributes.color, GL_UNSIGNED_BYTE, true, 4, stride, offset, true);

    return count;
}

GeometryPtr RendererImpl::makeGeometry(Geometry::Vertices vertices,
    Geometry::Indices indices, Geometry::Primitive primitive) {

    auto ptr = std::make_shared<GeometryImpl>(*this, vertices, indices, primitive);
    return ptr->init() ? ptr : GeometryPtr();
}

ImagePtr RendererImpl::makeImage(const Size& size, Image::Format format, bool filter) {

    auto ptr = std::make_shared<ImageImpl>(*this, size, format, filter);
    return ptr->init() ? ptr : ImagePtr();
}

FontPtr RendererImpl::makeFont(const char* filePath, uint32_t letterSize) {

    if (!filePath || strlen(filePath) <= 0) {
        setError(InvalidArgument);
        return FontPtr();
    }
    auto ptr = std::make_shared<FontImpl>(*this, filePath, letterSize);
    return ptr->init() ? ptr : FontPtr();
}

ShapePtr RendererImpl::makeFontRect() {

    auto ptr = std::make_shared<ShapeImpl>(*this, FillMode::Font);
    ptr->geometry(rectGeometry_);
    return ptr;
}

ShapePtr RendererImpl::makeRect() {

    auto ptr = std::make_shared<ShapeImpl>(*this);
    ptr->geometry(rectGeometry_);
    return ptr;
}

ShapePtr RendererImpl::makeShape() {

    return std::make_shared<ShapeImpl>(*this);
}

TextPtr RendererImpl::makeText() {

    return std::make_shared<TextImpl>(*this);
}

RendererPtr makeRenderer(ContextPtr context) {

    auto ptr = std::make_shared<RendererImpl>(std::move(context));
    return ptr->init() ? ptr : RendererPtr();
}

} // namespace draw