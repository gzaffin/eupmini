for vcpkg and cmake and Microsoft Visual Studio Community 2017

links to reference information pages concerning with how install and use Vcpkg
1.
https://blogs.msdn.microsoft.com/vcblog/2016/09/19/vcpkg-a-tool-to-acquire-and-build-c-open-source-libraries-on-windows/
2.
https://blogs.msdn.microsoft.com/vcblog/2018/04/24/announcing-a-single-c-library-manager-for-linux-macos-and-windows-vcpkg/

mkdir build
cd build
cmake-gui ..
Configure
fill Optional toolset to use (argument to -T) with
host=x64
OK
Add entry
Add Cache Entry
Name:
CMAKE_TOOLCHAIN_FILE
Type:
STRING
Value:
<PATH>vcpkg/scripts/buildsystems/vcpkg.cmake (see [1])
Description:
cmake entry point for vcpkg
OK
Double check that SDL2_DIR STRING variable is a path to find SDL2Config.cmake (see [2]), if not the case, fix it
Configure
Generate
Open Project
On Microsoft Visual Studio Community build solution

[1]
it is absolute or relative path to vcpkg's vcpkg.cmake e.g. C:/tempGZ/vcpkg/scripts/buildsystems/vcpkg.cmake

[2]
it is absolute or relative path to vcpkg's SDL2Config.cmake e.g. C:/tempGZ/vcpkg/installed/x64-windows/share/sdl2

=o=o=o=o=o=o=o=o=o=o=o=

for cmake and MSYS2 on Windows o.s. box

$ mkdir build
$ cd build
$ cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release ..
$ make -j 4 eupplay

=o=o=o=o=o=o=o=o=o=o=o=

for cmake and Ubuntu/Debian/GNU/LINUX o.s.

$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make -j 4 eupplay
