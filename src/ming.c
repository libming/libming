
#include "ming.h"

extern int SWF_gNumCharacters;

int SWF_versionNum = 5;

float Ming_scale = 20.0;
int Ming_cubicThreshold = 10000;

/* module-wide initialization.  return non-zero if error. */

int Ming_init()
{
  SWF_gNumCharacters = 0;
  SWF_versionNum = 5;

  Ming_cubicThreshold = 10000;
  Ming_scale = 20.0;

  return 0;
}

void Ming_setScale(float scale)
{
  Ming_scale = scale;
}

float Ming_getScale()
{
  return Ming_scale;
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

void Ming_useSWFVersion(int version)
{
  if(version < 4 || version > 5)
    SWF_error("Only SWF versions 4 and 5 are currently supported!\n");

  SWF_versionNum = version;
}
