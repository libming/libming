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

#include "ming.h"

extern int SWF_gNumCharacters;

int SWF_versionNum = 5;

float Ming_scale = 20.0;
int Ming_cubicThreshold = 10000;

/* module-wide initialization.	return non-zero if error. */

int Ming_init()
{
	SWF_gNumCharacters = 0;
	SWF_versionNum = 5;

	Ming_cubicThreshold = 10000;
	Ming_scale = 20.0;

	return 0;
}


void
Ming_cleanup()
{
	Ming_cleanupFonts();
}


void Ming_setScale(float scale)
{
	Ming_scale = scale;
}


float Ming_getScale()
{
	return Ming_scale;
}


void Ming_setCubicThreshold(int num)
{
	Ming_cubicThreshold = num;
}


void Ming_setWarnFunction(void (*warn)(char *msg, ...))
{
	setSWFWarnFunction(warn);
}


void Ming_setErrorFunction(void (*error)(char *msg, ...))
{
	setSWFErrorFunction(error);
}


void Ming_useSWFVersion(int version)
{
	if(version < 4 || version > 5)
		SWF_error("Only SWF versions 4 and 5 are currently supported!\n");

	SWF_versionNum = version;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
