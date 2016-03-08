#include "common.h"

using namespace details;

go_bandit([] {

    describe("draw::Geometry:", [] {

        RendererPtr renderer;

        before_each([&] {

            mockGL();
            renderer = makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
        });

        it("should be created", [&] {

            auto ptr = renderer->makeGeometry(
                {kVertices, kVertexCount}, {kIndices, kIndexCount}, kPrimitive);

            AssertThat(ptr, Is().Not().EqualTo(GeometryPtr()));
            AssertThat(ptr->vertexCount(), Is().EqualTo(kVertexCount));
            AssertThat(ptr->indexCount(), Is().EqualTo(kIndexCount));
            AssertThat(ptr->primitive(), Is().EqualTo(kPrimitive));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));
        });

        it("should throw InvalidArgument if vertices.data is invalid", [&] {

            auto ptr = renderer->makeGeometry(
                {nullptr, kVertexCount}, {kIndices, kIndexCount}, kPrimitive);

            AssertThat(ptr, Is().EqualTo(GeometryPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("should throw InvalidArgument if vertices.count is zero", [&] {

            auto ptr = renderer->makeGeometry(
                {kVertices, 0}, {kIndices, kIndexCount}, kPrimitive);

            AssertThat(ptr, Is().EqualTo(GeometryPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("should throw InvalidArgument if vertices.count > Geometry::kMaxVertexCount", [&] {

            auto ptr = renderer->makeGeometry(
                {kVertices, Geometry::kMaxVertexCount + 1}, {kIndices, kIndexCount}, kPrimitive);

            AssertThat(ptr, Is().EqualTo(GeometryPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("should throw InvalidArgument if indices.data is invalid", [&] {

            auto ptr = renderer->makeGeometry(
                {kVertices, kVertexCount}, {nullptr, kIndexCount}, kPrimitive);

            AssertThat(ptr, Is().EqualTo(GeometryPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("should throw InvalidArgument if indices.count is zero", [&] {

            auto ptr = renderer->makeGeometry(
                {kVertices, kVertexCount}, {kIndices, 0}, kPrimitive);

            AssertThat(ptr, Is().EqualTo(GeometryPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("throw OpenGLOutOfMemory if is not enough memory to create OpenGL resources", [&] {

            Given(::glMocked(), gl_GetError()).WillByDefault(Return(GL_OUT_OF_MEMORY));

            auto ptr = renderer->makeGeometry(
                {kVertices, kVertexCount}, {kIndices, kIndexCount}, kPrimitive);

            AssertThat(ptr, Is().EqualTo(GeometryPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::OpenGLOutOfMemory));
        });
    });
});