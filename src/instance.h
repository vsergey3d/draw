#pragma once
#include <draw.h>

namespace draw {

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
    GeometryPtr geometry;
    ImagePtr image;

    Key(FillMode fillMode, uint32_t order, GeometryPtr geometry, ImagePtr image) :
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

} // namespace draw