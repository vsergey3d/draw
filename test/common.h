#pragma once
#include "gl_mock.h"
#include <draw.h>
#include <gmock/gmock.h>
#include <bandit/bandit.h>

#define Given(obj, action) ON_CALL(obj, action)
#define Verify(obj, action) EXPECT_CALL(obj, action)

using namespace bandit;

using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::_;

class ContextImpl final : public draw::Context {

public:
    ContextImpl() = default;
    virtual ~ContextImpl() = default;
    virtual void setCurrent() final {};
};

inline void mockGL() {

    Given(::glMocked(), glew_Init()).WillByDefault(Return(GLEW_OK));
    Given(::glMocked(), glew_IsSupported(_)).WillByDefault(Return(true));
    Given(::glMocked(), gl_GetError()).WillByDefault(Return(GL_NO_ERROR));
    Given(::glMocked(), gl_CreateShader(_)).WillByDefault(Return(1));
    Given(::glMocked(), gl_GetShaderiv(_, _, _)).WillByDefault(SetArgPointee<2>(GL_TRUE));
    Given(::glMocked(), gl_CreateProgram()).WillByDefault(Return(1));
    Given(::glMocked(), gl_GetProgramiv(_, _, _)).WillByDefault(SetArgPointee<2>(GL_TRUE));
    Given(::glMocked(), gl_GetAttribLocation(_, _)).WillByDefault(Return(1));
    Given(::glMocked(), gl_GetUniformLocation(_, _)).WillByDefault(Return(1));
}