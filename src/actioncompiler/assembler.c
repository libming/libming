
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compile.h"
#include "assembler.h"

extern FILE *yyin;

int yyparse ();

int len;
Buffer asmBuffer;
int nLabels;

struct label
{
  char *name;
  int offset;
};

struct label labels[256];

static int
findLabel (char *label)
{
  int i;

  for (i = 0; i < nLabels; ++i)
  {
    if (strcmp (label, labels[i].name) == 0)
      return i;
  }

  return -1;
}

static void
addLabel (char *label)
{
  int i = findLabel (label);

  if (i == -1)
  {
    labels[nLabels].name = strdup (label);
    labels[nLabels].offset = len;
    ++nLabels;
  }
  else
    labels[i].offset = len;
}

int
bufferBranchTarget (Buffer output, char *label)
{
  int i = findLabel (label);

  if (i == -1)
  {
    i = nLabels;
    addLabel (label);
  }

  return bufferWriteS16 (output, i);
}

void
bufferPatchTargets (Buffer buffer)
{
  int l, i = 0;
  unsigned char *output = buffer->buffer;

  while (i < len)
  {
    if (output[i] & 0x80)	/* then it's a multibyte instruction */
    {
      if (output[i] == SWFACTION_BRANCHALWAYS ||
	  output[i] == SWFACTION_BRANCHIFTRUE)
      {
	int target, offset;

	i += 3;			/* plus instruction plus two-byte length */

	target = output[i];
	offset = labels[target].offset - (i + 2);
	output[i] = offset & 0xff;
	output[++i] = (offset >> 8) & 0xff;
	++i;
      }
      else
      {
	++i;
	l = output[i];
	++i;
	l += output[i] << 8;

	i += l + 1;
      }
    }
    else
      ++i;
  }
}
