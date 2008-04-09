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

#include <stdio.h>
#include <stdlib.h> // for exit()
#include <stdarg.h>
#include "error.h"
#include "libming.h"


void
warn_default(const char *msg, ...)
{
	va_list args;

	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
}


void
error_default(const char *msg, ...)
{
	va_list args;

	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	exit(EXIT_FAILURE);
}


SWFMsgFunc _SWF_warn = warn_default; //NULL;
SWFMsgFunc _SWF_error = error_default;

SWFMsgFunc
setSWFWarnFunction(SWFMsgFunc warnfunc)
{
	SWFMsgFunc old = _SWF_warn;
	_SWF_warn = warnfunc;
	return old;
}


SWFMsgFunc
setSWFErrorFunction(SWFMsgFunc errorfunc)
{
	SWFMsgFunc old = _SWF_error;
	_SWF_error = errorfunc;
	return old;
}

#if 0
void (*SWF_warn)(const char *msg, ...) = warn_default;
void (*SWF_error)(const char *msg, ...) = error_default;

void
setSWFWarnFunction(void (*warnfunc)(const char *msg, ...))
{
	SWF_warn = warnfunc;
}


void
setSWFErrorFunction(void (*errorfunc)(const char *msg, ...))
{
	SWF_error = errorfunc;
}
#endif






/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
