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
#include "output.h"
#include "method.h"
#include "tabindex.h"

static void
writeSWFTabIndexToMethod(SWFBlock block, SWFByteOutputMethod method, void* data)
{
	SWFTabIndex ti = (SWFTabIndex)block;
	methodWriteUInt16(ti->depth, method, data);
	methodWriteUInt16(ti->index, method, data);
}


static int
completeSWFTabIndex(SWFBlock block)
{
	return 4;
}


void
destroySWFTabIndex(SWFTabIndex ti)
{
	free(ti);	
}

SWFTabIndex
newSWFTabIndex(int depth, int index)
{
        SWFTabIndex ti = (SWFTabIndex)malloc(sizeof(struct SWFTabIndex_s));

        SWFBlockInit(BLOCK(ti));
        BLOCK(ti)->type = SWF_SETTABINDEX;
        BLOCK(ti)->writeBlock = writeSWFTabIndexToMethod;
        BLOCK(ti)->complete = completeSWFTabIndex;
        BLOCK(ti)->dtor = (destroySWFBlockMethod) destroySWFTabIndex;
	ti->depth = depth;
	ti->index = index;
        return ti;
}

