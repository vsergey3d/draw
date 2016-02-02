#pragma once
#include <draw.h>
#include <gl.h>

namespace draw {

class ImageImpl final : public Image {

public:
	ImageImpl(uint32_t width, uint32_t height, Format format, bool filter);
	virtual ~ImageImpl();

	ImageImpl(const ImageImpl&) = delete;
	ImageImpl& operator = (const ImageImpl&) = delete;

	bool init(Error::Code& errorCode);

	GLuint handle() { return glTexture_; }

	// Image

	virtual Format format() const final { return format_; }
	virtual uint32_t width() const final { return width_; }
	virtual uint32_t height() const final { return height_; }

	virtual void upload(const uint8_t* data, uint32_t byteSize) final;
	virtual bool upload(const uint8_t* data, uint32_t byteSize, Error::Code& errorCode) final;

private:
	uint32_t width_ {0};
	uint32_t height_ {0};
	Format format_ {Format::RGB};
	bool filter_ {false};

	GLuint glTexture_ {0};
	GLenum glFormat_ {0};
	GLenum glInternalFormat_ {0};
};

} // namespace draw
