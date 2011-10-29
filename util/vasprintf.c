#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef HAVE_VASPRINTF
/* Workaround for the lack of vasprintf()
 * As found on: http://unixpapa.com/incnote/stdio.html
 * Seems to be Public Domain
 */
int
vasprintf(char **ret, const char *format, va_list ap)
{
	va_list ap2;
	int len = 100;        /* First guess at the size */
	if ((*ret = (char *) malloc(len)) == NULL)
	{
		return -1;
	}
	while (1)
	{
		int nchar;
		va_copy(ap2, ap);
		nchar= vsnprintf(*ret, len, format, ap2);
		if (nchar > -1 && nchar < len)
		{
			return nchar;
		}
		if (nchar > len)
		{
			len= nchar+1;
		} else
		{
			len*= 2;
		}
		if ((*ret = (char *) realloc(*ret, len)) == NULL)
		{
			free(*ret);
			return -1;
		}
	}
}
#endif

