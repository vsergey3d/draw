#include "shape.h"
#include <renderer.h>

namespace draw {

inline void uvFrame(ImagePtr atlas, const Rect& rect,
    const Vector2& tile, Vector4& frame) {

    if (!atlas) {
        frame.x = 0.0f;
        frame.y = 0.0f;
        frame.z = 1.0f;
        frame.w = 1.0f;
    }
    else {
        auto w = atlas->size().width, h = atlas->size().height;
        frame.x = float(rect.left) / w;
        frame.y = float(rect.bottom) / h;
        frame.z = float(rect.right - rect.left) / w * tile.x;
        frame.w = float(rect.top - rect.bottom) / h * tile.y;
    }
}

inline uint32_t packColor(const Color& color) {

    return uint32_t(255 * color.r) | (uint32_t(255 * color.g) << 8) |
        (uint32_t(255 * color.b) << 16) | (uint32_t(255 * color.a) << 24);
}

ShapeImpl::ShapeImpl(RendererImpl& renderer, FillMode fillMode) :
    renderer_(renderer),
    fillMode_(fillMode) {
}

ShapeImpl::~ShapeImpl() {

    removeInstance();
}

void ShapeImpl::addInstance() {

    instance_ = renderer_.add(Key(fillMode_, order_, geometry_.get(), image_.get()));

    auto& posFrame = instance_->posFrame;
    posFrame.x = (float)position_.x;
    posFrame.y = (float)position_.y;
    posFrame.z = (float)size_.width;
    posFrame.w = (float)size_.height;

    uvFrame(image_, element_, tile_, instance_->uvFrame);
    instance_->color = packColor(color_);
}

void ShapeImpl::removeInstance() {

    if (instance_) {
        renderer_.remove(Key(fillMode_, order_, geometry_.get(), image_.get()), instance_);
        instance_ = nullptr;
    }
}

void ShapeImpl::visibility(bool enable) {

    if (visibility_ != enable) {
        visibility_ = enable;
        if (visibility_)
            addInstance();
        else
            removeInstance();
    }
}

void ShapeImpl::order(uint32_t order) {

    if (order_ != order) {
        if (visibility_) removeInstance();
        order_ = order;
        if (visibility_) addInstance();
    }
}

void ShapeImpl::position(const Point& position) {

    position_ = position;

    bounds_.left = position_.x;
    bounds_.bottom = position_.y;
    bounds_.right = position_.x + size_.width;;
    bounds_.top = position_.y + size_.height;

    if (visibility_) {
        auto& v = instance_->posFrame;
        v.x = (float)position_.x;
        v.y = (float)position_.y;
    }
}

void ShapeImpl::size(const Size& size) {

    size_ = size;

    bounds_.right = position_.x + size_.width;;
    bounds_.top = position_.y + size_.height;

    if (visibility_) {
        auto& posFrame = instance_->posFrame;
        posFrame.z = (float)size_.width;
        posFrame.w = (float)size_.height;
    }
}

void ShapeImpl::color(const Color& color) {

    color_ = color;
    if (visibility_)
        instance_->color = packColor(color_);
}

void ShapeImpl::transparency(bool value) {

    bool current = (fillMode_ == FillMode::Transparent);
    if (current != value) {
        if (visibility_) removeInstance();
        fillMode_ = value ? FillMode::Transparent : FillMode::Solid;
        if (visibility_) addInstance();
    }
}

void ShapeImpl::geometry(GeometryPtr geometry) {

    if (geometry_ != geometry) {
        if (visibility_) removeInstance();
        geometry_ = geometry;
        if (visibility_) addInstance();
    }
}

void ShapeImpl::image(ImagePtr atlas, const Rect& element, const Vector2& tile) {

    element_ = element;
    tile_ = tile;

    if (image_ != atlas) {
        if (visibility_) removeInstance();
        image_ = atlas;
        if (visibility_) addInstance();
    }
    else if (visibility_) {
        uvFrame(image_, element_, tile_, instance_->uvFrame);
    }
}

static const Vector2 kNoTile(1.0f, 1.0f);

void ShapeImpl::image(ImagePtr atlas, const Rect& element) {

    image(atlas, element, kNoTile);
}

void ShapeImpl::image(ImagePtr image) {

    ShapeImpl::image(image, kNoTile);
}

void ShapeImpl::image(ImagePtr image, const Vector2& tile) {

    element_.left = 0;
    element_.bottom = 0;
    element_.right = image ? image->size().width : 1;
    element_.top = image ? image->size().height : 1;

    ShapeImpl::image(image, element_, tile);
}

ImagePtr ShapeImpl::image(Rect& element) const {

    element = element_;
    return image_;
}

} // namespace draw
