#include <draw.h>

#include <gmock/gmock.h>
#include "glMock.h"
#include <bandit/bandit.h>
using namespace bandit;

#define Given(obj, action) ON_CALL(obj, action)
#define Verify(obj, action) EXPECT_CALL(obj, action)

go_bandit([](){

    describe("point:", [](){

        draw::Point point;
        draw::Point point1 {2, 3};
        draw::Point point2 {2, 3};

        it("should be zero by default", [&](){

            AssertThat(point.x, Equals(0));
            AssertThat(point.y, Equals(0));
        });

        describe("equal op", [&](){

            it("should return true if points are equal", [&](){

                AssertThat(point1, Is().EqualTo(point2));
            });
            it("should return false if points are not equal", [&](){

                AssertThat(point1, Is().Not().EqualTo(point));
            });
        });
    });

    describe("renderer:", [](){

        class ContextImpl : public draw::Context {
        public:
            ContextImpl() = default;
            virtual ~ContextImpl() = default;
            virtual void setCurrent() override {};
        };

        it("should create renderer", [&](){

            using ::testing::Return;
            using ::testing::SetArgPointee;
            using ::testing::_;

            Given(::glMocked(), glew_Init()).WillByDefault(Return(GLEW_OK));
            Verify(::glMocked(), glew_Init()).Times(1);
            Given(::glMocked(), glew_IsSupported(_)).WillByDefault(Return(true));
            Given(::glMocked(), gl_GetError()).WillByDefault(Return(GL_NO_ERROR));
            Given(::glMocked(), gl_CreateShader(_)).WillByDefault(Return(1));
            Given(::glMocked(), gl_GetShaderiv(_, _, _)).WillByDefault(SetArgPointee<2>(GL_TRUE));
            Given(::glMocked(), gl_CreateProgram()).WillByDefault(Return(1));
            Given(::glMocked(), gl_GetProgramiv(_, _, _)).WillByDefault(SetArgPointee<2>(GL_TRUE));
            Given(::glMocked(), gl_GetAttribLocation(_, _)).WillByDefault(Return(1));
            Given(::glMocked(), gl_GetUniformLocation(_, _)).WillByDefault(Return(1));

            auto r = draw::makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
            AssertThat(r, Is().Not().EqualTo(draw::RendererPtr()));
        });
    });

});

int main(int argc, char* argv[]) {

    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);

    return bandit::run(argc, argv);
}