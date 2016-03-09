#include "common.h"

using namespace details;

go_bandit([] {

    describe("draw::Shape:", [] {

        RendererPtr renderer;

        before_each([&] {

            mockGL();
            renderer = makeRenderer(std::unique_ptr<ContextImpl>(new ContextImpl()));
        });

        it("should be created", [&] {

            auto ptr = renderer->makeShape();

            AssertThat(ptr->visibility(), Is().EqualTo(false));
            AssertThat(ptr->position(), Is().EqualTo(Point(0, 0)));
            AssertThat(ptr->bounds(), Is().EqualTo(Rect(0, 0, 0, 0)));
            AssertThat(ptr->size(), Is().EqualTo(Size(0, 0)));
            AssertThat(ptr->color(), Is().EqualTo(0xFFFFFFFF));
            AssertThat(ptr->transparency(), Is().EqualTo(false));
            AssertThat(ptr->geometry(), Is().EqualTo(nullptr));
            AssertThat(ptr->image(), Is().EqualTo(nullptr));

            AssertThat(ptr, Is().Not().EqualTo(ShapePtr()));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));
        });

        it("should get/set properties", [&] {

            auto ptr = renderer->makeShape();

            auto visibility = true;
            ptr->visibility(visibility);
            AssertThat(ptr->visibility(), Is().EqualTo(visibility));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));

            Point position(12, 34);
            ptr->position(position);
            AssertThat(ptr->position(), Is().EqualTo(position));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));

            Size size(56, 78);
            ptr->size(size);
            AssertThat(ptr->size(), Is().EqualTo(size));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));
            AssertThat(ptr->bounds(), Is().EqualTo(Rect(position.x, position.y,
                position.x + size.width, position.y + size.height)));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));

            uint32_t color{0xAABBCCDD};
            ptr->color(color);
            AssertThat(ptr->color(), Is().EqualTo(color));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));

            auto transparency = true;
            ptr->transparency(transparency);
            AssertThat(ptr->transparency(), Is().EqualTo(transparency));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));

            auto geometry = renderer->makeGeometry(
                {kVertices, kVertexCount}, {kIndices, kIndexCount}, kPrimitive);
            ptr->geometry(geometry);
            AssertThat(ptr->geometry(), Is().EqualTo(geometry));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));

            auto image = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);
            ptr->image(image);
            AssertThat(ptr->image(), Is().EqualTo(image));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));

            Vector2 tile{1.2f, 3.4f}, tileResult;
            ptr->image(image, tile);
            AssertThat(ptr->image(tileResult), Is().EqualTo(image));
            AssertThat(tile.x, Is().EqualToWithDelta(tileResult.x, FLT_EPSILON));
            AssertThat(tile.y, Is().EqualToWithDelta(tileResult.y, FLT_EPSILON));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));

            Rect element{11, 22, 33, 44}, elementResult;
            ptr->image(image, element);
            AssertThat(ptr->image(elementResult), Is().EqualTo(image));
            AssertThat(elementResult, Is().EqualTo(element));
            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));
        });
/*
        it("upload: should upload bytes to the image", [&] {

            auto ptr = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);
            ptr->upload({kImageData, kImageDataSize});

            AssertThat(getLastError(), Is().EqualTo(ErrorCode::NoError));
        });

        it("upload: should throw InvalidArgument if bytes.data is invalid", [&] {

            auto ptr = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);
            ptr->upload({nullptr, kImageDataSize});

            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });

        it("upload: should throw InvalidArgument if bytes.count != width * height * (bpp)", [&] {

            auto ptr = renderer->makeImage(kImageSize, kImageFormat, kImageFilter);
            ptr->upload({kImageData, kImageDataSize + 1});

            AssertThat(getLastError(), Is().EqualTo(ErrorCode::InvalidArgument));
        });*/
    });
});