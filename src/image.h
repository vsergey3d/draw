#pragma once
#include <draw.h>
#include <GL/glew.h>

namespace draw {

class RendererImpl;

class ImageImpl final : public Image {

public:
    ImageImpl(RendererImpl& renderer, const Size& size, Format format, bool filter);
    virtual ~ImageImpl();

    ImageImpl(const ImageImpl&) = delete;
    ImageImpl& operator = (const ImageImpl&) = delete;

    bool init();
    GLuint handle() { return handle_; }

    // Image

    virtual const Size& size() const final { return size_; }
    virtual Format format() const final { return format_; }
    virtual bool filter() const final { return filter_; }

    virtual bool upload(Bytes data) final;

private:
    RendererImpl& renderer_;
    Size size_ {0, 0};
    Format format_ {Format::RGB};
    bool filter_ {false};
    GLuint handle_ {0};
};

} // namespace draw
