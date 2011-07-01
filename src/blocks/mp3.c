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

#include "libming.h"
#include "input.h"
#include "mp3.h"
#include "error.h"
#include "sound.h"


// [version][idx]
static unsigned short mp3_samplingrate_tbl[4][4] =
{ 
	// mpeg 2.5
	{ 11025, 12000, 8000, 0},
	// reserved
	{ 0, 0, 0, 0},
	// mpeg 2
	{ 22050, 24000, 16000, 0},
	// mpeg 1
	{ 44100, 48000, 32000, 0}
};

// [version][layer][idx]
static unsigned short mp3_bitrate_tbl[4][4][16] = 
{
	// mpeg 2.5
	{ 
		// reserved
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		// layer 3
		{ 0, 8, 16, 24,	32, 40,	48, 56, 64, 80, 96, 112, 128, 144, 160, 0},
		// layer 2
		{ 0, 8, 16, 24,	32, 40,	48, 56, 64, 80, 96, 112, 128, 144, 160, 0},
		// layer 1
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	},
	
	// reserved
	{ 
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	},

	// mpeg version 2
	{
		// reserved
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		// layer 3
		{ 0, 8, 16, 24,	32, 40,	48, 56, 64, 80, 96, 112, 128, 144, 160, 0},
		// layer 2
		{ 0, 8, 16, 24,	32, 40,	48, 56, 64, 80, 96, 112, 128, 144, 160, 0},
		// layer 1
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

	},
	// mpeg version 1
	{ 
		// reserved
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		// layer 3	
		{ 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0},
		// layer 2
		{ 0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 0 },
		// layer 1
		{ 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 382, 416, 448, 0}
	}
};


#define CHECK_STREAM(__in)			\
	do {					\
		if(SWFInput_eof(__in))		\
			return 0;		\
	} while(0);

	
/*
 * reads a MP3 header 
 * returns:
 * 	0 if EOF
 * 	-1 if invalid header; stream is unaltered
 * 	1 on success
 */	
int readMP3Header(SWFInput input, struct mp3_header *mp3h)
{
	int frameSync, br, sr;
	if(SWFInput_length(input) - SWFInput_tell(input) < 4)
		return 0;
	
	frameSync = SWFInput_readBits(input, 11);
	CHECK_STREAM(input);

	mp3h->version = SWFInput_readBits(input, 2);
	mp3h->layer = SWFInput_readBits(input, 2);
	SWFInput_readBits(input, 1); // protect bit
	CHECK_STREAM(input);

	br = SWFInput_readBits(input, 4);
	sr = SWFInput_readBits(input, 2);
	mp3h->bitrate = mp3_bitrate_tbl[mp3h->version][mp3h->layer][br];
	mp3h->samplingRate = mp3_samplingrate_tbl[mp3h->version][sr];
	mp3h->padding = SWFInput_readBits(input, 1);
	SWFInput_readBits(input, 1); // private bit
	CHECK_STREAM(input);
	
	mp3h->channelMode = SWFInput_readBits(input, 2);
	SWFInput_readBits(input, 2); // extension
	SWFInput_readBits(input, 3); // copyright, orig, emphasis
	CHECK_STREAM(input);
	SWFInput_byteAlign(input);
	if((frameSync & 0x7ff) != 0x7ff)
	{
		SWFInput_seek(input, -4, SEEK_CUR);
		return -1;
	}

	if(mp3h->version == 1 || mp3h->layer == 0)
	{
		SWFInput_seek(input, -4, SEEK_CUR);  
		return -1;
	}
	return 1;
}

int nextMP3Frame(SWFInput input)
{
	int frameLen, ret;
	struct mp3_header mp3h;

	ret = readMP3Header(input, &mp3h);
	
	if(ret < 0)
		return -1;
	
	if(ret == 0 || SWFInput_eof(input))
		return 0;
	
	if(mp3h.samplingRate == 0 || mp3h.bitrate == 0)
		SWF_error("invalid mp3 file\n");
	if(mp3h.version == MP3_VERSION_1)
	{
		frameLen = 144 * mp3h.bitrate * 1000 
				/ mp3h.samplingRate + mp3h.padding;
	}
	else
		frameLen = 72 * mp3h.bitrate * 1000 
			/ mp3h.samplingRate + mp3h.padding;

	SWFInput_seek(input, frameLen-4, SEEK_CUR);
	return frameLen;
}

/*
 * returns the sound flags used within ming
 * stream is set to the first mp3 frame header.
 */
int getMP3Flags(SWFInput input, byte *flags)
{
	struct mp3_header mp3h;	
	int rate=0, channels, start = 0;
	int ret;

	/* 
	 * skip stream until first MP3 header which starts with 0xffe 
	 */
	while((ret = readMP3Header(input, &mp3h)) < 0)
	{
		SWFInput_seek(input, 1, SEEK_CUR);
		start++;
	}

	if(ret == 0 || SWFInput_eof(input))
		return -1;

	SWFInput_seek(input, start, SEEK_SET);
	if (mp3h.channelMode == MP3_CHANNEL_MONO )
		channels = SWF_SOUND_MONO;
	else
		channels = SWF_SOUND_STEREO;

	switch ( mp3h.version )
	{
		case MP3_VERSION_1:
			rate = SWF_SOUND_44KHZ; 
			break;

		case MP3_VERSION_2:
			rate = SWF_SOUND_22KHZ; 
			break;

		case MP3_VERSION_25:
			rate = SWF_SOUND_11KHZ; 
			break;
	}

	*flags =
		SWF_SOUND_MP3_COMPRESSED | rate | SWF_SOUND_16BITS | channels;
	return start;	
}

/*
 * Read at most the wanted number of samples from the input mp3 stream
 * The stream read pointer is advanced.
 * The actual number of samples read is written in *wanted and their
 * total length is returned.
 * If *wanted is < 0 read all there is in the stream.
 */
int getMP3Samples(SWFInput input, int flags, int *wanted)
{
	int frameSize, length;
	int numSamples = 0;
	int totalLength = 0;

	switch(flags & SWF_SOUND_RATE)
	{
	case SWF_SOUND_44KHZ:
		frameSize = 1152;
		break;
	case SWF_SOUND_22KHZ:
	case SWF_SOUND_11KHZ:
		frameSize = 576;
		break;
	default:
		*wanted = 0;
		return -1;
	}

	while(*wanted < 0 || numSamples < *wanted - frameSize) 
	{
		length = nextMP3Frame(input);
		if(length <= 0)
			break; /* EOF */
		totalLength += length;
		numSamples += frameSize;
	}

	*wanted = numSamples;			
	return totalLength;
}

/*
 * Returns the length of a mp3 stream in ms. The duration is
 * measured in ms. The stream is unaltered.
 */
unsigned int getMP3Duration(SWFInput input)
{
	int start, flags, samples = -1;
	int sampleRate;

	start = SWFInput_tell(input);
	if(getMP3Flags(input, (byte *)&flags) < 0)
		return 0;
	if(getMP3Samples(input, flags, &samples) <= 0)
		return 0;
	
	sampleRate = SWFSound_getSampleRate(flags);
	SWFInput_seek(input, start, SEEK_SET);
	return samples * 1000.0	/ sampleRate;
}
/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
