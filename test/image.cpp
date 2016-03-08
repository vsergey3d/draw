#include "common.h"

using namespace details;

go_bandit([] {

    describe("draw::Image:", [] {

        using namespace draw;

        static const auto kImageWidth = 2u, kImageHeight = 2u;
        static const Size kImageSize {kImageWidth, kImageHeight};
        static const auto kImageFormat = Image::Format::A;
        static const auto kImageFilter = true;

        static const uint8_t kBytes[kImageWidth * kImageHeight] = {
            0xAA, 0xAA, 0xAA, 0xAA
        };
        static const auto kByteSize = sizeof(kBytes) / sizeof(kBytes[0]);

        RendererPtr renderer;

        before_each([&] {

            mockGL();
            renderer = makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
        });

        it("should be created", [&] {

            auto ptr = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);
            
            AssertThat(ptr, Is().Not().EqualTo(ImagePtr()));
            AssertThat(ptr->size(), Is().EqualTo(kImageSize));
            AssertThat(ptr->format(), Is().EqualTo(kImageFormat));
            AssertThat(ptr->filter(), Is().EqualTo(kImageFilter));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));
        });
        
        it("should throw InvalidArgument if size.width is zero or > Image::kMaxSize", [&] {
            
            auto ptr = renderer->makeImage({Image::kMaxSize + 1, 1}, kImageFormat, kImageFilter);
            
            AssertThat(ptr, Is().EqualTo(ImagePtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });
        
        it("should throw InvalidArgument if size.height is zero or > Image::kMaxSize", [&] {

            auto ptr = renderer->makeImage({1, Image::kMaxSize + 1}, kImageFormat, kImageFilter);

            AssertThat(ptr, Is().EqualTo(ImagePtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });
        
        it("throw OpenGLOutOfMemory if is not enough memory to create OpenGL resources", [&] {

            Given(::glMocked(), gl_GetError()).WillByDefault(Return(GL_OUT_OF_MEMORY));

            auto ptr = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);

            AssertThat(ptr, Is().EqualTo(ImagePtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::OpenGLOutOfMemory));
        });

        it("upload: should upload bytes to the image", [&] {

            auto ptr = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);
            ptr->upload({kBytes, kByteSize});

            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));
        });

        it("upload: should throw InvalidArgument if bytes.data is invalid", [&] {

            auto ptr = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);
            ptr->upload({nullptr, kByteSize});

            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("upload: should throw InvalidArgument if bytes.count != width * height * (bpp)", [&] {

            auto ptr = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);
            ptr->upload({kBytes, kByteSize + 1});

            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });
    });
});