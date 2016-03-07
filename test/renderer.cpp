#include "common.h"

go_bandit([]{

    describe("draw::Renderer:", []{

        before_each([&]{

            mockGL();
        });

        it("should be created", [&]{

            auto r = makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            AssertThat(r, Is().Not().EqualTo(draw::RendererPtr()));
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::NoError));
        });

        it("should throw draw::InvalidArgument if context is invalid", [&]{

            auto r = draw::makeRenderer(nullptr);
            AssertThat(r, Is().EqualTo(draw::RendererPtr()));
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::InvalidArgument));
        });

        it("throw draw::OpenGLAbsentFeature if OpenGL 2.0 is not supported", [&]{

            Given(::glMocked(), glew_Init()).WillByDefault(Return(GLEW_ERROR_NO_GL_VERSION));

            auto r = draw::makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            AssertThat(r, Is().EqualTo(draw::RendererPtr()));
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::OpenGLAbsentFeature));
        });

        it("throw draw::OpenGLAbsentFeature if ARB_draw_instanced is not supported", [&]{

            Given(::glMocked(), glew_IsSupported(_)).WillByDefault(Return(false));

            auto r = draw::makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            AssertThat(r, Is().EqualTo(draw::RendererPtr()));
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::OpenGLAbsentFeature));
        });

        it("throw draw::OpenGLOutOfMemory if is not enough memory to create internal OpenGL resources", [&]{

            Given(::glMocked(), gl_GetError()).WillByDefault(Return(GL_OUT_OF_MEMORY));

            auto r = draw::makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            AssertThat(r, Is().EqualTo(draw::RendererPtr()));
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::OpenGLOutOfMemory));
        });

        it("should resize", [&]{

            draw::Size size;

            auto r = draw::makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            r->resize(size);
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::NoError));
        });

        it("should draw no objects", [&]{

            draw::Color color;

            auto r = draw::makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            AssertThat(r->draw(color), Is().EqualTo(0));
            AssertThat(draw::getLastError(), Is().EqualTo(draw::ErrorCode::NoError));
        });
    });
});