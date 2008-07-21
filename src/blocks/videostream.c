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
#include "../displaylist.h"

#include "flv.h"

#define VIDEO_SMOOTHING 0x01 
#define VIDEO_NO_SMOOTHING 0x0
#define VIDEOFRAME_BLOCK_SIZE 10

struct SWFVideoStream_s
{
	struct SWFCharacter_s character;
	
	FLVStream *flv;
	FLVTag *lastTag;
	int lastFrame;	

	int numFrames;	
	unsigned int frame;
	int width;
	int height;
	unsigned short embedded;
	unsigned char codecId;
	unsigned char smoothingFlag;
	int mode;
	int addFrame;
	int framesLoaded;
	int firstFrame;
};

struct SWFVideoFrame_s
{
	struct SWFBlock_s block;
	
	SWFVideoStream stream;
	int frameNum;

	FLVTag tag;
};

int SWFVideoStream_getFrameNumber(SWFVideoFrame frame) 
{
	return frame->frameNum;
}

int completeSWFVideoFrame(SWFBlock block) 
{
	SWFVideoFrame frame = (SWFVideoFrame)block;
	SWFInput input;
		
	input = FLVTag_getPayloadInput(&frame->tag);
	if(input == NULL)
		return 4;

	return SWFInput_length(input) + 4;
}

void writeSWFVideoFrameToMethod(SWFBlock block, SWFByteOutputMethod method, void *data) 
{
	int i, ichar, len;
	SWFVideoFrame frame = (SWFVideoFrame)block;
	SWFVideoStream stream;
	SWFInput input;
	
	if(!block)
		return;
	
	input = FLVTag_getPayloadInput(&frame->tag);
	if(input == NULL)
		return;
	len = SWFInput_length(input);
	stream = frame->stream;
	
	methodWriteUInt16(CHARACTERID(stream),method, data);
	methodWriteUInt16(frame->frameNum, method, data);

	for (i = 0; i < len; i++) {
		ichar = SWFInput_getChar(input);
		method(ichar, data);		
	}
}

/*
 * embedes a video frame in a SWFBlock object
 * This function reads a video frame for a FLV source and embeds it in a 
 * SWFBlock object.
 */
SWFBlock
SWFVideoStream_getVideoFrame(SWFVideoStream stream /* associated video stream */) {
	SWFVideoFrame block;
	int frame;

	if(!stream->embedded)
		return NULL;
	
	if(stream->frame >= stream->numFrames)
	{
		SWF_warn("SWFVideoStream_getVideoFrame: frame %i, numFrames %i\n", 
			stream->frame, stream->numFrames);
		return NULL;
	}

	if(stream->frame < stream->framesLoaded)
		return NULL;

	block = (SWFVideoFrame) malloc(sizeof(struct SWFVideoFrame_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == block)
		return NULL;

	SWFBlockInit((SWFBlock)block);

	BLOCK(block)->complete = completeSWFVideoFrame;
	BLOCK(block)->writeBlock = writeSWFVideoFrameToMethod;
	BLOCK(block)->dtor = NULL;
	BLOCK(block)->type = SWF_VIDEOFRAME;

	block->stream = stream;
	if(stream->lastTag != NULL && stream->lastFrame < stream->frame)
	{
		frame = stream->lastFrame;
	}
	else
	{
		stream->lastTag = NULL;
		frame = -1;
	}

	do {
		if(FLVStream_nextTag(stream->flv, &block->tag, stream->lastTag))
		{
			free(block);	
			return NULL;
		}
		stream->lastTag = &block->tag;
		if(block->tag.tagType == FLV_VIDEOTAG)
		{
			frame++;
		}
	} while (frame != stream->frame);
	
	block->frameNum = stream->frame;	
	stream->lastFrame = stream->frame;	
	stream->framesLoaded = stream->frame + 1;
	return (SWFBlock)block;
}

		
static int setH263CustomDimension(SWFVideoStream stream, SWFInput input, int flags) 
{
	int ichar, rshift, mask;
	int (*method) (SWFInput stream);

	SWFInput_seek(input, -1, SEEK_CUR);
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
	
	ichar = method(input);
	stream->width = ( (ichar << 1) & mask );
	
	ichar = method(input);
	stream->width |= ( (ichar >> rshift) & mask );
	stream->height = ( (ichar << 1) & mask );

	ichar = method(input);
	stream->height |= ( (ichar >> rshift) & mask );
	
	return 0;
}

static int setH263StreamDimension(SWFVideoStream stream, FLVTag *tag)
{
	SWFInput input;
	int ichar, flags;

	input = FLVTag_getPayloadInput(tag);
	if(input == NULL)
		return -1;

	/* skip:
	 * pictureStartCode UB[17] 2 byte ( 1 bit remaining )
	 * Version UB[5] 
	 * Temporal Reference UB[8]
	 * Picture Size UB[3] 
	 * */
	SWFInput_seek(input, 2, SEEK_CUR);
	ichar = SWFInput_getUInt16_BE(input);
		
		/* 3-bit flag */
	flags = ((0x0003 & ichar) << 1);
		
	ichar = SWFInput_getChar(input);
	flags |= ((0x80 & ichar) >> 7);
	
	switch(flags) 
	{
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
			return setH263CustomDimension(stream, input, flags);
	}
}				

static int setScreenStreamDimension(SWFVideoStream stream, FLVTag *tag) 
{
	unsigned int ui16 = 0;
	int ic;
	SWFInput input;
	
	input = FLVTag_getPayloadInput(tag);
	if(input == NULL)
		return -1;
	
	/* special case: skip 1 byte */ 
	ic  = SWFInput_getChar(input);
	
	ic = SWFInput_getChar(input);
	if(ic >= 0) 
		ui16 = ic << 8;
	
	ic = SWFInput_getChar(input);
	if(ic >= 0)
		ui16 |= ic;

	stream->width = ui16 & 0x0fff;

	ic = SWFInput_getChar(input);
	if(ic >= 0) 
		ui16 = ic << 8;
	
	ic = SWFInput_getChar(input);
	if(ic >= 0)
		ui16 |= ic;

	stream->height = ui16 & 0x0fff;
	
	return 0;

}


void writeSWFVideoStreamToMethod(SWFBlock block, SWFByteOutputMethod method, void *data)
{
	SWFVideoStream stream = (SWFVideoStream)block;
	
	methodWriteUInt16(CHARACTERID(stream), method, data);
	methodWriteUInt16(stream->numFrames, method, data);
	methodWriteUInt16(stream->width, method, data);
	methodWriteUInt16(stream->height, method, data);
	
	if(stream->embedded) {
		method(stream->smoothingFlag, data);
		method(stream->codecId, data);
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
	return VIDEOFRAME_BLOCK_SIZE;
}


void destroySWFVideoStream(SWFVideoStream stream) {
	destroySWFCharacter((SWFCharacter) stream);
}


static int setVP6Dimension(SWFVideoStream stream, FLVTag *tag)
{
	SWFInput input;
	int ichar;
	int render_x, render_y;

	input = FLVTag_getPayloadInput(tag);
	if(input == NULL)
		return -1;

	
	ichar = SWFInput_getChar(input);
	if(ichar == EOF) 
		return -1;

	if(ichar >> 7)
	{
		SWF_warn("setVP6Dimension: first frame is interframe\n");
		return -1;
	}

	if(ichar & 1)
	{
		SWF_warn("setVP6Dimension: VP60!\n");
		return -1;
	}

	ichar = SWFInput_getChar(input);
	ichar = SWFInput_getChar(input);
	render_x = SWFInput_getChar(input);
	render_y = SWFInput_getChar(input);

	stream->width = render_x * 16;
	stream->height = render_y * 16;	
	return 0;	
}


static int setStreamProperties(SWFVideoStream stream) 
{
	int ret;
	FLVTag tag, *tag_p = NULL;
	
	stream->numFrames = FLVStream_getNumFrames(stream->flv, FLV_VIDEOTAG);

	while((ret = FLVStream_nextTag(stream->flv, &tag, tag_p)) == 0) 
	{
		if(tag.tagType == FLV_VIDEOTAG)
			break;

		tag_p = &tag;
	}
	
	if(ret < 0)
		return -1;

	stream->codecId = tag.hdr.video.codec;	
	switch (stream->codecId) {
		case VIDEO_CODEC_H263:
			ret = setH263StreamDimension(stream, &tag);
			stream->smoothingFlag = VIDEO_SMOOTHING;
			break;
		case VIDEO_CODEC_SCREEN:
			ret = setScreenStreamDimension(stream, &tag);
			stream->smoothingFlag = 0;
			break;
		case VIDEO_CODEC_VP6:
			setVP6Dimension(stream, &tag);
			stream->smoothingFlag = VIDEO_SMOOTHING;
			ret = 0;
			break;
		case VIDEO_CODEC_VP6A:
		case VIDEO_CODEC_SCREEN2:
			SWF_warn("setStreamProperties: automatic dimension setting is not working with this codec yet!\n");
			ret = 0;
			break;
		default:
			SWF_warn("Unknown Codec %x\n", stream->codecId);
			ret = -1;
	}
	return ret;
}


static int onPlace(SWFDisplayItem item, SWFBlockList blocklist)
{
	SWFVideoStream stream = (SWFVideoStream)SWFDisplayItem_getCharacter(item); 
	SWFBlock video = SWFVideoStream_getVideoFrame(stream);
	if(video == NULL)
		return 0;
        SWFBlockList_addBlock(blocklist, video);
	stream->firstFrame = 0;
	return 1;
}

static int onFrame(SWFDisplayItem item, SWFBlockList blocklist)
{
	SWFPlaceObject2Block placeVideo;
	SWFVideoStream stream;
	SWFBlock video = NULL;

	/* if item is new -> onInit already inserted a frame */	
	if(item->flags != 0)
		return 0;

	stream = (SWFVideoStream)SWFDisplayItem_getCharacter(item);
	if(stream->mode == SWFVIDEOSTREAM_MODE_MANUAL &&
		stream->addFrame == 0)
		return 0;
	
	if(stream->mode != SWFVIDEOSTREAM_MODE_MANUAL)
		stream->frame++;

	if(stream->frame >= stream->framesLoaded)
	{
		video = SWFVideoStream_getVideoFrame(stream);
		if(video == NULL)
			return 0;
	}
	
	placeVideo = newSWFPlaceObject2Block(item->depth);
	SWFPlaceObject2Block_setRatio(placeVideo, stream->frame);
	SWFPlaceObject2Block_setMove(placeVideo);
	SWFBlockList_addBlock(blocklist, (SWFBlock)placeVideo);               
	if(video != NULL)
		SWFBlockList_addBlock(blocklist, video);
	
	stream->addFrame = 0;
	return 2;
}


/**
 * Seek within a video stream.
 *
 * This functions allows seeking in video stream. Semantics 
 * like SWFInput_seek(); 
 *
 * Works only with SWFVIDEOSTREAM_MODE_MANUAL!
 * @return old video position (frame)
 */
int SWFVideoStream_seek(SWFVideoStream stream, int frame, int whence)
{
	int old, pos;

	if(stream == NULL || stream->embedded == 0)
		return -1;

	if(stream->mode != SWFVIDEOSTREAM_MODE_MANUAL)
		return -1;

	old = stream->frame;
	switch(whence)
	{
	case SEEK_SET: 
		if(frame < 0 || frame >= stream->numFrames)
			return -1;
		stream->frame = frame;
		break;
	case SEEK_END:
		if(frame < 0 || frame >= stream->numFrames)	
 			return -1;
		stream->frame = stream->numFrames - frame;
		break;
	case SEEK_CUR:
		pos = stream->frame + frame;
		if(pos < 0 || pos >= stream->numFrames)
			return -1;
		break;
	default:
		return -1;
	}
	stream->addFrame = 1;
	return old;
} 

/**
 * Display next video frame
 *
 * Works only with embedded video streams. 
 *
 * @return -1 if an error happend.
 */
int SWFVideoStream_nextFrame(SWFVideoStream stream)
{
	if(stream == NULL || !stream->embedded)
		return -1;

	if(stream->mode != SWFVIDEOSTREAM_MODE_MANUAL)
		return -1;

	if(stream->addFrame == 1 || stream->firstFrame == 1)
		return 0;

	stream->addFrame = 1;
	stream->frame++;
	return 0;
}

/**
 * switch video stream frame mode
 * If the mode == SWFVIDEOSTREAM_MODE_AUTO (default) every swfmovie 
 * frame a video frame is added.
 * In SWFVIDEOSTREAM_MODE_MANUAL mode, the user needs to call 
 * SWFVideoStream_nextFrame() to change the video's frame. 
 *
 * Works only with embedded video streams.
 *
 * @return the previous mode or -1 if an invalid mode was passed.
 */
int SWFVideoStream_setFrameMode(SWFVideoStream stream, int mode)
{
	int oldmode;
	if(stream == NULL || !stream->embedded)
		return -1;
	
	oldmode = stream->mode;
	switch(mode)
	{
	case SWFVIDEOSTREAM_MODE_AUTO:
		stream->mode = mode;
		return oldmode;
	case SWFVIDEOSTREAM_MODE_MANUAL:
		stream->mode = mode;
		return oldmode;
	default:
		SWF_warn("SWFVideoStream_setFrameMode: mode %i is unknown", mode);
		return -1;	
	}
}

/* 
 * create a new SWFVideoSteam object
 * This function creates a new videostream object from a FLV-file.
 * Takes a SWFInput object as argument. 
 * Be aware: You need to keep the SWFInput valid until the movie is generated via save() or output()!
 */
SWFVideoStream
newSWFVideoStream_fromInput(SWFInput input) {

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
 	((SWFCharacter)stream)->onFrame = onFrame;       
 	((SWFCharacter)stream)->onPlace = onPlace;       
	block->type = SWF_DEFINEVIDEOSTREAM;
	block->writeBlock = writeSWFVideoStreamToMethod;
	block->complete = completeSWFVideoStream;
	block->dtor = (destroySWFBlockMethod)destroySWFVideoStream;
	
	stream->flv = FLVStream_fromInput(input);
	if(stream->flv == NULL)
	{
		free(stream);
		return NULL;
	}
	stream->lastTag = NULL;
	stream->lastFrame = 0;
	stream->frame = 0;
	stream->embedded = 1;
	stream->mode = SWFVIDEOSTREAM_MODE_AUTO;
	stream->addFrame = 0;
	stream->framesLoaded = 0;
	stream->firstFrame = 1;
	stream->width = VIDEO_DEF_WIDTH;
	stream->height = VIDEO_DEF_HEIGHT;
	if (setStreamProperties(stream) < 0)
	{
		free(stream);
		return NULL;
	}
	return stream;
}

/*
 * creates a new SWFVideoStream object
 * This function creates an empty videostream object. This object can be adressed via
 * ActionScript to connect and display a streamed video (progessive download / rtmp).
 */
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
        block->dtor = (destroySWFBlockMethod)destroySWFVideoStream;
        
	stream->flv = NULL;
	stream->lastTag = NULL;
        stream->frame = 0;
        stream->embedded = 0;
	stream->numFrames = -1;

	stream->width = VIDEO_DEF_WIDTH;
	stream->height = VIDEO_DEF_HEIGHT;
	return stream;
}

	
/* 
 * create a new SWFVideoSteam object
 * This function creates a new videostream object from a FLV-file.
 * Takes a FILE * as argument. 
 * Be aware: You need to keep the FILE open until the movie is generated via save() or output()!
 */
SWFVideoStream newSWFVideoStream_fromFile(FILE *f /* FILE pointer to a FLV-file */) {
	return newSWFVideoStream_fromInput(newSWFInput_file(f));
}


/* 
 * sets video dimension 
 * This function set width and height for streamed videos 
 * Works only _streamed_ videos (progressive download or rtmp)
 */
void SWFVideoStream_setDimension(SWFVideoStream stream /* stream object */, 
				int width, /* width in px */
				int height /* height in px */) 
{
	if(!stream->embedded) {
		stream->width = width;
		stream->height = height;
	}
}


/*
 * returns the number of video-frames
 * This function returns the number of video-frames of a SWFVideoStream.
 * Works only for embedded streams! 
 */
int SWFVideoStream_getNumFrames(SWFVideoStream stream /* Embedded video stream */) {
	if(!stream)
		return -1;
	return stream->numFrames;
}

/*
 * returns 1 if embedded video stream has also an audio stream
 * This functions test if the embedded FLV stream also has audio-data.
 */
int SWFVideoStream_hasAudio(SWFVideoStream stream /* Embedded video stream */)
{
	if(stream != NULL && stream->flv != NULL && stream->flv->has_audio)
		return 1;
	
	return 0;
}
