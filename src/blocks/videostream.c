/*
    
   Ming, an SWF output library
   Copyright (C) 2002  Opaque Industries - http://www.opaque.net/
   
   15.12.2003 Klaus Rechert
   
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




#include <stdio.h>
#include <stdlib.h>

#include "input.h"
#include "method.h"
#include "block.h"
#include "character.h"
#include "videostream.h"
#include "libming.h"

#define VIDEOTAG 0x09
#define AUDIOTAG 0x08

#define VIDEO_SMOOTHING 0x01 
#define VIDEO_NO_SMOOTHING 0x0

#define VIDEO_CODEC_H263 0x02

#define FLV_FLAGS_AUDIO 0x04
#define FLV_FLAGS_VIDEO 0x01

struct SWFVideoStream_s
{
	struct SWFCharacter_s character;
	
	SWFInput input;	
	
	int version;
	int flags;
	unsigned long start;
	unsigned int numFrames;	
	int frame;
	int width;
	int height;
	int embeded;
};

struct SWFVideoFrame_s
{
	struct SWFBlock_s block;
	
	SWFVideoStream stream;

	int frameNum;
	int offset;
	int length;
	int codecId;
	int frameType;
};


struct FLVTag_s
{
	int tagType;
	unsigned long dataSize;
	unsigned long timeStamp;
	
};

int SWFVideoStream_getFrameNumber(SWFVideoFrame frame) 
{
	return frame->frameNum;
}

struct FLVTag_s * getNextFLVTag(SWFInput input, struct FLVTag_s *tag) {
	int ichar;
	unsigned long ulchar;

	ichar = SWFInput_getChar(input);
	if(ichar == EOF)
		return NULL;
	tag->tagType = ichar;

	ulchar = SWFInput_getUInt24_BE(input);
	// XXX: check ulchar
	tag->dataSize = ulchar;
	
	ulchar = SWFInput_getUInt24_BE(input);
	tag->timeStamp = ulchar;

	/*
	 * 32 Bit reserved
	 * should be 0 
	 */
	ulchar = SWFInput_getUInt32_BE(input);
	if(ulchar)
		return NULL;
	
	return tag;
}


int completeSWFVideoFrame(SWFBlock block) 
{
	return ((SWFVideoFrame)block)->length + 4;
}

void writeSWFVideoFrameToMethod(SWFBlock block, SWFByteOutputMethod method, void *data) 
{
		
	int l, ichar;

	SWFVideoFrame vf;
	SWFVideoStream stream;
	
	if(!block)
		return;
	
	vf = (SWFVideoFrame)block;
	stream = vf->stream;
	
	SWFInput_seek(stream->input, vf->offset, SEEK_SET);
	
	methodWriteUInt16(CHARACTERID(stream),method, data);
	methodWriteUInt16(vf->frameNum, method, data);
	for (l = 0; l < vf->length; l++) {
		ichar = SWFInput_getChar(stream->input);
		method(ichar, data);		
	}	
}

SWFBlock
SWFVideoStream_getVideoFrame(SWFVideoStream stream) {
	struct FLVTag_s tag;
	SWFVideoFrame block;
	int ichar;
	
	/*
	 * if this stream is not a embeded videostream
	 * there are no frames to return :)
	 */
	if(!stream->embeded)
		return NULL;
	
	if(stream->frame >= stream->numFrames)
		return NULL;
	
	if(stream->frame == 0)
		SWFInput_seek(stream->input, stream->start, SEEK_SET);


	block = (SWFVideoFrame) malloc(sizeof(struct SWFVideoFrame_s));
	SWFBlockInit((SWFBlock)block);

	BLOCK(block)->complete = completeSWFVideoFrame;
	BLOCK(block)->writeBlock = writeSWFVideoFrameToMethod;
	BLOCK(block)->dtor = NULL;
	BLOCK(block)->type = SWF_VIDEOFRAME;

	block->stream = stream;
	
	if(!getNextFLVTag(stream->input, &tag)) 
		return NULL;

	/* TODO: AudioTag */
	if(tag.tagType != VIDEOTAG)
		return NULL;

	ichar = SWFInput_getChar(stream->input);
	if(ichar == EOF)
		return NULL;

	block->codecId = (ichar & 0xf0);
	block->frameType = (ichar & 0x0f);
	block->frameNum = stream->frame;	
	stream->frame++;
	block->offset = SWFInput_tell(stream->input);

	/* length = dataSize - VideoTag-Header (1 byte) */
	block->length = tag.dataSize - 1;
	
	/* seek block->length + UInt32 after VideoTag */
	SWFInput_seek(stream->input, block->length + 4, SEEK_CUR);
	
	return (SWFBlock)block;
}

static int setCustomDimension(SWFVideoStream stream, int flags) 
{
	int ichar, rshift, mask;
	int (*method) (SWFInput stream);

	if(flags == 0) {
		method = SWFInput_getChar;
		rshift = 7;
		mask = 0xff;
	}
	else if (flags == 1) {
		method = SWFInput_getUInt16_BE;
		rshift = 15;
		mask = 0xffff;
	}
	else
		return -1;
	
	ichar = method(stream->input);
	stream->width = ( (ichar << 1) & mask );
	
	ichar = method(stream->input);
	stream->width |= ( (ichar >> rshift) & mask );
	stream->height = ( (ichar << 1) & mask );

	ichar = method(stream->input);
	stream->height |= ( (ichar >> rshift) & mask );
	
	return 0;
}

static int setStreamDimension(SWFVideoStream stream)
{
	struct FLVTag_s tag;
	int ichar;
	int flags;
	
	SWFInput_seek(stream->input, stream->start, SEEK_SET);

	if(!getNextFLVTag(stream->input, &tag) || tag.tagType != VIDEOTAG) 
		return -1;	
	else 
	{
		/* skip:
		 * videodata-header 1 byte
		 * pictureStartCode UB[17] 2 byte ( 1 bit remaining )
		 * Version UB[5] 
		 * Temporal Reference UB[8]
		 * Picture Size UB[3] 
		 * */
		SWFInput_seek(stream->input, 3, SEEK_CUR);
		ichar = SWFInput_getUInt16_BE(stream->input);
		
		/* 3-bit flag */
		flags = ((0x0003 & ichar) << 1);
		
		ichar = SWFInput_getChar(stream->input);
		flags |= ((0x80 & ichar) >> 7);
	
		switch(flags) {

			case 6:
				stream->width = 160;
				stream->height = 120;
				return 0;

			case 5:
				stream->width = 320;
				stream->height = 240;
				return 0;

			case 4:
				stream->width = 128;
				stream->height = 96;
				return 0;

			case 3:
				stream->width = 176;
				stream->height = 144;
				return 0;
				
			case 2: 
				stream->width = 352;
				stream->height = 288;
				return 0;

			default:
				SWFInput_seek(stream->input, -1, SEEK_CUR);
				return setCustomDimension(stream, flags);
		}
						
	}
}
		
		
		
		


static int getNumFrames(SWFVideoStream stream) 
{

	int numFrames = 0;
	int seek;
	struct FLVTag_s tag;
	
	SWFInput_seek(stream->input, stream->start, SEEK_SET);
	
	while(getNextFLVTag(stream->input, &tag)) {
		if(tag.tagType == VIDEOTAG)
			numFrames++;
		seek = tag.dataSize + 4;
		SWFInput_seek(stream->input, seek, SEEK_CUR);
	}

	return numFrames;
}


void writeSWFVideoStreamToMethod(SWFBlock block, SWFByteOutputMethod method, void *data)
{
	SWFVideoStream stream = (SWFVideoStream)block;
	
	methodWriteUInt16(CHARACTERID(stream), method, data);
	methodWriteUInt16(stream->numFrames, method, data);
	methodWriteUInt16(stream->width, method, data);
	methodWriteUInt16(stream->height, method, data);
	
	if(stream->embeded) {
		method(VIDEO_SMOOTHING, data);
		method(VIDEO_CODEC_H263, data);
	}
	/*
	 * in case of streaming video, flags and codec id must be 0x0
	 * if not 0 it craches player + browser
	 */
	else {
		method(0x0, data);
		method(0x0, data);
	}
};


int completeSWFVideoStream(SWFBlock block) {
	return 10;
}


void destroySWFVideoStream(SWFBlock stream) {
	destroySWFCharacter((SWFCharacter) stream);
}


SWFVideoStream
newSWFVideoStream_fromInput(SWFInput input) {

	int ichar;
	unsigned long ulchar;
	SWFBlock block;
	SWFVideoStream stream;
	
	if(!input)
		return NULL;
	
	stream = (SWFVideoStream)malloc(sizeof(struct SWFVideoStream_s));
	if(!stream)
		return NULL;
	block = (SWFBlock)stream;
	
	
	SWFCharacterInit((SWFCharacter)stream);
	CHARACTERID(stream) = ++SWF_gNumCharacters;
        
	block->type = SWF_DEFINEVIDEOSTREAM;
	block->writeBlock = writeSWFVideoStreamToMethod;
	block->complete = completeSWFVideoStream;
	block->dtor = destroySWFVideoStream;
	
	stream->input = input;
	stream->frame = 0;
	stream->embeded = 1;

	if(!stream)
		return NULL;

	ichar = SWFInput_getChar(stream->input);
	if(ichar == EOF || ichar != 'F')
		return NULL;
	
	ichar = SWFInput_getChar(stream->input);
	if(ichar == EOF || ichar != 'L')
		return NULL;
	
	ichar = SWFInput_getChar(stream->input);
	if(ichar == EOF || ichar != 'V')
		return NULL;
	
	/*
	 * FLV Version 
	 */
	ichar = SWFInput_getChar(stream->input);
	if(ichar != EOF)
		stream->version = ichar;

	/*
	 * Audio / Video-Tags persent ? 
	 */
	ichar = SWFInput_getChar(stream->input);
	if(ichar != EOF) 
		stream->flags = ichar;

	/*
	 * UI32_BE: DataOffset
	 * should be 9 for FLV-Version 1
	 *
	 * start = DataOffset
	 * add 4: before first Tag there is a UI32 0
	 */
	ulchar = SWFInput_getUInt32_BE(stream->input);
	// XXX: CHECK ulchar for validity !!!
	stream->start = ulchar + 4;
	stream->numFrames = getNumFrames(stream);
	if(setStreamDimension(stream) < 0)
		return NULL;
	
	return stream;
}

SWFVideoStream newSWFVideoStream() {
	SWFBlock block;
	SWFVideoStream stream = (SWFVideoStream)malloc(sizeof(struct SWFVideoStream_s));
        if(!stream)
                return NULL;
        block = (SWFBlock)stream;
                                                                                                                                             
        SWFCharacterInit((SWFCharacter)stream);
        CHARACTERID(stream) = ++SWF_gNumCharacters;
                                                                                                                                             
        block->type = SWF_DEFINEVIDEOSTREAM;
        block->writeBlock = writeSWFVideoStreamToMethod;
        block->complete = completeSWFVideoStream;
        block->dtor = destroySWFVideoStream;
                                                                                                                                             
        stream->input = NULL;
        stream->frame = 0;
        stream->embeded = 0;
	stream->numFrames = 0;
	stream->start = 0;

	stream->width = VIDEO_DEF_WIDTH;
	stream->height = VIDEO_DEF_HEIGHT;
	return stream;
}
	


SWFVideoStream newSWFVideoStream_fromFile(FILE *f) {
	return newSWFVideoStream_fromInput(newSWFInput_file(f));
}

void SWFVideoStream_setDimension(SWFVideoStream stream, int width, int height) {
	stream->width = width;
	stream->height = height;
}

int SWFVideoStream_getNumFrames(SWFVideoStream stream) {
	if(!stream)
		return -1;
	else if(!stream->embeded)
		return -1;
	else
		return stream->numFrames;
}
