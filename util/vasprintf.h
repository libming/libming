#include <stdio.h>
#include "ming_config.h"

#ifndef HAVE_VASPRINTF
int vasprintf(char **ret, const char *format, va_list ap);
#endif

