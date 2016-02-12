# draw

C++11 cross-platform GPU-accelerated pixel-space 2D drawing library

## Features

- cross-platform: Linux, MaxOS, Windows, MinGW
- very fast (using OpenGL instancing under the hood)
- integer pixel space (resize agnostic)
- indexed primitives, images, atlases, transparency, z-order
- text rendering (real-time font atlas generation from TrueType file)

## How to use

The project is using [CMake](http://www.cmake.org/) as build solution.<br/>
You can build the library using one of the modern compiler: Clang, GCC, MSVC.<br/>
Some dependencies ([GLEW](http://glew.sourceforge.net/), [AGG](http://www.antigrain.com/), [FreeType](http://www.freetype.org/)) are required. You can find it in `deps` folder and build by yourself.

See [draw-wxWidgets](https://github.com/vsergey3d/draw-wxWidgets) repo as example of using the library with [wxWidgets](https://www.wxwidgets.org/).

## Licensing

The library is licensed under the [Simplified BSD License](https://github.com/vsergey3d/draw/LICENSE).
