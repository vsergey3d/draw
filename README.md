# draw&nbsp;&nbsp;[![Build Status](https://travis-ci.org/vsergey3d/draw.svg?branch=master)](https://travis-ci.org/vsergey3d/draw)&nbsp;[![codecov.io](https://codecov.io/github/vsergey3d/draw/coverage.svg?branch=master)](https://codecov.io/github/vsergey3d/draw?branch=master)

C++11 cross-platform GPU-accelerated pixel-space 2D drawing library.

[API documentation](http://vsergey3d.github.io/draw/) | 
[Contributing](https://github.com/vsergey3d/draw/blob/master/CONTRIBUTING.md/) | 
[FAQ](http://stackoverflow.com/questions/tagged/draw-cpp11-lib)

## Features

- cross-platform: Linux, MaxOS, Windows, MinGW
- very fast (using OpenGL instancing under the hood)
- integer pixel space (resize agnostic)
- indexed primitives, images, atlases, transparency, z-order
- text rendering (real-time font atlas generation from FreeType file)

## How to use

The project is using [CMake](http://www.cmake.org/) as build solution.<br/>
You can build the library using one of the modern compiler: Clang, GCC, MSVC.<br/>
Some dependencies ([GLEW](http://glew.sourceforge.net/), [AGG](http://www.antigrain.com/), [FreeType](http://www.freetype.org/)) are required. You can find it in `deps` folder and build by yourself.

See [draw-wxWidgets](https://github.com/vsergey3d/draw-wxWidgets) repo as example of using the library with [wxWidgets](https://www.wxwidgets.org/).

## Licensing

The library is licensed under the [Simplified BSD License](https://github.com/vsergey3d/draw/blob/master/LICENSE).

## Example

```cpp
using namespace draw;

class MyContext final : public Context {
public:
    /*...*/
    virtual void setCurrent() final { mySetCurrent(); }
};
auto renderer = makeRenderer(std::move(make_unique<MyContext>(/*...*/)));

auto image = renderer->makeImage({32, 32}, Image::Format::RGBA, true);
image->upload({dataPtr, dataSize});

auto rect = renderer->makeRect();
rect->position({10, 20});
rect->size({30, 30});
rect->color({1.0f, 0.5f, 0.5f});
rect->image(image);
rect->visibility(true);

auto text = renderer->makeText();
text->position({10, 60});
text->font(renderer->makeFont("cour.ttf", 10));
text->text(L"ABCDEFG");
text->visibility(true);

renderer->resize(myScreenSize);
renderer->draw({0.8f, 0.8f, 0.8f});
mySwapBuffers();
```
