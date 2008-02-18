/*
    Ming, an SWF output library
    Copyright (C) 2007  Klaus Rechert

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
#include "block.h"
#include "output.h"

struct SWFSymbolClass_s
{
	struct SWFBlock_s block;
	SWFOutput out;
	int numSymbols;
	int *cIds;
	char **names;
};

static void
writeSWFSymbolClassToMethod(SWFBlock block, 
                            SWFByteOutputMethod method, void* data)
{
	SWFOutput out = ((SWFSymbolClass)block)->out;
        SWFOutput_writeToMethod(out, method, data);
}

static int
completeSWFSymbolClass(SWFBlock block)
{
	int i;
	SWFSymbolClass sclass = (SWFSymbolClass)block;
	
	sclass->out = newSWFOutput();
	SWFOutput_writeUInt16(sclass->out, sclass->numSymbols);
	for(i = 0; i < sclass->numSymbols; i++)
	{
		SWFOutput_writeUInt16(sclass->out, sclass->cIds[i]);
		SWFOutput_writeString(sclass->out, (byte *)sclass->names[i]);
	}
	return SWFOutput_getLength(sclass->out);
}

void
destroySWFSymbolClass(SWFSymbolClass sclass)
{
	int i;

	for(i = 0; i < sclass->numSymbols; i++)
	{
		free(sclass->names[i]);
	}
	if(sclass->names)
		free(sclass->names);
	if(sclass->cIds)
		free(sclass->cIds);

	destroySWFOutput(sclass->out);
	free(sclass);
}

void
SWFSymbolClass_addSymbol(SWFSymbolClass sclass, 
                         SWFCharacter character, const char *name)
{
	size_t size;
	if(sclass == NULL || name == NULL)
		return;
	
	size = (sclass->numSymbols + 1) * sizeof(int);
	sclass->cIds = (int*)realloc(sclass->cIds, size);
	
	size = (sclass->numSymbols + 1) * sizeof(char *);
	sclass->names = (char **)realloc(sclass->names, size);

	sclass->names[sclass->numSymbols] = strdup(name);
	if(character == NULL)
		sclass->cIds[sclass->numSymbols] = 0;
	else
		sclass->cIds[sclass->numSymbols] = character->id;
 
	sclass->numSymbols++;
}

SWFSymbolClass newSWFSymbolClass()
{
        SWFSymbolClass sclass = (SWFSymbolClass)malloc(sizeof(struct SWFSymbolClass_s));

        SWFBlockInit(BLOCK(sclass));
        BLOCK(sclass)->type = SWF_SYMBOLCLASS;
        BLOCK(sclass)->writeBlock = writeSWFSymbolClassToMethod;
        BLOCK(sclass)->complete = completeSWFSymbolClass;
        BLOCK(sclass)->dtor = (destroySWFBlockMethod) destroySWFSymbolClass;

	sclass->numSymbols = 0;
	sclass->names = NULL;
	sclass->cIds = NULL;
	sclass->out = NULL;
	
        return sclass;
}

