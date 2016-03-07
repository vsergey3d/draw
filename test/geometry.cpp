#include "common.h"

go_bandit([] {

    describe("draw::Geometry:", [] {

        using namespace draw;

        static const Geometry::Vertex kVertices[] = {
            {{0.0f, 0.0f}, {0.0f, 0.0f}},
            {{1.0f, 0.0f}, {1.0f, 0.0f}},
            {{1.0f, 1.0f}, {1.0f, 1.0f}},
            {{0.0f, 1.0f}, {0.0f, 1.0f}}
        };
        static const auto kVertexCount = sizeof(kVertices) / sizeof(Geometry::Vertex);

        static const Geometry::Index kIndices[] = {0, 1, 2, 2, 3, 0};
        static const auto kIndexCount = sizeof(kIndices) / sizeof(Geometry::Index);

        static const auto kPrimitive = Geometry::Primitive::Triangle;

        RendererPtr renderer;

        before_each([&] {

            mockGL();
            renderer = makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
        });

        it("should be created", [&] {

            auto g = renderer->makeGeometry(
                {kVertices, kVertexCount}, {kIndices, kIndexCount}, kPrimitive);

            AssertThat(g, Is().Not().EqualTo(GeometryPtr()));
            AssertThat(g->vertexCount(), Is().EqualTo(kVertexCount));
            AssertThat(g->indexCount(), Is().EqualTo(kIndexCount));
            AssertThat(g->primitive(), Is().EqualTo(kPrimitive));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));
        });

        it("should throw InvalidArgument if vertices.data is invalid", [&] {

            auto g = renderer->makeGeometry(
                {nullptr, kVertexCount}, {kIndices, kIndexCount}, kPrimitive);

            AssertThat(g, Is().EqualTo(GeometryPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("should throw InvalidArgument if vertices.count is zero", [&] {

            auto g = renderer->makeGeometry(
                {kVertices, 0}, {kIndices, kIndexCount}, kPrimitive);

            AssertThat(g, Is().EqualTo(GeometryPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("should throw InvalidArgument if vertices.count > Geometry::kMaxVertexCount", [&] {

            auto g = renderer->makeGeometry(
                {kVertices, Geometry::kMaxVertexCount + 1}, {kIndices, kIndexCount}, kPrimitive);

            AssertThat(g, Is().EqualTo(GeometryPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("should throw InvalidArgument if indices.data is invalid", [&] {

            auto g = renderer->makeGeometry(
                {kVertices, kVertexCount}, {nullptr, kIndexCount}, kPrimitive);

            AssertThat(g, Is().EqualTo(GeometryPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("should throw InvalidArgument if indices.count is zero", [&] {

            auto g = renderer->makeGeometry(
                {kVertices, kVertexCount}, {kIndices, 0}, kPrimitive);

            AssertThat(g, Is().EqualTo(GeometryPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("throw OpenGLOutOfMemory if is not enough memory to create OpenGL resources", [&] {

            Given(::glMocked(), gl_GetError()).WillByDefault(Return(GL_OUT_OF_MEMORY));

            auto g = renderer->makeGeometry(
                {kVertices, kVertexCount}, {kIndices, kIndexCount}, kPrimitive);

            AssertThat(g, Is().EqualTo(GeometryPtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::OpenGLOutOfMemory));
        });
    });
});