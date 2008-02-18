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

#include <stdlib.h>
#include <string.h>
#include "ming.h"
#include "character.h"
#include "output.h"

struct SWFSceneData_s
{
	struct SWFCharacter_s character;
	
	unsigned int sceneCount;
	unsigned int *sceneOffset;
	char **sceneName;

	unsigned int frameLabelCount;
	unsigned int *frameNumber;
	char **frameLabel;
	
	SWFOutput out;
};

static void writeSWFSceneDataToMethod(SWFBlock block,
                                      SWFByteOutputMethod method, void* data)
{
	SWFSceneData sdata = (SWFSceneData)block;
	SWFOutput_writeToMethod(sdata->out, method, data);
}

static int completeSWFSceneData(SWFBlock block)
{
	SWFSceneData sdata = (SWFSceneData)block;
	int i;

	sdata->out = newSWFOutput();

	SWFOutput_writeEncUInt32(sdata->out, sdata->sceneCount);
	for(i = 0; i < sdata->sceneCount; i++)
	{
		SWFOutput_writeEncUInt32(sdata->out, sdata->sceneOffset[i]);
		SWFOutput_writeString(sdata->out, 
			(unsigned char *)sdata->sceneName[i]);
	}

	SWFOutput_writeEncUInt32(sdata->out, sdata->frameLabelCount);
	for(i = 0; i < sdata->frameLabelCount; i++)
	{
		SWFOutput_writeEncUInt32(sdata->out, sdata->frameNumber[i]);
		SWFOutput_writeString(sdata->out, 
			(unsigned char *)sdata->frameLabel[i]);
	}
	return SWFOutput_getLength(sdata->out);
}

void
destroySWFSceneData(SWFSceneData sdata)
{
	int i;
	if(sdata->sceneCount > 0)
	{
		for(i = 0; i < sdata->sceneCount; i++)
			free(sdata->sceneName[i]);
		free(sdata->sceneName);
		free(sdata->sceneOffset);
	}
	
	if(sdata->frameLabelCount > 0)
	{
		for(i = 0; i < sdata->frameLabelCount; i++)
			free(sdata->frameLabel[i]);
		free(sdata->frameNumber);
		free(sdata->frameLabel);
	}
	free(sdata);
}

void 
SWFSceneData_addScene(SWFSceneData sdata, unsigned int offset, const char *name)
{
	sdata->sceneName = (char **)realloc(sdata->sceneName, 
		(sdata->sceneCount + 1) * sizeof(char *));
	sdata->sceneOffset = (unsigned int*) realloc(sdata->sceneOffset,
		(sdata->sceneCount + 1) * sizeof(unsigned int));
	sdata->sceneName[sdata->sceneCount] = strdup(name);
	sdata->sceneOffset[sdata->sceneCount] = offset;
	sdata->sceneCount++;
}

void 
SWFSceneData_addFrameLabel(SWFSceneData sdata, unsigned int fnum, 
                           const char *label)
{
	sdata->frameLabel = (char **)realloc(sdata->frameLabel, 
		(sdata->frameLabelCount + 1) * sizeof(char *));
	sdata->frameNumber = (unsigned int*)realloc(sdata->frameNumber,
		(sdata->frameLabelCount + 1) * sizeof(unsigned int));
	sdata->frameLabel[sdata->frameLabelCount] = strdup(label);
	sdata->frameNumber[sdata->frameLabelCount] = fnum;
	sdata->frameLabelCount++;
}

SWFSceneData
newSWFSceneData()
{
	SWFSceneData sdata= (SWFSceneData)malloc(sizeof(struct SWFSceneData_s));

	SWFCharacterInit((SWFCharacter)sdata);
	BLOCK(sdata)->type = SWF_DEFINESCENEANDFRAMEDATA;
	BLOCK(sdata)->writeBlock = writeSWFSceneDataToMethod;
	BLOCK(sdata)->complete = completeSWFSceneData;
	BLOCK(sdata)->dtor = (destroySWFBlockMethod) destroySWFSceneData;
	
	sdata->sceneCount = 0;
	sdata->sceneOffset = NULL;
	sdata->sceneName = NULL;
	
	sdata->frameLabelCount = 0;
	sdata->frameNumber = NULL;
	sdata->frameLabel = NULL;

	sdata->out = NULL;
	return sdata;
}

