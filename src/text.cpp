#include "text.h"
#include <font.h>
#include <renderer.h>
#include <algorithm>

namespace draw {

TextImpl::TextImpl(RendererImpl& renderer) :
    renderer_(renderer) {
}

void TextImpl::computeBounds() {

    if (shapeCount_ == 0)
        return;

    const auto& position = shapes_[0]->position();
    bounds_.left = position.x;
    bounds_.bottom = position.y;
    bounds_.right = position.x + textSize_.width;
    bounds_.top = position.y + textSize_.height;
}

void TextImpl::buildLetter(wchar_t c, Point &pos, Size &size, uint32_t &shapeNum) {

    const auto& letters = static_cast<FontImpl*>(font_.get())->letters();
    auto it = letters.find(c);
    if (it == letters.end())
        return;

    const auto& rect = it->second;
    size.width = rect.right - rect.left;
    size.height = rect.top - rect.bottom;

    if (c != kSpaceSymbol) {
        if (shapeNum >= shapeCount_)
            shapes_[shapeNum] = renderer_.makeRect(FillMode::Font);

        auto& shape = shapes_[shapeNum];
        shape->image(static_cast<FontImpl*>(font_.get())->atlas(), rect);
        shape->color(color_);
        shape->visibility(visibility_);
        shape->order(order_);
        shape->position(pos);
        shape->size(size);

        ++shapeNum;
    }
    pos.x += size.width;
}

inline uint32_t calcLettersCount(std::wstring& str) {

    return (uint32_t)std::count_if(str.begin(), str.end(), [] (wchar_t c) {
        return c != kSpaceSymbol;
    });
}

inline int32_t computeHorizAlign(Text::HorizAlign alignment, const Size &size) {

    switch (alignment) {
    case Text::HorizAlign::Left: return 0;
    case Text::HorizAlign::Right: return -(int32_t)(size.width);
    case Text::HorizAlign::Center: return -(int32_t)(size.width / 2);
    }
    return 0;
}

inline int32_t computeVertAlign(Text::VertAlign alignment, const Size &size) {

    switch (alignment) {
    case Text::VertAlign::Top: return 0;
    case Text::VertAlign::Bottom: return -(int32_t)(size.height);
    case Text::VertAlign::Middle: return -(int32_t)(size.height / 2);
    }
    return 0;
}

void TextImpl::build() {

    Point shapePos(position_);
    Size shapeSize(0, 0);

    textSize_.width = 0;
    textSize_.height = 0;

    auto lettersCount = calcLettersCount(text_);
    if (lettersCount > shapeCount_)
        shapes_.resize(lettersCount);

    auto i = 0u;
    for (auto& c : text_)
        buildLetter(c, shapePos, shapeSize, i);

    textSize_.width = shapePos.x - position_.x;
    textSize_.height = shapeSize.height;

    shapeCount_ = lettersCount;
    for (i = shapeCount_; i < shapes_.size(); ++i)
        shapes_[i]->visibility(false);

    alignOffset_.x = computeHorizAlign(horizAlign_, textSize_);
    alignOffset_.y = computeVertAlign(vertAlign_, textSize_);

    for (i = 0; i < shapeCount_; ++i) {
        auto& shape = shapes_[i];
        const auto& pos = shape->position();
        shape->position(Point(pos.x + alignOffset_.x, pos.y + alignOffset_.y));
    }
    computeBounds();
}

void TextImpl::align() {

    auto last = alignOffset_;

    alignOffset_.x = computeHorizAlign(horizAlign_, textSize_);
    alignOffset_.y = computeVertAlign(vertAlign_, textSize_);

    if (alignOffset_.x != last.x || alignOffset_.y != last.y) {

        for (size_t i = 0; i < shapeCount_; ++i) {
            auto& shape = shapes_[i];
            const auto& pos = shape->position();
            shape->position(Point(pos.x - last.x + alignOffset_.x, 
                pos.y - last.y + alignOffset_.y));
        }
        computeBounds();
    }
}

void TextImpl::font(FontPtr font) {

    if (font_ != font) {
        font_ = font;
        if (!text_.empty())
            build();
    }
}

void TextImpl::text(const wchar_t* text) {

    if (text_ != text) {
        if (text && wcslen(text) > 0)
            text_ = text;
        else
            text_.clear();
        build();
    }
}

void TextImpl::color(const Color& color) {

    if (color_ != color) {
        color_ = color;
        for (size_t i = 0; i < shapeCount_; ++i)
            shapes_[i]->color(color_);
    }
}

void TextImpl::horizAlign(Text::HorizAlign alignment) {

    if (horizAlign_ != alignment) {
        horizAlign_ = alignment;
        align();
    }
}

void TextImpl::vertAlign(Text::VertAlign alignment) {

    if (vertAlign_ != alignment) {
        vertAlign_ = alignment;
        align();
    }
}

void TextImpl::visibility(bool enable) {

    if (visibility_ != enable) {
        visibility_ = enable;
        for (size_t i = 0; i < shapeCount_; ++i)
            shapes_[i]->visibility(visibility_);
    }
}

void TextImpl::order(uint32_t order) {

    if (order_ != order) {
        order_ = order;
        for (size_t i = 0; i < shapeCount_; ++i)
            shapes_[i]->order(order_);
    }
}

void TextImpl::position(const Point& position) {

    Point dPos(position.x - position_.x, position.y - position_.y);
    position_ = position;

    for (size_t i = 0; i < shapeCount_; ++i) {
        auto& shape = shapes_[i];
        const auto&pos = shape->position();
        shape->position(Point(pos.x + dPos.x, pos.y + dPos.y));
    }
    computeBounds();
}

} // namespace draw
