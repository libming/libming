
#include <stdio.h>
#include <math.h>
#include "soundstream.h"
#include "output.h"
#include "outputblock.h"


struct SWFSound_s
{
  byte flags;
  byte isFinished;
  int delay;
  int start;
  int samplesPerFrame;
  int sampleRate;
  byte freeInput;
  SWFInput input;
};

struct SWFSoundStreamBlock_s
{
  struct SWFBlock_s block;

  SWFSound sound;
  int numFrames;
  int delay;
  int length;
};


/* this is just a magic number, far as I can tell.. */
#define SWFSOUNDSTREAM_INITIAL_DELAY 1663


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

  methodWriteUInt16(stream->numFrames *
		    (stream->sound->sampleRate > 32000 ? 1152 : 576),
		    method, data);

  methodWriteUInt16(stream->delay, method, data);

  for(; l>0; --l)
    method(SWFInput_getChar(input), data);
}


SWFBlock SWFSound_getStreamBlock(SWFSound sound)
{
  int delay, length;
  SWFSoundStreamBlock stream;
  int frameSize;

  if(sound->isFinished)
    return NULL;

  stream = malloc(sizeof(struct SWFSoundStreamBlock_s));

  SWFBlockInit((SWFBlock)stream);

  BLOCK(stream)->complete = completeSWFSoundStream;
  BLOCK(stream)->writeBlock = writeSWFSoundStreamToMethod;
  BLOCK(stream)->dtor = NULL;
  BLOCK(stream)->type = SWF_SOUNDSTREAMBLOCK;

  stream->sound = sound;
  stream->length = 0;

  /* see how many frames we can put in this block,
     see how big they are */

  stream->delay = sound->delay;

  delay = sound->delay + sound->samplesPerFrame;

  if(sound->sampleRate > 32000)
    frameSize = 1152;
  else
    frameSize = 576;

  while(delay > frameSize)
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
    delay -= frameSize;
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

  int rate, channels, flags, start = 0;

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
    case MP3_VERSION_1:
      sound->sampleRate = 44100; rate = SWF_SOUND_44KHZ; break;

    case MP3_VERSION_2:
      sound->sampleRate = 22050; rate = SWF_SOUND_22KHZ; break;

    case MP3_VERSION_25:
      sound->sampleRate = 11025; rate = SWF_SOUND_11KHZ; break;
  }

  flags = SWF_SOUND_MP3_COMPRESSED | rate | SWF_SOUND_16BITS | channels;
  sound->flags = flags;

  sound->samplesPerFrame = floor(sound->sampleRate / frameRate);

  SWFOutput_writeUInt8(out, flags & 0x0f); /* preferred mix format.. (?) */
  SWFOutput_writeUInt8(out, flags);
  SWFOutput_writeUInt16(out, sound->samplesPerFrame);
  SWFOutput_writeUInt16(out, SWFSOUNDSTREAM_INITIAL_DELAY);

  return (SWFBlock)block;
}


/* XXX - kill this */
void SWFSound_rewind(SWFSound sound)
{
  SWFInput_seek(sound->input, sound->start, SEEK_SET);
}


SWFSound newSWFSound_fromInput(SWFInput input)
{
  SWFSound sound = malloc(sizeof(struct SWFSound_s));

  /* XXX - destructor? */

  sound->input = input;
  sound->delay = SWFSOUNDSTREAM_INITIAL_DELAY;
  sound->flags = 0;
  sound->isFinished = FALSE;
  sound->start = 0;
  sound->samplesPerFrame = 0;
  sound->sampleRate = 0;
  sound->freeInput = FALSE;

  return sound;
}


void destroySWFSound(SWFSound sound)
{
  if(sound->freeInput)
    destroySWFInput(sound->input);

  free(sound);
}


SWFSound newSWFSound(FILE *file)
{
  SWFSound s = newSWFSound_fromInput(newSWFInput_file(file));
  s->freeInput = TRUE;
  return s;
}
