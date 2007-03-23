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

#include "libming.h"
#include "blocks/output.h"
#include "blocks/action.h"
#include "blocks/error.h"
#include "compile.h"
#include "actiontypes.h"


SWFAction compileSWFActionCode(const char *script)
{
	SWF_warnOnce("Please do not use compileSWFActionCode() anymore\n"
	"Use newSWFAction(script) instead\n");
	return newSWFAction(script);
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
