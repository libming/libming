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

#include <stdlib.h>

#include "soundinstance.h"
#include "sound.h"
#include "block.h"
#include "method.h"
#include "character.h"
#include "libming.h"


typedef struct
{
	unsigned int mark44;
	unsigned short level0;
	unsigned short level1;
} envPoint;

struct SWFSoundInstance_s
{
	struct SWFBlock_s block;

	SWFSound sound;
	unsigned int inPoint;
	unsigned int outPoint;
	int numLoops;
	byte flags;
	byte numEnvPoints;
	envPoint *envPoints;
};

void writeSWFSoundInstanceToMethod(SWFBlock block,
																	 SWFByteOutputMethod method, void *data)
{
	SWFSoundInstance sound;
	byte flags;
	int i;

	// block may be null if we're calling from button.c:

	if ( block == NULL )
	{
		method(0, data);
		method(0, data);
		method(0, data);
		return;
	}

	sound = (SWFSoundInstance)block;
	flags = sound->flags;

	if ( sound->sound )
		methodWriteUInt16(CHARACTERID(sound->sound), method, data);
	else
		methodWriteUInt16(0, method, data);	 /* 0 means NULL character */

	method(flags, data);

	if ( flags & SWF_SOUNDINFO_HASINPOINT )
		methodWriteUInt32(sound->inPoint, method, data);

	if ( flags & SWF_SOUNDINFO_HASOUTPOINT )
		methodWriteUInt32(sound->outPoint, method, data);

	if ( flags & SWF_SOUNDINFO_HASLOOPS )
		methodWriteUInt16(sound->numLoops, method, data);

	if ( flags & SWF_SOUNDINFO_HASENVELOPE )
	{
		method(sound->numEnvPoints, data);

		for ( i=0; i<sound->numEnvPoints; ++i )
		{
			methodWriteUInt32((sound->envPoints[i]).mark44, method, data);
			methodWriteUInt16((sound->envPoints[i]).level0, method, data);
			methodWriteUInt16((sound->envPoints[i]).level1, method, data);
		}
	}
}


int completeSWFSoundInstance(SWFBlock block)
{
	SWFSoundInstance sound;
	byte flags;

	// block may be null if we're calling from button.c:

	if ( block == NULL )
		return 3;

	sound = (SWFSoundInstance)block;
	flags = sound->flags;

	return 3 /* (sound id + flags) */ +
		((flags & SWF_SOUNDINFO_HASINPOINT) ? 4 : 0) +
		((flags & SWF_SOUNDINFO_HASOUTPOINT) ? 4 : 0) +
		((flags & SWF_SOUNDINFO_HASLOOPS) ? 2 : 0) +
		((flags & SWF_SOUNDINFO_HASENVELOPE) ? (1 + 8*sound->numEnvPoints) : 0);
}

/*
 * adds a SoundEnvelope to event sound
 */
void SWFSoundInstance_addEnvelope(SWFSoundInstance inst,
                                  unsigned int mark44 /* position in 44khz samples */, 
                                  short left /* volume left channel */, 
                                  short right /* volume right channel */)
{
	inst->flags |= SWF_SOUNDINFO_HASENVELOPE;
	inst->envPoints = (envPoint*) realloc(inst->envPoints, 
		(inst->numEnvPoints + 1) * sizeof(envPoint));
	inst->envPoints[inst->numEnvPoints].mark44 = mark44;
	inst->envPoints[inst->numEnvPoints].level0 = left;
	inst->envPoints[inst->numEnvPoints].level1 = right;
	inst->numEnvPoints++;
}

/* 
 * private constructor
 * see SWFMove[Clip]_startSound()
 */
SWFSoundInstance newSWFSoundInstance(SWFSound sound)
{
	SWFSoundInstance instance = (SWFSoundInstance)malloc(sizeof(struct SWFSoundInstance_s));
	SWFBlock block = (SWFBlock)instance;

	SWFBlockInit(block);

	block->type = SWF_STARTSOUND;

	block->writeBlock = writeSWFSoundInstanceToMethod;
	block->complete = completeSWFSoundInstance;
	/* block->dtor = destroySWFStartSoundBlock; */

	instance->sound = sound;
	instance->inPoint = 0;
	instance->outPoint = 0;
	instance->numLoops = 0;
	instance->flags = 0;
	instance->numEnvPoints = 0;
	instance->envPoints = NULL;

	return instance;
}

/* 
 * destroys a SWFSounInstance object
 */
void
destroySWFSoundInstance(SWFSoundInstance soundInstance)
{
	destroySWFBlock((SWFBlock) soundInstance);
}


SWFSoundInstance newSWFSoundInstance_stop(SWFSound sound)
{
	SWFSoundInstance instance = newSWFSoundInstance(sound);
	instance->flags = SWF_SOUNDINFO_SYNCSTOPSOUND;
	return instance;
}

SWFSoundInstance newSWFSoundInstance_startNoMultiple(SWFSound sound)
{
	SWFSoundInstance instance = newSWFSoundInstance(sound);
	instance->flags = SWF_SOUNDINFO_SYNCNOMULTIPLE;
	return instance;
}

/*
 * don't start sound if already playing 
 */
void SWFSoundInstance_setNoMultiple(SWFSoundInstance instance)
{
	instance->flags |= SWF_SOUNDINFO_SYNCNOMULTIPLE;
}

/* 
 * set loop start point
 * Sets the loop start point counted in samples
 */
void SWFSoundInstance_setLoopInPoint(SWFSoundInstance instance, 
                                     unsigned int point /* number of samples */)
{
	instance->flags |= SWF_SOUNDINFO_HASINPOINT;
	instance->inPoint = point;	
}

/* 
 * set loop stop point
 * Sets the loop's last sample to play
 */
void SWFSoundInstance_setLoopOutPoint(SWFSoundInstance instance, 
                                      unsigned int point /* number of samples */)
{
	instance->flags |= SWF_SOUNDINFO_HASOUTPOINT;
	instance->outPoint = point;	
}

/*
 * sets loop count
 */
void SWFSoundInstance_setLoopCount(SWFSoundInstance instance, int count /* loop count */)
{
	instance->flags |= SWF_SOUNDINFO_HASLOOPS;
	instance->numLoops = count;
}

/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
