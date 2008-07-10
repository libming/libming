/* error.h
 *
 * $Id$
 * 
 * Notice: This header file contains declarations of functions and types that
 * are just used internally. All library functions and types that are supposed
 * to be publicly accessable are defined in ./src/ming.h.
 */

#ifndef SWF_ERROR_H_INCLUDED
#define SWF_ERROR_H_INCLUDED

#include "ming.h"

extern SWFMsgFunc _SWF_warn;
extern SWFMsgFunc _SWF_error;

void warn_default(const char *msg, ...);
void error_default(const char *msg, ...);

SWFMsgFunc setSWFWarnFunction(SWFMsgFunc warn);
SWFMsgFunc setSWFErrorFunction(SWFMsgFunc error);

#ifndef _MSC_VER
#define SWF_warn(msg, va...) 		\
do {					\
	if(_SWF_warn)			\
		_SWF_warn((msg), ##va); \
} while(0)
#else
#define SWF_warn _SWF_warn
#endif

#ifndef _MSC_VER
#define SWF_error(msg, va...) 		\
do {					\
	if(_SWF_error)			\
		_SWF_error((msg), ##va); \
} while(0)
#else
#define SWF_error _SWF_error
#endif

#ifndef _MSC_VER
#define SWF_warnOnce(msg, va...)	\
{					\
	static int __warned = 0;	\
					\
	if(!__warned)			\
	{				\
		_SWF_warn((msg), ##va);	\
		__warned = 1;		\
	}				\
}
#else
#define SWF_warnOnce _SWF_warn
#endif					

/* fix for cygwin compile */
#ifndef __STRING
#define __STRING(x) "x"
#endif

#define SWF_assert(__condition) 						\
	if ( !(__condition) )							\
		SWF_error("failed assertion '%s' in %s:%i\n", 			\
                           __STRING(__condition), __FILE__,__LINE__);		\

#endif /* SWF_ERROR_H_INCLUDED */
