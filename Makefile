#
# Makefile
#

CXX = g++
CC = gcc
LD = g++
AR = ar
LIBS = 
SLIBS =
LFLAGS =

ifdef DEBUG
CFLAGS = -g -O0
else
CFLAGS = -O3
endif

prefix = /usr/local
exec_prefix = ${prefix}

top_builddir = .

ifeq ($(OS),Windows_NT)
W32 = 1
export W32
endif

ifeq ($(W32),1)
EXE_SFX = .exe
endif

#
# SDL2 stuff
#

SDL_CONFIG = sdl2-config

SDL_SLIBS  := `$(SDL_CONFIG) --static-libs`
SDL_LIBS   := `$(SDL_CONFIG) --libs`
SDL_CFLAGS := `$(SDL_CONFIG) --cflags`

#
# Makefile for EUPPlayer
#

INCLUDES = 

bin_PROGRAMS = eupplay$(EXE_SFX)

eupplay_LDADD = -lstdc++

eupplay_SOURCES = eupplay.cpp eupplayer.cpp eupplayer.hpp eupplayer_towns.hpp eupplayer_townsEmulator.cpp eupplayer_townsEmulator.hpp eupplayer_townsInternal.cpp eupplayer_townsInternal.hpp sintbl.cpp sintbl.hpp

PROGRAMS =  $(bin_PROGRAMS)

TARGET = $(PROGRAMS)

#
# all
#

all : $(TARGET)

DEFS = -DPACKAGE=\"eupplayer\" -DVERSION=\"0.008\" -DSTDC_HEADERS=1 -DHAVE_UNISTD_H=1 -DEUPPLAYER_LITTLE_ENDIAN=1  -I. 
CPPFLAGS += $(SDL_CFLAGS) 
LDFLAGS += $(SDL_LIBS) 
LIBS += $(SDL_LIBS)
eupplay_OBJECTS =  eupplay.o eupplayer.o eupplayer_townsEmulator.o \
eupplayer_townsInternal.o sintbl.o
CXXFLAGS = -g -O2 -Wall -fomit-frame-pointer -funroll-loops
CXXCOMPILE = $(CXX) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
CXXLD = $(CXX)
CXXLINK = $(CXXLD) $(CXXFLAGS) $(LDFLAGS) -o $@
CFLAGS = 
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@

TAR = tar
GZIP_ENV = --best
SOURCES = $(eupplay_SOURCES)
OBJECTS = $(eupplay_OBJECTS)

%.o : %.cpp
	$(COMPILE) -o $@ $< -c

$(TARGET): $(eupplay_OBJECTS)
	@rm -f $(bin_PROGRAMS)
	$(CXXLINK) $(eupplay_LDFLAGS) $(eupplay_OBJECTS) $(eupplay_LDADD) $(LIBS)

clean:
	rm -f $(eupplay_OBJECTS)
	rm -f $(bin_PROGRAMS)

release:
	strip $(TARGET)

