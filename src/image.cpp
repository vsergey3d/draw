#include "image.h"
#include <renderer.h>
#include <error.h>

namespace draw {

inline GLenum glFormat(Image::Format format) {

    switch (format) {
    case Image::Format::A: return GL_ALPHA;
    case Image::Format::RGB: return GL_RGB;
    case Image::Format::RGBA: return GL_RGBA;
    }
    return 0;
}

inline GLenum glInternalFormat(Image::Format format) {

    switch (format) {
    case Image::Format::A: return GL_ALPHA8;
    case Image::Format::RGB: return GL_RGB8;
    case Image::Format::RGBA: return GL_RGBA8;
    }
    return 0;
}

inline int32_t bpp(Image::Format format) {

    switch (format) {
    case Image::Format::A: return 1;
    case Image::Format::RGB: return 3;
    case Image::Format::RGBA: return 4;
    }
    return 0;
}

ImageImpl::ImageImpl(RendererImpl& renderer, const Size& size,
    Image::Format format, bool filter) :
    renderer_(renderer),
    size_(size),
    format_(format),
    filter_(filter) {
}

ImageImpl::~ImageImpl() {

    renderer_.setContext();

    glDeleteTextures(1, &handle_);
}

bool ImageImpl::init() {

    if (size_.width <= 0 || size_.width > Image::kMaxSize ||
        size_.height <= 0 || size_.height > Image::kMaxSize) {
        setError(InvalidArgument);
        return false;
    }
    renderer_.setContext();

    glGenTextures(1, &handle_);
    glBindTexture(GL_TEXTURE_2D, handle_);

    auto glFilter = filter_ ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat(format_), size_.width,
        size_.height, 0, glFormat(format_), GL_UNSIGNED_BYTE, nullptr);

    if (glGetError() == GL_OUT_OF_MEMORY) {
        setError(OpenGLOutOfMemory);
        return false;
    }
    ASSERT(glGetError() == GL_NO_ERROR);
    return true;
}

void ImageImpl::upload(Image::Bytes data) {

    if (!data.data ||
        data.count != uint32_t(size_.width * size_.height * bpp(format_))) {
        setError(InvalidArgument);
        return;
    }
    renderer_.setContext();

    glBindTexture(GL_TEXTURE_2D, handle_);
    glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat(format_), size_.width,
        size_.height, 0, glFormat(format_), GL_UNSIGNED_BYTE, data.data);

    ASSERT(glGetError() == GL_NO_ERROR);
}

} // namespace draw
