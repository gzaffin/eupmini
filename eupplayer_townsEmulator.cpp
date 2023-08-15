// $Id: eupplayer_townsEmulator.cc,v 1.24 2000/04/12 23:14:35 hayasaka Exp $

/*      Artistic Style
 *
 * ./astyle --style=stroustrup --convert-tabs --add-braces eupplayer_townsEmulator.cpp
 */

#include <cstdint>
#include <cstdio>
#include <sys/stat.h>
#include <cassert>
#include <cstring>
#include <cmath>
#if defined ( __MINGW32__ )
#include <_bsd_types.h>
#endif
#if defined ( _MSC_VER )
#include <SDL.h>
#endif // _MSC_VER
#if defined ( __MINGW32__ )
#include <SDL2/SDL.h>
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
#include <SDL2/SDL.h>
#endif // __GNUC__
#include "eupplayer_townsEmulator.hpp"
#include "sintbl.hpp"

#if EUPPLAYER_LITTLE_ENDIAN
static inline uint16_t P2(uint8_t const * const p)
{
    return *(uint16_t const *)p;
}
static inline uint32_t P4(uint8_t const * const p)
{
    return *(uint32_t const *)p;
}
#else
static inline uint16_t P2(uint8_t const * const p)
{
    uint16_t const x0 = *p;
    uint16_t const x1 = *(p + 1);
    return x0 + (x1 << 8);
}
static inline uint32_t P4(uint8_t const * const p)
{
    uint32_t const x0 = P2(p);
    uint32_t const x1 = P2(p + 2);
    return x0 + (x1 << 16);
}
#endif

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
#define DB(a) dbprintf##a
#else
#define DB(a)
#endif

#define CHECK_CHANNEL_NUM(funcname, channel) \
  if (channel < 0 || _maxChannelNum <= channel) { \
    fprintf(stderr, "%s: channel number %d out of range\n", funcname, channel); \
    fflush(stderr); \
    return; \
  } \
  (void)false

/* TownsPcmSound */

class TownsPcmSound {
    char _name[9];
    int _id;
    int _numSamples;
    int _loopStart;
    int _loopLength;
    int _samplingRate;
    int _keyOffset;
    int _keyNote;
    signed char *_samples;
public:
    TownsPcmSound(uint8_t const *p);
    ~TownsPcmSound();
    int id() const
    {
        return _id;
    }
    int numSamples() const
    {
        return _numSamples;
    }
    int loopStart() const
    {
        return _loopStart;
    }
    int loopLength() const
    {
        return _loopLength;
    }
    int samplingRate() const
    {
        return _samplingRate;
    }
    int keyNote() const
    {
        return _keyNote;
    }
    signed char const * samples() const
    {
        return _samples;
    }
};

TownsPcmSound::TownsPcmSound(uint8_t const *p)
{
    {
        u_int n = 0;
        for (; n < sizeof(_name)-1; n++) {
            _name[n] = p[n];
        }
        _name[n] = '\0';
    }
    _id = P4(p+8);
    _numSamples = P4(p+12);
    _loopStart = P4(p+16);
    _loopLength = P4(p+20);
    _samplingRate = (uint32_t)(P2(p+24)) * (1000*0x10000/0x62);
    _keyOffset = P2(p+26);
    _keyNote = *(uint8_t*)(p+28);
    _samples = new signed char[_numSamples];
    for (int i = 0; i < _numSamples; i++) {
        int n = p[32+i];
        _samples[i] = (n >= 0x80)?(n & 0x7f):(-n);
    }
    //cerr << this->describe() << '\n';
}

TownsPcmSound::~TownsPcmSound()
{
    if (_samples != NULL) {
        delete _samples;
    }
}

/* TownsPcmEnvelope */

class TownsPcmEnvelope {
    enum State { _s_ready=0, _s_attacking, _s_decaying, _s_sustaining, _s_releasing };
    State _state;
    State _oldState;
    int _currentLevel;
    int _rate;
    int _tickCount;
    int _totalLevel;
    int _attackRate;
    int _decayRate;
    int _sustainLevel;
    int _sustainRate;
    int _releaseLevel;
    int _releaseRate;
    int _rootKeyOffset;
public:
    TownsPcmEnvelope(TownsPcmEnvelope const *e);
    TownsPcmEnvelope(uint8_t const *p);
    ~TownsPcmEnvelope();
    void start(int rate);
    void release();
    int nextTick();
    int state()
    {
        return (int)(_state);
    }
    int rootKeyOffset()
    {
        return _rootKeyOffset;
    }
};

TownsPcmEnvelope::TownsPcmEnvelope(TownsPcmEnvelope const *e)
{
    memcpy(this, e, sizeof(*this));
}

TownsPcmEnvelope::TownsPcmEnvelope(uint8_t const *p)
{
    _state = _s_ready;
    _oldState = _s_ready;
    _currentLevel = 0;
    _totalLevel = *(uint8_t*)(p+0);
    _attackRate = *(uint8_t*)(p+1) * 10;
    _decayRate = *(uint8_t*)(p+2) * 10;
    _sustainLevel = *(uint8_t*)(p+3);
    _sustainRate = *(uint8_t*)(p+4) * 20;
    _releaseRate = *(uint8_t*)(p+5) * 10;
    _rootKeyOffset = *(char*)(p+6);
    //cerr << this->describe() << '\n';
}

TownsPcmEnvelope::~TownsPcmEnvelope()
{
}

void TownsPcmEnvelope::start(int rate)
{
    _state = _s_attacking;
    _currentLevel = 0;
    _rate = rate;
    _tickCount = 0;
}

void TownsPcmEnvelope::release()
{
    if (_state == _s_ready) {
        return;
    }
    _state = _s_releasing;
    _releaseLevel = _currentLevel;
    _tickCount = 0;
}

int TownsPcmEnvelope::nextTick()
{
    if (_oldState != _state) {
        switch (_oldState) {
        default:
            break;
        };
        _oldState = _state;
    }
    switch (_state) {
    case _s_ready:
        return 0;
        break;
    case _s_attacking:
        if (_attackRate == 0) {
            _currentLevel = _totalLevel;
        }
        else if (_attackRate >= 1270) {
            _currentLevel = 0;
        }
        else {
            _currentLevel = (_totalLevel*(_tickCount++)*(int64_t)1000) / (_attackRate*_rate);
        }
        if (_currentLevel >= _totalLevel) {
            _currentLevel = _totalLevel;
            _state = _s_decaying;
            _tickCount = 0;
        }
        break;
    case _s_decaying:
        if (_decayRate == 0) {
            _currentLevel = _sustainLevel;
        }
        else if (_decayRate >= 1270) {
            _currentLevel = _totalLevel;
        }
        else {
            _currentLevel = _totalLevel;
            _currentLevel -= ((_totalLevel-_sustainLevel)*(_tickCount++)*(int64_t)1000) / (_decayRate*_rate);
        }
        if (_currentLevel <= _sustainLevel) {
            _currentLevel = _sustainLevel;
            _state = _s_sustaining;
            _tickCount = 0;
        }
        break;
    case _s_sustaining:
        if (_sustainRate == 0) {
            _currentLevel = 0;
        }
        else if (_sustainRate >= 2540) {
            _currentLevel = _sustainLevel;
        }
        else {
            _currentLevel = _sustainLevel;
            _currentLevel -= (_sustainLevel*(_tickCount++)*(int64_t)1000) / (_sustainRate*_rate);
        }
        if (_currentLevel <= 0) {
            _currentLevel = 0;
            _state = _s_ready;
            _tickCount = 0;
        }
        break;
    case _s_releasing:
        if (_releaseRate == 0) {
            _currentLevel = 0;
        }
        else if (_releaseRate >= 1270) {
            _currentLevel = _releaseLevel;
        }
        else {
            _currentLevel = _releaseLevel;
            _currentLevel -= (_releaseLevel*(_tickCount++)*(int64_t)1000) / (_releaseRate*_rate);
        }
        if (_currentLevel <= 0) {
            _currentLevel = 0;
            _state = _s_ready;
        }
        break;
    default:
        // ここには来ないはず
        break;
    };

    return _currentLevel;
}

/* TownsPcmInstrument */

class TownsPcmInstrument {
    enum { _maxSplitNum = 8, };
    char _name[9];
    int _split[_maxSplitNum];
    int _soundId[_maxSplitNum];
    TownsPcmSound const *_sound[_maxSplitNum];
    TownsPcmEnvelope *_envelope[_maxSplitNum];
public:
    TownsPcmInstrument(uint8_t const *p);
    TownsPcmInstrument();
    void registerSound(TownsPcmSound const *sound);
    TownsPcmSound const *findSound(int note) const;
    TownsPcmEnvelope const *findEnvelope(int note) const;
};

TownsPcmInstrument::TownsPcmInstrument(uint8_t const *p)
{
    {
        u_int n = 0;
        for (; n < sizeof(_name)-1; n++) {
            _name[n] = p[n];
        }
        _name[n] = '\0';
    }
    for (int n = 0; n < _maxSplitNum; n++) {
        _split[n] = P2(p+16+2*n);
        _soundId[n] = P4(p+32+4*n);
        _sound[n] = NULL;
        _envelope[n] = new TownsPcmEnvelope(p+64+8*n);
    }
    //cerr << this->describe() << '\n';
}

TownsPcmInstrument::TownsPcmInstrument()
{
}

void TownsPcmInstrument::registerSound(TownsPcmSound const *sound)
{
    for (int i = 0; i < _maxSplitNum; i++) {
        if (_soundId[i] == sound->id()) {
            _sound[i] = sound;
        }
    }
}

TownsPcmSound const *TownsPcmInstrument::findSound(int note) const
{
    // 少なくともどれかの split を選択できるようにしておこう
    int splitNum;
    for (splitNum = 0; splitNum < _maxSplitNum-1; splitNum++) {
        if (note <= _split[splitNum]) {
            break;
        }
    }
    return _sound[splitNum];
}

TownsPcmEnvelope const *TownsPcmInstrument::findEnvelope(int note) const
{
    // 少なくともどれかの split を選択できるようにしておこう
    int splitNum;
    for (splitNum = 0; splitNum < _maxSplitNum-1; splitNum++) {
        if (note <= _split[splitNum]) {
            break;
        }
    }
    return _envelope[splitNum];
}

/* TownsFmEmulator_Operator */

TownsFmEmulator_Operator::TownsFmEmulator_Operator()
{
    _state = _s_ready;
    _oldState = _s_ready;
    _currentLevel = ((int64_t)0x7f << 31);
    _phase = 0;
    _lastOutput = 0;
    _feedbackLevel = 0;
    _detune = 0;
    _multiple = 1;
    _specifiedTotalLevel = 127;
    _keyScale = 0;
    _specifiedAttackRate = 0;
    _specifiedDecayRate = 0;
    _specifiedSustainRate = 0;
    _specifiedReleaseRate = 15;
    this->velocity(0);
}

TownsFmEmulator_Operator::~TownsFmEmulator_Operator()
{
}

void TownsFmEmulator_Operator::velocity(int velo)
{
    _velocity = velo;
    _totalLevel = (((int64_t)_specifiedTotalLevel << 31) +
                   ((int64_t)(127-_velocity) << 29));
    _sustainLevel = ((int64_t)_specifiedSustainLevel << (31+2));
}

void TownsFmEmulator_Operator::feedbackLevel(int level)
{
    _feedbackLevel = level;
}

void TownsFmEmulator_Operator::setInstrumentParameter(u_char const *instrument)
{
    _detune = (instrument[8] >> 4) & 7;
    _multiple = instrument[8] & 15;
    _specifiedTotalLevel = instrument[12] & 127;
    _keyScale = (instrument[16] >> 6) & 3;
    _specifiedAttackRate = instrument[16] & 31;
    _specifiedDecayRate = instrument[20] & 31;
    _specifiedSustainRate = instrument[24] & 31;
    _specifiedSustainLevel = (instrument[28] >> 4) & 15;
    _specifiedReleaseRate = instrument[28] & 15;
    _state = _s_ready; // 本物ではどうなのかな?
    this->velocity(_velocity);
}

void TownsFmEmulator_Operator::keyOn()
{
    _state = _s_attacking;
    _tickCount = 0;
    _phase = 0; // どうも、実際こうらしい
    _currentLevel = ((int64_t)0x7f << 31); // これも、実際こうらしい
}

void TownsFmEmulator_Operator::keyOff()
{
    if (_state != _s_ready) {
        _state = _s_releasing;
    }
}

void TownsFmEmulator_Operator::frequency(int freq)
{
    int r;

    //DB(("freq=%d=%d[Hz]\n", freq, freq/262205));
    _frequency = freq;

    r = _specifiedAttackRate;
    if (r != 0) {
        r = r * 2 + (keyscaleTable[freq/262205] >> (3-_keyScale));
        if (r >= 64) {
            r = 63; // するべきなんだろうとは思うんだけど (赤p.207)
        }
    }
#if 0
    _attackRate = 0x80;
    _attackRate *= powtbl[(r&3) << 7];
    _attackRate <<= 16 + (r >> 2);
    _attackRate >>= 1;
    _attackRate /= 9; // r == 4 のとき、0-96db が 8970.24ms
    //_attackRate /= 4; // r == 4 のとき、0-96db が 8970.24ms
    //DB(("AR=%d->%d, 0-96[db]=%d[ms]\n", _specifiedAttackRate, r, (((int64_t)0x80<<31) * 1000) / _attackRate));
#else
    {
        r = 63 - r;
        int64_t t;
        if (_specifiedTotalLevel >= 128) {
            t = 0;
        }
        else {
            t = powtbl[(r&3) << 7];
            t <<= (r >> 2);
            t *= 41; // r == 4 のとき、0-96db が 8970.24ms
            t >>= (15 + 5);
            t *= 127 - _specifiedTotalLevel;
            t /= 127;
        }
        _attackTime = t; // 1 秒 == (1 << 12)
        //DB(("AR=%d->%d, 0-96[db]=%d[ms]\n", _specifiedAttackRate, r, (int)((t*1000)>>12)));
    }
#endif

    r = _specifiedDecayRate;
    if (r != 0) {
        r = r * 2 + (keyscaleTable[freq/262205] >> (3-_keyScale));
        if (r >= 64) {
            r = 63;
        }
    }
    //DB(("DR=%d->%d\n", _specifiedDecayRate, r));
    _decayRate = 0x80;
    _decayRate *= powtbl[(r&3) << 7];
    _decayRate <<= 16 + (r >> 2);
    _decayRate >>= 1;
    _decayRate /= 124; // r == 4 のとき、0-96db が 123985.92ms

    r = _specifiedSustainRate;
    if (r != 0) {
        r = r * 2 + (keyscaleTable[freq/262205] >> (3-_keyScale));
        if (r >= 64) {
            r = 63;
        }
    }
    //DB(("SR=%d->%d\n", _specifiedSustainRate, r));
    _sustainRate = 0x80;
    _sustainRate *= powtbl[(r&3) << 7];
    _sustainRate <<= 16 + (r >> 2);
    _sustainRate >>= 1;
    _sustainRate /= 124;

    r = _specifiedReleaseRate;
    if (r != 0) {
        r = r * 2 + 1; // このタイミングで良いのかわからん
        r = r * 2 + (keyscaleTable[freq/262205] >> (3-_keyScale));
        // KS による補正はあるらしい。赤p.206 では記述されてないけど。
        if (r >= 64) {
            r = 63;
        }
    }
    //DB(("RR=%d->%d\n", _specifiedReleaseRate, r));
    _releaseRate = 0x80;
    _releaseRate *= powtbl[(r&3) << 7];
    _releaseRate <<= 16 + (r >> 2);
    _releaseRate >>= 1;
    _releaseRate /= 124;
}

int TownsFmEmulator_Operator::nextTick(int rate, int phaseShift)
{
    // sampling ひとつ分進める

    if (_oldState != _state) {
        //DB(("state %d -> %d\n", _oldState, _state));
        //DB(("(currentLevel = %08x %08x)\n", (int)(_currentLevel>>32), (int)(_currentLevel)));
        switch (_oldState) {
        default:
            break;
        };
        _oldState = _state;
    }

    switch (_state) {
    case _s_ready:
        return 0;
        break;
    case _s_attacking:
        ++_tickCount;
        if (_attackTime <= 0) {
            _currentLevel = 0;
            _state = _s_decaying;
        }
        else {
            int i = ((int64_t)_tickCount << (12+10)) / ((int64_t)rate * _attackTime);
            if (i >= 1024) {
                _currentLevel = 0;
                _state = _s_decaying;
            }
            else {
                _currentLevel = attackOut[i];
                _currentLevel <<= 31 - 8;
                //DB(("this=%08x, count=%d, time=%d, i=%d, out=%04x\n", this, _tickCount, _attackTime, i, attackOut[i]));
            }
        }
        break;
    case _s_decaying:
#if 1
        _currentLevel += _decayRate / rate;
        if (_currentLevel >= _sustainLevel) {
            _currentLevel = _sustainLevel;
            _state = _s_sustaining;
        }
#endif
        break;
    case _s_sustaining:
#if 1
        _currentLevel += _sustainRate / rate;
        if (_currentLevel >= ((int64_t)0x7f << 31)) {
            _currentLevel = ((int64_t)0x7f << 31);
            _state = _s_ready;
        }
#endif
        break;
    case _s_releasing:
#if 1
        _currentLevel += _releaseRate / rate;
        if (_currentLevel >= ((int64_t)0x7f << 31)) {
            _currentLevel = ((int64_t)0x7f << 31);
            _state = _s_ready;
        }
#endif
        break;
    default:
        // ここには来ないはず
        break;
    };

    int64_t level = _currentLevel + _totalLevel;
    int64_t output = 0;
    if (level < ((int64_t)0x7f << 31)) {
        int const feedback[] = {
            0,
            0x400 / 16,
            0x400 / 8,
            0x400 / 4,
            0x400 / 2,
            0x400,
            0x400 * 2,
            0x400 * 4,
        };

        _phase &= 0x3ffff;
        phaseShift >>= 2; // 正しい変調量は?  3 じゃ小さすぎで 2 じゃ大きいような。
        phaseShift += (((int64_t)(_lastOutput) * feedback[_feedbackLevel]) >> 16); // 正しい変調量は?  16から17の間のようだけど。
        output = sintbl[((_phase >> 7) + phaseShift) & 0x7ff];
        output >>= (level >> 34); // 正しい減衰量は?
        output *= powtbl[511 - ((level>>25)&511)];
        output >>= 16;
        output >>= 1;

        if (_multiple > 0) {
            _phase += (_frequency * _multiple) / rate;
        }
        else {
            _phase += _frequency / (rate << 1);
        }
    }

    _lastOutput = output;
    return output;
}

/* TownsFmEmulator */

TownsFmEmulator::TownsFmEmulator()
{
    _control7 = 127;
    _offVelocity = 0;
    _gateTime = 0;
    _note = 40;
    _frequency = 440;
    _frequencyOffs = 0x2000;
    _algorithm = 7;
    _opr = new TownsFmEmulator_Operator[_numOfOperators];
    this->velocity(0);
}

TownsFmEmulator::~TownsFmEmulator()
{
    delete [] _opr;
}

void TownsFmEmulator::velocity(int velo)
{
    EUP_TownsEmulator_MonophonicAudioSynthesizer::velocity(velo);
#if 0
    int v = (velo * _control7) >> 7; // これだと精度良くないですね
#else
    int v = velo + (_control7 - 127) * 4;
#endif
    bool iscarrier[8][4] = {
        { false, false, false,  true, }, /*0*/
        { false, false, false,  true, }, /*1*/
        { false, false, false,  true, }, /*2*/
        { false, false, false,  true, }, /*3*/
        { false,  true, false,  true, }, /*4*/
        { false,  true,  true,  true, }, /*5*/
        { false,  true,  true,  true, }, /*6*/
        {  true,  true,  true,  true, }, /*7*/
    };
    for (int opr = 0; opr < 4; opr++)
        if (iscarrier[_algorithm][opr]) {
            _opr[opr].velocity(v);
        }
        else {
            _opr[opr].velocity(127);
        }
}

void TownsFmEmulator::setControlParameter(int control, int value)
{
    switch (control) {
    case 7:
        _control7 = value;
        this->velocity(this->velocity());
        break;
    case 10:
        // panpot
        break;
    default:
        fprintf(stderr, "TownsFmEmulator::setControlParameter: unknown control %d, val=%d\n", control, value);
        fflush(stderr);
        break;
    };
}

void TownsFmEmulator::setInstrumentParameter(u_char const *fmInst,
        u_char const *pcmInst)
{
    u_char const *instrument = fmInst;

    if (instrument == NULL) {
        fprintf(stderr, "%s@%p: can not set null instrument\n",
                "TownsFmEmulator::setInstrumentParameter", this);
        fflush(stderr);
        return;
    }

    _algorithm = instrument[32] & 7;
    _opr[0].feedbackLevel((instrument[32] >> 3) & 7);
    _opr[1].feedbackLevel(0);
    _opr[2].feedbackLevel(0);
    _opr[3].feedbackLevel(0);
    _opr[0].setInstrumentParameter(instrument + 0);
    _opr[1].setInstrumentParameter(instrument + 2);
    _opr[2].setInstrumentParameter(instrument + 1);
    _opr[3].setInstrumentParameter(instrument + 3);
}

void TownsFmEmulator::nextTick(int *outbuf, int buflen)
{
    // steptime ひとつ分進める

    if (_gateTime > 0) {
        if (--_gateTime <= 0) {
            this->velocity(_offVelocity);
            for (int i = 0; i < _numOfOperators; i++) {
                _opr[i].keyOff();
            }
        }
    }

    if (this->velocity() == 0) {
        return;
    }

    for (int i = 0; i < buflen; i++) {
        int d = 0;
        int d1, d2, d3, d4;
        switch (_algorithm) {
        case 0:
            d1 = _opr[0].nextTick(this->rate(), 0);
            d2 = _opr[1].nextTick(this->rate(), d1);
            d3 = _opr[2].nextTick(this->rate(), d2);
            d4 = _opr[3].nextTick(this->rate(), d3);
            d = d4;
            break;
        case 1:
            d1 = _opr[0].nextTick(this->rate(), 0);
            d2 = _opr[1].nextTick(this->rate(), 0);
            d3 = _opr[2].nextTick(this->rate(), d1+d2);
            d4 = _opr[3].nextTick(this->rate(), d3);
            d = d4;
            break;
        case 2:
            d1 = _opr[0].nextTick(this->rate(), 0);
            d2 = _opr[1].nextTick(this->rate(), 0);
            d3 = _opr[2].nextTick(this->rate(), d2);
            d4 = _opr[3].nextTick(this->rate(), d1+d3);
            d = d4;
            break;
        case 3:
            d1 = _opr[0].nextTick(this->rate(), 0);
            d2 = _opr[1].nextTick(this->rate(), d1);
            d3 = _opr[2].nextTick(this->rate(), 0);
            d4 = _opr[3].nextTick(this->rate(), d2+d3);
            d = d4;
            break;
        case 4:
            d1 = _opr[0].nextTick(this->rate(), 0);
            d2 = _opr[1].nextTick(this->rate(), d1);
            d3 = _opr[2].nextTick(this->rate(), 0);
            d4 = _opr[3].nextTick(this->rate(), d3);
            d = d2 + d4;
            break;
        case 5:
            d1 = _opr[0].nextTick(this->rate(), 0);
            d2 = _opr[1].nextTick(this->rate(), d1);
            d3 = _opr[2].nextTick(this->rate(), d1);
            d4 = _opr[3].nextTick(this->rate(), d1);
            d = d2 + d3 + d4;
            break;
        case 6:
            d1 = _opr[0].nextTick(this->rate(), 0);
            d2 = _opr[1].nextTick(this->rate(), d1);
            d3 = _opr[2].nextTick(this->rate(), 0);
            d4 = _opr[3].nextTick(this->rate(), 0);
            d = d2 + d3 + d4;
            break;
        case 7:
            d1 = _opr[0].nextTick(this->rate(), 0);
            d2 = _opr[1].nextTick(this->rate(), 0);
            d3 = _opr[2].nextTick(this->rate(), 0);
            d4 = _opr[3].nextTick(this->rate(), 0);
            d = d1 + d2 + d3 + d4;
            break;
        default:
            break;
        };

        outbuf[i] += d;
    }
}

void TownsFmEmulator::note(int n, int onVelo, int offVelo, int gateTime)
{
    _note = n;
    this->velocity(onVelo);
    _offVelocity = offVelo;
    _gateTime = gateTime;
    this->recalculateFrequency();
    for (int i = 0; i < _numOfOperators; i++) {
        _opr[i].keyOn();
    }
}

void TownsFmEmulator::pitchBend(int value)
{
    _frequencyOffs = value;
    this->recalculateFrequency();
}

void TownsFmEmulator::recalculateFrequency()
{
    // MIDI とも違うし....
    // どういう仕様なんだろうか?
    // と思ったら、なんと、これ (↓) が正解らしい。
    int64_t basefreq = frequencyTable[_note];
    int cfreq = frequencyTable[_note - (_note % 12)];
    int oct = _note / 12;
    int fnum = (basefreq << 13) / cfreq; // OPL の fnum と同じようなもの。
    fnum += _frequencyOffs - 0x2000;
    if (fnum < 0x2000) {
        fnum += 0x2000;
        oct--;
    }
    if (fnum >= 0x4000) {
        fnum -= 0x2000;
        oct++;
    }

    // _frequency は最終的にバイアス 256*1024 倍
    _frequency = (frequencyTable[oct*12] * (int64_t)fnum) >> (13 - 10);

    for (int i = 0; i < _numOfOperators; i++) {
        _opr[i].frequency(_frequency);
    }
}

/* TownsPcmEmulator */


TownsPcmEmulator::TownsPcmEmulator()
{
    _control7 = 127;
    this->velocity(0);
    _gateTime = 0;
    _frequencyOffs = 0x2000;
    _currentInstrument = NULL;
    _currentEnvelope = NULL;
    _currentSound = NULL;
}

TownsPcmEmulator::~TownsPcmEmulator()
{
}

void TownsPcmEmulator::setControlParameter(int control, int value)
{
    switch (control) {
    case 0:
        // Bank Select (for devices with more than 128 programs)
        // base for "program change" commands
        if (value > 0) {
            fprintf(stderr, "warning: unsupported Bank Select: %d\n", value);
        }
        break;

    case 1:
        // Modulation controls a vibrato effect (pitch, loudness, brighness)
        if (value > 0) {
            fprintf(stderr, "warning: use of unimplemented PCM Modulation: %d\n", value);
        }
        break;

    case 7:
        _control7 = value;
        break;

    case 10:
        // panpot - rf5c68 seems to have an 8-bit pan register where low-nibble controls output to left
        // speaker and high-nibble the right..
        //value -= 0x40;
        //_volL = 0x10 - (0x10*value/0x40);
        //_volR = 0x10 + (0x10*value/0x40);
        break;

    case 11:
        // MIDI: Expression  - testcase; Passionate Ocean
        // Expression is a "percentage" of volume (CC7).
        //_expression= value;
        //if (value != 127) {
        //    fprintf(stderr, "warning: song uses unimplemented Expression control\n");
        //}
        break;

    case 64:
        // Sustain Pedal (on/off) testcase: Bach: Aria on G String"
        if (value > 0) {
            fprintf(stderr, "warning: use of unimplemented PCM Sustain Pedal: %d\n", value);
        }
        break;

    default:
        fprintf(stderr, "TownsFmEmulator::setControlParameter: unknown control %d, val=%d\n", control, value);
        fflush(stderr);
        break;
    };
}

void TownsPcmEmulator::setInstrumentParameter(uint8_t const *fmInst,
        uint8_t const *pcmInst)
{
    uint8_t const *instrument = pcmInst;
    if (instrument == NULL) {
        fprintf(stderr, "%s@%p: can not set null instrument\n",
                "TownsPcmEmulator::setInstrumentParameter", this);
        fflush(stderr);
        return;
    }
    _currentInstrument = (TownsPcmInstrument*)instrument;
    //fprintf(stderr, "0x%08x: program change (to 0x%08x)\n", this, instrument);
    //fflush(stderr);
}

void TownsPcmEmulator::nextTick(int *outbuf, int buflen)
{
    // steptime ひとつ分進める

    if (_currentEnvelope == NULL) {
        return;
    }
    if (_gateTime > 0 && --_gateTime <= 0) {
        this->velocity(_offVelocity);
        _currentEnvelope->release();
    }
    if (_currentEnvelope->state() == 0) {
        this->velocity(0);
    }
    if (this->velocity() == 0) {
        delete _currentEnvelope;
        _currentEnvelope = NULL;
        return;
    }

    uint32_t phaseStep;
    {
        int64_t ps = frequencyTable[_note];
        ps *= powtbl[_frequencyOffs>>4];
        ps /= frequencyTable[_currentSound->keyNote() - _currentEnvelope->rootKeyOffset()];
        ps *= _currentSound->samplingRate();
        ps /= this->rate();
        ps >>= 16;
        phaseStep = ps;
    }
    int loopLength = _currentSound->loopLength() << 16; // あらかじめ計算して
    int numSamples = _currentSound->numSamples() << 16; // おくのは危険だぞ
    signed char const *soundSamples = _currentSound->samples();
    for (int i = 0; i < buflen; i++) {
        if (loopLength > 0)
            while (_phase >= numSamples) {
                _phase -= loopLength;
            }
        else if (_phase >= numSamples) {
            _gateTime = 0;
            this->velocity(0);
            delete _currentEnvelope;
            _currentEnvelope = NULL;
            // 上との関係もあるしもっといい方法がありそう
            break;
        }

        // 線型補間する。
        int output;
        {
            uint32_t phase0 = _phase;
            uint32_t phase1 = _phase + 0x10000;
            if (phase1 >= numSamples) {
                // it's safe even if loopLength == 0, because soundSamples[] is extended by 1 and filled with 0 (see TownsPcmSound::TownsPcmSound).
                phase1 -= loopLength;
            }
            phase0 >>= 16;
            phase1 >>= 16;

            int weight1 = _phase & 0xffff;
            int weight0 = 0x10000 - weight1;

            output = soundSamples[phase0] * weight0 + soundSamples[phase1] * weight1;
            output >>= 16;
        }

        output *= this->velocity(); // 信じられないかも知れないけど、FM と違うんです。
        output <<= 1;
        output *= _currentEnvelope->nextTick();
        output >>= 7;
        output *= _control7; // 正しい減衰量は?
        output >>= 7;
        // FM との音量バランスを取る。
        output *= 185; // くらい?  半端ですねぇ。
        output >>= 8;
        outbuf[i] += output;
        _phase += phaseStep;
    }
}

void TownsPcmEmulator::note(int n, int onVelo, int offVelo, int gateTime)
{
    _note = n;
    this->velocity(onVelo);
    _offVelocity = offVelo;
    _gateTime = gateTime;
    _phase = 0;
    if (_currentInstrument != NULL) {
        _currentSound = _currentInstrument->findSound(n);
        _currentEnvelope = new TownsPcmEnvelope(_currentInstrument->findEnvelope(n));
        _currentEnvelope->start(this->rate());
    }
    else {
        _currentSound = NULL;
    }
}

void TownsPcmEmulator::pitchBend(int value)
{
    _frequencyOffs = value;
}

/* EUP_TownsEmulator_Channel */

EUP_TownsEmulator_Channel::EUP_TownsEmulator_Channel()
{
    _dev[0] = NULL;
    _lastNotedDeviceNum = 0;
}

EUP_TownsEmulator_Channel::~EUP_TownsEmulator_Channel()
{
    for (int n = 0; _dev[n] != NULL; n++) {
        delete _dev[n];
    }
}

void EUP_TownsEmulator_Channel::add(EUP_TownsEmulator_MonophonicAudioSynthesizer *device)
{
    for (int n = 0; n < _maxDevices; n++)
        if (_dev[n] == NULL) {
            _dev[n] = device;
            _dev[n+1] = NULL;
            break;
        }
}

void EUP_TownsEmulator_Channel::note(int note, int onVelo, int offVelo, int gateTime)
{
    int n = _lastNotedDeviceNum;
    if (_dev[n] == NULL || _dev[n+1] == NULL) {
        n = 0;
    }
    else {
        n++;
    }

    if (_dev[n] != NULL) {
        _dev[n]->note(note, onVelo, offVelo, gateTime);
    }

    _lastNotedDeviceNum = n;
    //fprintf(stderr, "ch=%08x, dev=%d, note=%d, on=%d, off=%d, gate=%d\n",
    // this, n, note, onVelo, offVelo, gateTime);
    //fflush(stderr);
}

void EUP_TownsEmulator_Channel::setControlParameter(int control, int value)
{
    // いいのかこれで?
    for (int n = 0; _dev[n] != NULL; n++) {
        _dev[n]->setControlParameter(control, value);
    }
}

void EUP_TownsEmulator_Channel::setInstrumentParameter(uint8_t const *fmInst,
        uint8_t const *pcmInst)
{
    for (int n = 0; _dev[n] != NULL; n++) {
        _dev[n]->setInstrumentParameter(fmInst, pcmInst);
    }
}

void EUP_TownsEmulator_Channel::pitchBend(int value)
{
    // いいのかこれで?
    for (int n = 0; _dev[n] != NULL; n++) {
        _dev[n]->pitchBend(value);
    }
}

void EUP_TownsEmulator_Channel::nextTick(int *outbuf, int buflen)
{
    for (int n = 0; _dev[n] != NULL; n++) {
        _dev[n]->nextTick(outbuf, buflen);
    }
}

void EUP_TownsEmulator_Channel::rate(int r)
{
    for (int n = 0; _dev[n] != NULL; n++) {
        _dev[n]->rate(r);
    }
}

/* EUP_TownsEmulator */

EUP_TownsEmulator::EUP_TownsEmulator()
{
    _ostr = NULL;
    for (int n = 0; n < _maxChannelNum; n++) {
        _channel[n] = new EUP_TownsEmulator_Channel;
        _enabled[n] = true;
    }
    this->outputSampleUnsigned(true);
    this->outputSampleSize(1);
    this->outputSampleLSBFirst(true);
    this->rate(8000);
    std::memset(&_fmInstrumentData[0], 0, sizeof(_fmInstrumentData));
    for (int n = 0; n < _maxFmInstrumentNum; n++) {
        _fmInstrument[n] = _fmInstrumentData + 8 + 48*n;
    }
    for (int n = 0; n < _maxPcmInstrumentNum; n++) {
        _pcmInstrument[n] = NULL;
    }
    for (int n = 0; n < _maxPcmSoundNum; n++) {
        _pcmSound[n] = NULL;
    }
}

EUP_TownsEmulator::~EUP_TownsEmulator()
{
    for (int n = 0; n < _maxChannelNum; n++) {
        delete _channel[n];
    }
    for (int n = 0; n < _maxPcmInstrumentNum; n++)
        if (_pcmInstrument[n] != NULL) {
            delete _pcmInstrument[n];
        }
    for (int n = 0; n < _maxPcmSoundNum; n++)
        if (_pcmSound[n] != NULL) {
            delete _pcmSound[n];
        }
}

void EUP_TownsEmulator::assignFmDeviceToChannel(int channel)
{
    CHECK_CHANNEL_NUM("EUP_TownsEmulator::assignFmDeviceToChannel", channel);

    EUP_TownsEmulator_MonophonicAudioSynthesizer *dev = new TownsFmEmulator;
    dev->rate(_rate);
    _channel[channel]->add(dev);
}

void EUP_TownsEmulator::assignPcmDeviceToChannel(int channel)
{
    CHECK_CHANNEL_NUM("EUP_TownsEmulator::assignPcmDeviceToChannel", channel);

	if (channel >= _maxChannelNum) return;	// original impl created out of bounds write to _channel[]

    EUP_TownsEmulator_MonophonicAudioSynthesizer *dev = new TownsPcmEmulator;
    dev->rate(_rate);
    _channel[channel]->add(dev);
}

void EUP_TownsEmulator::setFmInstrumentParameter(int num, uint8_t const *instrument)
{
    if (num < 0 || num >= _maxFmInstrumentNum) {
        fprintf(stderr, "%s: FM instrument number %d out of range\n",
                "EUP_TownsEmulator::setFmInstrumentParameter",  num);
        fflush(stderr);
        return;
    }
    memcpy(_fmInstrument[num], instrument, 48);
}

void EUP_TownsEmulator::setPcmInstrumentParameters(uint8_t const *instrument, size_t size)
{
    for (int n = 0; n < _maxPcmInstrumentNum; n++) {
        if (_pcmInstrument[n] != NULL) {
            delete _pcmInstrument[n];
        }
        _pcmInstrument[n] = new TownsPcmInstrument(instrument+8+128*n);
    }
    uint8_t const *p = instrument + 8 + 128*32;
    for (int m = 0; m < _maxPcmSoundNum && p<(instrument+size); m++) {
        if (_pcmSound[m] != NULL) {
            delete _pcmSound[m];
        }
        _pcmSound[m] = new TownsPcmSound(p);
        for (int n = 0; n < _maxPcmInstrumentNum; n++) {
            _pcmInstrument[n]->registerSound(_pcmSound[m]);
        }
        p += 32 + P4(p+12);
    }
}

void EUP_TownsEmulator::outputStream(FILE *ostr)
{
    _ostr = ostr;
}

FILE * EUP_TownsEmulator::outputStream_get()
{
    return _ostr;
}

void EUP_TownsEmulator::enable(int ch, bool en)
{
    DB(("enable ch=%d, en=%d\n", ch, en));
    CHECK_CHANNEL_NUM("EUP_TownsEmulator::enable", ch);
    _enabled[ch] = en;
}

void EUP_TownsEmulator::nextTick()
{
    // steptime ひとつ分進める

    struct timeval tv = this->timeStep();
    int64_t buflen = (int64_t)_rate * (int64_t)tv.tv_usec; /* 精度上げなきゃ  */
    //buflen /= 1000 * 1000;
    buflen /= 1000 * 1012; // 1010 から 1015 くらいですね、うちでは。曲によるけど。
    buflen++;
#if defined ( _MSC_VER )
    int *buf0 = new int[buflen];
    if (NULL == buf0) {
        fprintf(stderr, "heap allocation problem.\n");
        fflush(stderr);
        exit(0);
    }
#endif // _MSC_VER
#if defined ( __MINGW32__ )
    int *buf0 = new int[buflen];
    if (NULL == buf0) {
        fprintf(stderr, "heap allocation problem.\n");
        fflush(stderr);
        exit(0);
    }
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
    int buf0[buflen];
#endif // __GNUC__

    memset(buf0, 0, sizeof(buf0[0]) * buflen);

    for (int i = 0; i < _maxChannelNum; i++) {
        if (_enabled[i]) {
            _channel[i]->nextTick(buf0, buflen);
        }
    }

    if (_outputSampleSize == 1) {
#if defined ( _MSC_VER )
        u_char *buf1 = new u_char[buflen];
        if (NULL == buf1) {
            fprintf(stderr, "heap allocation problem.\n");
            fflush(stderr);
            exit(0);
        }
#endif // _MSC_VER
#if defined ( __MINGW32__ )
        u_char *buf1 = new u_char[buflen];
        if (NULL == buf1) {
            fprintf(stderr, "heap allocation problem.\n");
            fflush(stderr);
            exit(0);
        }
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
    u_char buf1[buflen];
#endif // __GNUC__

    for (int i = 0; i < buflen; i++) {
        int d = buf0[i];
        d *= this->volume();
        d >>= 10; // いいかげんだなぁ
        d ^= (_outputSampleUnsigned) ? 0x8000 : 0;
        buf1[i] = ((d >> 8) & 0xff);
    }
    if (true == this->output2File_read()) {
        fwrite(buf1, sizeof(buf1[0]), buflen, _ostr);

        pcm.count += buflen;
    }

#if defined ( _MSC_VER )
    delete buf1;
#endif // _MSC_VER
#if defined ( __MINGW32__ )
    delete buf1;
#endif // __MINGW32__
    } else {
        if (true == this->output2File_read()) {
#if defined ( _MSC_VER )
            u_char *buf1 = new u_char[buflen * 2];
            if (NULL == buf1) {
                fprintf(stderr, "heap allocation problem.\n");
                fflush(stderr);
                exit(0);
            }
#endif // _MSC_VER
#if defined ( __MINGW32__ )
            u_char *buf1 = new u_char[buflen * 2];
            if (NULL == buf1) {
                fprintf(stderr, "heap allocation problem.\n");
                fflush(stderr);
                exit(0);
            }
#endif // __MINGW32__
#if defined ( __GNUC__ ) && !defined ( __MINGW32__ )
            u_char buf1[buflen * 2];
#endif // __GNUC__

            for (int i = 0; i < buflen; i++) {
                int d = buf0[i];
                d *= this->volume();
                d >>= 10; // いいかげんだなぁ
                d ^= (_outputSampleUnsigned) ? 0x8000 : 0;
                if (_outputSampleLSBFirst) {
                    buf1[i*2+1] = ((d >> 8) & 0xff);
                    buf1[i*2+0] = ((d >> 0) & 0xff);
                }
                else {
                    buf1[i*2+0] = ((d >> 8) & 0xff);
                    buf1[i*2+1] = ((d >> 0) & 0xff);
                }
            }

            fwrite(buf1, sizeof(buf1[0])*2, buflen, _ostr);

            pcm.count += buflen;

#if defined ( _MSC_VER )
            delete buf1;
#endif // _MSC_VER
#if defined ( __MINGW32__ )
            delete buf1;
#endif // __MINGW32__
        } else {
            while (true) { /* infinite loop waiting for empty space in buffer */
                // there's enough space for buflen samples
                if (((pcm.read_pos < pcm.write_pos) && (buflen <= (streamAudioBufferSamples - pcm.write_pos + pcm.read_pos)))
                    ||
                    ((pcm.read_pos >= pcm.write_pos) && (buflen <= (pcm.read_pos - pcm.write_pos)))) {
                    int renderData = pcm.write_pos;
                    int16_t * renderBuffer =(int16_t *) pcm.buffer; 

                    if (true == _outputSampleLSBFirst) {
                        for (int i = 0; i < buflen; i++) {
                            if (streamAudioBufferSamples <= renderData) {
                                renderData = 0;
                            }

                            int d = buf0[i];
                            d *= this->volume();
                            int16_t dd = (int16_t)(d >> 10); // いいかげんだなぁ
                            renderBuffer[renderData++] = dd;

                            /* left channel sample first place */
                            /*renderData++;*/
                            /* right channel sample second place */
                            /*pcm.write_pos++;*/
                        }
                    } else {
                        for (int i = 0; i < buflen; i++) {
                            if (streamAudioBufferSamples <= renderData) {
                                renderData = 0;
                            }

                            int d = buf0[i];
                            d *= this->volume();
                            int16_t dd = (int16_t)(((d >> 18) & 0xff) + ((d >> 2) & 0xff00)); // ������������ʤ�
                            renderBuffer[renderData++] = dd;

                            /* left channel sample first place */
                            /*renderData++;*/
                            /* right channel sample second place */
                            /*pcm.write_pos++;*/
                        }
                    }

                    pcm.write_pos = renderData;
		    break; /* leave infinite loop waiting for empty space in buffer */
                // there's not space in buffer, please wait
                } else {
                    SDL_Delay(1);
                }
            }
        }
    }

#if defined ( _MSC_VER )
    delete buf0;
#endif // _MSC_VER
#if defined ( __MINGW32__ )
    delete buf0;
#endif // __MINGW32__
}

void EUP_TownsEmulator::note(int channel, int n,
                             int onVelo, int offVelo, int gateTime)
{
    CHECK_CHANNEL_NUM("EUP_TownsEmulator::note", channel);
    _channel[channel]->note(n, onVelo, offVelo, gateTime);
}

void EUP_TownsEmulator::pitchBend(int channel, int value)
{
    CHECK_CHANNEL_NUM("EUP_TownsEmulator::pitchBend", channel);
    _channel[channel]->pitchBend(value);
}

void EUP_TownsEmulator::controlChange(int channel, int control, int value)
{
    CHECK_CHANNEL_NUM("EUP_TownsEmulator::controlChange", channel);
    _channel[channel]->setControlParameter(control, value);
}

void EUP_TownsEmulator::programChange(int channel, int num)
{
    CHECK_CHANNEL_NUM("EUP_TownsEmulator::programChange", channel);

    uint8_t *fminst = NULL;
    uint8_t *pcminst = NULL;

    if (0 <= num && num < _maxFmInstrumentNum) {
        fminst = _fmInstrument[num];
    }
    if (0 <= num && num < _maxPcmInstrumentNum) {
        pcminst = (uint8_t*)_pcmInstrument[num];
    }

    _channel[channel]->setInstrumentParameter(fminst, pcminst);
}

void EUP_TownsEmulator::rate(int r)
{
    _rate = r;
    for (int n = 0; n < _maxChannelNum; n++) {
        _channel[n]->rate(r);
    }
}
