/*
    
   Ming, an SWF output library
   Copyright (C) 2002  Opaque Industries - http://www.opaque.net/
   
   1.2.2007 Klaus Rechert
   
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


#ifndef _FLV_H_
#define _FLV_H_

#define AUDIO_CODEC_UNCOMPRESSED   0
#define AUDIO_CODEC_ADPCM	0x01
#define AUDIO_CODEC_MP3		0x02
#define AUDIO_CODEC_NELLY_MONO	0x05
#define AUDIO_CODEC_NELLY	0x06

#define VIDEO_CODEC_H263 	0x02
#define VIDEO_CODEC_SCREEN 	0x03
#define VIDEO_CODEC_VP6		0x04
#define VIDEO_CODEC_VP6A	0x05
#define VIDEO_CODEC_SCREEN2	0x06

#define FLV_AUDIO_PRESENT 	0x04
#define FLV_VIDEO_PRESENT 	0x01

#define FLV_VIDEOTAG 		0x09
#define FLV_AUDIOTAG 		0x08
#define FLV_SCRIPTTAG 		0x12

struct FLVStream_s
{
	char 		version;
	SWFInput 	input;
	
	unsigned int 	offset;
	unsigned int 	stream_start;

	char 		has_video;
	char 		has_audio;
};


struct video_hdr 
{
	unsigned int codec;
	unsigned int frameType;
};

struct audio_hdr 
{
	unsigned char format;
	unsigned char samplingRate;
	unsigned char sampleSize;
	unsigned char channel;
};


struct FLVTag_s
{
	struct FLVStream_s 	*stream;
	int 			tagType;
	long 		dataSize;
	long 		timeStamp;
	long 		offset;
	long		data;
	union {
		struct video_hdr video;
		struct audio_hdr audio;
	} hdr;
};

typedef struct FLVTag_s 	FLVTag;
typedef struct FLVStream_s 	FLVStream;

int FLVStream_getNumFrames(FLVStream *flv, int type);
int FLVStream_nextTag(FLVStream *flv, FLVTag *tag, FLVTag *prev);
int FLVStream_nextTagType(FLVStream *flv, FLVTag *tag, FLVTag *prev, int type);
FLVStream *FLVStream_fromInput(SWFInput input);
void destroyFLVStream(FLVStream *flv);
SWFInput FLVTag_getPayloadInput(FLVTag *tag);
int FLVStream_setStreamOffset(FLVStream *flv, unsigned int msecs);
unsigned int FLVStream_getDuration(FLVStream *flv, int type);
#endif
