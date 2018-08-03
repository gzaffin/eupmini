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

/*      Artistic Style
 *
 * ./astyle --style=stroustrup --convert-tabs --add-braces eupplay.cpp
 */

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <vector>
#include <strstream>
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
char * optarg = NULL;

int getopt(int argc, char ** argv, const char * opts)
{
    static int sp = 1;
    int c;
    const char * cp = NULL;

    if (sp == 1) {
        if ((optind >= argc) || (argv[optind][0] != '-') || (argv[optind][1] == '\0')) {
            return GETOPT_EOF;
        }
        else if (strcmp(argv[optind], "--") == NULL) {
            optind++;
            return GETOPT_EOF;
        }
    }

    optopt = c = argv[optind][sp];
    if (c == ':' || (cp = strchr(opts, c)) == NULL) {
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
        optarg = NULL;
    }

    return(c);
}

#endif  /* _GETOPT_H_ */
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
#include <SDL2/SDL.h>
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

static void audio_callback(void *param, Uint8 *data, int len)
{
    int i;
    int readData2Buffer = pcm.read_pos;

    int16_t * audio_buffer = (int16_t *)data;
    int16_t * source_buffer = (int16_t *)pcm.buffer;

    if (true != pcm.on) {
        memset(data, 0, len);
        len = 0; /* statement cuts off following for loop */
    }

    for(i = 0; i < len / 2; i++) {
        if (streamAudioBufferSamples <= readData2Buffer) {
            readData2Buffer = 0;
        }
	audio_buffer[i] = source_buffer[readData2Buffer++];
    }

    if (len) {
        pcm.count += i;
	pcm.read_pos = readData2Buffer;
    }
}

static string const downcase(string const &s)
{
    string r;
    for (string::const_iterator i = s.begin(); i != s.end(); i++) {
        r += (*i >= 'A' && *i <= 'Z') ? (*i - 'A' + 'a') : *i;
    }
    return r;
}

static string const upcase(string const &s)
{
    string r;
    for (string::const_iterator i = s.begin(); i != s.end(); i++) {
        r += (*i >= 'a' && *i <= 'z') ? (*i - 'a' + 'A') : *i;
    }
    return r;
}

static FILE *openFile_inPath(string const &filename, string const &path)
{
    FILE *f = NULL;
    vector<string> fn2;
    fn2.push_back(filename);
    fn2.push_back(downcase(filename));
    fn2.push_back(upcase(filename));

    string::const_iterator i = path.begin();
    while (i != path.end()) {
        string dir;
        while (i != path.end() && *i != PATH_DELIMITER_CHAR) {
            dir += *i++;
        }
        if (i != path.end() && *i == PATH_DELIMITER_CHAR) {
            i++;
        }
        for (vector<string>::const_iterator j = fn2.begin(); j != fn2.end(); j++) {
            string const filename(dir + "/" + *j);
            //cerr << "trying " << filename << endl;
            f = fopen(filename.c_str(), "rb");
            if (f != NULL) {
                //cerr << "loading " << filename << endl;
                return f;
            }
        }
    }
    if (f == NULL) {
        fprintf(stderr, "error finding %s\n", filename.c_str());
    }

    return f;
}

u_char *EUPPlayer_readFile(EUPPlayer *player,
                           TownsAudioDevice *device,
                           string const &nameOfEupFile)
{
    // ?��Ȥꤢ?��?��?��?��, TOWNS emu ?��Τߤ?��?��б?��.

    u_char *eupbuf = NULL;
    player->stopPlaying();

    {
        FILE *f = fopen(nameOfEupFile.c_str(), "rb");
        if (f == NULL) {
            perror(nameOfEupFile.c_str());
            return NULL;
        }

        struct stat statbufEupFile;
        if (fstat(fileno(f), &statbufEupFile) != 0) {
            perror(nameOfEupFile.c_str());
            fclose(f);
            return NULL;
        }

        if (statbufEupFile.st_size < 2048 + 6 + 6) {
            fprintf(stderr, "%s: too short file.\n", nameOfEupFile.c_str());
            fclose(f);
            return NULL;
        }

        eupbuf = new u_char[statbufEupFile.st_size];
        fread(eupbuf, statbufEupFile.st_size, 1, f);
        fclose(f);
    }

    player->tempo(eupbuf[2048 + 5] + 30);
    // ?��?��?��?��ƥ?��ݤ?��?��?��?��?��ΤĤ?��?��.  ?��?��?��?��?��?��?��?��?��?��?��?

    for (int n = 0; n < 32; n++) {
        player->mapTrack_toChannel(n, eupbuf[0x394 + n]);
    }

    for (int n = 0; n < 6; n++) {
        device->assignFmDeviceToChannel(eupbuf[0x6d4 + n]);
    }
    for (int n = 0; n < 8; n++) {
        device->assignPcmDeviceToChannel(eupbuf[0x6da + n]);
    }

    string fmbPath("." PATH_DELIMITER "C:\\tempGZ\\eupmini-Windows\\fmb\\");
    string pmbPath("." PATH_DELIMITER "C:\\tempGZ\\eupmini-Windows\\pmb\\");
    {
        char *s;
        s = getenv("EUP_FMINST");
        if (s != NULL) {
            fmbPath = string(s);
        }
        s = getenv("EUP_PCMINST");
        if (s != NULL) {
            pmbPath = string(s);
        }
        string eupDir = nameOfEupFile.substr(0, nameOfEupFile.rfind("/") + 1) + ".";
        fmbPath = eupDir + PATH_DELIMITER + fmbPath;
        pmbPath = eupDir + PATH_DELIMITER + pmbPath;
    }

    {
#if 0
        u_char instrument[] = {
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
        u_char instrument[] = {
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

    {
        char fn0[16];
        memcpy(fn0, eupbuf + 0x6e2, 8);
        fn0[8] = '\0';
        string fn1(string(fn0) + ".fmb");
        FILE *f = openFile_inPath(fn1, fmbPath);
        if (f != NULL) {
#if defined ( _MSC_VER )
            struct stat statbuf1;
            fstat(fileno(f), &statbuf1);
            u_char *buf1 = new u_char[statbuf1.st_size];
            if (NULL == buf1) {
                fprintf(stderr, "heap allocation problem.\n");
                fclose(f);
                return NULL;
            }
#endif // _MSC_VER
#if defined ( __MINGW32__ )
            struct stat statbuf1;
            fstat(fileno(f), &statbuf1);
            u_char *buf1 = new u_char[statbuf1.st_size];
            if (NULL == buf1) {
                fprintf(stderr, "heap allocation problem.\n");
                fclose(f);
                return NULL;
            }
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
            struct stat statbuf1;
            fstat(fileno(f), &statbuf1);
            u_char buf1[statbuf1.st_size];
#endif // __GNUC__
            fread(buf1, statbuf1.st_size, 1, f);
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

    {
        char fn0[16];
        memcpy(fn0, eupbuf + 0x6ea, 8);
        fn0[8] = '\0';
        string fn1(string(fn0) + ".pmb");
        FILE *f = openFile_inPath(fn1, pmbPath);
        if (f != NULL) {
#if defined ( _MSC_VER )
            struct stat statbuf1;
            fstat(fileno(f), &statbuf1);
            u_char *buf1 = new u_char[statbuf1.st_size];
            if (NULL == buf1) {
                fprintf(stderr, "heap allocation problem.\n");
                fclose(f);
                return NULL;
            }
#endif // _MSC_VER
#if defined ( __MINGW32__ )
            struct stat statbuf1;
            fstat(fileno(f), &statbuf1);
            u_char *buf1 = new u_char[statbuf1.st_size];
            if (NULL == buf1) {
                fprintf(stderr, "heap allocation problem.\n");
                fclose(f);
                return NULL;
            }
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
            struct stat statbuf1;
            fstat(fileno(f), &statbuf1);
            u_char buf1[statbuf1.st_size];
#endif // __GNUC__
            fread(buf1, statbuf1.st_size, 1, f);
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
#endif // __GNUC__
{
    FILE *outputFile = NULL;

    EUP_TownsEmulator *dev = new EUP_TownsEmulator;
    EUPPlayer *player = new EUPPlayer();
    /* signed 16 bit sample, little endian */
    dev->outputSampleUnsigned(false);
    dev->outputSampleLSBFirst(true);
    dev->outputSampleSize(streamAudioSampleOctectSize); /* octects a.k.a. bytes */
    dev->rate(streamAudioRate); /* Hz */

    /*fprintf(stderr, "sizeof(int16_t) = %d\n", (int)sizeof(int16_t));*/

    int c;
    while ((c = getopt(argc, argv, "v:d:o:")) != -1) {
        switch(c) {
            case 'v': {
                dev->volume(strtol(optarg, NULL, 0));
            }
            break;
            case 'd': {
                istrstream str(optarg);
                while (!str.eof() && !str.fail()) {
                    int n;
                    str >> n;
                    dev->enable(n, false);
                }
            }
            break;
            case 'o': {
                outputFile = fopen(optarg, "wb");
                if (outputFile == NULL) {
                    fprintf(stderr, "Cannot open output file(%s)\n", optarg);
                    exit(1);
                }
                dev->output2File(true);
            }
            break;
        }
    }

    if ( (NULL != outputFile) && (true == dev->output2File_read()) ) {
        dev->outputStream(outputFile);

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
        setmode(fileno(stdout), _O_BINARY);
#endif
        dev->outputStream(stdout);
        dev->output2File(false);
    }

    player->outputDevice(dev);

    int optindex = optind;
    if (optindex > argc-1) {
        fprintf(stderr, "usage: %s [-v vol] [-d ch[,ch...]] [-o output] EUP-filename\n", argv[0]);
        exit(1);
    }

    SDL_version compiled;
    SDL_VERSION(&compiled);
    printf("compiled with SDL version %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);

    SDL_version linked;
    SDL_GetVersion(&linked);
    printf("linked SDL version %d.%d.%d\n", linked.major, linked.minor, linked.patch);

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
    memset(&aRequested, 0, sizeof(aRequested));

    aRequested.freq = streamAudioRate;
    aRequested.format = ( true == dev->outputSampleLSBFirst_read() ) ? AUDIO_S16LSB : AUDIO_S16MSB;
    aRequested.channels = streamAudioChannelsNum;
    aRequested.samples = streamAudioSamplesBlock;
    aRequested.callback = audio_callback;
    aRequested.userdata = NULL;

/*    if (SDL_OpenAudio(&aRequested,&aGranted) < 0) {*/
    if (SDL_OpenAudio(&aRequested,NULL) < 0) {
        printf("Audio open error!!\n");
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
    memset(&pcm, 0, sizeof(pcm));

    char const *nameOfEupFile = argv[optindex++];
    u_char *buf = EUPPlayer_readFile(player, dev, nameOfEupFile);
    if (buf == NULL) {
        fprintf(stderr, "%s: read failed\n", argv[1]);
        exit(1);
    }

    player->startPlaying(buf + 2048 + 6);

    if (true == player->isPlaying()) {
        pcm.on = true;
        /* Let start the callback function play the audio chunk */
        SDL_PauseAudio(SDL_FALSE);
    }

    while (player->isPlaying()) {
        player->nextTick();
    }

    /* Stop the callback function playing the audio chunk */
    SDL_PauseAudio(SDL_TRUE);

    if (true == dev->output2File_read()) {
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
