/*
    Ming, an SWF output library
    Copyright (C) 2002  Opaque Industries - http://www.opaque.net/

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

/* $Id$ */

#include <stdio.h>
#include <stdlib.h>

#include "outputblock.h"
#include "sound.h"
#include "soundstream.h"
#include "character.h"
#include "method.h"
#include "input.h"
#include "libming.h"
#include "mp3.h"

struct SWFSound_s
{
	struct SWFCharacter_s character;

	byte flags;
	int seekSamples;
	byte freeInput;

	SWFInput input;
	struct SWFSoundStream_s *soundStream;
};


void
writeSWFSoundWithSoundStreamToMethod(SWFSoundStream stream,
	SWFByteOutputMethod method, void *data);

static int
soundDataSize(SWFSound sound)
{
	if ((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_NOT_COMPRESSED ||
		 (sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_NOT_COMPRESSED_LE)
	{
		int sampleCount = SWFInput_length(sound->input);

		if ((sound->flags & SWF_SOUND_BITS) == SWF_SOUND_16BITS)
			sampleCount /= 2;

		if ((sound->flags & SWF_SOUND_CHANNELS) == SWF_SOUND_STEREO)
			sampleCount /= 2;

		return sampleCount;
	}
	else if ((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_ADPCM_COMPRESSED)
	{
		int filesize, channels, nbits;
		int bitsize, blocksize, n, res, m;

		SWF_assert((sound->flags & SWF_SOUND_BITS) == SWF_SOUND_16BITS);

		filesize = SWFInput_length(sound->input);

		if ((sound->flags&SWF_SOUND_CHANNELS) == SWF_SOUND_MONO)
			channels = 1;
		else if ((sound->flags & SWF_SOUND_CHANNELS) == SWF_SOUND_STEREO)
			channels = 2;
		else
			channels = 1;	 /* ? */

		nbits = 4;	/* XXX - testing.. */

		/*
		 * Estimation of the sample count in ADPCM data from file size of the data.
		 * This is an approximate calculation.
		 */
		bitsize = 8 * filesize - (2 + (8 - 1));
		/* 2: header, (8 - 1): possible padding */
		blocksize = ((16 + 6) + nbits * 4095) * channels;
		n = bitsize / blocksize;
		res = bitsize % blocksize;
		m = (res - (16 + 6) * channels) / (nbits * channels);
		return 4096 * n + m;
	}
	else if ((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_MP3_COMPRESSED)
	{
		int pos = SWFInput_tell(sound->input);
		int samples = -1;
		getMP3Samples(sound->input, sound->flags, &samples);
		SWFInput_seek(sound->input, pos, SEEK_SET);
		return samples;
	}
	else 
	{
		SWF_warn("SWFSound: can't determine sampleCount\n");
		return 0;
	}
}


void
writeSWFSoundToStream(SWFBlock block, SWFByteOutputMethod method, void *data)
{
	int l, i;
	SWFSound sound = (SWFSound)block;

	methodWriteUInt16(CHARACTERID(sound), method, data);
	method(sound->flags, data);

	l = SWFInput_length(sound->input);

	methodWriteUInt32(soundDataSize(sound), method, data);

	if ( (sound->flags & SWF_SOUND_COMPRESSION) == SWF_SOUND_MP3_COMPRESSED )
		methodWriteUInt16(sound->seekSamples, method, data);	

	/* write samples */
	for ( i=0; i<l; ++i )
		method((unsigned char)SWFInput_getChar(sound->input), data);
}


int
completeDefineSWFSoundBlock(SWFBlock block)
{
	SWFSound sound = (SWFSound)block;

	if ((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_MP3_COMPRESSED)
		return 7 + 2 + SWFInput_length(sound->input);
	else
		return 7 + SWFInput_length(sound->input);
}


void
writeSWFSoundWithSoundStreamToStream(SWFBlock block, SWFByteOutputMethod method, void *data)
{
	SWFSound sound = (SWFSound)block;

	methodWriteUInt16(CHARACTERID(sound), method, data);
	method(sound->flags, data);

	writeSWFSoundWithSoundStreamToMethod(sound->soundStream, method, data);
}


int
completeDefineSWFSoundWithSoundStreamBlock(SWFBlock block)
{
	SWFSound sound = (SWFSound)block;
	int len = SWFSoundStream_getLength(sound->soundStream, 0) + 9;
	SWFSoundStream_rewind(sound->soundStream);
	return len;
}

/*
 * Set number of samples to seek forward or delay (works with MP3 only)
 *
 * If this value is positive, the player seeks this
 * number of samples into the sound block before the
 * sound is played.
 * If this value is negative the player plays this
 * number of silent samples before playing the sound block
 */
void
SWFSound_setInitialMp3Delay(SWFSound sound, int delaySeek)
{
	sound->seekSamples = delaySeek;
}


void
destroySWFSound(SWFSound sound)
{
	if (sound->freeInput)
		destroySWFInput(sound->input);
	destroySWFCharacter((SWFCharacter) sound);
}

/*
 * Creates a new EventSound object.
 *
 * The sound to be played is contained in a file and specified with flags.
 *
 * Flags must contain a sound format, sampling rate, size (in bits) and channels.
 * If the file contains mp3 data it is not necessary to specify sampling rate, 
 * sound size and channels.
 *
 * Possible sound formats are:
 * - SWF_SOUND_NOT_COMPRESSED 
 * - SWF_SOUND_ADPCM_COMPRESSED
 * - SWF_SOUND_MP3_COMPRESSED
 * - SWF_SOUND_NOT_COMPRESSED_LE
 * - SWF_SOUND_NELLY_COMPRESSED
 *
 * Sampling rate must be one of the following values:
 * - SWF_SOUND_5KHZ
 * - SWF_SOUND_11KHZ
 * - SWF_SOUND_22KHZ
 * - SWF_SOUND_44KHZ
 *
 * Sound size is either SWF_SOUND_8BITS or SWF_SOUND_16BITS
 * Channels are either SWF_SOUND_MONO or SWF_SOUND_STEREO
 */
SWFSound
newSWFSound(FILE *f, byte flags)
{
	SWFSound s = newSWFSound_fromInput(newSWFInput_file(f), flags);
	s->freeInput = TRUE;
	return s;
}

/* added by David McNab <david@rebirthing.co.nz> */
/* required so that python can pass in file descriptors instead of FILE* streams */
SWFSound
newSWFSoundFromFileno(int fd, byte flags)
{
	FILE *fp = fdopen(fd, "r");
	return newSWFSound(fp, flags);
}


SWFSound
newSWFSound_fromInput(SWFInput input, byte flags)
{
	SWFSound sound = (SWFSound)malloc(sizeof(struct SWFSound_s));
	SWFBlock block = (SWFBlock)sound;

	SWFCharacterInit((SWFCharacter)sound);

	CHARACTERID(sound) = ++SWF_gNumCharacters;

	block->type = SWF_DEFINESOUND;

	block->writeBlock = writeSWFSoundToStream;
	block->complete = completeDefineSWFSoundBlock;
	block->dtor = (destroySWFBlockMethod) destroySWFSound;

	sound->input = input;
	sound->flags = flags;
	if((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_MP3_COMPRESSED)
	{
		if(getMP3Flags(input, &sound->flags) < 0)
		{
			free(sound);
			return NULL;
		}
	}
	sound->soundStream = 0;
	sound->seekSamples = SWFSOUND_INITIAL_DELAY;
	sound->freeInput = FALSE;
	return sound;
}

/*
 * Creates an Event Sound object from a given SoundStream object
 *
 * see also newSWFSoundStream()
 */
SWFSound
newSWFSound_fromSoundStream(SWFSoundStream stream)
{
	SWFSound sound = (SWFSound)malloc(sizeof(struct SWFSound_s));
	SWFBlock block = (SWFBlock)sound;

	SWFCharacterInit((SWFCharacter)sound);

	CHARACTERID(sound) = ++SWF_gNumCharacters;

	block->type = SWF_DEFINESOUND;

	block->writeBlock = writeSWFSoundWithSoundStreamToStream;
	block->complete = completeDefineSWFSoundWithSoundStreamBlock;
	block->dtor = (destroySWFBlockMethod) destroySWFSound;

	sound->freeInput = FALSE;
	sound->input = 0;
	sound->flags = SWFSoundStream_getFlags(stream);
	sound->soundStream = stream;

	return sound;
}

/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
