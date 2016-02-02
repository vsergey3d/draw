#include "image.h"
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

inline uint8_t bpp(Image::Format format) {

	switch (format) {
	case Image::Format::A: return 1;
	case Image::Format::RGB: return 3;
	case Image::Format::RGBA: return 4;
	}
	return 0;
}

ImageImpl::ImageImpl(uint32_t width, uint32_t height, Image::Format format, bool filter) :
	width_(width), height_(height), format_(format), filter_(filter),
	glTexture_(0), glFormat_(glFormat(format_)),
	glInternalFormat_(glInternalFormat(format)) {
}

ImageImpl::~ImageImpl() {

	gl::setCurrentContext();
	glDeleteTextures(1, &glTexture_);
}

bool ImageImpl::init(Error::Code& errorCode) {

	errorCode = Code::NoError;
	if (width_ <= 0 || width_ > Image::kMaxSize ||
		height_ <= 0 || height_ > Image::kMaxSize) {
		errorCode = Code::InvalidImageWidthHeight;
		return false;
	}
	gl::setCurrentContext();

	glGenTextures(1, &glTexture_);
	glBindTexture(GL_TEXTURE_2D, glTexture_);

	auto glFilter = filter_ ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat_, width_, height_, 0,
		glFormat_, GL_UNSIGNED_BYTE, nullptr);

	ASSERT(gl::checkError());
	return true;
}

bool ImageImpl::upload(const uint8_t* data, uint32_t byteSize, Error::Code& errorCode) {

	errorCode = Code::NoError;
	if (width_ * height_ * bpp(format_) != byteSize) {
		errorCode = Code::IncorrectImageDataSize;
		return false;
	}
	gl::setCurrentContext();

	glBindTexture(GL_TEXTURE_2D, glTexture_);
	glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat_, width_, height_, 0,
		glFormat_, GL_UNSIGNED_BYTE, data);

	ASSERT(gl::checkError());
	return true;
}

void ImageImpl::upload(const uint8_t* data, uint32_t byteSize) {

	auto errorCode = Code::NoError;
	if (!upload(data, byteSize, errorCode))
		throw error(errorCode);
}

ImagePtr makeImage(uint32_t width, uint32_t height, Image::Format format, bool filter,
	Error::Code& errorCode) {

	auto image = std::make_shared<ImageImpl>(width, height, format, filter);
	if (!image->init(errorCode)) {
		return ImagePtr();
	}
	return image;
}

ImagePtr makeImage(uint32_t width, uint32_t height, Image::Format format, bool filter) {

	auto errorCode = Code::NoError;
	auto image = makeImage(width, height, format, filter, errorCode);
	if (!image) {
		throw error(errorCode);
	}
	return image;
}

} // namespace draw
