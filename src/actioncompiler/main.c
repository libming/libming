
#include<stdio.h>

#include "compile.h"

extern FILE *yyin;
extern Buffer out;
extern int yydebug;

#define print(x)	{fputs(x,stdout);}

extern char *lexBuffer;
extern int lexBufferLen;

#define BUFFER_INC 1024

int yyparse(void *b);
void printDoAction(Buffer f, int length);

int main(int argc, char *argv[])
{
  FILE *f;
  Buffer b;
  int size = 0;

  lexBuffer = malloc(BUFFER_INC);
  lexBufferLen = 0;

  yydebug = 1;

  if(argc > 1)
  {
    f = fopen(argv[1], "r");

    if(f == NULL)
    {
      printf("Couldn't open input file %s\n", argv[1]);
      exit(-1);
    }
  }
  else
    f = stdin;

  while((size = fread(lexBuffer+lexBufferLen, 1, BUFFER_INC, f)) == BUFFER_INC)
  {
    lexBuffer = realloc(lexBuffer, lexBufferLen+2*BUFFER_INC);
    lexBufferLen += BUFFER_INC;
  }

  lexBufferLen += size;

  yyparse((void *)&b);

  if(b == NULL)
  {
    printf("\nOutput buffer empty\n");
    exit(1);
  }

  bufferWriteU8(b, SWFACTION_END);

  putchar('\n');
  putchar('\n');

  printDoAction(b, bufferLength(b));

  putchar('\n');

  exit(0);
}
