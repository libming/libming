
#include "ming.h"

/* module-wide initialization.  return non-zero if error. */

extern int SWF_gNumCharacters;

float Ming_scale = 20.0;
int Ming_cubicThreshold = 10000;

int Ming_init()
{
  SWF_gNumCharacters = 0;
  Ming_cubicThreshold = 10000;
  Ming_scale = 20.0;

  return 0;
}

void Ming_setScale(float scale)
{
  Ming_scale = scale;
}

void Ming_setCubicThreshold(int num)
{
  Ming_cubicThreshold = num;
}


void Ming_setWarnFunction(void (*warn)(char *msg, ...))
{
  setSWFWarnFunction(warn);
}

void Ming_setErrorFunction(void (*error)(char *msg, ...))
{
  setSWFErrorFunction(error);
}
