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

#ifndef WIN32
	#include <unistd.h>
#endif

#include <time.h>
#include <stdio.h>

#include "../blocks/output.h"
#include "../blocks/action.h"
#include "compile.h"

extern int SWF_versionNum;

SWFAction compileSWFActionCode(char *script)
{
	SWFOutput output;
	Buffer b;

	/* yydebug = 1; */

	if(SWF_versionNum == 4)
	{
		swf4ParseInit(script, 0);

		if(swf4parse((void *)&b) != 0)
			return NULL;
	}
	else
	{
		swf5ParseInit(script, 0);

		if(swf5parse((void *)&b) != 0)
			return NULL;
	}

	output = newSWFOutput();

	if(b)
	{
		SWFOutput_writeBuffer(output, b->buffer, bufferLength(b));
		destroyBuffer(b);
	}

	SWFOutput_writeUInt8(output, SWFACTION_END);

	return newSWFAction_fromOutput(output);
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
