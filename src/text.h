#pragma once
#include <instance.h>
#include <string>
#include <vector>

namespace draw {

class RendererImpl;

class TextImpl final : public Text {

public:
    TextImpl(RendererImpl& renderer);
    virtual ~TextImpl() = default;

    TextImpl(const TextImpl&) = delete;
    TextImpl& operator = (const TextImpl&) = delete;

    // Text

    virtual void font(FontPtr font) final;
    virtual FontPtr font() const final { return font_; }

    virtual void text(const wchar_t* text) final;
    virtual const wchar_t* text() const final { return text_.c_str(); }

    virtual void color(const Color& color) final;
    virtual const Color& color() const final { return color_; }

    virtual void horizAlign(HorizAlign alignment) final;
    virtual HorizAlign horizAlign() const final { return horizAlign_; }

    virtual void vertAlign(VertAlign alignment) final;
    virtual VertAlign vertAlign() const final { return vertAlign_; }

    // Visual

    virtual void visibility(bool enable) final;
    virtual bool visibility() const final { return visibility_; }

    virtual void order(uint32_t order) final;
    virtual uint32_t order() const final { return order_; }

    virtual void position(const Point& position) final;
    virtual const Point& position() const final { return position_; }

    virtual const Rect& bounds() const final { return bounds_; }

private:
    void computeBounds();
    void buildLetter(wchar_t c, Point &pos, Size &size, uint32_t &shapeNum);

    void build();
    void align();

    RendererImpl& renderer_;
    FontPtr font_;
    uint32_t order_;
    std::wstring text_;
    std::vector<ShapePtr> shapes_;
    size_t shapeCount_ {0};

    bool visibility_ {false};
    Point position_ {0, 0};
    Point alignOffset_ {0, 0};
    Size textSize_ {0, 0};
    Rect bounds_ {0, 0, 0, 0};
    Color color_ {1.0f, 1.0f, 1.0f, 1.0f};
    HorizAlign horizAlign_ {HorizAlign::Left};
    VertAlign vertAlign_ {VertAlign::Top};

};

} // namespace draw
