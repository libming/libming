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
#include "libming.h"
#include "blocks/character.h"
#include "font_util.h"
#include "shape_cubic.h"
#include "blocks/error.h"
#include "ming_config.h"


int SWF_versionNum = 5;
int SWF_compression = -1;

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
	/* TODO: destroy all dangling objects */
#if WRAP_MEMORY_MANAGEMENT
	ming_garbage_collect();
#endif
}


void Ming_setScale(float scale)
{
	Ming_scale = scale;
}

/*
 * Set output compression level.
 * Return previous value.
 */ 
int
Ming_setSWFCompression(int level)
{
	int oldlevel = SWF_compression;
	SWF_compression = level;
	return oldlevel;
}


float Ming_getScale()
{
	return Ming_scale;
}


void Ming_setCubicThreshold(int num)
{
	Ming_cubicThreshold = num;
}


void Ming_setWarnFunction(void (*warn)(const char *msg, ...))
{
	setSWFWarnFunction(warn);
}


void Ming_setErrorFunction(void (*error)(const char *msg, ...))
{
	setSWFErrorFunction(error);
}


void Ming_useSWFVersion(int version)
{
	if(version < 4 || version > 6)
		SWF_error("Only SWF versions 4, 5 and 6 are currently supported!\n");

	SWF_versionNum = version;

}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
