
#ifndef WIN32
  #include <unistd.h>
#endif

#include <time.h>
#include <stdio.h>

#include "../blocks/output.h"
#include "../blocks/action.h"
#include "compile.h"

extern int yydebug;
extern char *lexBuffer;
extern int lexBufferLen;

int yyparse(void *b);

SWFAction compileSWFActionCode(char *script)
{
  SWFOutput output;
  Buffer b;

  /* yydebug = 1; */

  lexBuffer = script;
  lexBufferLen = strlen(script);

  if(yyparse((void *)&b) != 0)
    return NULL;

  output = newSWFOutput();

  if(b)
  {
    SWFOutput_writeBuffer(output, b->buffer, bufferLength(b));
    destroyBuffer(b);
  }

  SWFOutput_writeUInt8(output, SWFACTION_END);

  return newSWFAction_fromOutput(output);
}
