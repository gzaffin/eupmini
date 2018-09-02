/*      Artistic Style
 *
 * ./astyle --style=stroustrup --convert-tabs --add-braces eupplayer_townsInternal.cpp
 */

#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#if defined ( __MINGW32__ )
#include <_bsd_types.h>
#endif
#include "eupplayer_townsInternal.hpp"

#ifdef DEBUG
#include <stdarg.h>
static void dbprintf(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fflush(stderr);
}
#define DB1(a) dbprintf(a)
#define DB2(a,b) dbprintf(a,b)
#define DB3(a,b,c) dbprintf(a,b,c)
#else
#define DB1(a)
#define DB2(a,b)
#define DB3(a,b,c)
#endif

/* EUP_TownsInternal_MonophonicAudioDevice */

class EUP_TownsInternal_MonophonicAudioDevice {
public:
    EUP_TownsInternal_MonophonicAudioDevice() {}
    virtual ~EUP_TownsInternal_MonophonicAudioDevice() {}
    virtual void setInstrumentParameter(u_char const *instrument) = 0;
    virtual void nextTick() = 0;
    virtual void note(int n, int onVelo, int offVelo, int gateTime) = 0;
    virtual void pitchBend(int value) = 0;
};

/* TownsFm */

class TownsFm : public EUP_TownsInternal_MonophonicAudioDevice {
    int _gateTime;
    int _channel;
    int _regBank;
    FILE *_port;
public:
    TownsFm(int channel);
    ~TownsFm();
    void setInstrumentParameter(u_char const *instrument);
    void nextTick();
    void note(int n, int onVelo, int offVelo, int gateTime);
    void pitchBend(int value);
    void writeReg(int addr, int value);
};

TownsFm::TownsFm(int channel)
{
    _port = fopen("/dev/port", "rbw");
    if (_port == NULL) {
        perror("TownsFm: /dev/port");
        exit(1);
    }
    _gateTime = 0;
    _channel = channel % 3;
    _regBank = channel / 3;

    this->writeReg(0x21, 0x00);
    this->writeReg(0x2c, 0x00);
    this->writeReg(0x27, 0x00);
}

TownsFm::~TownsFm()
{
    fclose(_port);
}

void TownsFm::setInstrumentParameter(u_char const *instrument)
{
    instrument += 8;
    int addr = 0x30+_channel;

    for (; addr < 0x90; addr += 4) {
        this->writeReg(addr, *instrument++);
    }

    addr += 0x20;

    for (; addr < 0xb8; addr += 4) {
        this->writeReg(addr, *instrument++);
    }
}

void TownsFm::nextTick()
{
    if (_gateTime <= 0) {
        return;
    }

    if (--_gateTime > 0) {
        return;
    }

    this->writeReg(0x28, 0x00 | _channel);
}

void TownsFm::note(int n, int onVelo, int offVelo, int gateTime)
{
//DB3("note: v=%d, gt=%d\n", onVelo, gateTime);
    _gateTime = gateTime;
    this->writeReg(0xa4 + _channel, 0x24);
    this->writeReg(0xa0 + _channel, 0x81);
    this->writeReg(0x28, 0xf0 | _channel);
}

void TownsFm::pitchBend(int value)
{
}

void TownsFm::writeReg(int addr, int value)
{
    DB3("writereg: addr=%02x, val=%02x\n", addr, value);
    for (;;) {
        fseek(_port, 0x04d8, SEEK_SET);
        if (!(getc(_port) & 0x80)) {
            break;
        }
        DB1("1\n");
    }
    fseek(_port, 0x04d8 + 4*_regBank, SEEK_SET);
    putc(addr, _port);
    fflush(_port);
    for (;;) {
        fseek(_port, 0x04d8, SEEK_SET);
        if (!(getc(_port) & 0x80)) {
            break;
        }
        DB1("2\n");
    }
    fseek(_port, 0x04da + 4*_regBank, SEEK_SET);
    putc(value, _port);
    fflush(_port);
}

/* TownsPcm */

class TownsPcm : public EUP_TownsInternal_MonophonicAudioDevice {
public:
    TownsPcm() {}
    ~TownsPcm() {}
    void setInstrumentParameter(u_char const *instrument);
    void nextTick();
    void note(int n, int onVelo, int offVelo, int gateTime);
    void pitchBend(int value);
};

void TownsPcm::setInstrumentParameter(u_char const *instrument)
{
}

void TownsPcm::nextTick()
{
}

void TownsPcm::note(int n, int onVelo, int offVelo, int gateTime)
{
}

void TownsPcm::pitchBend(int value)
{
}

/* EUP_TownsInternal */

EUP_TownsInternal::EUP_TownsInternal()
{
    {
        int n = 0;
        for (; n < _maxFmChannelNum; n++) {
            _dev[n] = new TownsFm(n);
        }
        for (; n < _maxChannelNum; n++) {
            _dev[n] = new TownsPcm;
        }
    }

    for (int n = 0; n < _maxFmInstrumentNum; n++) {
        _fmInstrument[n] = _fmInstrumentData + 8 + 48*n;
    }
}

EUP_TownsInternal::~EUP_TownsInternal()
{
    for (int n = 0; n < _maxChannelNum; n++) {
        delete _dev[n];
    }
}

void EUP_TownsInternal::setFmInstrumentParameter(int num, u_char const *instrument)
{
    if (0 <= num && num < _maxFmInstrumentNum) {
        memcpy(_fmInstrument[num], instrument, 48);
    }
}

void EUP_TownsInternal::nextTick()
{
    for (int n = 0; n < _maxChannelNum; n++) {
        _dev[n]->nextTick();
    }
}

void EUP_TownsInternal::note(int track, int n,
                             int onVelo, int offVelo, int gateTime)
{
    _dev[track]->note(n, onVelo, offVelo, gateTime);
}

void EUP_TownsInternal::pitchBend(int track, int value)
{
}

void EUP_TownsInternal::programChange(int track, int num)
{
    /* track -> channel ���Ѵ��򤷤ʤ���  */
    int channel = track;
    _dev[channel]->setInstrumentParameter(_fmInstrument[num]);
}

