rem draw build on Win32 (MinGW, MSVC 12)

mkdir build
cd build

mkdir mingw
mkdir mingw\release
mkdir mingw\debug
cd mingw\release
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..\..\..\
cmake --build ./ --config Release --target draw
cd ..\debug
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..\..\..\
cmake --build ./ --config Debug --target draw
cd ..\..\

mkdir msvc
cd msvc
cmake -G "Visual Studio 12" ..\..\
cmake --build .\ --config Release --target draw
cmake --build .\ --config Debug --target draw

cd ..\..
mkdir lib
mkdir lib\mingw_lib
mkdir lib\msvc_lib
copy "build\mingw\debug\draw\libdrawd.a" "lib\mingw_lib"
copy "build\mingw\release\draw\libdraw.a" "lib\mingw_lib"
copy "build\msvc\draw\Debug\drawd.lib" "lib\msvc_lib"
copy "build\msvc\draw\Release\draw.lib" "lib\msvc_lib"

