
#ifndef SWF_ERROR_H_INCLUDED
#define SWF_ERROR_H_INCLUDED

/* XXX - error calls should be macros to save the file/lineno */

extern void (*SWF_warn)(char *msg, ...);
extern void (*SWF_error)(char *msg, ...);

void warn_default(char *msg, ...);
void error_default(char *msg, ...);

void setSWFWarnFunction(void (*error)(char *msg, ...));
void setSWFErrorFunction(void (*error)(char *msg, ...));

void SWF_assert(int c);

#endif /* SWF_ERROR_H_INCLUDED */
