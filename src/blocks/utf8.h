/* utf8.h
 * 
 * $Id$
 * 
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_UTF8_H_INCLUDED
#define SWF_UTF8_H_INCLUDED

#include "ming.h"

int UTF8Length(const char *string);
unsigned short UTF8GetChar(const char** strptr);
int UTF8ExpandString(const char* string, unsigned short** outstr);

#endif /* SWF_UTF8_H_INCLUDED */
