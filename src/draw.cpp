#include "draw.h"

namespace draw {

bool operator == (const Point& left, const Point& right) {

    return left.x == right.x && left.y == right.y;
}

bool operator != (const Point& left, const Point& right) {

    return !(left == right);
}

bool operator == (const Size& left, const Size& right) {

    return left.width == right.width && left.height == right.height;
}

bool operator != (const Size& left, const Size& right) {

    return !(left == right);
}

bool operator == (const Rect& left, const Rect& right) {

    return left.left == right.left && left.bottom == right.bottom &&
        left.right == right.right && left.top == right.top;
}

bool operator != (const Rect& left, const Rect& right) {

    return !(left == right);
}

} // namespace draw