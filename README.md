# eupmini  
Performance music driver EUPHONY (Extension ".EUP") format player using Simple DirectMedia Layer (SDL) version 2.x .  

This project is not started from scratch, in the begining it was coded for GNU/Linux, then Windows o.s. binary version came along made by Mr.anonymous K., Mr.Sen and others.  

EUPHONY format music data was broadly used with past years favourite machine Fujitsu FM TOWNS.
EUPHONY is supported by FUJITSU FBASIC386. See [FUJITSU FM TOWNS F BASIC 386 V 2.1 reference](https://archive.org/details/FUJITSUFMTOWNSFBASIC386V2.11992_201809/page/n103/mode/2up) @ 8 . Music/Sound . p.85 8.1 Music performance . p.86 8.2 Tone data . p.87 8.3 MML . p.90 .  

HEat-Oh! is the first EUP creation software and text editor that was published in FMTOWNS magazine. HEat-Oh! is free software (= freeware) developed by TaroPYON (now taro), the name stands for "High EUP active tool".
It is a tool for MML compiling and creating of .EUP format file, but, because of its characteristics, it acts also as a powerful text editor.  
Following [chiptune create ROPCHIPTUNE LABORATORY 3.00](http://rophon.music.coocan.jp/chiptune.htm) DOCUMENTATION about [5. using Unz emulator for editing and compiling EUPHONY MML with HEat-Oh!](http://rophon.music.coocan.jp/chiptune/UnzMMLcomp.pdf), after installing TaroPYON HEat with the emulator, a complete definition of MML for FBASIC386|High-EUP compiler can be retrieved, here added as verbatim Shift-JIS encoded txt file HE386.TXT, also added a UTF-8 converted version HE386_UTF8.TXT.  

Also reference document for EUPHONY information is [Revised 3rd edition FM TOWNS Technical Data Book](https://archive.org/details/3FmTowns/mode/2up) @ 18 MIDI manager BIOS p.605 . 18.4 About MIDI EUPHONY p.607 . 18.4.3 EUP File Format p.609 .  

Hereafter some informative tables taken fron that reference document are copied here  
#### table II-18-5 EUP フォーマット(チャンネルイベント)  
| イベント | フォーマット | | | | | | 説明 |
| -- | -- | -- | -- | -- | -- | -- | -- |
|| ステータス(1バイト) | 2バイト | 3バイト | 4バイト | 5バイト | 6バイト ||
| ノートオフ | $8n | Duration LSW L | Duration LSW H | Duration MSW L | Duration MSW H | OFF VELO | ノートの長さ(Duration)は下位4ビットずつを取り出して16ビットにする. |
| ノートオン | $9n | TRACK番号 | TIME LSB | TIME MSB | 音程 | ON VELO | ノートオンは必ず対となるノートオフが直後に来る. |
| ポリフォニックキープレッシャー | $An | TRACK番号 | TIME LSB | TIME MSB | 音程 (NOTE) | プレッシャー | ノート番号とする. |
| コントロールチェンジ | $Bn | TRACK番号 | TIME LSB | TIME MSB | CONTROL値 | 設定値 | コントロール番号とコントロール値を指定する. |
| プログラムチェンジ | $Cn | TRACK番号 | TIME LSB | TIME MSB | PROGRAM値 | ダミー | プログラム番号を指定する. |
| チャネルプレッシャー | $Dn | TRACK番号 | TIME LSB | TIME MSB | PRESSER値 | ダミー | 圧力値を指定する. |
| ピッチベンド | $En | TRACK番号 | TIME LSB | TIME MSB | BEND値 LSB | BEND値 MSB | ベンド値(上位,下位7ビットずつの計14番号ビット)を指定する. |

#### table II-18-7 EUP フォーマット(その他のイベント)  
| イベント | フォーマット |||||| 説明 |
| -- | -- | -- | -- | -- | -- | -- | -- |
|| ステータス(1バイト) | 2バイト | 3バイト | 4バイト | 5バイト | 6バイト ||
| 小節マーカー | $F2 | 拍子値 | TIME LSB | TIME MSB | ダミー | ダミー | 重要なイベントで,EUPフォーマットでは1小節毎に必ずこの小節マーカーがなければならない.TimeL,TimeHはこの小節マーカーまでの時間,つまり前の小節の長さが入る.signatureにはこの小節の拍子が入る.この小節マーカーのTimeL,TimeHと前の小節マーカーのsignatureが矛盾しないことが重要で,例えば,前の小節マーカーのsignatureが4/4なら,この小節マーカーのTimeは384(96*4)でなければならない. |
| テンポ | $F8 | ダミー | TIME LSB | TIME MSB | tempo LSB | tempo MSB | テンポは14ビットで表現する.取りうる範囲は0\~250までで,実際の値はそれに30を足した,30bpm\~280bpmとなる. |
| USER CALL PROGRAM (NOT SUPPORT) | $FA | TRACK番号 | TIME LSB | TIME MSB | PROGRAM値 | ダミー | 未サポート |
| パターン番号 (NOT SUPPORT) | $FB | TRACK番号 | TIME LSB | TIME MSB | パターン番号 | ダミー | 未サポート |
| TRACK COMMAND | $FC | TRACK番号 | TIME LSB | TIME MSB | コマンド | データ(変更値) | 演奏中にポートやMIDIチャネルを変更する.コマンド=1:ポート,2:チャネル |
| DATA CONTINUE | $FD | - | - | - | - | - | 曲データが継続することを意味する,リングバッファなどにデータを転送しながら演奏する場合などに,MIDIマネーが書き込みポインタを追い越しでもしまわないようにす計ために利用する. |
| 終端マーカー | $FE | 拍子値 | TIME LSB | TIME MSB | ダミー | ダミー | 曲の最後には必ずこのマーカーを入れる.TimeL,TimeHはこの小節マーカー同様にこの終端マーカーまでの時間,つまり最後の小節の長さが入る. |
| ダミーコード | $FF | - | - | - | - | - | 何もしないイベントとして扱われる(無視される). |

translated they should be  

#### Table II-18-5 EUP Format (Channel Event)  
| Event | Format |||||| Description |
| -- | -- | -- | -- | -- | -- | -- | -- |
|| Status (1st byte) | 2nd byte | 3rd byte | 4th byte | 5th byte | 6th byte        ||
| Note Off                | $8n | Duration LSW L | Duration LSW H | Duration MSW L | Duration MSW H | OFF VELO       | Note duration is extracted in 4-bit chunks to form a 16-bit value.    |
| Note On                 | $9n | TRACK number   | TIME LSB       | TIME MSB       | Pitch          | ON VELO        | Note On must be followed immediately by its corresponding Note Off.   |
| Polyphonic Key Pressure | $An | TRACK number   | TIME LSB       | TIME MSB       | Pitch (NOTE)   | Pressure       | Used as the note number.                                              |
| Control Change          | $Bn | TRACK number   | TIME LSB       | TIME MSB       | CONTROL value  | Setting value  | Specifies control number and control value.                           |
| Programme Change        | $Cn | TRACK number   | TIME LSB       | TIME MSB       | PROGRAM value  | Dummy          | Specifies programme number.                                           |
| Channel Pressure        | $Dn | TRACK Number   | TIME LSB       | TIME MSB       | PRESSER Value  | Dummy          | Specifies the pressure value.                                         |
| Pitch Bend              | $En | TRACK Number   | TIME LSB       | TIME MSB       | BEND Value LSB | BEND Value MSB | Specifies the bend value (14 bits total: upper 7 bits, lower 7 bits). |

#### table II-18-7 EUP Format (Other Events)  
| Event | Format |||||| Description |
| -- | -- | -- | -- | -- | -- | -- | -- |
|| Status (1st byte) | 2nd byte | 3rd byte | 4th byte | 5th byte | 6th byte        ||
| Measure Marker                  | $F2 | Time Signature | TIME LSB | TIME MSB | Dummy | Dummy | In the EUP format, this measure marker must be present for every measure during important events. TimeL and TimeH contain the time up to this measure marker, i.e. the length of the preceding measure. The signature contains the time signature for this measure. It is crucial that the TimeL and TimeH of this measure marker do not conflict with the signature of the preceding measure marker. For example, if the signature of the preceding measure marker is 4/4, then the Time of this measure marker must be 384 (96*4). |
| Tempo                           | $F8 | Dummy | TIME LSB | TIME MSB | tempo LSB | tempo MSB | Tempo is expressed in 14-bit resolution. The possible range is 0~250, with the actual value being 30 added to this, resulting in 30 bpm to 280 bpm. |
| USER CALL PROGRAM (NOT SUPPORT) | $FA | TRACK number | TIME LSB | TIME MSB | PROGRAM value | Dummy | Not supported |
| Pattern Number (NOT SUPPORT)    | $FB | TRACK Number | TIME LSB | TIME MSB | Pattern Number | Dummy | Not supported |
| TRACK COMMAND                   | $FC | TRACK Number | TIME LSB | TIME MSB | Command | Data (Modified Value) | Changing ports or MIDI channels during performance. Command=1:Port,2:Channel |
| DATA CONTINUE                   | $FD | - | - | - | - | - |  This is used to prevent the MIDI stream from overwriting the write pointer when transferring data to a ring buffer or similar during playback, thereby ensuring the track data remains continuous. |
| Termination Marker              | $FE | Time Value | TIME LSB | Time MSB | Dummy | Dummy | This marker must always be inserted at the end of the piece. TimeL and TimeH, like this measure marker, contain the time to this end marker, that is, the length of the final measure. |
| Dummy Code                      | $FF | - | - | - | - | - | Treated as an event that does nothing (ignored). |  

Project improvements are driven by looking into wothke's, id est Juergen Wothke, [webEuphony WebAudio plugin of Eupony](https://bitbucket.org/wothke/webeuphony/src/master/) code,
and tjhayasaka's, id est Tomoaki Hayasaka, [eupplayer](https://github.com/tjhayasaka/eupplayer) code.  
Thank You, Juergen Wothke !!  
Thank You, Tomoaki Hayasaka !!  

Copyright
1995-1997, 2000 Tomoaki Hayasaka.
Win32 porting 2002, 2003 IIJIMA Hiromitsu aka Delmonta, and anonymous K.
2023 Giangiacomo Zaffini  

### License  

This code is available open source under the terms of the [GNU General Public License version 2](https://opensource.org/licenses/GPL-2.0).  

### How to build eupplay player  

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

#### Links:

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
