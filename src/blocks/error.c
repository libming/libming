
#include <stdio.h>
#include <stdarg.h>

#include "error.h"

void warn_default(char *msg, ...)
{
  va_list args;

  va_start(args, msg);
  vprintf(msg, args);
  va_end(args);
}

void error_default(char *msg, ...)
{
  va_list args;

  va_start(args, msg);
  vprintf(msg, args);
  va_end(args);
  exit(0);
}

void (*SWF_warn)(char *msg, ...) = warn_default;
void (*SWF_error)(char *msg, ...) = error_default;

void setSWFWarnFunction(void (*warnfunc)(char *msg, ...))
{
  SWF_warn = warnfunc;
}

void setSWFErrorFunction(void (*errorfunc)(char *msg, ...))
{
  SWF_error = errorfunc;
}

void SWF_assert(int c)
{
  if(!c)
    SWF_error("failed assertion");
}
