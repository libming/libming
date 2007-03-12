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

#include "block.h"
#include "soundstream.h"
#include "output.h"
#include "outputblock.h"
#include "sound.h"
#include "method.h"
#include "input.h"
#include "libming.h"
#include "flv.h"
#include "error.h"

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

struct SWFSoundStream_s
{
	byte isFinished;
	byte streamSource;
	int delay;
	int samplesPerFrame;
	int sampleRate;
	byte freeInput;
	
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
	int numFrames;
	int delay;
	int length;
};


int
nextMP3Frame(SWFInput input);

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
			printf("no more frames to skip \n");
			break;
		}
		--skipFrames;
		stream->source.mp3.start += l;
	}
}

int
completeSWFSoundStream(SWFBlock block)
{
	return ((SWFSoundStreamBlock)block)->length + 4;
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
	
	methodWriteUInt16(streamblock->numFrames *
                          (streamblock->stream->sampleRate > 32000 ? 1152 : 576),
                          method, data);

	methodWriteUInt16(streamblock->delay, method, data);
	if(source == STREAM_MP3)
		write_mp3(streamblock, method, data);
	else if(source == STREAM_FLV)
		write_flv(streamblock, method, data);
}


static void 
fillStreamBlock_flv(SWFSoundStream stream, SWFSoundStreamBlock block)
{
	FLVStream *flv = stream->source.flv.stream;
	FLVTag *tag = &stream->source.flv.tag; 
	int tagOffset = stream->source.flv.tagOffset;
	int delay, length;
        int frameSize, ret;
	SWFInput input;

	
	block->delay = stream->delay;
	delay = stream->delay + stream->samplesPerFrame;

	if(tagOffset < 0)
	{
		ret = FLVStream_nextTagType(flv, tag, NULL, FLV_AUDIOTAG);
	
		if(ret < 0)
		{
			SWF_warn("fillStreamBlock_flv: not a valid flv audio stream\n");
			goto stream_finished;
		}
	}
	
	input = FLVTag_getPayloadInput(tag);
	if(input == NULL)
		goto stream_finished;
		
	if(tagOffset > 0)
		SWFInput_seek(input, tagOffset, SEEK_SET);
	
	if ( stream->sampleRate > 32000 )
		frameSize = 1152;
	else
		frameSize = 576;

	while ( delay > frameSize)
	{
		length = nextMP3Frame(input);	
		if (length < 0) /* parse error: not a valid mp3 frame */
			goto stream_finished;
		else if(length == 0) /* eof */
		{
			ret = FLVStream_nextTagType(flv, tag, tag, FLV_AUDIOTAG);
			if(ret < 0)
				goto stream_finished;
			
			input = FLVTag_getPayloadInput(tag);
			if(input == NULL)
				goto stream_finished;
		}
		else 
		{
			++block->numFrames;
			block->length += length;
			delay -= frameSize;
		}
	}

	stream->source.flv.tag = *tag;
	stream->source.flv.tagOffset = SWFInput_tell(input);
	stream->delay = delay;
	return;

stream_finished:
	stream->isFinished = TRUE;
        SWFSoundStream_rewind(stream);
}

static void 
fillStreamBlock_mp3(SWFSoundStream stream, SWFSoundStreamBlock block)
{
	int delay, length;
	int frameSize;
	
	/* see how many frames we can put in this block,
	 see how big they are */

	block->delay = stream->delay;
	delay = stream->delay + stream->samplesPerFrame;

	if ( stream->sampleRate > 32000 )
		frameSize = 1152;
	else
		frameSize = 576;

	while ( delay > frameSize )
	{
		++block->numFrames;
		length = nextMP3Frame(stream->source.mp3.input);
		if ( length <= 0 )
		{
			stream->isFinished = TRUE;
			SWFSoundStream_rewind(stream);
			break;
		}

		block->length += length;
		delay -= frameSize;
	}

	stream->delay = delay;
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
	block->numFrames = 0;

	if(stream->streamSource == STREAM_MP3)
		fillStreamBlock_mp3(stream, block);
	else if(stream->streamSource == STREAM_FLV)
		fillStreamBlock_flv(stream, block);
	return (SWFBlock)block;
}

int SWFSoundStream_getFrames(SWFSoundStream stream)
{	
	int n, frameSize;
	if(stream->streamSource == STREAM_FLV)
		return -1;
	
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
	
#define MP3_FRAME_SYNC			0xFFE00000

#define MP3_VERSION			0x00180000
#define MP3_VERSION_25			0x00000000
#define MP3_VERSION_RESERVED		0x00080000
#define MP3_VERSION_2			0x00100000
#define MP3_VERSION_1			0x00180000

#define MP3_SAMPLERATE			0x00000C00
#define MP3_SAMPLERATE_SHIFT		10

#define MP3_CHANNEL			0x000000C0
#define MP3_CHANNEL_STEREO		0x00000000
#define MP3_CHANNEL_JOINT		0x00000040
#define MP3_CHANNEL_DUAL		0x00000080
#define MP3_CHANNEL_MONO		0x000000C0

static int
getStreamFlag_mp3File(SWFSoundStream stream, float frameRate, float skip)
{
	
	SWFInput input = stream->source.mp3.input;
	
	int rate=0, channels, flags, start = 0;

	/* 
	 * skip stream until first MP3 header which starts with 0xffe 
	 */

	flags = SWFInput_getUInt32_BE(input);
	if(flags == EOF)
		return -1;
	
	while((flags & MP3_FRAME_SYNC) != MP3_FRAME_SYNC)
	{
		SWFInput_seek(input, -3, SEEK_CUR);
		start++;
		flags = SWFInput_getUInt32_BE(input);
		if(flags == EOF)
			return -1;
	}

	SWFInput_seek(input, start, SEEK_SET);
	stream->source.mp3.start = start;	// for rewind	
	
	if ( (flags & MP3_CHANNEL) == MP3_CHANNEL_MONO )
		channels = SWF_SOUNDSTREAM_MONO;
	else
		channels = SWF_SOUNDSTREAM_STEREO;

	/* XXX - this is a gross oversimplification */
	switch ( flags & MP3_VERSION )
	{
		case MP3_VERSION_1:
			stream->sampleRate = 44100; rate = SWF_SOUNDSTREAM_44KHZ; break;

		case MP3_VERSION_2:
			stream->sampleRate = 22050; rate = SWF_SOUNDSTREAM_22KHZ; break;

		case MP3_VERSION_25:
			stream->sampleRate = 11025; rate = SWF_SOUNDSTREAM_11KHZ; break;
	}

	flags =
		SWF_SOUNDSTREAM_MP3_COMPRESSED | rate | SWF_SOUNDSTREAM_16BITS | channels;

	
	stream->samplesPerFrame = (int)floor(stream->sampleRate / frameRate);
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

	skip_msec = round(skip * 1000);
	if(FLVStream_setStreamOffset(stream->source.flv.stream, skip_msec) < 0)
		return -1;

	return flags;
}


SWFBlock
SWFSoundStream_getStreamHead(SWFSoundStream stream, float frameRate, float skip)
{
	int flags = 0;

	SWFOutput out = newSizedSWFOutput(6);
	SWFOutputBlock block = newSWFOutputBlock(out, SWF_SOUNDSTREAMHEAD);
	
	if(stream->streamSource == STREAM_MP3)
		flags = getStreamFlag_mp3File(stream, frameRate, skip);
	else if(stream->streamSource == STREAM_FLV)
		flags = getStreamFlag_flv(stream, frameRate, skip);
	
	if(flags < 0)
	{
		destroySWFOutputBlock(block);
		return NULL;
	}

	SWFOutput_writeUInt8(out, flags & 0x0f); 
	SWFOutput_writeUInt8(out, flags);
	SWFOutput_writeUInt16(out, stream->samplesPerFrame);
	SWFOutput_writeUInt16(out, SWFSOUND_INITIAL_DELAY);
	
	return (SWFBlock)block;
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
	
	stream->delay = SWFSOUND_INITIAL_DELAY;
	stream->isFinished = FALSE;
	stream->samplesPerFrame = 0;
	stream->sampleRate = 0;
	stream->freeInput = FALSE;

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
