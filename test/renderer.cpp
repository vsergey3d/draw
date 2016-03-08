#include "common.h"

using namespace details;

go_bandit([]{

    describe("draw::Renderer:", []{

        before_each([&]{

            mockGL();
        });

        it("should be created", [&]{

            auto ptr = makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            AssertThat(ptr, Is().Not().EqualTo(draw::RendererPtr()));
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::NoError));
        });

        it("should throw draw::InvalidArgument if context is invalid", [&]{

            auto ptr = draw::makeRenderer(nullptr);
            AssertThat(ptr, Is().EqualTo(draw::RendererPtr()));
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::InvalidArgument));
        });

        it("throw draw::OpenGLAbsentFeature if OpenGL 2.0 is not supported", [&]{

            Given(::glMocked(), glew_Init()).WillByDefault(Return(GLEW_ERROR_NO_GL_VERSION));

            auto ptr = draw::makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            AssertThat(ptr, Is().EqualTo(draw::RendererPtr()));
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::OpenGLAbsentFeature));
        });

        it("throw draw::OpenGLAbsentFeature if ARB_draw_instanced is not supported", [&]{

            Given(::glMocked(), glew_IsSupported(_)).WillByDefault(Return(false));

            auto ptr = draw::makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            AssertThat(ptr, Is().EqualTo(draw::RendererPtr()));
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::OpenGLAbsentFeature));
        });

        it("throw draw::OpenGLOutOfMemory if is not enough memory to create internal OpenGL resources", [&]{

            Given(::glMocked(), gl_GetError()).WillByDefault(Return(GL_OUT_OF_MEMORY));

            auto ptr = draw::makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            AssertThat(ptr, Is().EqualTo(draw::RendererPtr()));
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::OpenGLOutOfMemory));
        });

        it("should resize", [&]{

            draw::Size size;

            auto ptr = draw::makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            ptr->resize(size);
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::NoError));
        });

        it("should draw no objects", [&]{

            draw::Color color;

            auto ptr = draw::makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            AssertThat(ptr->draw(color), Is().EqualTo(0));
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::NoError));
        });
    });
});