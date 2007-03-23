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

/* XXX - error calls should be macros to save the file/lineno */

extern SWFMsgFunc SWF_warn;
extern SWFMsgFunc SWF_error;

void warn_default(const char *msg, ...);
void error_default(const char *msg, ...);

SWFMsgFunc setSWFWarnFunction(SWFMsgFunc warn);
SWFMsgFunc setSWFErrorFunction(SWFMsgFunc error);

#define SWF_warnOnce(msg, va...)	\
{					\
	static int __warned = 0;	\
					\
	if(!__warned)			\
	{				\
		SWF_warn((msg), ##va);	\
		__warned = 1;		\
	}				\
}					\



#define SWF_assert(__condition) 						\
	if ( !(__condition) )							\
		SWF_error("failed assertion '%s' in %s:%i\n", 			\
                           __STRING(__condition), __FILE__,__LINE__);		\

#endif /* SWF_ERROR_H_INCLUDED */
