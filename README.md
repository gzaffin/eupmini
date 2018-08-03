# eupmini
This is performance music driver EUPHONY (Extension ".Eup") format player using Simple DirectMedia Layer (SDL) version 2.0.x

This project is not started from scratch, in the begining it was coded for GNU/Linux, then Windows o.s. binary version exists made by anonymous K.-Sama, Mr.Sen and others (http://www.dennougedougakkai-ndd.org/pub/werkzeug/EUPPlayer/).

EUPHONY format music data was broadly used with past years favourite machine Fujitsu FM TOWNS.

HEat-Oh! is the first EUP creation software and text editor that was published in FMTOWNS magazine. HEat-Oh! is free software (= freeware) developed by TaroPYON (now taro), the name stands for "High EUP active tool". It was a tool for MML compiling and creating of .EUP format file, but, because of its characteristics, it acts also as a powerful text editor.

Copyright
1995-1997, 2000 Tomoaki Hayasaka.
Win32 porting 2002, 2003 IIJIMA Hiromitsu aka Delmonta, and anonymous K.
2018 Giangiacomo Zaffini

Licence
GNU General Public License, version 2
https://www.gnu.org/licenses/old-licenses/gpl-2.0.html

links:

1. DRMSoundwork ( http://www.boreas.dti.ne.jp/~nudi-drm/ )
All available EUP and Basic file archive is http://www.boreas.dti.ne.jp/~nudi-drm/sound/townsmml/arcfiles.lzh

2. Freescale 6 to 11 directory of FM-TOWNS MUSIC Performance files ( http://mdxoarchive.webcrow.jp/ )
http://mdxoarchive.webcrow.jp/EUP/FCEUP.part01.rar
http://mdxoarchive.webcrow.jp/EUP/FCEUP.part02.rar
http://mdxoarchive.webcrow.jp/EUP/FCEUP.part03.rar
http://mdxoarchive.webcrow.jp/EUP/FCEUP.part04.rar
http://mdxoarchive.webcrow.jp/EUP/FCEUP.part05.rar

3. chiptune create ROPCHIPTUNE LABORATORY 3.00 ( http://rophon.music.coocan.jp/chiptune.htm )
TOWNS EUP 2015 http://rophon.music.coocan.jp/chiptune.htm

4. MML Compiler HE386 for TownsOS, Windows NT/95/98, Linux
http://www.runser.jp/softlib.html

# How to build

The following steps build `eupplay` on Ubuntu/Debian/GNU/LINUX o.s. box with SDL2 and cmake.

```
$ git clone https://github.com/gzaffin/eupmini.git
$ cd eupmini
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make -j 4 eupplay
```

The following steps build `eupplay.exe` on a MSYS2/MinGW-w64 Windows o.s. box with SDL2 and cmake.

```
$ git clone https://github.com/gzaffin/eupmini.git
$ cd eupmini
$ mkdir build
$ cd build
$ cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release ..
$ make -j 4 eupplay
```

The following steps build `eupplay.exe` on a Windows o.s. box with vcpkg, SDL2 and cmake.

links to reference information pages concerning with how install and use Vcpkg
1.
https://blogs.msdn.microsoft.com/vcblog/2016/09/19/vcpkg-a-tool-to-acquire-and-build-c-open-source-libraries-on-windows/
2.
https://blogs.msdn.microsoft.com/vcblog/2018/04/24/announcing-a-single-c-library-manager-for-linux-macos-and-windows-vcpkg/

```
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
```

[1]
it is absolute or relative path to vcpkg's vcpkg.cmake e.g. C:/tempGZ/vcpkg/scripts/buildsystems/vcpkg.cmake

[2]
it is absolute or relative path to vcpkg's SDL2Config.cmake e.g. C:/tempGZ/vcpkg/installed/x64-windows/share/sdl2

