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

#include "libming.h"

/* this file is #include'd into shape.c, not compiled on its own.
   hacky, but it works for now.. */

static int buffer; 
static int bufbits = 0; /* # of bits in buffer */

static void byteAlign()
{
	if(bufbits > 0)
	{
		bufbits = 0;
		buffer = 0;
	}
}

static int readBitsP(byte **p, int number)
{
	byte *ptr = *p;
	int ret = buffer;

	if(number == bufbits)
	{
		ret = buffer;
		bufbits = 0;
		buffer = 0;
	}
	else if(number > bufbits)
	{
		number -= bufbits;

		while(number>8)
		{
			ret <<= 8;
			ret += *ptr++;
			number -= 8;
		}

		buffer = *ptr++;

		if(number>0)
		{
			ret <<= number;
			bufbits = 8-number;
			ret += buffer >> (8-number);
			buffer &= (1<<bufbits)-1;
		}
	}
	else
	{
		ret = buffer >> (bufbits-number);
		bufbits -= number;
		buffer &= (1<<bufbits)-1;
	}

	*p = ptr;
	return ret;
}

static int readSBitsP(byte **p, int number)
{
	int num = readBitsP(p, number);

	if(num & (1<<(number-1)))
		return num - (1<<number);
	else
		return num;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
