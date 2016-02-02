rem draw build on Win32 (MinGW, MSVC 12)

mkdir build
cd build

mkdir mingw
mkdir mingw\release
mkdir mingw\debug
cd mingw\release
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..\..\..\
cmake --build ./ --config Release
cd ..\debug
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..\..\..\
cmake --build ./ --config Debug
cd ..\..\

mkdir msvc
cd msvc
cmake -G "Visual Studio 12" ..\..\
cmake --build .\ --config Release
cmake --build .\ --config Debug

cd ..\..
mkdir lib
mkdir lib\mingw_lib
mkdir lib\msvc_lib
copy "build\mingw\debug\*.a" "lib\mingw_lib"
copy "build\mingw\release\*.a" "lib\mingw_lib"
copy "build\msvc\Debug\*.lib" "lib\msvc_lib"
copy "build\msvc\Release\*.lib" "lib\msvc_lib"

