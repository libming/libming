/*
    Ming, an SWF output library
    Copyright (C) 2001  Opaque Industries - http://www.opaque.net/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* sound.h */

#ifndef SWF_SOUND_H_INCLUDED
#define SWF_SOUND_H_INCLUDED

#include "block.h"
#include "method.h"
#include "character.h"
#include "output.h"

#define SWF_SOUND_COMPRESSION      0xf0
#define SWF_SOUND_NOT_COMPRESSED   (0<<4)
#define SWF_SOUND_ADPCM_COMPRESSED (1<<4)
#define SWF_SOUND_MP3_COMPRESSED   (2<<4)

#define SWF_SOUND_RATE             0x0c
#define SWF_SOUND_5KHZ             (0<<2)
#define SWF_SOUND_11KHZ            (1<<2)
#define SWF_SOUND_22KHZ            (2<<2)
#define SWF_SOUND_44KHZ            (3<<2)

#define SWF_SOUND_BITS             0x02
#define SWF_SOUND_8BITS            (0<<1)
#define SWF_SOUND_16BITS           (1<<1)

#define SWF_SOUND_CHANNELS         0x01
#define SWF_SOUND_MONO             (0<<0)
#define SWF_SOUND_STEREO           (1<<0)

struct _sound
{
  swfCharacter character;
  byte flags;
  byte isFinished;
  int numSamples;
  int delay;
  int samplesPerFrame;
  FILE *file;
};
typedef struct _sound *SWFSound;

#define SWFSOUND_SIZE sizeof(struct _sound)

#define SWF_SOUNDINFO_SYNCSTOPSOUND  (1<<5)
#define SWF_SOUNDINFO_SYNCNOMULTIPLE (1<<4)
#define SWF_SOUNDINFO_HASENVELOPE    (1<<3)
#define SWF_SOUNDINFO_HASLOOPS       (1<<2)
#define SWF_SOUNDINFO_HASOUTPOINT    (1<<1)
#define SWF_SOUNDINFO_HASINPOINT     (1<<0)

typedef struct
{
  unsigned int mark44;
  unsigned short level0;
  unsigned short level1;
} envPoint;

struct _soundInfo
{
  swfBlock block;
  SWFSound sound;
  unsigned int inPoint;
  unsigned int outPoint;
  int numLoops;
  byte flags;
  byte numEnvPoints;
  envPoint *envPoints;
};
typedef struct _soundInfo *SWFSoundInfo;

#define SWF_SOUNDINFO_SIZE sizeof(struct _soundInfo)

SWFBlock newDefineSWFSoundBlock(SWFSound sound);
SWFBlock newDefineSWFSoundStreamBlock(SWFOutput output);
SWFBlock newSWFSoundStreamHeadBlock(SWFSound sound);
SWFBlock newSWFSoundStreamHead2Block(SWFSound sound);
SWFBlock newSWFStartSoundBlock();

SWFSound newSWFSound();
void SWFSound_setData(SWFSound sound, byte flags, int numSamples, byte *data);

#endif /* SWF_SOUND_H_INCLUDED */
