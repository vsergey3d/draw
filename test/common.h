#pragma once
#include "gl_mock.h"
#include <draw.h>
#include <gmock/gmock.h>
#include <bandit/bandit.h>

#define Given(obj, action) ON_CALL(obj, action)
#define Verify(obj, action) EXPECT_CALL(obj, action)

namespace details {

using namespace draw;
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

static const auto kImageWidth = 2u, kImageHeight = 2u;
static const Size kImageSize {kImageWidth, kImageHeight};
static const auto kImageFormat = Image::Format::A;
static const auto kImageFilter = true;
static const uint8_t kImageData[kImageWidth * kImageHeight] = {0xAA, 0xAA, 0xAA, 0xAA};
static const auto kImageDataSize = sizeof(kImageData) / sizeof(kImageData[0]);

static const Geometry::Vertex kVertices[] = {
    {{0.0f, 0.0f}, {0.0f, 0.0f}}, {{1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f}, {1.0f, 1.0f}}, {{0.0f, 1.0f}, {0.0f, 1.0f}}
};
static const auto kVertexCount = sizeof(kVertices) / sizeof(kVertices[0]);
static const Geometry::Index kIndices[] = {0, 1, 2, 2, 3, 0};
static const auto kIndexCount = sizeof(kIndices) / sizeof(kIndices[0]);
static const auto kPrimitive = Geometry::Primitive::Triangle;

} // namespace details