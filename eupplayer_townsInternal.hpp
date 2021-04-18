// $Id: eupplayer_townsInternal.h,v 1.2 1996/08/20 16:33:41 tjh Exp $

/*      Artistic Style
 *
 * ./astyle --style=stroustrup --convert-tabs --add-braces eupplayer_townsInternal.hpp
 */

#ifndef TJH__EUP_TOWNSINTERNAL_H
#define TJH__EUP_TOWNSINTERNAL_H

#include "eupplayer_towns.hpp"

class EUP_TownsInternal_MonophonicAudioDevice;

class EUP_TownsInternal : public TownsAudioDevice {
    enum { _maxFmChannelNum = 6, _maxPcmChannelNum = 8,
           _maxChannelNum = _maxFmChannelNum + _maxPcmChannelNum,
           _maxFmInstrumentNum = 128
         };
    EUP_TownsInternal_MonophonicAudioDevice *_dev[_maxFmChannelNum + _maxPcmChannelNum];
    u_char _fmInstrumentData[8 + 48*_maxFmInstrumentNum];
    u_char *_fmInstrument[_maxFmInstrumentNum];
public:
    EUP_TownsInternal();
    ~EUP_TownsInternal();
    void setFmInstrumentParameter(int num, u_char const *instrument);
    void nextTick();
    void note(int track, int n,
              int onVelo, int offVelo, int gateTime);
    void pitchBend(int track, int value);
    void programChange(int track, int num);
};

#endif
