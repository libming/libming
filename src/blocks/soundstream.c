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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

#include "block.h"
#include "soundstream.h"
#include "output.h"
#include "outputblock.h"
#include "character.h"
#include "method.h"
#include "input.h"
#include "libming.h"
#include "flv.h"
#include "error.h"
#include "mp3.h"
#include "sound.h"

#ifndef round 
#define round
#endif

#define STREAM_MP3 1
#define STREAM_FLV 2

struct StreamSourceMp3
{
	int start;
	SWFInput input;
};

struct StreamSourceFlv
{
	FLVStream *stream;
	FLVTag tag;
	int tagOffset;
};

#define AUDIO_CODEC(__stream) (((__stream)->flags & 0xf0) >> 4)

struct SWFSoundStream_s
{
	byte isFinished;
	byte streamSource;
	int initialDelay;
	int delay;
	int samplesPerFrame;
	int sampleRate;
	byte freeInput;
	float frameRate;
	int flags;	
	union {
	/* either an MP3 file -> handled by SWFInput */
		struct StreamSourceMp3 mp3;
	/* or FLV stream -> handled by generic flv reader */
		struct StreamSourceFlv flv;
	} source;
};

struct SWFSoundStreamBlock_s
{
	struct SWFBlock_s block;

	SWFSoundStream stream;
	int numSamples;
	int delay;
	int length;
};


void skipMP3(SWFSoundStream stream, float skip) 
{
	int frameSize;
	int skipFrames, l;
	
	if ( stream->sampleRate > 32000 )
		frameSize = 1152;
	else
	        frameSize = 576;
		 
	skipFrames = (int)floor((skip  / frameSize) / stream->sampleRate);

	if(skipFrames <= 0)
		return;

	while(skipFrames > 0) {
		l = nextMP3Frame(stream->source.mp3.input);
		if (l < 0) {
			SWF_warn("no more frames to skip \n");
			break;
		}
		--skipFrames;
		stream->source.mp3.start += l;
	}
}

int
completeSWFSoundStream(SWFBlock block)
{
	SWFSoundStreamBlock streamblock = (SWFSoundStreamBlock)block;
	if(streamblock->stream->streamSource == STREAM_MP3 || 
		AUDIO_CODEC(streamblock->stream) == AUDIO_CODEC_MP3)
		return streamblock->length + 4;
	else
		return streamblock->length;
}


static void 
write_flv(SWFSoundStreamBlock streamblock, SWFByteOutputMethod method, void *data)
{
	SWFInput input; 
	FLVStream *flv = streamblock->stream->source.flv.stream;
        FLVTag *tag = &streamblock->stream->source.flv.tag;
        int tagOffset = streamblock->stream->source.flv.tagOffset;
	int l = streamblock->length;
	int ret;
	
	if(tagOffset < 0)
	{
		ret = FLVStream_nextTagType(flv, tag, NULL, FLV_AUDIOTAG);
	
		if(ret < 0)
			return;
	}
	
	input = FLVTag_getPayloadInput(tag);
	if(input == NULL)
		return;

	if(tagOffset > 0)
		SWFInput_seek(input, tagOffset, SEEK_SET);

	while( l>0 )
	{
		int ichar = SWFInput_getChar(input);
		if(ichar != EOF)
		{
			method(ichar, data);
			l--;
			continue;
		}

		ret = FLVStream_nextTagType(flv, tag, tag, FLV_AUDIOTAG);
		if(ret < 0)
                	return;

		input = FLVTag_getPayloadInput(tag);
	 	if(input == NULL)
			return;
	}

	streamblock->stream->source.flv.tag = *tag;
	streamblock->stream->source.flv.tagOffset = SWFInput_tell(input);
}

static void 
write_mp3(SWFSoundStreamBlock streamblock, SWFByteOutputMethod method, void *data)
{
	SWFInput input = streamblock->stream->source.mp3.input;
	int l = streamblock->length;

	for ( ; l>0; --l )
		method((unsigned char)SWFInput_getChar(input), data);
}


void
writeSWFSoundStreamToMethod(SWFBlock block,
                            SWFByteOutputMethod method, void *data)
{
	SWFSoundStreamBlock streamblock = (SWFSoundStreamBlock)block;
	int source = streamblock->stream->streamSource;
	
	if(source == STREAM_MP3)
	{
		methodWriteUInt16(streamblock->numSamples, method, data);
		methodWriteUInt16(streamblock->delay, method, data);
		write_mp3(streamblock, method, data);
	}
	else if(source == STREAM_FLV)
	{
		if(AUDIO_CODEC(streamblock->stream) == AUDIO_CODEC_MP3)
		{
			methodWriteUInt16(streamblock->numSamples, method, data);
			methodWriteUInt16(streamblock->delay, method, data);
		}
		write_flv(streamblock, method, data);
	}
}

static int
fillBlock_flv_mp3(SWFSoundStream stream, SWFSoundStreamBlock block)
{
	FLVStream *flv = stream->source.flv.stream;
	FLVTag *tag = &stream->source.flv.tag; 
	int tagOffset = stream->source.flv.tagOffset;
	int delay, length, frameSize, ret;
	SWFInput input;

	block->delay = stream->delay;
	delay = stream->delay + stream->samplesPerFrame;

	if(tagOffset < 0)
	{
		ret = FLVStream_nextTagType(flv, tag, NULL, FLV_AUDIOTAG);
		if(ret < 0)
		{
			SWF_warn("fillStreamBlock_flv: not a valid flv audio stream\n");
			return -1;
		}
	}
	
	input = FLVTag_getPayloadInput(tag);
	if(input == NULL)
		return -1;
		
	if(tagOffset > 0)
		SWFInput_seek(input, tagOffset, SEEK_SET);

	if ( stream->sampleRate > 32000 )
		frameSize = 1152;
	else
		frameSize = 576;

	while ( delay > frameSize)
	{
		length = nextMP3Frame(input);	
		if (length < 0)
		{
			SWF_warn("parse error: not a valid mp3 frame\n");
			return -1;
		}
		else if(length == 0) /* eof */
		{
			ret = FLVStream_nextTagType(flv, tag, tag, FLV_AUDIOTAG);
			if(ret < 0)
				return -1;
			
			input = FLVTag_getPayloadInput(tag);
			if(input == NULL)
				return -1;
		}
		else 
		{
			block->numSamples += frameSize;
			block->length += length;
			delay -= frameSize;
		}
	}
	static const int maxSC = 65535; 
	if ( block->numSamples > maxSC ) {
		SWF_warn("fillBlock_flv_mp3: number of samples in block (%d) exceed max allowed value of %d\n", block->numSamples, maxSC);
	}
	stream->source.flv.tag = *tag;
	stream->source.flv.tagOffset = SWFInput_tell(input);
	stream->delay = delay;
	return 0;
}

static int
fillBlock_flv_nelly(SWFSoundStream stream, SWFSoundStreamBlock block)
{
	FLVStream *flv = stream->source.flv.stream;
	FLVTag *tag = &stream->source.flv.tag; 
	int length, samples, ret;
	SWFInput input;
	int tagOffset = stream->source.flv.tagOffset;

	if(tagOffset < 0)
	{
		ret = FLVStream_nextTagType(flv, tag, NULL, FLV_AUDIOTAG);
		if(ret < 0)
		{
			SWF_warn("fillStreamBlock_flv: not a valid flv audio stream\n");
			return -1;
		}
	}
	
	input = FLVTag_getPayloadInput(tag);
	if(input == NULL)
		return -1;

	samples = stream->samplesPerFrame;
	while (samples > 0)
	{
		length = SWFInput_length(input);	
		if (length < 0)
			return -1;

		samples -= length * 64;
		block->length += length;
		if(samples > 0) 		{
			ret = FLVStream_nextTagType(flv, tag, tag, FLV_AUDIOTAG);
			if(ret < 0)
				return -1;
			
			input = FLVTag_getPayloadInput(tag);
			if(input == NULL)
				return -1;
		}
	}
	stream->source.flv.tag = *tag;
	stream->source.flv.tagOffset = 0;
	return 0;
}


static void 
fillStreamBlock_flv(SWFSoundStream stream, SWFSoundStreamBlock block)
{
	int ret;
	
	if(AUDIO_CODEC(stream) == AUDIO_CODEC_MP3)
		ret = fillBlock_flv_mp3(stream, block);
	else if(AUDIO_CODEC(stream) == AUDIO_CODEC_NELLY || 
		AUDIO_CODEC(stream) == AUDIO_CODEC_NELLY_MONO)
		ret = fillBlock_flv_nelly(stream, block);
	else
	{
		SWF_warn("unsupported codec %i\n", AUDIO_CODEC(stream));
		ret = -1;
	}
			
	if(ret < 0)
	{
		stream->isFinished = TRUE;
		SWFSoundStream_rewind(stream);
	}
}

static void 
fillStreamBlock_mp3(SWFSoundStream stream, SWFSoundStreamBlock block)
{
	int delay, wanted;

	/* see how many frames we can put in this block,
	 see how big they are */
	block->delay = stream->delay;
	delay = stream->delay + stream->samplesPerFrame;
	wanted = delay;
	block->length = getMP3Samples(stream->source.mp3.input, stream->flags, &delay);
	block->numSamples = delay;
	static const int maxSC = 65535; 
	if ( block->numSamples > maxSC ) {
		SWF_warn("fillStreamBlock_mp3: number of samples in block (%d) exceed max allowed value of %d\n", block->numSamples, maxSC);
	}
	if(block->length <= 0)
	{
		stream->isFinished = TRUE;
		SWFSoundStream_rewind(stream);
	}
	stream->delay = wanted - delay;
}

SWFBlock
SWFSoundStream_getStreamBlock(SWFSoundStream stream)
{
	SWFSoundStreamBlock block;

	if ( stream->isFinished )
		return NULL;

	block = (SWFSoundStreamBlock) malloc(sizeof(struct SWFSoundStreamBlock_s));

	SWFBlockInit((SWFBlock)block);

	BLOCK(block)->complete = completeSWFSoundStream;
	BLOCK(block)->writeBlock = writeSWFSoundStreamToMethod;
	BLOCK(block)->dtor = NULL;
	BLOCK(block)->type = SWF_SOUNDSTREAMBLOCK;

	block->stream = stream;
	block->length = 0;
	block->numSamples = 0;

	if(stream->streamSource == STREAM_MP3)
		fillStreamBlock_mp3(stream, block);
	else if(stream->streamSource == STREAM_FLV)
		fillStreamBlock_flv(stream, block);
	
	if(block->length == 0)
	{
		free(block);
		return NULL;
	}
	
	return (SWFBlock)block;
}


/*
 * returns the duration of a stream in ms
 *
 * This function returns the duration of a given stream in ms
 */
unsigned int SWFSoundStream_getDuration(SWFSoundStream stream)
{	
	if(stream->streamSource == STREAM_MP3)
		return getMP3Duration(stream->source.mp3.input);
	else if(stream->streamSource == STREAM_FLV)
		return FLVStream_getDuration(stream->source.flv.stream, FLV_AUDIOTAG);
	else
		return 0;
}

/* 
 * DEPRECATED!
 * returns the number of movie frames for a given sound stream
 *
 * This function returns the number of movie frames necessary to 
 * play the full sound stream. 
 * Works only if the sound stream object was added to a movie and the stream 
 * source is a mp3 file.
 *
 * Use SWFSoundStream_getDuration() instead.
 */
int SWFSoundStream_getFrames(SWFSoundStream stream)
{	
	int n, frameSize;
	if(stream->streamSource == STREAM_FLV || stream->samplesPerFrame == 0)
	{
		SWF_warn("SWFSoundStream_getFrames works only if stream was assigned to a movie\n");
		return -1;
	}
	
	if ( stream->sampleRate > 32000 )
		frameSize = 1152;
	else
		frameSize = 576;
	n = 0;
	while(nextMP3Frame(stream->source.mp3.input) > 0)
		n++;
	SWFSoundStream_rewind(stream);
	return n * frameSize / stream->samplesPerFrame;
}
	

static int
getStreamFlag_mp3File(SWFSoundStream stream, float frameRate, float skip)
{
	SWFInput input = stream->source.mp3.input;
	int start;
	byte flags;
	
	start = getMP3Flags(input, &flags);
	if(start < 0)
		return -1;
	stream->source.mp3.start = start;
	stream->sampleRate = SWFSound_getSampleRate(flags); 
	stream->flags = flags; // XXX: fixme
	stream->samplesPerFrame = (int)floor(stream->sampleRate / frameRate);
	static const int maxSPF = 65535; 
	if ( stream->samplesPerFrame > maxSPF ) {
		SWF_warn("getStreamFlag_mp3File: computed number of samples per frame (%d) exceed max allowed value of %d\n", stream->samplesPerFrame, maxSPF);
	}
	skipMP3(stream, skip);
	return flags;
}

static int
getStreamFlag_flv(SWFSoundStream stream, float frameRate, float skip)
{
	int flags = 0, ret;
	FLVTag tag, *tag_p = NULL;
	unsigned int skip_msec;
		
	while((ret = FLVStream_nextTag(stream->source.flv.stream, &tag, tag_p)) == 0)
	{
		if(tag.tagType == FLV_AUDIOTAG)
			break;

		tag_p = &tag;
	}

	if(ret < 0)
		return -1;
	
	switch(tag.hdr.audio.samplingRate >> 2)
	{
		case 1:
			stream->sampleRate = 11025; break;
		case 2: 
			stream->sampleRate = 22050; break;
		case 3: 
			stream->sampleRate = 44100; break;
		default:
			SWF_warn("getStreamFlag_flv: unsupported sampleRate\n");
	}
	
	stream->samplesPerFrame = (int)floor(stream->sampleRate / frameRate);
	
	flags = tag.hdr.audio.samplingRate | tag.hdr.audio.sampleSize;
	flags |= tag.hdr.audio.channel | tag.hdr.audio.format;
	
	stream->flags = flags; // XXX: fixme
	skip_msec = round(skip * 1000);
	if(FLVStream_setStreamOffset(stream->source.flv.stream, skip_msec) < 0)
		return -1;

	return flags;
}


SWFBlock
SWFSoundStream_getStreamHead(SWFSoundStream stream, float frameRate, float skip)
{
	int flags = 0;

	SWFOutput out = newSizedSWFOutput(4);
	SWFOutputBlock block = newSWFOutputBlock(out, SWF_SOUNDSTREAMHEAD2);
	
	if(stream->streamSource == STREAM_MP3)
		flags = getStreamFlag_mp3File(stream, frameRate, skip);
	else if(stream->streamSource == STREAM_FLV)
		flags = getStreamFlag_flv(stream, frameRate, skip);
	
	stream->flags = flags;	
	stream->frameRate = frameRate;	
	if(flags < 0)
	{
		destroySWFOutputBlock(block);
		return NULL;
	}

	SWFOutput_writeUInt8(out, flags & 0x0f); 
	SWFOutput_writeUInt8(out, flags);
	SWFOutput_writeUInt16(out, stream->samplesPerFrame);
	if(((flags & 0xf0) >> 4) == 2)	// MP3 only
	{
		SWFOutput_writeUInt16(out, stream->initialDelay);
		stream->delay = stream->initialDelay;
	}
	
	return (SWFBlock)block;
}

/*
 * Set number of samples to seek forward (or delay?) 
 *
 * Works with MP3 only
 *
 * If this value is positive, the player seeks this
 * number of samples into the sound block before the
 * sound is played.
 * (If this value is negative the player plays this
 * number of silent samples before playing the sound block)
 */
void SWFSoundStream_setInitialMp3Delay(SWFSoundStream stream, int delay)
{
	stream->initialDelay = delay;
}

int
SWFSoundStream_getFlags(SWFSoundStream stream)
{
	if(stream->streamSource == STREAM_MP3)
		return getStreamFlag_mp3File(stream, 1.0f, 0);
	else if(stream->streamSource == STREAM_FLV)
		return getStreamFlag_flv(stream, 1.0f, 0);
	return 0;
}


int SWFSoundStream_getLength(SWFSoundStream stream, SWFSoundStreamBlock streamblock)
{
	int source = stream->streamSource;
	struct SWFSoundStreamBlock_s block;

	if (streamblock == NULL)
		streamblock = &block;
		
	streamblock->stream = stream;
	streamblock->length = 0;
	streamblock->numSamples = 0;
	stream->delay = INT_MAX - stream->samplesPerFrame - 1;
	if(source == STREAM_MP3) {
		fillStreamBlock_mp3(stream, streamblock);
	} else if(source == STREAM_FLV) {
		fillStreamBlock_flv(stream, streamblock);
	}
	return streamblock->length;
}


void
writeSWFSoundWithSoundStreamToMethod(SWFSoundStream stream,
                            SWFByteOutputMethod method, void *data)
{
	int source = stream->streamSource;
	struct SWFSoundStreamBlock_s streamblock;

	// need to get the sample count && and rewind
	SWFSoundStream_getLength(stream, &streamblock);
	SWFSoundStream_rewind(stream);

	methodWriteUInt32(streamblock.numSamples, method, data);
	methodWriteUInt16(stream->initialDelay, method, data);
	if(source == STREAM_MP3)
		write_mp3(&streamblock, method, data);
	else if(source == STREAM_FLV)
		write_flv(&streamblock, method, data);
}


/* XXX - kill this somehow.. */
void
SWFSoundStream_rewind(SWFSoundStream stream)
{
	if(stream->streamSource == STREAM_MP3)
		SWFInput_seek(stream->source.mp3.input, stream->source.mp3.start, SEEK_SET);
	else if(stream->streamSource == STREAM_FLV)
		stream->source.flv.tagOffset = -1;
}


SWFSoundStream
newSWFSoundStream_fromInput(SWFInput input)
{
	FLVStream *flvStream;
	SWFSoundStream stream = (SWFSoundStream)malloc(sizeof(struct SWFSoundStream_s));

	/* test if input stream is a FLV file first. */
	flvStream = FLVStream_fromInput(input);
	/* if input is not a valid FLV file a MP3 file is assumed. */
	if(flvStream == NULL)
	{
		SWFInput_seek(input, 0, SEEK_SET);
		stream->streamSource = STREAM_MP3;
		stream->source.mp3.input = input;
		stream->source.mp3.start = 0;
	}
	else
	{
		stream->streamSource = STREAM_FLV;
		stream->source.flv.stream = flvStream;
		stream->source.flv.tagOffset = -1;
	}

	stream->initialDelay = SWFSOUND_INITIAL_DELAY;
	stream->delay = 0;
	stream->isFinished = FALSE;
	stream->samplesPerFrame = 0;
	stream->sampleRate = 0;
	stream->freeInput = FALSE;
	stream->flags = -1;
	return stream;
}


void
destroySWFSoundStream(SWFSoundStream stream)
{
	if (stream->freeInput)
	{
		if(stream->streamSource == STREAM_MP3)
			destroySWFInput(stream->source.mp3.input);
		else if(stream->streamSource == STREAM_FLV)
			destroyFLVStream(stream->source.flv.stream);
		else
			SWF_warn("destroySWFSoundStream: unknown stream\n");
	}

	free(stream);
}


SWFSoundStream
newSWFSoundStream(FILE* file)
{
	SWFSoundStream s = newSWFSoundStream_fromInput(newSWFInput_file(file));
	s->freeInput = TRUE;
	return s;
}

SWFSoundStream
newSWFSoundStreamFromFileno(int fd)
{
	FILE *fp = fdopen(fd, "r");
	return newSWFSoundStream(fp);
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
