// $Id: eupplayer_townsEmulator.h,v 1.13 2000/04/12 23:12:51 hayasaka Exp $

/*      Artistic Style
 *
 * ./astyle --style=stroustrup --convert-tabs --add-braces eupplayer_townsEmulator.hpp
 */

#ifndef TJH__EUP_TOWNSEMULATOR_H
#define TJH__EUP_TOWNSEMULATOR_H

#if defined ( __MINGW32__ )
#include <sys/time.h>
#endif // __MINGW32__
#if defined ( __GNUC__ )
#include <sys/time.h>
#endif // __GNUC__
#include <sys/types.h>
#include <iostream>
#include <cstdint>

#include "eupplayer_towns.hpp"

/*
 * global data
 *
 */
const int streamAudioSampleOctectSize = 2;
/* rate 44100 Hz stream */
const int streamAudioRate = 44100;
const int streamAudioSamplesBlock = 512;
const int streamAudioChannelsNum = 1;
const int streamAudioSamplesBlockNum = 16;
const int streamAudioChannelsSamplesBlock = streamAudioSamplesBlock * streamAudioChannelsNum;
const int streamAudioBufferSamples = streamAudioChannelsSamplesBlock * streamAudioSamplesBlockNum;
const int streamAudioBufferOctectsSize = streamAudioBufferSamples * streamAudioSampleOctectSize;
const int streamBytesPerSecond = streamAudioRate * streamAudioChannelsNum * streamAudioSampleOctectSize;

struct pcm_struct {
    bool on;
    int stop;

    int write_pos;
    int read_pos;

    int count;

    uint8_t buffer[streamAudioBufferOctectsSize];
};
extern struct pcm_struct pcm;

class TownsPcmInstrument;
class TownsPcmSound;
class TownsPcmEnvelope;

class EUP_TownsEmulator_MonophonicAudioSynthesizer {
    int _rate;
    int _velocity;
public:
    EUP_TownsEmulator_MonophonicAudioSynthesizer() {}
    virtual ~EUP_TownsEmulator_MonophonicAudioSynthesizer() {}
    virtual void setControlParameter(int control, int value) = 0;
    virtual void setInstrumentParameter(uint8_t const *fmInst,
                                        uint8_t const *pcmInst) = 0;
    virtual int velocity() const
    {
        return _velocity;
    }
    virtual void velocity(int velo)
    {
        _velocity = velo;
    }
    virtual void nextTick(int *outbuf, int buflen) = 0;
    virtual int rate() const
    {
        return _rate;
    }
    virtual void rate(int r)
    {
        _rate = r;
    }
    virtual void note(int n, int onVelo, int offVelo, int gateTime) = 0;
    virtual void pitchBend(int value) = 0;
};

class TownsFmEmulator_Operator {
    enum State { _s_ready, _s_attacking, _s_decaying, _s_sustaining, _s_releasing };
    State _state;
    State _oldState;
    int64_t _currentLevel;
    int _frequency;
    int _phase;
    int _lastOutput;
    int _feedbackLevel;
    int _detune;
    int _multiple;
    int64_t _totalLevel;
    int _keyScale;
    int _velocity;
    int _specifiedTotalLevel;
    int _specifiedAttackRate;
    int _specifiedDecayRate;
    int _specifiedSustainLevel;
    int _specifiedSustainRate;
    int _specifiedReleaseRate;
    int _tickCount;
    int _attackTime;
    // int64_t _attackRate;
    int64_t _decayRate;
    int64_t _sustainLevel;
    int64_t _sustainRate;
    int64_t _releaseRate;
public:
    TownsFmEmulator_Operator();
    ~TownsFmEmulator_Operator();
    void feedbackLevel(int level);
    void setInstrumentParameter(uint8_t const *instrument);
    void velocity(int velo);
    void keyOn();
    void keyOff();
    void frequency(int freq);
    int nextTick(int rate, int phaseShift);
};

class TownsFmEmulator : public EUP_TownsEmulator_MonophonicAudioSynthesizer {
    enum { _numOfOperators = 4 };
    TownsFmEmulator_Operator *_opr;
    int _chn_volume;
    int _expression;
    int _gateTime;
    int _offVelocity;
    int _note;
    int _frequencyOffs;
    int _frequency;
    int _algorithm;
public:
    TownsFmEmulator();
    ~TownsFmEmulator();
    void setControlParameter(int control, int value);
    void setInstrumentParameter(uint8_t const *fmInst, uint8_t const *pcmInst);
    int velocity()
    {
        return EUP_TownsEmulator_MonophonicAudioSynthesizer::velocity();
    }
    void velocity(int velo);
    void nextTick(int *outbuf, int buflen);
    void note(int n, int onVelo, int offVelo, int gateTime);
    void pitchBend(int value);
    void recalculateFrequency();
};

class TownsPcmEmulator : public EUP_TownsEmulator_MonophonicAudioSynthesizer {
    int _chn_volume;
    int _expression;
    int _envTick;
    int _currentLevel;
    int _gateTime;
    int _offVelocity;
    int _note;
    int _frequencyOffs;
    uint64_t _phase;

    TownsPcmInstrument const *_currentInstrument;
    TownsPcmSound const *_currentSound;
    TownsPcmEnvelope *_currentEnvelope;
public:
    TownsPcmEmulator();
    ~TownsPcmEmulator();
    void setControlParameter(int control, int value);
    void setInstrumentParameter(uint8_t const *fmInst, uint8_t const *pcmInst);
    void nextTick(int *outbuf, int buflen);
    void note(int n, int onVelo, int offVelo, int gateTime);
    void pitchBend(int value);
};

class EUP_TownsEmulator_Channel {
    enum { _maxDevices = 16 };
    EUP_TownsEmulator_MonophonicAudioSynthesizer *_dev[_maxDevices+1];
    int _lastNotedDeviceNum;
public:
    EUP_TownsEmulator_Channel();
    ~EUP_TownsEmulator_Channel();
    void add(EUP_TownsEmulator_MonophonicAudioSynthesizer *device);
    void note(int note, int onVelo, int offVelo, int gateTime);
    void setControlParameter(int control, int value);
    void setInstrumentParameter(uint8_t const *fmInst, uint8_t const *pcmInst);
    void pitchBend(int value);
    void nextTick(int *outbuf, int buflen);
    void rate(int r);
};

class EUP_TownsEmulator : public TownsAudioDevice {
    FILE *_ostr;
    enum { _maxChannelNum = 16,
           _maxFmInstrumentNum = 128,
           _maxPcmInstrumentNum = 32,
           _maxPcmSoundNum = 128,
         };
    EUP_TownsEmulator_Channel *_channel[_maxChannelNum];
    bool _enabled[_maxChannelNum];
    uint8_t _fmInstrumentData[8 + 48*_maxFmInstrumentNum];
    uint8_t *_fmInstrument[_maxFmInstrumentNum]; // pointers into above _fmInstrumentData buffer
    TownsPcmInstrument *_pcmInstrument[_maxPcmInstrumentNum];
    TownsPcmSound *_pcmSound[_maxPcmSoundNum];
    int _rate;
    bool _outputSampleUnsigned;
    int _outputSampleSize;
    bool _outputSampleLSBFirst;
    bool _output2File;
public:
    EUP_TownsEmulator();
    ~EUP_TownsEmulator();
    void outputSampleUnsigned(bool u)
    {
        _outputSampleUnsigned = u;
    }
    void outputSampleSize(int l)
    {
        _outputSampleSize = l;
    }
    void outputSampleLSBFirst(bool l)
    {
        _outputSampleLSBFirst = l;
    }
    bool outputSampleLSBFirst_read()
    {
        return _outputSampleLSBFirst;
    }
    void output2File(bool u)
    {
        _output2File = u;
    }
    bool output2File_read()
    {
        return _output2File;
    }
    void assignFmDeviceToChannel(int channel);
    void assignPcmDeviceToChannel(int channel);
    void setFmInstrumentParameter(int num, uint8_t const *instrument);
    void setPcmInstrumentParameters(uint8_t const *instrument, size_t size);
    void outputStream(FILE *ostr);
    FILE * outputStream_get();
    void nextTick();
    void enable(int ch, bool en=true);
    void note(int channel, int n,
              int onVelo, int offVelo, int gateTime);
    void pitchBend(int channel, int value);
    void controlChange(int channel, int control, int value);
    void programChange(int channel, int num);
    void rate(int r);
};

#endif
