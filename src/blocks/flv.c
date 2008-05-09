/*
   Ming, an SWF output library
   Copyright (C) 2002  Opaque Industries - http://www.opaque.net/
   
   2.2.2007 Klaus Rechert
   
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
#include "libming.h"
#include "error.h"
#include "flv.h"

static inline int readAudioHdr(FLVStream *flv, FLVTag *tag)
{
	int ichar;
	ichar = SWFInput_getChar(flv->input);
	if(ichar == EOF)
		return -1;
	tag->hdr.audio.format = (0xf0 & ichar);
	tag->hdr.audio.samplingRate = (0xc & ichar);
	tag->hdr.audio.sampleSize = (0x2 & ichar);
	tag->hdr.audio.channel = 0x1 & ichar;
	return 0;
}

static inline int readVideoHdr(FLVStream *flv, FLVTag *tag)
{
	int ichar;
	ichar = SWFInput_getChar(flv->input);
	if(ichar == EOF)
		return -1;
	
	tag->hdr.video.frameType = (0xf0 & ichar);
	tag->hdr.video.codec = 0x0f & ichar;
	return 0;
}

long FLVStream_skipTagData(FLVStream *flv, FLVTag *tag)
{
	if(flv == NULL || tag == NULL)
		return -1;
		
	SWFInput_seek(flv->input, tag->data + tag->dataSize + 4, SEEK_SET);
	
	return SWFInput_tell(flv->input);
}

void destroyFLVStream(FLVStream *flv)
{
	free(flv);
}

FLVStream *FLVStream_fromInput(SWFInput input)
{
	int ichar;
	unsigned long ulchar;
	FLVStream *flv;
	
	if(!input)
		return NULL;

	ichar = SWFInput_getChar(input);
	if(ichar == EOF || ichar != 'F')
		return NULL;
	
	ichar = SWFInput_getChar(input);
	if(ichar == EOF || ichar != 'L')
		return NULL;
	
	ichar = SWFInput_getChar(input);
	if(ichar == EOF || ichar != 'V')
		return NULL;

	flv = (FLVStream *)malloc(sizeof(FLVStream));
	if(flv == NULL)
		return  NULL;
	flv->input = input;
	
	ichar = SWFInput_getChar(input);
	if(ichar != EOF)
		flv->version = ichar;
	
	flv->has_video = 0;
	flv->has_audio = 0;
	ichar = SWFInput_getChar(input);
	if(ichar != EOF)
	{
		if(ichar & FLV_AUDIO_PRESENT)
			flv->has_audio = 1;

		if(ichar & FLV_VIDEO_PRESENT)
			flv->has_video = 1;
	}

	ulchar = SWFInput_getUInt32_BE(input);
	flv->offset = ulchar + 4;
	flv->stream_start = flv->offset;
	return flv;
}

void FLVStream_rewind(FLVStream *flv)
{
	SWFInput_seek(flv->input, flv->offset, SEEK_SET);
}

int FLVStream_nextTag(FLVStream *flv, FLVTag *tag, FLVTag *prev) 
{
	int ichar;
	unsigned long ulchar;
	
	if(prev == NULL)
		SWFInput_seek(flv->input, flv->offset, SEEK_SET);
	else
	{
		unsigned long off;
		if(prev->data < 0)
			return -1;
		
		off = prev->data + prev->dataSize + 4;
		SWFInput_seek(flv->input, off, SEEK_SET);
	}
	
	tag->offset = SWFInput_tell(flv->input);

	tag->stream = flv;

	ichar = SWFInput_getChar(flv->input);
	if(ichar == EOF)
		return -1;
	if(ichar != FLV_VIDEOTAG && ichar != FLV_AUDIOTAG && ichar != FLV_SCRIPTTAG)
	{
		SWF_warn("FLV: stream out of sync!\n");
		return -1;
	}
	tag->tagType = ichar;
	
	ulchar = SWFInput_getUInt24_BE(flv->input);
	tag->dataSize = ulchar;

	ulchar = SWFInput_getUInt24_BE(flv->input);
	tag->timeStamp = ulchar;

	ulchar = SWFInput_getUInt32_BE(flv->input);
	if(ulchar)
	{
		SWF_warn("FLV: stream out of sync!\n");
		return -1;
	}

	tag->data = SWFInput_tell(flv->input);
	if(tag->tagType == FLV_VIDEOTAG)
		readVideoHdr(flv, tag);
	else if(tag->tagType == FLV_AUDIOTAG)
		readAudioHdr(flv, tag);
	
	return 0;
}

int FLVStream_nextTagType(FLVStream *flv, FLVTag *tag, FLVTag *prev, int type)
{
	
	while(FLVStream_nextTag(flv, tag, prev) == 0)
	{
		if(tag->tagType == type) 
			return 0;
		prev = tag;
	}
	return -1;

}

unsigned int FLVStream_getDuration(FLVStream *flv, int type)
{
	unsigned int duration = 0;
	FLVTag tag, *p_tag = NULL;
	
	while(FLVStream_nextTag(flv, &tag, p_tag) == 0) 
	{
		if(tag.tagType == type)
		{
			/* optimistic approach */
			duration = tag.timeStamp; 
		}
		p_tag = &tag;
	}
	return duration;
}

int FLVStream_getNumFrames(FLVStream *flv, int type)
{
	int numFrames = 0;
	FLVTag tag, *p_tag = NULL;
	
	while(FLVStream_nextTag(flv, &tag, p_tag) == 0) 
	{
		if(tag.tagType == type)
			numFrames++;
		p_tag = &tag;
	}
	return numFrames;
}

SWFInput FLVTag_getPayloadInput(FLVTag *tag)
{
	int length;
	SWFInput input;
	if(tag == NULL || tag->stream == NULL)
		return NULL;
	
	input = tag->stream->input;

	/* screen video needs this extra byte undocumented! */
	if(tag->tagType == FLV_VIDEOTAG 
		&& tag->hdr.video.codec == VIDEO_CODEC_SCREEN)
	{
		length = tag->dataSize;
		SWFInput_seek(input, tag->data, SEEK_SET);
	}
	else if(tag->tagType == FLV_VIDEOTAG
                && tag->hdr.video.codec == VIDEO_CODEC_VP6)
	{
		length = tag->dataSize - 2;
		SWFInput_seek(input, tag->data + 2, SEEK_SET);
	}
	else /* skip flv-audio/video-data byte */
	{
		length = tag->dataSize - 1;
		SWFInput_seek(input, tag->data + 1, SEEK_SET);
	}

	return newSWFInput_input(input, length);
}

int FLVStream_setStreamOffset(FLVStream *flv, unsigned int msecs)
{
	FLVTag tag, *p_tag = NULL;
	
	/* reset stream offset */
	flv->offset = flv->stream_start;	
	
	while(FLVStream_nextTag(flv, &tag, p_tag) == 0) 
	{
		if(tag.timeStamp >= msecs)
		{
			flv->offset = tag.offset;
			return 0;
		}
		p_tag = &tag;
	}
	return -1;
}

