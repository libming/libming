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

#if 0

#include <stdio.h>

#include "sound.h"

int soundDataSize(SWFSound sound)
{
  if((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_NOT_COMPRESSED)
  {
    return sound->numSamples *
      ((sound->flags&SWF_SOUND_BITS) == SWF_SOUND_16BITS) ? 2 : 1 *
      ((sound->flags&SWF_SOUND_CHANNELS) == SWF_SOUND_STEREO) ? 2 : 1;
  }
  else
  {
    int nBits = (sound->data[0] >> 6) + 2;

    assert((sound->flags&SWF_SOUND_BITS) == SWF_SOUND_16BITS);

    /* 16 bits for initial sample, 6 for initial index */
    /* double for stereo */
    return ((16 + 6 + (sound->numSamples-1) * nBits) *
	    (((sound->flags&SWF_SOUND_CHANNELS) == SWF_SOUND_STEREO) ? 2 : 1) + 7)/8;
  }
}

void writeSWFSoundToStream(SWFBlock block,
			   SWFByteOutputMethod method, void *data)
{
  int l, i;
  SWFSound sound = (SWFSound)block;

  methodWriteUInt16(CHARACTERID(sound), method, data);
  method(sound->flags, data);
  methodWriteUInt32(sound->numSamples, method, data);

  /* write samples */
  l = soundDataSize(sound);
  for(i=0; i<l; ++i)
    method(sound->data[l], data);
}
int completeDefineSWFSoundBlock(SWFBlock block)
{
  SWFSound sound = (SWFSound)block;
  return 7 + soundDataSize(sound);
}

SWFBlock newDefineSWFSoundStreamBlock(SWFOutput data)
{
  return newSWFOutputBlock(data, SWF_SOUNDSTREAMBLOCK);
}

void writeSWFSoundStreamHeadToMethod(SWFBlock block,
				     SWFByteOutputMethod method, void *data)
{
  SWFSound sound = (SWFSound)block;
  int numSamples = sound->numSamples;

  method(sound->flags, data);
  method(sound->flags, data);
  methodWriteUInt16(numSamples, method, data);
}
int completeSWFSoundStreamHead(SWFBlock block)
{
  return 4;
}
SWFBlock newSWFSoundStreamHead(SWFSound sound)
{
  
  return newSWFBlock(SWF_SOUNDSTREAMHEAD, sound,
		     &writeSWFSoundStreamHeadToMethod,
		     &completeSWFSoundStreamHead);
}
SWFBlock newSWFSoundStreamHead2(SWFSound sound)
{
  return newSWFBlock(SWF_SOUNDSTREAMHEAD2, sound,
		     &writeSWFSoundStreamHeadBlockToMethod,
		     &completeSWFSoundStreamHeadBlock);
}

void writeSWFStartSoundToMethod(SWFBlock block,
				SWFByteOutputMethod method, void *data)
{
  SWFSoundInfo soundInfo = (SWFSoundInfo)block;
  byte flags = soundInfo->flags;
  int i;

  methodWriteUInt16(soundInfo->sound->soundNum, method, data);
  method(flags, data);

  if(flags & SWF_SOUNDINFO_HASINPOINT)
    methodWriteUInt32(soundInfo->inPoint, method, data);
  if(flags & SWF_SOUNDINFO_HASOUTPOINT)
    methodWriteUInt32(soundInfo->outPoint, method, data);
  if(flags & SWF_SOUNDINFO_HASLOOPS)
    methodWriteUInt16(soundInfo->numLoops, method, data);
  if(flags & SWF_SOUNDINFO_HASENVELOPE)
  {
    method(soundInfo->numEnvPoints, data);
    for(i=0; i<soundInfo->numEnvPoints; ++i)
    {
      methodWriteUInt32((soundInfo->envPoints[i]).mark44, method, data);
      methodWriteUInt16((soundInfo->envPoints[i]).level0, method, data);
      methodWriteUInt16((soundInfo->envPoints[i]).level1, method, data);
    }
  }
}
int completeSWFStartSound(SWFBlock block)
{
  SWFSoundInfo soundInfo = (SWFSoundInfo)block;
  byte flags = soundInfo->flags;

  return 3 + /* sound id + flags */
    (flags&SWF_SOUNDINFO_HASINPOINT) ? 4 : 0 +
    (flags&SWF_SOUNDINFO_HASOUTPOINT) ? 4 : 0 +
    (flags&SWF_SOUNDINFO_HASLOOPS) ? 2 : 0 +
    (flags&SWF_SOUNDINFO_HASENVELOPE) ? (1+8*soundInfo->numEnvPoints) : 0;
}
SWFBlock newSWFStartSound(SWFSoundInfo soundInfo)
{
  return newSWFBlock(SWF_STARTSOUND, soundInfo,
		     &writeSWFStartSoundBlockToMethod,
		     &completeSWFStartSoundBlock);
}

SWFSoundInfo newSWFSoundInfo(SWFSound sound)
{
  SWFSoundInfo soundInfo = calloc(SWF_SOUNDINFO_SIZE);
  soundInfo->sound = sound;
  soundInfo->envPoints = NULL;
  return soundInfo;
}

/* XXX - temporary standin, ignoring all the soundinfo options */
SWFBlock newStartSWFSoundBlock(SWFSound sound, int loops)
{
  SWFOutput out = newSizedSWFOutput(5);

  SWFOutput_writeUInt16(CHARACTERID(sound));

  if(loops>1)
  {
    SWFOutput_writeUInt8(out, 0x04);
    SWFOutput_writeUInt16(out, loops);
  }
  else
    SWFOutput_writeUInt8(out, 0);

  return newSWFOutputBlock(out, SWF_STARTSOUND);
}
SWFBlock newStopSWFSoundBlock(SWFSound sound)
{
  SWFOutput out = newSizedSWFOutput(3);

  SWFOutput_writeUInt16(CHARACTERID(sound));
  SWFOutput_writeUInt8(out, 0x20);

  return newSWFOutputBlock(out, SWF_STARTSOUND);
}

SWFSound newSWFSound()
{
  SWFSound sound = calloc(SWF_SOUND_SIZE);
  sound->file = NULL;
  return sound;
}

void SWFSound_setData(SWFSound sound, byte flags, int numSamples, byte *data)
{
  sound->flags = flags;
  sound->numSamples = numSamples;
  sound->data = data;
}

#endif
