# eupmini  
Performance music driver EUPHONY (Extension ".EUP") format player using Simple DirectMedia Layer (SDL) version 2.x .  

This project is not started from scratch, in the begining it was coded for GNU/Linux, then Windows o.s. binary version came along made by Mr.anonymous K., Mr.Sen and others.  

EUPHONY format music data was broadly used with past years favourite machine Fujitsu FM TOWNS.
EUPHONY is supported by FUJITSU FBASIC386. See [FUJITSU FM TOWNS F BASIC 386 V 2.1 reference](https://archive.org/details/FUJITSUFMTOWNSFBASIC386V2.11992_201809/page/n103/mode/2up) @ 8 . Music/Sound . p.85 8.1 Music performance . p.86 8.2 Tone data . p.87 8.3 MML . p.90 .  

HEat-Oh! is the first EUP creation software and text editor that was published in FMTOWNS magazine. HEat-Oh! is free software (= freeware) developed by TaroPYON (now taro), the name stands for "High EUP active tool".
It is a tool for MML compiling and creating of .EUP format file, but, because of its characteristics, it acts also as a powerful text editor.  
Following [chiptune create ROPCHIPTUNE LABORATORY 3.00](http://rophon.music.coocan.jp/chiptune.htm) DOCUMENTATION about [5. using Unz emulator for editing and compiling EUPHONY MML with HEat-Oh!](http://rophon.music.coocan.jp/chiptune/UnzMMLcomp.pdf), after installing TaroPYON HEat with the emulator, a complete definition of MML for FBASIC386|High-EUP compiler can be retrieved, here added as verbatim Shift-JIS encoded txt file HE386.TXT, also added a UTF-8 converted version HE386_UTF8.TXT.  

Also reference document for EUPHONY information is [Revised 3rd edition FM TOWNS Technical Data Book](https://archive.org/details/3FmTowns/mode/2up) @ 18 MIDI manager BIOS p.605 . 18.4 About MIDI EUPHONY p.607 . 18.4.3 EUP File Format p.609 .  

Project improvements are driven by looking into wothke's, id est Juergen Wothke, [webEuphony WebAudio plugin of Eupony](https://bitbucket.org/wothke/webeuphony/src/master/) code,
and tjhayasaka's, id est Tomoaki Hayasaka, [eupplayer](https://github.com/tjhayasaka/eupplayer) code.  
Thank You, Juergen Wothke !!  
Thank You, Tomoaki Hayasaka !!  

Copyright
1995-1997, 2000 Tomoaki Hayasaka.
Win32 porting 2002, 2003 IIJIMA Hiromitsu aka Delmonta, and anonymous K.
2023 Giangiacomo Zaffini  

# License  

This code is available open source under the terms of the [GNU General Public License version 2](https://opensource.org/licenses/GPL-2.0).  

# How to build eupplay player  

The following steps build `eupplay` on Ubuntu/Debian/GNU/LINUX o.s. box with SDL2 and cmake.  

```GNU/linux bash
$ git clone https://github.com/gzaffin/eupmini.git
$ cd eupmini
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build . --config Release --target eupplay
```

The following steps build `eupplay.exe` on a Windows o.s. box with MSVC, vcpkg, SDL2 installed with vcpkg. Both MSVC and vcpkg have git, let's suppose git is in path environment for Windows command prompt console or Windows PowerShell console.
Choosing Windows command prompt console.  

```windows command-line interface
C:\>git clone https://github.com/gzaffin/eupmini.git
C:\>cd eupmini
C:\eupmini>mkdir build
C:\eupmini>cd build
C:\eupmini\build>cmake -G "Visual Studio 17 2022" -A x64 -T host=x64 -D CMAKE_TOOLCHAIN_FILE=<PATH>/vcpkg/scripts/buildsystems/vcpkg.cmake ..
C:\eupmini\build>cmake --build . --config Release --target eupplay
```

For the case that Visual Studio can be used  

```windows command-line interface
C:\>git clone https://github.com/gzaffin/eupmini.git
C:\>cd eupmini
C:\eupmini>mkdir build
C:\eupmini>cd build
C:\eupmini\build>cmake -G "Visual Studio 17 2022" -A x64 -T host=x64 -D CMAKE_TOOLCHAIN_FILE=C:/Users/gzaff/Devs/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```
Now Microsoft Visual Studio can be started and eupmini solution can be built and debugged.  

# Links:

1. Lost and Found projects
[EUPPlayer for Windows O.S.](http://heisei.dennougedougakkai-ndd.org/pub/werkzeug/EUPPlayer/)  
[BEFiS Webpage Download page](http://www.purose.net/befis/download/)  
[BEFiS Webpage Download direct link to eupplay](http://www.purose.net/befis/download/lib/eupplay.lzh)  
[EUP player for MSW  ver.0.08f Programmed by 仙](http://www.ym2149.com/fmtowns_eup.zip)  

2. [DRMSoundwork](http://www.boreas.dti.ne.jp/~nudi-drm/)  
[All available EUP and Basic file archive](http://www.boreas.dti.ne.jp/~nudi-drm/sound/townsmml/arcfiles.lzh)  

3. [Fujitsu FreeSoftware Collection/フリーソフトウェアコレクション](https://archive.org/search?query=%E3%83%95%E3%83%AA%E3%83%BC%E3%82%BD%E3%83%95%E3%83%88%E3%82%A6%E3%82%A7%E3%82%A2%E3%82%B3%E3%83%AC%E3%82%AF%E3%82%B7%E3%83%A7%E3%83%B3) from 6th to 11th numbers archives of FM-TOWNS MUSIC Performance files - once it was http://mdxoarchive.webcrow.jp/  
[FCEUP.part01.rar](https://1drv.ms/u/s!Ajr-D58Azu8jiCTHyA1vUPxhrBQB?e=45oXmT)  
[FCEUP.part02.rar](https://1drv.ms/u/s!Ajr-D58Azu8jiCJQp0cOXtFV4A6K?e=3dSphi)  
[FCEUP.part03.rar](https://1drv.ms/u/s!Ajr-D58Azu8jiCODxoiyh-K0Qri2?e=y5xiRF)  
[FCEUP.part04.rar](https://1drv.ms/u/s!Ajr-D58Azu8jiCY7yolQ9048VFEj?e=R2NfL6)  
[FCEUP.part05.rar](https://1drv.ms/u/s!Ajr-D58Azu8jiCUmFNyllNmNOHr4?e=pIG7Km)  

5. [ROPCHIPTUNE LABORATORY 3.00 Documents]([http://rophon.music.coocan.jp/chiptune.htm](http://rophon.music.coocan.jp/document.htm))  
[ROPCHIPTUNE LABORATORY 3.00 FM TOWNS]([http://rophon.music.coocan.jp/chiptune.htm](http://rophon.music.coocan.jp/chiptune.htm#C_FMT))  

6. [MML Compiler HE386 for TownsOS, Windows NT/95/98, Linux](http://www.runser.jp/softlib.html)  

7. Vcpkg official GitHub repository  
[GitHub Microsoft vcpkg](https://github.com/Microsoft/vcpkg)  

8. Vcpkg documentation  
[vcpkg: A C++ package manager for Windows, Linux and MacOS](https://docs.microsoft.com/en-us/cpp/build/vcpkg?view=vs-2019)  

9. Microsoft developer blog  
[Eric Mittelette's blog](https://devblogs.microsoft.com/cppblog/vcpkg-a-tool-to-acquire-and-build-c-open-source-libraries-on-windows/)  
