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

/* $Id$ */

#ifndef __C2MAN__
#include <stdlib.h>
#include <string.h>
#endif

#include "protect.h"
#include "blocktypes.h"
#include "method.h"
#include "libming.h"
#include "outputblock.h"


static void
writeSWFProtectToMethod(SWFBlock block, SWFByteOutputMethod method, void* data)
{
        SWFOutput out = ((SWFProtect)block)->out;
        SWFOutput_writeToMethod(out, method, data);
}


static int
completeSWFProtect(SWFBlock block)
{
	SWFProtect protect = (SWFProtect)block;

	if( protect->Password ) {
		SWFOutput_writeString(protect->out, (byte*)protect->Password);
	}

	return SWFOutput_getLength(protect->out);
}


void
destroySWFProtect(SWFProtect protect)
{
	if( protect->out )
		destroySWFOutput(protect->out );
	if( protect->Password )
		free(protect->Password);
}


SWFBlock
newSWFProtect(const char *password)
{
	SWFProtect protect = (SWFProtect)malloc(sizeof(struct SWFProtect_s));

	SWFBlockInit(BLOCK(protect));
	BLOCK(protect)->type = SWF_PROTECT;
	BLOCK(protect)->writeBlock = writeSWFProtectToMethod;
	BLOCK(protect)->complete = completeSWFProtect;
	BLOCK(protect)->dtor = (destroySWFBlockMethod) destroySWFProtect;

	protect->out = newSWFOutput();
	if(password != NULL)
		protect->Password = strdup(password);
	else
		protect->Password = NULL;

	return (SWFBlock)protect;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
