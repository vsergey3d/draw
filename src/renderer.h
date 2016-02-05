#pragma once
#include <draw.h>
#include <instance.h>
#include <GL/glew.h>
#include <vector>
#include <map>

#if defined(_MSC_VER)
#pragma warning(disable: 4503)
#endif

namespace draw {

class Program;
using ProgramPtr = std::unique_ptr<Program>;

class RendererImpl final : public Renderer {

public:
    RendererImpl(ContextPtr context);
    virtual ~RendererImpl();

    RendererImpl(const RendererImpl&) = delete;
    RendererImpl& operator = (const RendererImpl&) = delete;

    bool init();
    void setContext() { context_->setCurrent(); }

    Instance* add(const Key& key);
    void remove(const Key& key, Instance* instance);

    ShapePtr makeRect(FillMode fillMode);

    // Renderer

    virtual GeometryPtr makeGeometry(Geometry::Vertices vertices,
        Geometry::Indices indices, Geometry::Primitive primitive) final;
    virtual ImagePtr makeImage(const Size& size, Image::Format format, bool filter) final;
    virtual FontPtr makeFont(const char* filePath, uint32_t letterSize) final;

    virtual ShapePtr makeRect(bool transparent) final;
    virtual ShapePtr makeShape(GeometryPtr geometry, bool transparent) final;
    virtual TextPtr makeText() final;

    virtual uint32_t draw(const Size& screen, const Color& clearColor) final;

private:
    ContextPtr context_;
    GeometryPtr rectGeom_;
    ImagePtr stubImage_;

    using InstancePtr = std::unique_ptr<Instance>;
    using Batch = std::vector<InstancePtr>;
    std::map<Key, Batch> batches_;
    uint32_t bindBatch(Program* program, const Batch& batch);

    static const uint32_t kDataInitCapacity {1000};
    static const uint32_t kDataGrowthFactor {2};
    std::vector<Instance> dataBuffer_;
    GLuint glBuffer_ {0};
    void resizeDataBuffer(uint32_t size);

    ProgramPtr geomProgram_;
    ProgramPtr fontProgram_;
    Program* getProgram(FillMode fillMode);
};

} // namespace draw

#if defined(_MSC_VER)
#pragma warning(disable: 4503)
#endif