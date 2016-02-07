#pragma once
#include <instance.h>

namespace draw {

class RendererImpl;

class ShapeImpl final : public Shape {

public:
    ShapeImpl(RendererImpl& renderer, const Key& key);
    virtual ~ShapeImpl();

    ShapeImpl(const ShapeImpl&) = delete;
    ShapeImpl& operator = (const ShapeImpl&) = delete;

    // Shape

    virtual void size(const Size& size) final;
    virtual const Size& size() const final { return size_; }

    virtual void color(const Color& color) final;
    virtual const Color& color() const final { return color_; }

    virtual void transparency(bool value) final;
    virtual bool transparency() const final { return key_.fillMode == FillMode::Transparent ; }

    virtual void geometry(GeometryPtr geometry) final;
    virtual GeometryPtr geometry() const final { return key_.geometry; }

    virtual void image(ImagePtr image) final;
    virtual void image(ImagePtr image, const Vector2& tile) final;
    virtual ImagePtr image() const final { return key_.image; }

    virtual void image(ImagePtr atlas, const Rect& element) final;
    virtual ImagePtr image(Rect& element) const final;

    // Visual

    virtual void visibility(bool enable) final;
    virtual bool visibility() const final { return visibility_; }

    virtual void order(uint32_t order) final;
    virtual uint32_t order() const final { return key_.order; }

    virtual void position(const Point& position) final;
    virtual const Point& position() const final { return position_; }

    virtual const Rect& bounds() const final { return bounds_; }

private:
    void addInstance();
    void removeInstance();

    void image(ImagePtr atlas, const Rect& element, const Vector2& tile);

    RendererImpl& renderer_;
    Instance* instance_ {nullptr};
    Key key_;

    bool visibility_ {false};
    Point position_ {0, 0};
    Size size_ {0, 0};
    Rect bounds_ {0, 0, 0, 0};
    Rect element_ {0, 0, 1, 1};
    Vector2 tile_ {1.0f, 1.0f};
    Color color_ {1.0f, 1.0f, 1.0f, 1.0f};
};

} // namespace draw
