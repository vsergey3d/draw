#include "common.h"

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

            auto i = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);
            
            AssertThat(i, Is().Not().EqualTo(ImagePtr()));
            AssertThat(i->size(), Is().EqualTo(kImageSize));
            AssertThat(i->format(), Is().EqualTo(kImageFormat));
            AssertThat(i->filter(), Is().EqualTo(kImageFilter));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));
        });
        
        it("should throw InvalidArgument if size.width is zero or > Image::kMaxSize", [&] {
            
            auto i = renderer->makeImage({Image::kMaxSize + 1, 1}, kImageFormat, kImageFilter);
            
            AssertThat(i, Is().EqualTo(ImagePtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });
        
        it("should throw InvalidArgument if size.height is zero or > Image::kMaxSize", [&] {

            auto i = renderer->makeImage({1, Image::kMaxSize + 1}, kImageFormat, kImageFilter);

            AssertThat(i, Is().EqualTo(ImagePtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });
        
        it("throw OpenGLOutOfMemory if is not enough memory to create OpenGL resources", [&] {

            Given(::glMocked(), gl_GetError()).WillByDefault(Return(GL_OUT_OF_MEMORY));

            auto i = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);

            AssertThat(i, Is().EqualTo(ImagePtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::OpenGLOutOfMemory));
        });

        it("upload: should upload bytes to the image", [&] {

            auto i = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);
            i->upload({kBytes, kByteSize});

            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));
        });

        it("upload: should throw InvalidArgument if bytes.data is invalid", [&] {

            auto i = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);
            i->upload({nullptr, kByteSize});

            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("upload: should throw InvalidArgument if bytes.count != width * height * (bpp)", [&] {

            auto i = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);
            i->upload({kBytes, kByteSize + 1});

            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });
    });
});