/*
    Ming, an SWF output library
    Copyright (C) 2001  Opaque Industries - http://www.opaque.net/
	
    3.3.2007 Klaus Rechert

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
#include "scriptlimits.h"

static void
writeSWFScriptLimitsToMethod(SWFBlock block, SWFByteOutputMethod method, void* data)
{
	SWFScriptLimits sl = (SWFScriptLimits)block;
	
	methodWriteUInt16(sl->maxRecursion, method, data);
	methodWriteUInt16(sl->timeout, method, data);
}

static int
completeSWFScriptLimits(SWFBlock block)
{
	return 4;
}

void
destroySWFScriptLimits(SWFScriptLimits sl)
{
	free(sl);	
}

void 
SWFScriptLimits_maxRecursion(SWFScriptLimits sl, int count)
{
	sl->maxRecursion = count;
}

void 
SWFScriptLimits_setTimeout(SWFScriptLimits sl, int timeout)
{
	sl->timeout = timeout;
}

SWFScriptLimits
newSWFScriptLimits()
{
        SWFScriptLimits sl = (SWFScriptLimits)malloc(sizeof(struct SWFScriptLimits_s));

        SWFBlockInit(BLOCK(sl));
        BLOCK(sl)->type = SWF_SCRIPTLIMITS;
        BLOCK(sl)->writeBlock = writeSWFScriptLimitsToMethod;
        BLOCK(sl)->complete = completeSWFScriptLimits;
        BLOCK(sl)->dtor = (destroySWFBlockMethod) destroySWFScriptLimits;
	
	sl->maxRecursion = 265;
	sl->timeout = 15;
        return sl;
}

