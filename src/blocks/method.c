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

#include "method.h"
#include "output.h"

#include "libming.h"

void methodWriteUInt16(int i, SWFByteOutputMethod method, void *data)
{
	method((unsigned char)(i&0xff), data);
	i>>=8;
	method((unsigned char)(i&0xff), data);
}


void methodWriteUInt32(int i, SWFByteOutputMethod method, void *data)
{
	method((unsigned char)(i&0xff), data);
	i>>=8;
	method((unsigned char)(i&0xff), data);
	i>>=8;
	method((unsigned char)(i&0xff), data);
	i>>=8;
	method((unsigned char)(i&0xff), data);
}


void fileOutputMethod(byte b, void *data)
{
	FILE *f = (FILE *)data;
	fputc(b, f);
}


void SWFOutputMethod(byte i, void *data)
{
	SWFOutput_writeUInt8( (SWFOutput) data, i);
}
/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
