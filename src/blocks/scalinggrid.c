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
#include "ming.h"
#include "block.h"
#include "character.h"
#include "output.h"
#include "method.h"
#include "rect.h"
#include "scalinggrid.h"

static inline int isValidGridChar(SWFCharacter c)
{
	switch(BLOCK(c)->type)
	{
		case SWF_DEFINEBUTTON:
		case SWF_DEFINEBUTTON2:
		case SWF_DEFINESPRITE:
			return 1;
		default:
			return 0;	
	}
}

static void
writeSWFScalingGridToMethod(SWFBlock block, SWFByteOutputMethod method, void* data)
{
	SWFScalingGrid grid = (SWFScalingGrid)block;
	SWFOutput_writeToMethod(grid->out, method, data);	
}

static int
completeSWFScalingGrid(SWFBlock block)
{
	SWFScalingGrid grid = (SWFScalingGrid)block;

	SWFOutput_writeUInt16(grid->out, grid->charId);
	SWFOutput_writeRect(grid->out, grid->rect);

	return SWFOutput_getLength(grid->out);
}

void
destroySWFScalingGrid(SWFScalingGrid grid)
{
	destroySWFOutput(grid->out);
	destroySWFRect(grid->rect);
	free(grid);
}


SWFScalingGrid
newSWFScalingGrid(SWFCharacter c, int x, int y, int w, int h)
{
        SWFScalingGrid grid;

	if(!isValidGridChar(c))
	{
		SWF_warn("ScalingGrid only available for buttons and sprites/movieclips\n");
		return NULL;
	} 
	
	grid = (SWFScalingGrid)malloc(sizeof(struct SWFScalingGrid_s));

        SWFBlockInit(BLOCK(grid));
        BLOCK(grid)->type = SWF_DEFINESCALINGGRID;
        BLOCK(grid)->writeBlock = writeSWFScalingGridToMethod;
        BLOCK(grid)->complete = completeSWFScalingGrid;
        BLOCK(grid)->dtor = (destroySWFBlockMethod) destroySWFScalingGrid;
	
	grid->rect = newSWFRect(x*20, y*20, (x+w)*20, (y+h)*20);
	grid->charId = c->id;
	grid->out = newSWFOutput();
        return grid;
}
