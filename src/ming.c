
#include "ming.h"

/* module-wide initialization.  return non-zero if error. */

extern int SWF_gNumCharacters;
extern int Ming_cubicThreshold;

int Ming_init()
{
  SWF_gNumCharacters = 0;
  Ming_cubicThreshold = 10000;

  return 0;
}
