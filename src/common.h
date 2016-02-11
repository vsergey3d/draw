#pragma once
#include <memory>

#if defined(__clang__)
#if __has_feature(cxx_noexcept)
#define HAS_NOEXCEPT
#endif
#elif !defined(__clang__) && \
    defined(__GXX_EXPERIMENTAL_CXX0X__) && __GNUC__ * 10 + __GNUC_MINOR__ >= 46 || \
    defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 190023026
#define HAS_NOEXCEPT
#endif
#ifdef HAS_NOEXCEPT
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

namespace draw {

class Geometry;
class Image;

struct Vector4 {

    float x {0.0f};
    float y {0.0f};
    float z {0.0f};
    float w {0.0f};

    Vector4() = default;
    Vector4(float x, float y, float z, float w) :
        x(x), y(y), z(z), w(w) {}
};

struct Instance {

    Vector4 posFrame;
    Vector4 uvFrame;
    uint32_t color {0xFFFFFFFF};
};

enum class FillMode {

    Solid = 0,
    Transparent,
    Font
};

struct Key {

    FillMode fillMode {FillMode::Solid};
    uint32_t order {0};
    Geometry* geometry;
    Image* image;

    Key(FillMode fillMode, uint32_t order, Geometry* geometry, Image* image) :
        fillMode(fillMode), order(order), geometry(geometry), image(image) {}

    bool operator < (const Key& other) const {

        if (fillMode < other.fillMode)
            return true;
        else if (other.fillMode < fillMode)
            return false;
        if (order < other.order)
            return true;
        else if (other.order < order)
            return false;
        if (geometry < other.geometry)
            return true;
        else if (other.geometry < geometry)
            return false;
        if (image < other.image)
            return true;
        else if (other.image < image)
            return false;
        return false;
    }
};

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace draw