
#ifndef WIN32
#include <unistd.h>
#endif

#include <time.h>
#include <stdio.h>

#include "../blocks/output.h"
#include "../blocks/action.h"
#include "compile.h"

extern int SWF_versionNum;

SWFAction
compileSWFActionCode (char *script)
{
  SWFOutput output;
  Buffer b;

  /* yydebug = 1; */

  if (SWF_versionNum == 4)
  {
    swf4ParseInit (script, 0);

    if (swf4parse ((void *) &b) != 0)
      return NULL;
  }
  else
  {
    swf5ParseInit (script, 0);

    if (swf5parse ((void *) &b) != 0)
      return NULL;
  }

  output = newSWFOutput ();

  if (b)
  {
    SWFOutput_writeBuffer (output, b->buffer, bufferLength (b));
    destroyBuffer (b);
  }

  SWFOutput_writeUInt8 (output, SWFACTION_END);

  return newSWFAction_fromOutput (output);
}
