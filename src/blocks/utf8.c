/*
  Licence: LGPL(?)
  author: goto <harpy@lily.freemail.ne.jp>
*/

/* $Id */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utf8.h"

#include "libming.h"

int UTF8Length(const char *string)
{
	int l, str_len, i;

	str_len = 0;
	l = strlen(string);

	for(i=0; i<l; ++i)
	{
		unsigned int byte0, byte1, byte2;

		byte0 = (unsigned int)string[i];

		if (byte0 & 0x80)
		{
			if ((byte0 & 0xe0) == 0xc0)
			{
				if (++i >= l)
					break;

				byte1 = (unsigned int)string[i];
				++str_len;
			}
			else if ((byte0 & 0xf0) == 0xe0)
			{
				if (++i >= l)
					break;

				byte1 = (unsigned int)string[i];

				if (++i >= l)
					break;

				byte2 = (unsigned int)string[i];
				++str_len;
			}
			else
				break;

			/* MMM - currently up to 16-bit code are supported */
		}
		else
			++str_len;
	}

	return str_len;
}


unsigned short UTF8GetChar(const char** strptr)
{
	const char* p = *strptr;
	unsigned int byte0, byte1, byte2;
	unsigned short mbcode;

	byte0 = (unsigned int)*p;

	if ( byte0 == 0 )
		return 0xffff;

	++p;

	if ( byte0 & 0x80 )
	{
		if ( (byte0 & 0xe0) == 0xc0 )
		{
			byte1 = (unsigned int)*p;

			if ( byte1 == 0 )
				return 0xffff;

			++p;

			mbcode = ((byte0 & 0x1f)<<6) | (byte1 & 0x3f);
		}
		else if ( (byte0 & 0xf0) == 0xe0 )
		{
			byte1 = (unsigned int)*p;

			if ( byte1 == 0 )
				return 0xffff;

			++p;
			byte2 = (unsigned int)*p;

			if ( byte2 == 0 )
				return 0xffff;

			++p;

			mbcode = ((byte0 & 0x0f)<<12) | ((byte1 & 0x3f)<<6) | (byte2 & 0x3f);
		}
		else
			return 0xffff;

			/* MMM - currently up to 16-bit code are supported */
	}
	else
		mbcode = byte0;

	*strptr = p;

	return mbcode;
}


#define EXPANDSTRING_INCREMENT 256

int UTF8ExpandString(const char* string, unsigned short** outstr)
{
	unsigned short* output = NULL;
	unsigned short c;
	int count = 0;

	while ( (c = UTF8GetChar(&string)) != 0xffff )
	{
		if ( (count % EXPANDSTRING_INCREMENT) == 0 )
		{
			output =
				(unsigned short*) realloc(output,
								sizeof(unsigned short) * (count + EXPANDSTRING_INCREMENT));
		}

		output[count] = c;
		++count;
	}

	*outstr = output;
	return count;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
