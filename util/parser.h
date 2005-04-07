#include <stdio.h>

struct Rect
{ 
  int xMin;
  int xMax;
  int yMin;
  int yMax;
};

struct Movie
{ 
  int version;
  int size;                     /* in bytes */
  struct Rect frame;
  float rate;
  int nFrames;
};


#include "swftypes.h"

typedef SWF_Parserstruct *(*SWFParseFunc)(FILE *, int);

#include "parserdecl.h"
