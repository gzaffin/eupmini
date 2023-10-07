/*      Copyright (C) 1995-1996, 2000 Tomoaki HAYASAKA.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

// $Id: eupplay.cc,v 1.15 2000/04/12 23:20:56 hayasaka Exp $

/*      Artistic Style
 *
 * ./astyle --style=stroustrup --convert-tabs --add-braces eupplay.cpp
 */

#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <strstream>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <vector>
#include <strstream>

#ifdef _MSC_VER
 // following SDL_MAIN_HANDLED macro is managed with CMake
 //#define SDL_MAIN_HANDLED
#include <SDL.h>
#else
#include <SDL.h>
#endif

#if defined ( _MSC_VER )
#ifndef _WINGETOPT_H_
#define _WINGETOPT_H_

#define GETOPT_EOF (-1)
#define GETOPT_ERR(s, c) if (opterr) { \
    char errbuf[2]; \
    errbuf[0] = c; errbuf[1] = '\n'; \
    fputs(argv[0], stderr); \
    fputs(s, stderr); \
    fputc(c, stderr); \
}

int opterr = 1;
int optind = 1;
int optopt = 0;
char * optarg = nullptr;

int getopt(int argc, char ** argv, const char * opts)
{
    static int sp = 1;
    int c;
    const char * cp = nullptr;

    if (sp == 1) {
        if ((optind >= argc) || (argv[optind][0] != '-') || (argv[optind][1] == '\0')) {
            return GETOPT_EOF;
        }
        else if (std::strcmp(argv[optind], "--") == 0) {
            optind++;
            return GETOPT_EOF;
        }
    }

    optopt = c = argv[optind][sp];
    if (c == ':' || (cp = std::strchr(opts, c)) == nullptr) {
        GETOPT_ERR(": illegal option -- ", c);

        if (argv[optind][++sp] == '\0') {
            optind++;
            sp = 1;
        }
        return '?';
    }

    if (*++cp == ':') {
        if (argv[optind][sp + 1] != '\0') {
            optarg = &argv[optind++][sp + 1];
        }
        else if (++optind >= argc) {
            GETOPT_ERR(": option requires an argument -- ", c);
            sp = 1;
            return('?');
        }
        else {
            optarg = argv[optind++];
        }

        sp = 1;
    }
    else {
        if (argv[optind][++sp] == '\0') {
            sp = 1;
            optind++;
        }
        optarg = nullptr;
    }

    return(c);
}

#endif  /* _WINGETOPT_H_ */
#endif // _MSC_VER

#if defined ( __MINGW32__ )
#include <unistd.h> /* is it for declaring int getopt(int, char * const [], const char *) ? */
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
#include <unistd.h> /* is it for declaring int getopt(int, char * const [], const char *) ? */
#endif // __GNUC__
#if defined ( _MSC_VER )
#define PATH_DELIMITER      ";"
#define PATH_DELIMITER_CHAR ';'
#endif // _MSC_VER
#if defined ( __MINGW32__ )
#include <fcntl.h>
#include <_bsd_types.h>
#define PATH_DELIMITER      ";"
#define PATH_DELIMITER_CHAR ';'
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
#define PATH_DELIMITER      ":"
#define PATH_DELIMITER_CHAR ':'
#endif // __GNUC__
#if defined ( _MSC_VER )
#include <SDL.h>
#endif // _MSC_VER
#if defined ( __MINGW32__ )
#include <SDL2/SDL.h>
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
#include <SDL2/SDL.h>
#endif // __GNUC__
#include "eupplayer.hpp"
#include "eupplayer_townsEmulator.hpp"

/*
 * global data
 *
 */
/* 16 bit little endian sample stream -> sample size = 2 octects a.k.a. bytes */
struct pcm_struct pcm;

/*
* param an application-specific parameter saved in the SDL_AudioSpec structure's userdata field
* data a pointer to the audio data buffer filled in by SDL_AudioCallback()
* len the length of data buffer in bytes
* 
* Stereo samples are stored in a LRLRLR ordering.
*/
static void audio_callback(void *param, Uint8 *data, int len)
{

    if (true != pcm.on) {
        memset(data, 0, len);
        len = 0; /* statement cuts off following for loop */
    }

    if (len) {
        int readData2Buffer = pcm.read_pos;
        int octects_per_sample = streamAudioSampleOctectSize * streamAudioChannelsNum;
        int i;
        if (2 == streamAudioChannelsNum) {
            int32_t* audio_buffer = reinterpret_cast<int32_t*>(data);
            int32_t* source_buffer = reinterpret_cast<int32_t*>(pcm.buffer);

            for (i = 0; i < len / octects_per_sample; i++) {
                if (streamAudioSamplesBuffer <= readData2Buffer) {
                    readData2Buffer = 0;
                }
                audio_buffer[i] = source_buffer[readData2Buffer++];
            }
        }
        else /*if (1 == streamAudioChannelsNum)*/ {
            int16_t* audio_buffer = reinterpret_cast<int16_t*>(data);
            int16_t* source_buffer = pcm.buffer;

            for (i = 0; i < len / octects_per_sample; i++) {
                if (streamAudioSamplesBuffer <= readData2Buffer) {
                    readData2Buffer = 0;
                }
                audio_buffer[i] = source_buffer[readData2Buffer++];
            }
        }

        pcm.count += i;
        pcm.read_pos = readData2Buffer;
    }
}

static std::string const downcase(std::string const &s)
{
    std::string r;
    for (std::string::const_iterator i = s.begin(); i != s.end(); i++) {
        r += (*i >= 'A' && *i <= 'Z') ? (*i - 'A' + 'a') : *i;
    }
    return r;
}

static std::string const upcase(std::string const &s)
{
    std::string r;
    for (std::string::const_iterator i = s.begin(); i != s.end(); i++) {
        r += (*i >= 'a' && *i <= 'z') ? (*i - 'a' + 'A') : *i;
    }
    return r;
}

static FILE *openFile_inPath(std::string const &filename, std::string const &path)
{
    FILE *f = nullptr;
    std::vector<std::string> fn2;
    fn2.push_back(filename);
    fn2.push_back(downcase(filename));
    fn2.push_back(upcase(filename));

    std::string::const_iterator i = path.begin();
    while (i != path.end()) {
        std::string dir{};
        while (i != path.end() && *i != PATH_DELIMITER_CHAR) {
            dir += *i++;
        }
        if (i != path.end() && *i == PATH_DELIMITER_CHAR) {
            i++;
        }
        for (std::vector<std::string>::const_iterator j = fn2.begin(); j != fn2.end(); j++) {
#ifdef _MSC_VER
            std::string const filename(dir /*+ "\\"*/ + *j);
#else
            std::string const filename(dir /*+ "/"*/ +*j);
#endif
            std::cerr << "trying " << filename << std::endl;
            f = fopen(filename.c_str(), "rb");
            if (f != nullptr) {
                std::cerr << "loading " << filename << std::endl;
                return f;
            }
        }
    }
    if (f == nullptr) {
        fprintf(stderr, "error finding %s\n", filename.c_str());
        std::fflush(stderr);
    }

    return f;
}

uint8_t *EUPPlayer_readFile(EUPPlayer *player,
        TownsAudioDevice *device,
        std::string const &nameOfEupFile)
{
    // EUPﾌｧｲﾙﾍｯﾀﾞ構造体
    typedef struct {
        char    title[32]; // オフセット/offset 000h タイトルは半角32文字，全角で16文字以内の文字列で指定します。The title is specified as a character string of 32 half-width characters or less and 16 full-width characters or less.
        char    artist[8]; // オフセット/offset 020h
        char    dummy[44]; // オフセット/offset 028h
        char    trk_name[32][16]; // オフセット/offset 084h 512 = 32 * 16
        char    short_trk_name[32][8]; // オフセット/offset 254h 256 = 32 * 8
        char    trk_mute[32]; // オフセット/offset 354h
        char    trk_port[32]; // オフセット/offset 374h
        char    trk_midi_ch[32]; // オフセット/offset 394h
        char    trk_key_bias[32]; // オフセット/offset 3B4h
        char    trk_transpose[32]; // オフセット/offset 3D4h
        char    trk_play_filter[32][7]; // オフセット/offset 3F4h 224 = 32 * 7
                                        // ＦＩＬＴＥＲ：形式１ ベロシティフィルター/Format 1 velocity filter ＦＩＬＴＥＲ：形式２ ボリュームフィルター/FILTER: Format 2 volume filter ＦＩＬＴＥＲ：形式３ パンポットフィルター/FILTER: Format 3 panpot filter have 7 typed parameters 1 byte sized each parameter
                                        // ＦＩＬＴＥＲ：形式４ ピッチベンドフィルター/Format 4 pitch bend filter has 7 typed parameters some parameters are 2 bytes sized
        char    instruments_name[128][4]; // オフセット/offset 4D4h 512 = 128 * 4
        char    fm_midi_ch[6]; // オフセット/offset 6D4h
        char    pcm_midi_ch[8]; // オフセット/offset 6DAh
        char    fm_file_name[8]; // オフセット/offset 6E2h
        char    pcm_file_name[8]; // オフセット/offset 6EAh
        char    reserved[260]; // オフセット/offset 6F2h 260 = (32 * 8) + 4 ??? ＦＩＬＴＥＲ：形式４ ピッチベンドフィルター/Format 4 pitch bend filter additional space ???
        char    appli_name[8]; // オフセット/offset 7F6h
        char    appli_version[2]; // オフセット/offset 7FEh
        int32_t size; // オフセット/offset 800h
        char    signature; // オフセット/offset 804h
        char    first_tempo; // オフセット/offset 805h
    } EUPHEAD;

    typedef struct {
        char    trk_mute[32];
        char    trk_port[32];
        char    trk_midi_ch[32];
        char    trk_key_bias[32];
        char    trk_transpose[32];
        char    fm_midi_ch[6];
        char    pcm_midi_ch[8];
        int32_t size;
        char    signature;
        char    first_tempo;
        char*   eupData;
    } EUPINFO;

    // ヘッダ読み込み用バッファ
    EUPHEAD eupHeader;
    // EUP情報領域
    EUPINFO eupInfo;

    // とりあえず, TOWNS emu のみに対応.

    uint8_t *eupbuf = nullptr;
    player->stopPlaying();

    {
        FILE *f = fopen(nameOfEupFile.c_str(), "rb");
        if (f == nullptr) {
            perror(nameOfEupFile.c_str());
            return nullptr;
        }

        struct stat statbufEupFile;
        if (fstat(fileno(f), &statbufEupFile) != 0) {
            perror(nameOfEupFile.c_str());
            fclose(f);
            return nullptr;
        }

        if (statbufEupFile.st_size < 2048 + 6 + 6) {
            fprintf(stderr, "%s: too short file.\n", nameOfEupFile.c_str());
            std::fflush(stderr);
            fclose(f);
            return nullptr;
        }

        size_t eupRead = fread(&eupHeader, 1, sizeof(EUPHEAD), f);
        if (eupRead != sizeof(EUPHEAD)) {
            fprintf(stderr, "EUP file does not follow specification.\n");
            std::fflush(stderr);
            fclose(f);
            return nullptr;
        }
        else {
            int trk;

            // データー領域の確保
            eupInfo.eupData = nullptr/*new uint8_t[eupHeader.size]*/;
            //if (eupInfo.eupData == nullptr break;

            eupInfo.first_tempo = eupHeader.first_tempo;
            //player->tempo(eupInfo.first_tempo + 30);
            fprintf(stderr, "eupInfo.first_tempo + 30 = %d.\n", eupInfo.first_tempo + 30);

            // ヘッダ情報のコピー
            for (trk = 0; trk < 32; trk++) {
                eupInfo.trk_mute[trk] = eupHeader.trk_mute[trk];
                eupInfo.trk_port[trk] = eupHeader.trk_port[trk];
                eupInfo.trk_midi_ch[trk] = eupHeader.trk_midi_ch[trk];
                eupInfo.trk_key_bias[trk] = eupHeader.trk_key_bias[trk];
                eupInfo.trk_transpose[trk] = eupHeader.trk_transpose[trk];

                player->mapTrack_toChannel(trk, eupInfo.trk_midi_ch[trk]);
                fprintf(stderr, "eupInfo.trk_midi_ch[%d] is %d.\n", trk, eupInfo.trk_midi_ch[trk]);
            }
            for (trk = 0; trk < 6; trk++) {
                eupInfo.fm_midi_ch[trk] = eupHeader.fm_midi_ch[trk];

                device->assignFmDeviceToChannel(eupInfo.fm_midi_ch[trk]);
                fprintf(stderr, "eupInfo.fm_midi_ch[%d] is %d.\n", trk, eupInfo.fm_midi_ch[trk]);
            }
            for (trk = 0; trk < 8; trk++) {
                eupInfo.pcm_midi_ch[trk] = eupHeader.pcm_midi_ch[trk];

                device->assignPcmDeviceToChannel(eupInfo.pcm_midi_ch[trk]);
                fprintf(stderr, "eupInfo.pcm_midi_ch[%d] is %d.\n", trk, eupInfo.pcm_midi_ch[trk]);
            }
            eupInfo.signature = eupHeader.signature;
            //eupInfo.first_tempo = eupHeader.first_tempo;
            eupInfo.size = eupHeader.size;
        }

        eupbuf = new uint8_t[statbufEupFile.st_size];
        fseek(f, 0, SEEK_SET); // seek to start
        eupRead = fread(eupbuf, 1, statbufEupFile.st_size, f);
        if (eupRead != statbufEupFile.st_size) {
            fprintf(stderr, "EUP not fully read: %zu instead of %lu.\n", eupRead, statbufEupFile.st_size);
        }
        fclose(f);
    }

    //player->tempo(eupbuf[2048 + 5] + 30);
    player->tempo(eupInfo.first_tempo + 30);
    // 初期テンポの設定のつもり.  これで正しい?

    // moved above
    //for (int n = 0; n < 32; n++) {
    //    player->mapTrack_toChannel(n, eupbuf[0x394 + n]);
    //}

    // moved above
    //for (int n = 0; n < 6; n++) {
    //    device->assignFmDeviceToChannel(eupbuf[0x6d4 + n]);
    //}

    // moved above
    //for (int n = 0; n < 8; n++) {
    //    device->assignPcmDeviceToChannel(eupbuf[0x6da + n]);
    //}

    char nameBuf[1024];
#ifdef _MSC_VER
    char *fmbpmbdir = std::getenv("USERPROFILE");
#else
    char *fmbpmbdir = std::getenv("HOME");
#endif
    if (nullptr != fmbpmbdir)
    {
        std::strcpy(nameBuf, fmbpmbdir);
#ifdef _MSC_VER
        std::strcat(nameBuf, "\\.eupplay\\");
#else
        std::strcat(nameBuf,"/.eupplay/");
#endif
    }
    else
    {
        nameBuf[0] = 0;
    }
    std::string fmbPath(nameBuf);
    std::string pmbPath(nameBuf);
#ifdef _MSC_VER
    std::string eupDir(nameOfEupFile.substr(0, nameOfEupFile.rfind("\\") + 1)/* + "\\"*/);
#else
    std::string eupDir(nameOfEupFile.substr(0, nameOfEupFile.rfind("/") + 1)/* + "/"*/);
#endif
    fmbPath = eupDir + PATH_DELIMITER + fmbPath;
    std::cerr << "fmbPath is " << fmbPath << std::endl;
    pmbPath = eupDir + PATH_DELIMITER + pmbPath;
    std::cerr << "pmbPath is " << pmbPath << std::endl;

    {
#if 0
        uint8_t instrument[] = {
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // name
            1, 7, 1, 1,                             // detune / multiple
            23, 40, 38, 0,                          // output level
            140, 140, 140, 83,                      // key scale / attack rate
            13, 13, 13, 3,                          // amon / decay rate
            0, 0, 0, 0,                             // sustain rate
            19, 250, 19, 10,                        // sustain level / release rate
            58,                                     // feedback / algorithm
            0xc0,                                   // pan, LFO
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        };
#else
        uint8_t instrument[] = {
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // name
            17, 33, 10, 17,                         // detune / multiple
            25, 10, 57, 0,                          // output level
            154, 152, 218, 216,                     // key scale / attack rate
            15, 12, 7, 12,                          // amon / decay rate
            0, 5, 3, 5,                             // sustain rate
            38, 40, 70, 40,                         // sustain level / release rate
            20,                                     // feedback / algorithm
            0xc0,                                   // pan, LFO
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        };
#endif
        for (int n = 0; n < 128; n++) {
            device->setFmInstrumentParameter(n, instrument);
        }
    }
    /* FMB */
    {
        char fn0[16];
        memcpy(fn0, eupHeader.fm_file_name/*eupbuf + 0x6e2*/, 8); /* verified 0x06e2 offset */
        fn0[8] = '\0';
        std::string fn1(std::string(fn0) + ".fmb");
        FILE *f = openFile_inPath(fn1, fmbPath);
        if (f != nullptr) {
#if defined ( _MSC_VER )
            struct stat statbuf1;
            fstat(fileno(f), &statbuf1);
            uint8_t *buf1 = new uint8_t[statbuf1.st_size];
            if (nullptr == buf1) {
                fprintf(stderr, "heap allocation problem.\n");
                std::fflush(stderr);
                fclose(f);
                return nullptr;
            }
#endif // _MSC_VER
#if defined ( __MINGW32__ )
            struct stat statbuf1;
            fstat(fileno(f), &statbuf1);
            uint8_t *buf1 = new uint8_t[statbuf1.st_size];
            if (nullptr == buf1) {
                fprintf(stderr, "heap allocation problem.\n");
                std::fflush(stderr);
                fclose(f);
                return nullptr;
            }
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
            struct stat statbuf1;
            fstat(fileno(f), &statbuf1);
            uint8_t buf1[statbuf1.st_size];
#endif // __GNUC__
            size_t fmbRead = fread(buf1, 1, statbuf1.st_size, f);
            if (fmbRead != statbuf1.st_size) {
                fprintf(stderr, "FMB not fully read: %zu instead of %lu.\n", fmbRead, statbuf1.st_size);
            }
            fclose(f);
            for (int n = 0; n < (statbuf1.st_size - 8) / 48; n++) {
                device->setFmInstrumentParameter(n, buf1 + 8 + 48 * n);
            }

#if defined ( _MSC_VER )
            delete buf1;
#endif // _MSC_VER
#if defined ( __MINGW32__ )
            delete buf1;
#endif // __MINGW32__
        }
    }
    /* PMB */
    {
        char fn0[16];
        memcpy(fn0, eupHeader.pcm_file_name/*eupbuf + 0x6ea*/, 8); /* verified 0x06ea offset */
        fn0[8] = '\0';
        std::string fn1(std::string(fn0) + ".pmb");
        FILE *f = openFile_inPath(fn1, pmbPath);
        if (f != nullptr) {
#if defined ( _MSC_VER )
            struct stat statbuf1;
            fstat(fileno(f), &statbuf1);
            uint8_t *buf1 = new uint8_t[statbuf1.st_size];
            if (nullptr == buf1) {
                fprintf(stderr, "heap allocation problem.\n");
                std::fflush(stderr);
                fclose(f);
                return nullptr;
            }
#endif // _MSC_VER
#if defined ( __MINGW32__ )
            struct stat statbuf1;
            fstat(fileno(f), &statbuf1);
            uint8_t *buf1 = new uint8_t[statbuf1.st_size];
            if (nullptr == buf1) {
                fprintf(stderr, "heap allocation problem.\n");
                std::fflush(stderr);
                fclose(f);
                return nullptr;
            }
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
            struct stat statbuf1;
            fstat(fileno(f), &statbuf1);
            uint8_t buf1[statbuf1.st_size];
#endif // __GNUC__
            size_t pmbRead = fread(buf1, 1, statbuf1.st_size, f);
            if (pmbRead != statbuf1.st_size) {
                fprintf(stderr, "PMB not fully read: %zu instead of %lu.\n", pmbRead, statbuf1.st_size);
            }
            fclose(f);
            device->setPcmInstrumentParameters(buf1, statbuf1.st_size);
#if defined ( _MSC_VER )
            delete buf1;
#endif // _MSC_VER
#if defined ( __MINGW32__ )
            delete buf1;
#endif // __MINGW32__
        }
    }

    return eupbuf;
}

volatile std::sig_atomic_t signal_raised = 0;

void set_signal_raised(int signal) {
    signal_raised = 1;
}

#if defined ( _MSC_VER )
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char *argv[])
#endif // _MSC_VER
#if defined ( __MINGW32__ )
int main(int argc, char **argv)
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
int main(int argc, char **argv)
#endif // __GNUC__ && !__MINGW32__
{
    FILE *outputFile = nullptr;

    EUP_TownsEmulator *dev = new EUP_TownsEmulator;
    EUPPlayer *player = new EUPPlayer;
    /* signed 16 bit sample, little endian */
    dev->outputSampleUnsigned(false);
    dev->outputSampleLSBFirst(true);
    dev->outputSampleSize(streamAudioSampleOctectSize); /* octects a.k.a. bytes */
    dev->outputSampleChannels(streamAudioChannelsNum); /* 1 = monaural, 2 = stereophonic */
    dev->rate(streamAudioRate); /* Hz */

    /*fprintf(stderr, "sizeof(int16_t) = %d\n", (int)sizeof(int16_t));*/
    /*std::fflush(stderr);*/

    int c;
    while ((c = getopt(argc, argv, "v:d:o:")) != -1) {
        switch(c) {
            case 'v': {
                dev->volume(strtol(optarg, nullptr, 0));
            }
            break;
            case 'd': {
                std::istrstream str(optarg);
                while (!str.eof() && !str.fail()) {
                    int n;
                    str >> n;
                    dev->enable(n, false);
                }
            }
            break;
            case 'o': {
                outputFile = fopen(optarg, "wb");
                if (outputFile == nullptr) {
                    fprintf(stderr, "Cannot open output file(%s)\n", optarg);
                    std::fflush(stderr);
                    exit(1);
                }
                dev->output2File(true);
                dev->outputStream(outputFile);
            }
            break;
        }
    }

    if ( (nullptr != dev->outputStream_get()) && (true == dev->output2File_read()) ) {
        // audio_write_wav_header
        unsigned char hdr[0x80];

        memcpy(hdr,"RIFF", 4);
        int pcm_bytesize = pcm.count * streamAudioSampleOctectSize * streamAudioChannelsNum;
        hdr[4 + 0] = (pcm_bytesize + 44) & 0xff;
        hdr[4 + 1] = ((pcm_bytesize + 44)>>8) & 0xff;
        hdr[4 + 2] = ((pcm_bytesize + 44)>>16) & 0xff;
        hdr[4 + 3] = ((pcm_bytesize + 44)>>24) & 0xff;
        memcpy(hdr + 8,"WAVEfmt ", 8);
        // chunk length
        hdr[16 + 0] = (16) & 0xff;
        hdr[16 + 1] = ((16)>>8) & 0xff;
        hdr[16 + 2] = ((16)>>16) & 0xff;
        hdr[16 + 3] = ((16)>>24) & 0xff;
        // id, pcm id = 1 i.e. linear quantization
        hdr[20 + 0] = (1) & 0xff;
        hdr[20 + 1] = ((1)>>8) & 0xff;
        // audio stream channels
        hdr[22 + 0] = (streamAudioChannelsNum) & 0xff;
        hdr[22 + 1] = ((streamAudioChannelsNum)>>8) & 0xff;
        // audio stream frequency
        hdr[24 + 0] = (streamAudioRate) & 0xff;
        hdr[24 + 1] = ((streamAudioRate)>>8) & 0xff;
        hdr[24 + 2] = ((streamAudioRate)>>16) & 0xff;
        hdr[24 + 3] = ((streamAudioRate)>>24) & 0xff;
        // bytes per sec
        hdr[28 + 0] = (streamBytesPerSecond) & 0xff;
        hdr[28 + 1] = ((streamBytesPerSecond)>>8) & 0xff;
        hdr[28 + 2] = ((streamBytesPerSecond)>>16) & 0xff;
        hdr[28 + 3] = ((streamBytesPerSecond)>>24) & 0xff;
        // bytes per frame
        hdr[32 + 0] = (streamAudioChannelsNum * streamAudioSampleOctectSize) & 0xff;
        hdr[32 + 1] = ((streamAudioChannelsNum * streamAudioSampleOctectSize)>>8) & 0xff;
        // bits per sample
        hdr[34 + 0] = (streamAudioSampleOctectSize * 8) & 0xff;
        hdr[34 + 1] = ((streamAudioSampleOctectSize * 8)>>8) & 0xff;
        memcpy(hdr + 36, "data",4);
        hdr[40 + 0] = (pcm_bytesize) & 0xff;
        hdr[40 + 1] = ((pcm_bytesize)>>8) & 0xff;
        hdr[40 + 2] = ((pcm_bytesize)>>16) & 0xff;
        hdr[40 + 3] = ((pcm_bytesize)>>24) & 0xff;

        fseek(dev->outputStream_get(), 0, SEEK_SET);
        fwrite(hdr, 44, 1, dev->outputStream_get());
    }
    else {
#if defined ( _MSC_VER )
//    setmode(fileno(stdout), _O_BINARY);
#endif // _MSC_VER
#if defined ( __MINGW32__ )
//      setmode(fileno(stdout), _O_BINARY);
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
//      setmode(fileno(stdout), _O_BINARY);
#endif // __GNUC__ && !__MINGW32__
    }

    player->outputDevice(dev);

    int optindex = optind;
    if (optindex > argc-1) {
        fprintf(stderr, "usage: %s [-v vol] [-d ch[,ch...]] [-o output] EUP-filename\n", argv[0]);
        std::fflush(stderr);
        exit(1);
    }

    SDL_version compiled;
    SDL_VERSION(&compiled);
    printf("compiled with SDL version %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
    std::fflush(stdout);

    SDL_version linked;
    SDL_GetVersion(&linked);
    printf("linked SDL version %d.%d.%d\n", linked.major, linked.minor, linked.patch);
    std::fflush(stdout);

    /* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    /* Load the SDL library */
    if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_EVENTS) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    /* Log the list of available audio drivers */
    SDL_Log("Available audio drivers:");
    for (c = 0; c < SDL_GetNumAudioDrivers(); ++c) {
        SDL_Log("%i: %s", c, SDL_GetAudioDriver(c));
    }

    /* Log used audio driver */
    SDL_Log("Using audio driver: %s\n", SDL_GetCurrentAudioDriver());

    SDL_AudioSpec aRequested/*, aGranted*/;

    /* re-inizialize aRequested struct */
    std::memset(&aRequested, 0, sizeof(aRequested));

    aRequested.freq = streamAudioRate;
    aRequested.format = ( true == dev->outputSampleLSBFirst_read() ) ? AUDIO_S16LSB : AUDIO_S16MSB;
    aRequested.channels = streamAudioChannelsNum;
    aRequested.samples = streamAudioSamplesBlock;
    aRequested.callback = audio_callback;
    aRequested.userdata = nullptr;

/*    if (SDL_OpenAudio(&aRequested,&aGranted) < 0) {*/
    if (SDL_OpenAudio(&aRequested, nullptr) < 0) {
        printf("Audio open error!!\n");
        std::fflush(stdout);
        exit(1);
    }
/*    if (aRequested.freq != aGranted.freq) {
        SDL_Log("Granted freq %d\n", (int)aGranted.freq);
    }
    if (aRequested.format != aGranted.format) {
        SDL_Log("Granted format %d\n", (int)aGranted.format);
    }
    if (aRequested.channels != aGranted.channels) {
        SDL_Log("Granted channels %d\n", (int)aGranted.channels);
    }
    if (aRequested.samples != aGranted.samples) {
        SDL_Log("Granted samples %d\n", (int)aGranted.samples);
    }*/

    /* re-inizialize pcm struct */
    std::memset(&pcm, 0, sizeof(pcm));

    char const *nameOfEupFile = argv[optindex++];
    uint8_t *buf = EUPPlayer_readFile(player, dev, nameOfEupFile);
    if (buf == nullptr) {
        fprintf(stderr, "%s: read failed\n", argv[1]);
        std::fflush(stderr);
        exit(1);
    }

    player->startPlaying(buf + 2048 + 6);

    if (true == player->isPlaying()) {
        pcm.on = true;
        /* Let start the callback function play the audio chunk */
        SDL_PauseAudio(SDL_FALSE);
    }

    std::signal(SIGINT, set_signal_raised);
    while (player->isPlaying()) {
        player->nextTick();

        if (1 == signal_raised)
        {
            signal_raised = 0;
            break;
        }
    }

    pcm.on = false;
    /* Stop the callback function playing the audio chunk */
    SDL_PauseAudio(SDL_TRUE);

    if ( (nullptr != dev->outputStream_get()) && (true == dev->output2File_read()) ) {
        // audio_write_wav_header
        unsigned char littleEndianDWORD[4];

        int pcm_bytesize = pcm.count * streamAudioSampleOctectSize * streamAudioChannelsNum;
        littleEndianDWORD[0] = (pcm_bytesize) & 0xff;
        littleEndianDWORD[1] = ((pcm_bytesize)>>8) & 0xff;
        littleEndianDWORD[2] = ((pcm_bytesize)>>16) & 0xff;
        littleEndianDWORD[3] = ((pcm_bytesize)>>24) & 0xff;
        fseek(dev->outputStream_get(), 40, SEEK_SET);
        fwrite(littleEndianDWORD, 4, 1, dev->outputStream_get());

        pcm_bytesize += 4 /* WAVE format */ + 8 /* fmt sub-chunk header */ + 16 /* fmt sub-chunk length */ + 8 /* data sub-chunk header */;
        littleEndianDWORD[0] = (pcm_bytesize) & 0xff;
        littleEndianDWORD[1] = ((pcm_bytesize)>>8) & 0xff;
        littleEndianDWORD[2] = ((pcm_bytesize)>>16) & 0xff;
        littleEndianDWORD[3] = ((pcm_bytesize)>>24) & 0xff;
        fseek(dev->outputStream_get(), 4, SEEK_SET);
        fwrite(littleEndianDWORD, 4, 1, dev->outputStream_get());

        fclose(outputFile);
    }

    delete player;
    delete dev;
    delete buf;

    SDL_CloseAudio();
    SDL_Quit();

    return 0;
}
