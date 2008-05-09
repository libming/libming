/*
    Ming, an SWF output library
    Copyright (C) 2007 Klaus Rechert

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



#ifndef __MP3_H
#define __MP3_H

#define MP3_VERSION_25			0
#define MP3_VERSION_RESERVED		1
#define MP3_VERSION_2			2
#define MP3_VERSION_1			3

#define MP3_LAYER_RESERVED		0
#define MP3_LAYER_3			1
#define MP3_LAYER_2			2
#define MP3_LAYER_1			3

#define MP3_CHANNEL_STEREO		0	
#define MP3_CHANNEL_JOINT		1
#define MP3_CHANNEL_DUAL		2
#define MP3_CHANNEL_MONO		3

#define SWFSOUND_INITIAL_DELAY 1663 

struct mp3_header
{
	unsigned int version;
	unsigned int layer;
	unsigned int bitrate;
	unsigned int samplingRate;
	unsigned int padding;
	unsigned int channelMode;
};	

int nextMP3Frame(SWFInput input);
int readMP3Header(SWFInput input, struct mp3_header *mp3h);
int getMP3Flags(SWFInput input, byte *flags);
int getMP3Samples(SWFInput input, int flags, int *wanted);
unsigned int getMP3Duration(SWFInput input);
#endif
