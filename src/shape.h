#pragma once
#include <draw.h>

namespace draw {

struct Instance;

class ShapeImpl final : public Shape {

public:
	ShapeImpl(BatcherPtr batcher);
	virtual ~ShapeImpl();

	ShapeImpl(const ShapeImpl&) = delete;
	ShapeImpl& operator = (const ShapeImpl&) = delete;

	// Shape

	virtual void size(const Size& size) final;
	virtual const Size& size() const final { return size_; }

	virtual void color(const Color& color) final;
	virtual const Color& color() const final { return color_; }

	virtual void image(ImagePtr image) final;
	virtual void image(ImagePtr image, const Vector2& tile) final;
	virtual ImagePtr image() const final { return image_; }

	virtual void image(ImagePtr atlas, const Rect& element) final;
	virtual ImagePtr image(Rect& element) const final;

	// Visual

	virtual void visibility(bool enable) final;
	virtual bool visibility() const final { return visibility_; }

	virtual void order(uint32_t order) final;
	virtual uint32_t order() const final { return order_; }

	virtual void position(const Point& position) final;
	virtual const Point& position() const final { return position_; }

	virtual const Rect& bounds() const final { return bounds_; }

private:
	void addInstance();
	void removeInstance();

	void image(ImagePtr atlas, const Rect& element, const Vector2& tile);

	BatcherPtr batcher_ {nullptr};
	Instance* instance_ {nullptr};

	bool visibility_ {false};
	uint32_t order_ {0};
	ImagePtr image_ {nullptr};
	Vector2 tile_ {1.0f, 1.0f};

	Point position_ {0, 0};
	Size size_ {0, 0};
	Rect bounds_ {0, 0, 0, 0};
	Rect element_ {0, 0, 1, 1};
	Color color_ {1.0f, 1.0f, 1.0f, 1.0f};
};

} // namespace draw
