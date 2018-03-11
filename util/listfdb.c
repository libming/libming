#include <stdlib.h>
#include <stdio.h>

#include "read.h"
#include "blocks/blocktypes.h"
#include "action.h"

#define error(x)	{puts(x); exit(1);}
#define print(x)	{fputs(x,stdout);}
#define println(x)	{fputs(indent(),stdout);fputs(x,stdout);putchar('\n');}

int gIndent = 0;
char indentBuf[256];
int lastIndent = 0;

typedef unsigned char boolean;
#define true (boolean)1
#define false (boolean)0

//char *blockName(Blocktype type);

char *indent()
{
  int i;

  if(gIndent>63)
    error("indent level > 63!");

  if(lastIndent != gIndent)
  {
    for(i=0; i<3*gIndent; ++i)
      indentBuf[i] = ' ';

    indentBuf[i] = '\0';

    lastIndent = gIndent;
  }

  return indentBuf;
}

void printMatrix(FILE *f)
{
  int nBits;
  float num;

  byteAlign();

  if(readBits(f, 1)) /* has scale */
  {
    nBits = readBits(f, 5);
    num = (float)readSBits(f, nBits)/0x10000;
    printf("%sxScale: %f\n", indent(), num);
    num = (float)readSBits(f, nBits)/0x10000;
    printf("%syScale: %f\n", indent(), num);
  }
  if(readBits(f, 1)) /* has rotate */
  {
    nBits = readBits(f, 5);
    num = (float)readSBits(f, nBits)/0x10000;
    printf("%srotate0: %f\n", indent(), num);
    num = (float)readSBits(f, nBits)/0x10000;
    printf("%srotate1: %f\n", indent(), num);
  }

  nBits = readBits(f, 5);
  printf("%sx: %i\n", indent(), readSBits(f, nBits));
  printf("%sy: %i\n", indent(), readSBits(f, nBits));
}

void printRect(FILE *f)
{
  int nBits, xMin, xMax, yMin, yMax;

  byteAlign();

  nBits = readBits(f, 5);
  xMin = readSBits(f, nBits);
  xMax = readSBits(f, nBits);
  yMin = readSBits(f, nBits);
  yMax = readSBits(f, nBits);

  printf("(%i,%i)x(%i,%i)", xMin, xMax, yMin, yMax);
}

int printShapeRec(FILE *f, int *lineBits, int *fillBits, int shapeType)
{
  int type;

  printf("(%i:%i)",fileOffset,bufbits);

  type = readBits(f, 1);

  if(type==0) /* state change */
  {
    int newStyles = readBits(f, 1);
    int lineStyle = readBits(f, 1);
    int fillStyle1 = readBits(f, 1);
    int fillStyle0 = readBits(f, 1);
    int moveTo = readBits(f, 1);

    if(newStyles==0 && lineStyle==0 && fillStyle1==0 && fillStyle0==0 && moveTo==0)
    {
      println("EndShape");
      return 0;
    }

    if(moveTo==1)
    {
      int moveBits = readBits(f, 5);
      int x = readSBits(f, moveBits);
      int y = readSBits(f, moveBits);

      printf("%sMoveTo (%i) - (%i,%i)\n", indent(), moveBits, x, y);
    }

    if(fillStyle0==1)
      printf("%sFillStyle0: %i\n", indent(), readBits(f, *fillBits));

    if(fillStyle1==1)
      printf("%sFillStyle1: %i\n", indent(), readBits(f, *fillBits));

    if(lineStyle==1)
      printf("%sLineStyle1: %i\n", indent(), readBits(f, *lineBits));
  }
  else /* it's an edge record */
  {
    int straight = readBits(f, 1);
    int numBits = readBits(f, 4)+2;

    if(straight==1)
    {
      if(readBits(f, 1)) /* general line */
      {
	int x = readSBits(f, numBits);
	int y = readSBits(f, numBits);

	printf("%sStraightEdge: (%i) - (%i,%i)\n", indent(), numBits, x, y);
      }
      else
	if(readBits(f, 1)) /* vert = 1 */
	  printf("%sStraightEdge: (%i) - (0,%i)\n", indent(), numBits, readSBits(f, numBits));
	else
	  printf("%sStraightEdge: (%i) - (%i,0)\n", indent(), numBits, readSBits(f, numBits));
    }
    else
    {
      int controlX = readSBits(f, numBits);
      int controlY = readSBits(f, numBits);
      int anchorX = readSBits(f, numBits);
      int anchorY = readSBits(f, numBits);
      printf("%sCurvedEdge: (%i) - (%i,%i)->(%i,%i)\n", indent(), numBits, controlX, controlY, anchorX, anchorY);
    }
  }

  return 1;
}

#define FONTINFO2_HASLAYOUT		(1<<7)
#define FONTINFO2_SHIFTJIS		(1<<6)
#define FONTINFO2_UNICODE		(1<<5)
#define FONTINFO2_ANSI			(1<<4)
#define FONTINFO2_WIDEOFFSETS	        (1<<3)
#define FONTINFO2_WIDECODES		(1<<2)
#define FONTINFO2_ITALIC		(1<<1)
#define FONTINFO2_BOLD			(1<<0)

void printDefineFont2(FILE *f)
{
  int flags, nGlyphs, namelen, off, i, fillBits, lineBits;
  unsigned int *offset;

  flags = readUInt8(f);

  readUInt8(f); /* "reserved" */

  namelen = readUInt8(f);

  printf("%sFont Name: ", indent());

  for(; namelen>0; --namelen)
    putchar((unsigned char)readUInt8(f));

  putchar('\n');

  nGlyphs = readUInt16(f);
  printf("%snumber of glyphs: %i\n\n", indent(), nGlyphs);

  offset = (unsigned int *)malloc(nGlyphs*sizeof(int));

  /* offset table */

  for(i=0; i<nGlyphs; ++i)
  {
    if(flags & FONTINFO2_WIDEOFFSETS)
      off = readUInt32(f);
    else
      off = readUInt16(f);

    offset[i] = off-nGlyphs*4-4;
    printf("%sOffset%i: %i\n", indent(), i, offset[i]);
  }

  /* shape table */
  for(i=0; i<nGlyphs; ++i)
  {
    byteAlign();
    printf("%sGlyph %i:\n", indent(), i);

    fillBits = readBits(f, 4);
    lineBits = readBits(f, 4);

    byteAlign();
    while(printShapeRec(f, &fillBits, &lineBits, 2)) ;

    putchar('\n');
  }

  /* code table */
  for(i=0; i<nGlyphs; ++i)
  {
    if(flags & FONTINFO2_WIDECODES)
      printf("%sglyph code %i: %i\n", indent(), i, readUInt16(f));
    else
      printf("%sglyph code %i: %i\n", indent(), i, readUInt8(f));
  }

  if(flags & FONTINFO2_HASLAYOUT)
  {
    int kernCount, code1, code2;

    printf("%sascender height: %i\n", indent(), readSInt16(f));
    printf("%sdescender height: %i\n", indent(), readSInt16(f));
    printf("%sleading height: %i\n", indent(), readSInt16(f));

    for(i=0; i<nGlyphs; ++i)
      printf("\tadvance %i: %i\n", i, readSInt16(f));

    for(i=0; i<nGlyphs; ++i)
    {
      printf("%sbounds %i: ", indent(), i);
      printRect(f);
      putchar('\n');
    }

    kernCount = readUInt16(f);

    for(i=0; i<kernCount; ++i)
    {
      code1 = (flags & FONTINFO2_WIDECODES) ? readUInt16(f) : readUInt8(f);
      code2 = (flags & FONTINFO2_WIDECODES) ? readUInt16(f) : readUInt8(f);
      printf("%s(%i,%i): adjustment = %i\n", indent(), code1, code2, readSInt16(f));
    }
  }

  putchar('\n');
}

int main(int argc, char *argv[])
{
  FILE *f;

  if(argc<2)
    error("Give me a filename.\n");

  if(!(f = fopen(argv[1],"rb")))
    error("Sorry, can't seem to read that file.\n");

  if(!(readUInt8(f)=='f' && readUInt8(f)=='d' && readUInt8(f)=='b'))
    error("Doesn't look like an fdb file to me..\n");

  readUInt8(f); /* version */

  printDefineFont2(f);

  putchar('\n');

  return 0;
}
