# eupmini
Performance music driver EUPHONY (Extension ".Eup") format player using Simple DirectMedia Layer (SDL) version 2.0.x .

This project is not started from scratch, in the begining it was coded for GNU/Linux, then Windows o.s. binary version came along made by Mr.anonymous K., Mr.Sen and others (link ref. Lost and Found project).

EUPHONY format music data was broadly used with past years favourite machine Fujitsu FM TOWNS.

HEat-Oh! is the first EUP creation software and text editor that was published in FMTOWNS magazine. HEat-Oh! is free software (= freeware) developed by TaroPYON (now taro), the name stands for "High EUP active tool".
It is a tool for MML compiling and creating of .EUP format file, but, because of its characteristics, it acts also as a powerful text editor.

Copyright
1995-1997, 2000 Tomoaki Hayasaka.
Win32 porting 2002, 2003 IIJIMA Hiromitsu aka Delmonta, and anonymous K.
2018 Giangiacomo Zaffini

# License

This code is available open source under the terms of the [GNU General Public License version 2](https://opensource.org/licenses/GPL-2.0).

# How to build

The following steps build `eupplay` on Ubuntu/Debian/GNU/LINUX o.s. box, or `eupplay.exe` on a MSYS2/MinGW-w64 Windows o.s. box, with provided Makefile, SDL2 and make.

```shell/bash shell
$ git clone https://github.com/gzaffin/eupmini.git
$ cd eupmini
$ make
```

The following steps build `eupplay` on Ubuntu/Debian/GNU/LINUX o.s. box with SDL2 and cmake.

```GNU/linux bash
$ git clone https://github.com/gzaffin/eupmini.git
$ cd eupmini
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build . --config Release --target eupplay
```

The following steps build `eupplay.exe` on a MSYS2/MinGW-w64 Windows o.s. box with SDL2 and cmake.

```msys2/mingw bash
$ git clone https://github.com/gzaffin/eupmini.git
$ cd eupmini
$ mkdir build
$ cd build
$ cmake -G "MSYS Makefiles" ..
$ cmake --build . --config Release --target eupplay
```

If MSYS Makefiles generator set with `-G "MSYS Makefiles"` cannot properly set make-utility,
then add `-DCMAKE_MAKE_PROGRAM=<[PATH]/make-utility>` PATH of make-utility (see [1])

```windows command-line interface
$ git clone https://github.com/gzaffin/eupmini.git
$ cd eupmini
$ mkdir build
$ cd build
$ cmake -G "MSYS Makefiles" -DCMAKE_MAKE_PROGRAM=mingw32-make ..
$ cmake --build . --config Release --target eupplay
```

The following steps build `eupplay.exe` on a Windows o.s. box with MSVC, vcpkg, SDL2 installed with vcpkg.

You can have Your build environment set, on a Windows 10 box, if Your MSVC is Microsoft Visual Studio 2019 Community edition, using Windows 10 taskbar search box writing `x64 Native Tools Command Prompt for VS 2019` and starting matching App.
Otherwise, if MSVC is installed in default localtion, if Windows SDK is 10.0.18362.0 (please see what is in 'C:\Program Files (x86)\Microsoft SDKs\Windows Kits\10\ExtensionSDKs\Microsoft.UniversalCRT.Debug' folder) (see [2]) issuing

```windows command-line interface
C:\Users\gzaff>"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64 10.0.18362.0
```

Then

```windows command-line interface
C:\>"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\TeamFoundation\Team Explorer\Git\cmd\git" clone https://github.com/gzaffin/eupmini.git
C:\>cd eupmini
C:\eupmini>mkdir build
C:\eupmini>cd build
C:\eupmini\build>cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=C:/Users/gzaff/Devs/vcpkg/scripts/buildsystems/vcpkg.cmake ..
C:\eupmini\build>ninja eupplay
```

For the case that Visual Studio can be used

```windows command-line interface
C:\>"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\TeamFoundation\Team Explorer\Git\cmd\git" clone https://github.com/gzaffin/eupmini.git
C:\>cd eupmini
C:\eupmini>mkdir build
C:\eupmini>cd build
C:\eupmini\build>cmake -G "Visual Studio 16 2019" -A x64 -T host=x64 -DCMAKE_TOOLCHAIN_FILE=C:/Users/gzaff/Devs/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

For building from command line

```windows command-line interface
C:\eupmini\build>cmake --build . --config Release --target eupplay
```

Otherwise start Microsoft Visual Studio and debug eupmini solution.

You can have Your build environment set, on a Windows 7 box, if Your MSVC is Microsoft Visual Studio 2017 Community edition, using Windows 7 taskbar search box writing `x64 Native Tools Command Prompt for VS 2017` and starting matching App.
Otherwise, if MSVC is installed in default localtion, if Windows SDK is 10.0.17763.0 (please see what is in 'C:\Program Files (x86)\Microsoft SDKs\Windows Kits\10\ExtensionSDKs\Microsoft.UniversalCRT.Debug' folder) (see [2]) issuing

```windows command-line interface
C:\Users\gzaff>"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64 10.0.17763.0
```

Then

```windows command-line interface
C:\>"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\CommonExtensions\Microsoft\TeamFoundation\Team Explorer\Git\cmd\git" clone https://github.com/gzaffin/eupmini.git
C:\>cd eupmini
C:\eupmini>mkdir build
C:\eupmini>cd build
C:\eupmini\build>cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=C:/Users/gzaff/Devs/vcpkg/scripts/buildsystems/vcpkg.cmake ..
C:\pmdmini\build>ninja eupplay
```

For the case that Visual Studio can be used

```windows command-line interface
C:\>"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\CommonExtensions\Microsoft\TeamFoundation\Team Explorer\Git\cmd\git" clone https://github.com/gzaffin/eupmini.git
C:\>cd eupmini
C:\eupmini>mkdir build
C:\eupmini>cd build
C:\eupmini\build>cmake -G "Visual Studio 15 2017 Win64" -T host=x64 -DCMAKE_TOOLCHAIN_FILE=C:/Users/gzaff/Devs/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

For building from command line

```windows command-line interface
C:\eupmini\build>cmake --build . --config Release --target eupplay
```

Otherwise start Microsoft Visual Studio and debug eupmini solution.

Recap of required MACRO definitions:

`CMAKE_TOOLCHAIN_FILE`: full PATH of vcpkg.cmake

[1]
it is make-utility name e.g. `mingw32-make` with specified PATH if make is not within search PATH as it should be

[2]
calling vcvarsall.bat update PATH variable, so "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake" and "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja" can be called as cmake and ninja respectively

# Links:

1. Lost and Found project
http://www.dennougedougakkai-ndd.org/pub/werkzeug/EUPPlayer/

2. [DRMSoundwork](http://www.boreas.dti.ne.jp/~nudi-drm/)
[All available EUP and Basic file archive](http://www.boreas.dti.ne.jp/~nudi-drm/sound/townsmml/arcfiles.lzh)

3. [Freescale 6 to 11 directory of FM-TOWNS MUSIC Performance files](http://mdxoarchive.webcrow.jp/)
http://mdxoarchive.webcrow.jp/EUP/FCEUP.part01.rar
http://mdxoarchive.webcrow.jp/EUP/FCEUP.part02.rar
http://mdxoarchive.webcrow.jp/EUP/FCEUP.part03.rar
http://mdxoarchive.webcrow.jp/EUP/FCEUP.part04.rar
http://mdxoarchive.webcrow.jp/EUP/FCEUP.part05.rar

4. [chiptune create ROPCHIPTUNE LABORATORY 3.00](http://rophon.music.coocan.jp/chiptune.htm)
[TOWNS EUP 2015](http://rophon.music.coocan.jp/chiptune.htm)

5. [MML Compiler HE386 for TownsOS, Windows NT/95/98, Linux](http://www.runser.jp/softlib.html)

6. Vcpkg official GitHub repository
[GitHub Microsoft vcpkg](https://github.com/Microsoft/vcpkg)

7. Vcpkg documentation
[vcpkg: A C++ package manager for Windows, Linux and MacOS](https://docs.microsoft.com/en-us/cpp/build/vcpkg?view=vs-2019)

8. Microsoft developer blog
[Eric Mittelette's blog](https://devblogs.microsoft.com/cppblog/vcpkg-a-tool-to-acquire-and-build-c-open-source-libraries-on-windows/)
