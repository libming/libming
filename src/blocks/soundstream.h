
#ifndef SWF_SOUNDSTREAM_H_INCLUDED
#define SWF_SOUNDSTREAM_H_INCLUDED

#include "block.h"
#include "input.h"

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
  byte flags;
  byte isFinished;
  int delay;
  int start;
  int samplesPerFrame;
  SWFInput input;
};
typedef struct _sound *SWFSound;

#define SWFSOUND_SIZE sizeof(struct _sound)

SWFSound newSWFSound(FILE *file);
SWFSound newSWFSound_fromInput(SWFInput input);
void destroySWFSound(SWFSound sound);


#define SWFSOUNDSTREAM_INITIAL_DELAY 1663

struct _soundStreamBlock
{
  swfBlock block;
  SWFSound sound;
  byte channels;
  int numFrames;
  int delay;
  int length;
};
typedef struct _soundStreamBlock *SWFSoundStreamBlock;

#define SWFSOUNDSTREAMBLOCK_SIZE sizeof(struct _soundStreamBlock)

SWFBlock SWFSound_getStreamHead(SWFSound sound, float frameRate);
SWFBlock SWFSound_getStreamBlock(SWFSound sound);
void SWFSound_rewind(SWFSound sound);

#endif /* SWF_SOUNDSTREAM_H_INCLUDED */
