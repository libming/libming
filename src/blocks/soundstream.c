
#include <stdio.h>
#include <math.h>
#include "soundstream.h"
#include "output.h"
#include "outputblock.h"

int nextMP3Frame(SWFInput input);

int completeSWFSoundStream(SWFBlock block)
{
  return ((SWFSoundStreamBlock)block)->length + 4;
}
void writeSWFSoundStreamToMethod(SWFBlock block,
				 SWFByteOutputMethod method, void *data)
{
  SWFSoundStreamBlock stream = (SWFSoundStreamBlock)block;
  SWFInput input = stream->sound->input;
  int l = stream->length;

  methodWriteUInt16(stream->numFrames * 1152, /* stream->channels * 576, */
		    method, data);

  methodWriteUInt16(stream->delay, method, data);

  for(; l>0; --l)
    method(SWFInput_getChar(input), data);
}
SWFBlock SWFSound_getStreamBlock(SWFSound sound)
{
  int delay, length;
  SWFSoundStreamBlock stream;

  if(sound->isFinished)
    return NULL;

  stream = calloc(1, SWFSOUNDSTREAMBLOCK_SIZE);

  BLOCK(stream)->complete = completeSWFSoundStream;
  BLOCK(stream)->writeBlock = writeSWFSoundStreamToMethod;
  BLOCK(stream)->dtor = NULL;
  BLOCK(stream)->type = SWF_SOUNDSTREAMBLOCK;
  stream->sound = sound;
  stream->channels = (sound->flags&SWF_SOUND_CHANNELS)+1;

  /* see how many frames we can put in this block,
     see how big they are */

  stream->delay = sound->delay;

  delay = sound->delay + sound->samplesPerFrame;

  while(delay > 1152)
  {
    ++stream->numFrames;
    length = nextMP3Frame(sound->input);

    if(length <= 0)
    {
      sound->isFinished = TRUE;
      SWFSound_rewind(sound);
      break;
    }

    stream->length += length;
    delay -= 1152;
  }

  sound->delay = delay;

  return (SWFBlock)stream;
}

#define MP3_FRAME_SYNC       0xFFE00000

#define MP3_VERSION          0x00180000
#define MP3_VERSION_25       0x00000000
#define MP3_VERSION_RESERVED 0x00080000
#define MP3_VERSION_2        0x00100000
#define MP3_VERSION_1        0x00180000

#define MP3_SAMPLERATE       0x00000C00
#define MP3_SAMPLERATE_SHIFT 10

#define MP3_CHANNEL          0x000000C0
#define MP3_CHANNEL_STEREO   0x00000000
#define MP3_CHANNEL_JOINT    0x00000040
#define MP3_CHANNEL_DUAL     0x00000080
#define MP3_CHANNEL_MONO     0x000000C0

SWFBlock SWFSound_getStreamHead(SWFSound sound, float frameRate)
{
  SWFOutput out = newSizedSWFOutput(6);
  SWFOutputBlock block = newSWFOutputBlock(out, SWF_SOUNDSTREAMHEAD);
  SWFInput input = sound->input;
  int rate, sampleRate, samplesPerFrame, channels, flags, start = 0;

  /* get 4-byte header, bigendian */
  flags = SWFInput_getChar(input);

  if(flags == EOF)
    return NULL;

  /* XXX - fix this mad hackery */

  if(flags == 'I' &&
     SWFInput_getChar(input) == 'D' &&
     SWFInput_getChar(input) == '3')
  {
    start = 2;

    do
    {
      ++start;
      flags = SWFInput_getChar(input);
    }
    while(flags != 0xFF && flags != EOF);
  }

  if(flags == EOF)
    return NULL;

  SWFInput_seek(input, -1, SEEK_CUR);
  flags = SWFInput_getUInt32_BE(input);

  SWFInput_seek(input, start, SEEK_SET);

  sound->start = start;

  if((flags & MP3_FRAME_SYNC) != MP3_FRAME_SYNC)
    return NULL;

  if((flags & MP3_CHANNEL) == MP3_CHANNEL_MONO)
    channels = SWF_SOUND_MONO;
  else
    channels = SWF_SOUND_STEREO;

  /* XXX - this is a gross oversimplification */
  switch(flags & MP3_VERSION)
  {
    case MP3_VERSION_1:  sampleRate = 44100; rate = SWF_SOUND_44KHZ; break;
    case MP3_VERSION_2:  sampleRate = 22050; rate = SWF_SOUND_22KHZ; break;
    case MP3_VERSION_25: sampleRate = 11025; rate = SWF_SOUND_11KHZ; break;
  }

  flags = SWF_SOUND_MP3_COMPRESSED | rate | SWF_SOUND_16BITS | channels;
  sound->flags = flags;

  sound->samplesPerFrame = samplesPerFrame = floor(sampleRate/frameRate);

  SWFOutput_writeUInt8(out, 0x0A); /* XXX - mix format. ?? */
  SWFOutput_writeUInt8(out, flags);
  SWFOutput_writeUInt16(out, samplesPerFrame);
  SWFOutput_writeUInt16(out, SWFSOUNDSTREAM_INITIAL_DELAY);

  return (SWFBlock)block;
}

/* XXX - kill this */
void SWFSound_rewind(SWFSound sound)
{
  SWFInput_seek(sound->input, sound->start, SEEK_SET);
}

void destroySWFSound(SWFSound sound)
{
  free(sound);
}

SWFSound newSWFSound_fromInput(SWFInput input)
{
  SWFSound sound = calloc(1, SWFSOUND_SIZE);

  /* XXX - destructor? */

  sound->input = input;
  sound->delay = SWFSOUNDSTREAM_INITIAL_DELAY;
  return sound;
}

SWFSound newSWFSound(FILE *file)
{
  return newSWFSound_fromInput(newSWFInput_file(file));
}
