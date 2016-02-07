rem freetype2 build on Win32 (MinGW, MSVC 14)

ren CMakeLists.txt CMakeLists_copy.txt
echo set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_SOURCE_DIR}/../c_flag_overrides.cmake) > CMakeLists.txt
echo set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/../cxx_flag_overrides.cmake) >> CMakeLists.txt
type CMakeLists_copy.txt >> CMakeLists.txt

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

del CMakeLists.txt
ren CMakeLists_copy.txt CMakeLists.txt

mkdir lib
mkdir lib\mingw_lib
mkdir lib\msvc_lib
copy "build\mingw\debug\libfreetyped.a" "lib\mingw_lib"
copy "build\mingw\release\libfreetype.a" "lib\mingw_lib"
copy "build\msvc\Debug\freetyped.lib" "lib\msvc_lib"
copy "build\msvc\Release\freetype.lib" "lib\msvc_lib"
