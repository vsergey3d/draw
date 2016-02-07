rem glew build on Win32 (MinGW, MSVC 14)

ren build\cmake\CMakeLists.txt CMakeLists_copy.txt
echo set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_SOURCE_DIR}/../../../c_flag_overrides.cmake) > build\cmake\CMakeLists.txt
echo set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/../../../cxx_flag_overrides.cmake) >> build\cmake\CMakeLists.txt
type build\cmake\CMakeLists_copy.txt >> build\cmake\CMakeLists.txt

cd build\cmake
mkdir build
cd build

mkdir mingw
mkdir mingw\release
mkdir mingw\debug
cd mingw\release
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..\..\..\
cmake --build ./ --target glew_s --config Release
cd ..\debug
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..\..\..\
cmake --build ./ --target glew_s --config Debug
cd ..\..\

mkdir msvc
cd msvc
cmake -G "Visual Studio 12" ..\..\
cmake --build .\ --target glew_s --config Release
cmake --build .\ --target glew_s --config Debug 
cd ..\..\..\..\

del  build\cmake\CMakeLists.txt
ren  build\cmake\CMakeLists_copy.txt CMakeLists.txt

mkdir lib
mkdir lib\mingw_lib
mkdir lib\msvc_lib
copy "build\cmake\build\mingw\debug\lib\libglew32d.a" "lib\mingw_lib"
copy "build\cmake\build\mingw\release\lib\libglew32.a" "lib\mingw_lib"
copy "build\cmake\build\msvc\lib\Debug\libglew32d.lib" "lib\msvc_lib\glew32d.lib"
copy "build\cmake\build\msvc\lib\Release\libglew32.lib" "lib\msvc_lib\glew32.lib"

