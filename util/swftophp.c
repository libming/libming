
/* convert everything to objects, resolve dependencies, etc.. */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "blocktypes.h"
#include "action.h"
#include "read.h"
#include "decompile.h"

#include "swftophp.h"

#ifndef M_PI
  #define M_PI 3.14159265358979f
#endif

void skipBytes(FILE *f, int length);
char *blockName(int);

void readMatrix(FILE *f, struct Matrix *s)
{
  int nBits;

  byteAlign();

  if(readBits(f, 1)) /* has scale */
  {
    nBits = readBits(f, 5);
    s->xScale = (float)readSBits(f, nBits)/0x10000;
    s->yScale = (float)readSBits(f, nBits)/0x10000;
  }
  else
    s->xScale = s->yScale = 1.0;

  if(readBits(f, 1)) /* has rotate */
  {
    nBits = readBits(f, 5);
    s->rot0 = (float)readSBits(f, nBits)/0x10000;
    s->rot1 = (float)readSBits(f, nBits)/0x10000;
  }
  else
    s->rot0 = s->rot1 = 0;

  nBits = readBits(f, 5);
  s->x = readSBits(f, nBits);
  s->y = readSBits(f, nBits);
}

void readCXForm(FILE *f, struct CXForm *s, boolean hasAlpha)
{
  int hasAdd, hasMult, nBits;

  byteAlign();

  hasAdd = readBits(f, 1);
  hasMult = readBits(f, 1);
  nBits = readBits(f, 4);

  if(hasMult)
  {
    s->rMult = readSBits(f, nBits);
    s->gMult = readSBits(f, nBits);
    s->bMult = readSBits(f, nBits);

    if(hasAlpha)
      s->aMult = readSBits(f, nBits);
    else
      s->aMult = 0x100;
  }
  else
  {
    s->aMult = 0x100;
    s->rMult = 0x100;
    s->gMult = 0x100;
    s->bMult = 0x100;
  }

  if(hasAdd)
  {
    s->rAdd = readSBits(f, nBits);
    s->gAdd = readSBits(f, nBits);
    s->bAdd = readSBits(f, nBits);

    if(hasAlpha)
      s->aAdd = readSBits(f, nBits);
    else
      s->aAdd = 0;
  }
  else
  {
    s->aAdd = 0;
    s->rAdd = 0;
    s->gAdd = 0;
    s->bAdd = 0;
  }
}

void readRect(FILE *f, struct Rect *s)
{
  int nBits;

  byteAlign();

  nBits = readBits(f, 5);
  s->xMin = readSBits(f, nBits);
  s->xMax = readSBits(f, nBits);
  s->yMin = readSBits(f, nBits);
  s->yMax = readSBits(f, nBits);
}

void readRGB(FILE *f, struct RGBA *s)
{
  s->r = readUInt8(f);
  s->g = readUInt8(f);
  s->b = readUInt8(f);
  s->a = 0xff;
}

void readRGBA(FILE *f, struct RGBA *s)
{
  s->r = readUInt8(f);
  s->g = readUInt8(f);
  s->b = readUInt8(f);
  s->a = readUInt8(f);
}

void printRGBA(struct RGBA *s)
{
  printf("0x%02x, 0x%02x, 0x%02x", s->r, s->g, s->b);

  if(s->a != 0xff)
    printf(", 0x%02x", s->a);
}

void readGradient(FILE *f, struct Gradient *s, Blocktype shapeType)
{
  int i;
  int numGrads = readUInt8(f);

  s->nGradients = numGrads;
  s->colors = malloc(sizeof(struct RGBA)*numGrads);
  s->ratio = malloc(sizeof(int)*numGrads);

  for(i=0; i<numGrads; ++i)
  {
    s->ratio[i] = readUInt8(f);

    if(shapeType==DEFINESHAPE3)
      readRGBA(f, &(s->colors[i]));
    else
      readRGB(f, &(s->colors[i]));
  }
}

void printGradient(struct Gradient *g)
{
  int i;

  printf("\t$g = new SWFGradient();\n");

  for(i=0; i<g->nGradients; ++i)
  {
    printf("\t$g->addEntry(%f, ", g->ratio[i]/255.0);
    printRGBA(&(g->colors[i]));
    printf(");\n");
  }
}

void readLineStyleArray(FILE *f, struct Shape *shape, int isMorph)
{
  struct LineStyles *s;
  struct LineStyle *l;
  int count, i;
  int start = shape->lines.nLines;

  shape->lineOffset = start;

  count = readUInt8(f);

  if(count==255)
    count = readUInt16(f);

  s = &(shape->lines);

  s->nLines += count;
  s->line = realloc(s->line, s->nLines * sizeof(struct LineStyle));

  for(i=0; i<count; ++i)
  {
    l = &(s->line[start+i]);
    l->width = readUInt16(f);

    if(isMorph)
      l->width2 = readUInt16(f);

    if(shape->shapeType == DEFINESHAPE3)
      readRGBA(f, &(l->color));
    else
      readRGB(f, &(l->color));

    if(isMorph)
      readRGBA(f, &(l->color2));
  }
}

void readFillStyle(FILE *f, struct FillStyle *s, Blocktype shapeType, int isMorph)
{
  int type;

  type = readUInt8(f);
  s->type = type;

  if(type==0) /* solid fill */
  {
    if(shapeType==DEFINESHAPE3)
      readRGBA(f, &(s->fill.color));
    else
      readRGB(f, &(s->fill.color));

    if(isMorph)
      readRGBA(f, &(s->fill2.color));
  }
  else if(type==0x10 || type==0x12)
  {
    /* linear (0x10) or radial (0x10) gradient */

    readMatrix(f, &(s->matrix));

    if(isMorph)
      readMatrix(f, &(s->matrix2));

    readGradient(f, &(s->fill.gradient), shapeType);
  }
  else if(type==0x40 || type==0x41)
  {
    /* tiled bitmap (0x40) or clipped bitmap (0x41) fill */

    s->fill.bitmap = readUInt16(f);
    readMatrix(f, &(s->matrix));

    if(isMorph)
      readMatrix(f, &(s->matrix2));
  }
  else
    error("Unknown fill type: %i\n", type);
}

void printTransform(struct Matrix *m, char ch, int num)
{
  float a = m->xScale, b = m->rot0, c = m->rot1, d = m->yScale;

  float angle;
  float xScale = sqrt(a*a+c*c);
  float yScale = (a*d-b*c)/xScale;
  float skew = (a*b+c*d)/(a*a+c*c);

  if(a==0)
  {
    if(c<0)
      angle = -90;
    else
      angle = 90;
  }
  else
  {
    angle = atan(c/a)*180/M_PI;

    if(a<0)
    {
      if(angle < 0)
	angle += 180;
      else
	angle -= 180;
    }
  }

#define TOLERANCE 0.02

  if(skew < -TOLERANCE || skew > TOLERANCE)
    printf("\t$%c%i->skewXTo(%f);\n", ch, num, skew);

  if(xScale > 1.0-TOLERANCE && xScale < 1.0+TOLERANCE)
    xScale = 1.0;

  if(yScale > 1.0-TOLERANCE && yScale < 1.0+TOLERANCE)
    yScale = 1.0;

  if(xScale != 1.0 || yScale != 1.0)
  {
    if(xScale == yScale)
      printf("\t$%c%i->scaleTo(%f);\n", ch, num, xScale);
    else
      printf("\t$%c%i->scaleTo(%f, %f);\n", ch, num, xScale, yScale);
  }

  if(angle < -TOLERANCE || angle > TOLERANCE)
    printf("\t$%c%i->rotateTo(%f);\n", ch, num, angle);

  if(m->x != 0 || m->y != 0)
    printf("\t$%c%i->moveTo(%i, %i);\n", ch, num, m->x, m->y);
}

void printFillStyle(struct FillStyle *s, int id, int num, int isMorph)
{
  switch(s->type)
  {
    case 0:
      return;

    case 0x10:
      printGradient(&(s->fill.gradient));
      printf("\t$f%i = $s%i->addFill($g, SWFFILL_LINEAR_GRADIENT);\n", num, id);
      break;

    case 0x12:
      printGradient(&(s->fill.gradient));
      printf("\t$f%i = $s%i->addFill($g, SWFFILL_RADIAL_GRADIENT);\n", num, id);
      break;

    case 0x40:
      printf("\t$f%i = $s%i->addFill($b%i, SWFFILL_TILED_BITMAP);\n", num, id, s->fill.bitmap);
      break;

    case 0x41:
      printf("\t$f%i = $s%i->addFill($b%i, SWFFILL_CLIPPED_BITMAP);\n", num, id, s->fill.bitmap);
      break;

    default:
      error("Unknown fill type: 0x%02x!", s->type);
  }

  if(isMorph)
  {
    if(s->type == 0x40 || s->type == 0x41)
      printTransform(&(s->matrix2), 'f', num);
  }
  else
    printTransform(&(s->matrix), 'f', num);
}

void readFillStyleArray(FILE *f, struct Shape *shape, int isMorph)
{
  struct FillStyles *s;
  int count, i, start;

  start = shape->fills.nFills;

  shape->fillOffset = start;

  count = readUInt8(f);

  if(count==255)
    count = readUInt16(f);

  s = &(shape->fills);

  s->nFills += count;
  s->fill = realloc(s->fill, s->nFills * sizeof(struct FillStyle));

  for(i=0; i<count; ++i)
    readFillStyle(f, &(s->fill[start+i]), shape->shapeType, isMorph);
}

int readShapeRec(FILE *f, struct Shape *shape)
{
  struct ShapeRecord *s;
  int type;

  shape->shapes.shape = realloc(shape->shapes.shape,
				(shape->shapes.nShapes+1) *
				sizeof(struct ShapeRecord));

  s = &(shape->shapes.shape[shape->shapes.nShapes]);

  ++shape->shapes.nShapes;

  type = readBits(f, 1);

  if(type==0) /* state change */
  {
    int newStyles = readBits(f, 1);
    int lineStyle = readBits(f, 1);
    int fillStyle1 = readBits(f, 1);
    int fillStyle0 = readBits(f, 1);
    int moveTo = readBits(f, 1);

    if(newStyles==0 && lineStyle==0 && fillStyle1==0 &&
       fillStyle0==0 && moveTo==0)
    {
      s->type = SHAPERECORD_END;
      return 0;
    }

    s->type = SHAPERECORD_STATECHANGE;

    if(moveTo==1)
    {
      int moveBits = readBits(f, 5);
      s->data.change.movetox = readSBits(f, moveBits);
      s->data.change.movetoy = readSBits(f, moveBits);
    }
    else
    {
      s->data.change.movetox = 0;
      s->data.change.movetoy = 0;
    }

    if(fillStyle0==1)
    {
      int num = readBits(f, shape->fillBits);
      s->data.change.fill0 = (num==0) ? 0 : num + shape->fillOffset;
    }
    else
      s->data.change.fill0 = -1;

    if(fillStyle1==1)
    {
      int num = readBits(f, shape->fillBits);
      s->data.change.fill1 = (num==0) ? 0 : num + shape->fillOffset;
    }
    else
      s->data.change.fill1 = -1;

    if(lineStyle==1)
    {
      int num = readBits(f, shape->lineBits);
      s->data.change.line = (num==0) ? 0 : num + shape->lineOffset;
    }
    else
      s->data.change.line = -1;

    if(newStyles==1)
    {
      readFillStyleArray(f, shape, 0);
      readLineStyleArray(f, shape, 0);
      shape->fillBits = readBits(f, 4);
      shape->lineBits = readBits(f, 4);
    }
  }
  else /* it's an edge record */
  {
    int straight = readBits(f, 1);
    int numBits = readBits(f, 4)+2;

    if(straight==1)
    {
      s->type = SHAPERECORD_LINE;

      if(readBits(f, 1)) /* general line */
      {
	s->data.line.x = readSBits(f, numBits);
	s->data.line.y = readSBits(f, numBits);
      }
      else
	if(readBits(f, 1)) /* vert = 1 */
	{
	  s->data.line.x = 0;
	  s->data.line.y = readSBits(f, numBits);
	}
	else
	{
	  s->data.line.x = readSBits(f, numBits);
	  s->data.line.y = 0;
	}
    }
    else
    {
      s->type = SHAPERECORD_CURVE;

      s->data.curve.controlx = readSBits(f, numBits);
      s->data.curve.controly = readSBits(f, numBits);
      s->data.curve.anchorx = readSBits(f, numBits);
      s->data.curve.anchory = readSBits(f, numBits);
    }
  }

  return 1;
}

void printFillChange(int id, struct Shape *shape, int fillNum, int side)
{
  if(fillNum == 0)
    printf("\t$s%i->set%sFill(0);\n", id, side==0 ? "Left" : "Right");

  else if(shape->fills.fill[fillNum-1].type == 0)
  {
    printf("\t$s%i->set%sFill(", id, side==0 ? "Left" : "Right");
    printRGBA(&(shape->fills.fill[fillNum-1].fill.color));
    printf(");\n");
  }

  else
    printf("\t$s%i->set%sFill($f%i);\n", id, side==0 ? "Left" : "Right",
	   fillNum);
}

void printShapeRec(struct Shape *shape, struct ShapeRecord *s, int id)
{
  switch(s->type)
  {
    case SHAPERECORD_END:
      return;

    case SHAPERECORD_STATECHANGE:

      if(s->data.change.movetox != 0 || s->data.change.movetoy != 0)
	printf("\t$s%i->movePenTo(%i, %i);\n", id,
	       s->data.change.movetox, s->data.change.movetoy);

      if(s->data.change.fill0 != -1)
	printFillChange(id, shape, s->data.change.fill0, 0);

      if(s->data.change.fill1 != -1)
	printFillChange(id, shape, s->data.change.fill1, 1);

      if(s->data.change.line != -1)
      {
	if(s->data.change.line == 0)
	  printf("\t$s%i->setLine(0);\n", id);
	else
	{
	  struct LineStyle *line = &(shape->lines.line[s->data.change.line-1]);

	  printf("\t$s%i->setLine(%i, ", id, line->width);
	  printRGBA(&(line->color));
	  printf(");\n");
	}
      }
      break;

    case SHAPERECORD_LINE:
      printf("\t$s%i->drawLine(%i, %i);\n", id, s->data.line.x, s->data.line.y);
      break;

    case SHAPERECORD_CURVE:
      printf("\t$s%i->drawCurve(%i, %i, %i, %i);\n", id,
	     s->data.curve.controlx, s->data.curve.controly,
	     s->data.curve.anchorx, s->data.curve.anchory);
      break;

    default:
      error("Bad shape type: %i!\n", s->type);
  }
}

void printDefineShape(struct Shape *shape, int id, int isMorph)
{
  int i;

  for(i=0; i<shape->fills.nFills; ++i)
    printFillStyle(&(shape->fills.fill[i]), id, i+1, isMorph);

  for(i=0; i<shape->shapes.nShapes; ++i)
    printShapeRec(shape, &(shape->shapes.shape[i]), id);
}

void printShape(FILE *f, int length)
{
  struct Shape shape;
  int id, start = fileOffset;

  memset(&shape, 0, sizeof(struct Shape));

  id = readUInt16(f);

  readRect(f, &(shape.bounds)); /* bounds */

  readFillStyleArray(f, &shape, 0);
  readLineStyleArray(f, &shape, 0);

  byteAlign();

  shape.fillBits = readBits(f,4);
  shape.lineBits = readBits(f,4);

  while(fileOffset < length+start &&
	readShapeRec(f, &shape)) ;

  /* go for end tag..
  if(fileOffset == length+start && bufbits > 5)
    printShapeRec(f, &lineBits, &fillBits, 2);
  */

  printf("\n\t/* -- Shape %i -- */\n", id);
  printf("\t$s%i = new SWFShape();\n", id);

  printDefineShape(&shape, id, 0);
}

void printMorphShape(FILE *f, int length)
{
  int offset, start = fileOffset, here;
  struct Rect r;
  struct Shape shape1, shape2;
  int id = readUInt16(f);

  printf("\n\t/* -- Shape %i -- */\n", id);
  printf("\t$s%i = new SWFMorph();\n", id);

  readRect(f, &r); /* bounds 1 */
  readRect(f, &r); /* bounds 2 */

  offset = readUInt32(f);

  here = fileOffset+1;

  memset(&shape1, 0, sizeof(struct Shape));
  memset(&shape2, 0, sizeof(struct Shape));

  shape1.shapeType = shape2.shapeType = DEFINESHAPE3;

  readFillStyleArray(f, &shape1, 1);
  readLineStyleArray(f, &shape1, 1);

  byteAlign();

  shape1.fillBits = shape2.fillBits = readBits(f,4);
  shape1.lineBits = shape2.lineBits = readBits(f,4);

  while(fileOffset < here+offset &&
	readShapeRec(f, &shape1)) ;

  printf("\n\t$s0 = $s%i->getShape1();\n", id);

  printDefineShape(&shape1, 0, 0);

  byteAlign();

  readUInt8(f); /* redundant fill/line bits */

  while(fileOffset < start+length &&
	readShapeRec(f, &shape2)) ;

  printf("\n\t$s0 = $s%i->getShape2();\n", id);

  printDefineShape(&shape1, 0, 1);
}

/* JPEG stream markers: */
#define JPEG_MARKER 0xFF

/* Start of Image, End of Image */
#define JPEG_SOI  0xD8
#define JPEG_EOI  0xD9

#define JPEG_JFIF 0xE0

/* encoding markers, quantization tables and Huffman tables */
#define JPEG_QUANT 0xDB
#define JPEG_HUFF  0xC4

/* image markers, start of frame and start of scan */
#define JPEG_SOF0 0xC0
#define JPEG_SOF1 0xC1
#define JPEG_SOF2 0xC2
#define JPEG_SOS  0xDA
/*
void printJpegStream(FILE *f, int length)
{
  int end = fileOffset+length;
  int c, l;

  while(fileOffset < end)
  {
    if(readUInt8(f) != JPEG_MARKER)
    {
      printf("%sJpeg marker not found!\n", indent());
      break;
    }

    switch(c=readUInt8(f))
    {
      case JPEG_SOI:   printf("%sSOI\n", indent()); break;
      case JPEG_EOI:   printf("%sEOI\n", indent()); break;
      case JPEG_JFIF:  printf("%sJFIF\n", indent()); break;
      case JPEG_QUANT: printf("%sQuantization table\n", indent()); break;
      case JPEG_HUFF:  printf("%sHuffman table\n", indent()); break;
      case JPEG_SOF0:  printf("%sStart of frame 0\n", indent()); break;
      case JPEG_SOF1:  printf("%sStart of frame 1\n", indent()); break;
      case JPEG_SOF2:  printf("%sStart of frame 2\n", indent()); break;
      case JPEG_SOS:   printf("%sStart of scan\n", indent()); break;
      default:         printf("%sUnknown JPEG block: %02x\n", indent(), c);
    }

    if(c==JPEG_SOS)
      break;

    if(c != JPEG_SOI && c != JPEG_EOI)
    {
      l = (readUInt8(f)<<8) + readUInt8(f);
      skipBytes(f, l-2);
    }
  }

  skipBytes(f, end-fileOffset);
}

void printDefineBitsJpeg(FILE *f, int length)
{
  printf("%sBitmap id: %i\n", indent(), readUInt16(f));
  printJpegStream(f, length-2);
}
*/
void printDefineBitsLossless(FILE *f)
{
/*
  id = readUInt16(f);
  format = readUInt8(f);	 format = 3, 4, 5 -> 2^format bpp
  width = readUInt16(f);
  height = readUInt16(f);
  if(format == 3)
  colorTableSize = readUInt8(f)-1;
*/
}

/*
int printActionRecord(FILE *f)
{
  int length = 0, type = readUInt8(f);

  if((type&0x80) == 0x80)
    length = readUInt16(f);

  switch(type)
  {
    case ACTION_ADD:
      printf("%sAdd\n", indent());
      break;
    case ACTION_SUBTRACT:
      printf("%sSubtract\n", indent());
      break;
    case ACTION_MULTIPLY:
      printf("%sMultiply\n", indent());
      break;
    case ACTION_DIVIDE:
      printf("%sDivide\n", indent());
      break;
    case ACTION_EQUAL:
      printf("%sEquals\n", indent());
      break;
    case ACTION_LESSTHAN:
      printf("%sLess Than\n", indent());
      break;
    case ACTION_LOGICALAND:
      printf("%sAnd\n", indent());
      break;
    case ACTION_LOGICALOR:
      printf("%sOr\n", indent());
      break;
    case ACTION_LOGICALNOT:
      printf("%sNot\n", indent());
      break;
    case ACTION_STRINGEQ:
      printf("%sString eq\n", indent());
      break;
    case ACTION_STRINGLENGTH:
      printf("%sString Length\n", indent());
      break;
    case ACTION_SUBSTRING:
      printf("%sSubstring\n", indent());
      break;
    case ACTION_INT:
      printf("%sInt\n", indent());
      break;
    case ACTION_GETVARIABLE:
      printf("%sGet Variable\n", indent());
      break;
    case ACTION_SETVARIABLE:
      printf("%sSet Variable\n", indent());
      break;
    case ACTION_SETTARGETEXPRESSION:
      printf("%sSet Target Expression\n", indent());
      break;
    case ACTION_STRINGCONCAT:
      printf("%sString Concat\n", indent());
      break;
    case ACTION_GETPROPERTY:
      printf("%sGet Property\n", indent());
      break;
    case ACTION_SETPROPERTY:
      printf("%sSet Property\n", indent());
      break;
    case ACTION_DUPLICATECLIP:
      printf("%sDuplicate Clip\n", indent());
      break;
    case ACTION_REMOVECLIP:
      printf("%sRemove Clip\n", indent());
      break;
    case ACTION_TRACE:
      printf("%sTrace\n", indent());
      break;
    case ACTION_STARTDRAGMOVIE:
      printf("%sStart Drag Movie\n", indent());
      break;
    case ACTION_STOPDRAGMOVIE:
      printf("%sStop Drag Movie\n", indent());
      break;
    case ACTION_STRINGCOMPARE:
      printf("%sString Compare\n", indent());
      break;
    case ACTION_RANDOM:
      printf("%sRandom\n", indent());
      break;
    case ACTION_MBLENGTH:
      printf("%sString MB Length\n", indent());
      break;
    case ACTION_ORD:
      printf("%sOrd\n", indent());
      break;
    case ACTION_CHR:
      printf("%sChr\n", indent());
      break;
    case ACTION_GETTIMER:
      printf("%sGet Timer\n", indent());
      break;
    case ACTION_MBSUBSTRING:
      printf("%sMB Substring\n", indent());
      break;
    case ACTION_MBORD:
      printf("%sMB Ord\n", indent());
      break;
    case ACTION_MBCHR:
      printf("%sMB Chr\n", indent());
      break;
    case ACTION_NEXTFRAME:
      printf("%sNext Frame\n", indent());
      break;
    case ACTION_PREVFRAME:
      printf("%sPrevious Frame\n", indent());
      break;
    case ACTION_PLAY:
      printf("%sPlay\n", indent());
      break;
    case ACTION_STOP:
      printf("%sStop\n", indent());
      break;
    case ACTION_TOGGLEQUALITY:
      printf("%sToggle Quality\n", indent());
      break;
    case ACTION_STOPSOUNDS:
      printf("%sStop Sounds\n", indent());
      break;

*/    /* ops with args */ /*
    case ACTION_PUSHDATA:
    {
      int type = readUInt8(f);
      if(type==0)
        printf("%sPush String: %s\n", indent(), readString(f));
      else
      {
	readUInt16(f);
        printf("%sPush Property: %04x\n", indent(), readUInt16(f));
      }
      break;
    }
    case ACTION_GOTOFRAME:
      printf("%sGoto Frame %i\n", indent(), readUInt16(f));
      break;
    case ACTION_GETURL:
    {
      char *url = readString(f);
      printf("%sGet URL \"%s\" target \"%s\"\n", indent(), url, readString(f));
      break;
    }
    case ACTION_WAITFORFRAMEEXPRESSION:
      printf("%sWait For Frame Expression, skip %i\n", indent(), readUInt8(f));
      break;
    case ACTION_BRANCHALWAYS:
      printf("%sBranch Always %i\n", indent(), readSInt16(f));
      break;
    case ACTION_GETURL2:
      switch(readUInt8(f))
      {
        case 0: printf("%sGet URL2 (Don't send)\n", indent()); break;
        case 1: printf("%sGet URL2 (GET)\n", indent()); break;
        case 2: printf("%sGet URL2 (POST)\n", indent()); break;
      }
      break;
    case ACTION_BRANCHIFTRUE:
      printf("%sBranch If True %i\n", indent(), readSInt16(f));
      break;
    case ACTION_CALLFRAME:
      printf("%sCall Frame\n", indent());
      dumpBytes(f, length);
      break;
    case ACTION_GOTOEXPRESSION:
      printf("%sAdd\n", indent());
      dumpBytes(f, length);
      break;
    case ACTION_WAITFORFRAME:
    {
      int frame = readUInt16(f);
      printf("%sWait for frame %i else skip %i\n", indent(), frame, readUInt8(f));
      break;
    }
    case ACTION_SETTARGET:
      printf("%sSet Target %s\n", indent(), readString(f));
      break;
    case ACTION_GOTOLABEL:
      printf("%sGoto Label %s\n", indent(), readString(f));
      break;
    case ACTION_END:
      return 0;
      break;
    default:
      printf("%sUnknown Action: %02X\n", indent(), type);
      dumpBytes(f, length);
  }
  return 1;
}
*/

int printButtonRecord(FILE *f, int recordType, int id)
{
  int character, layer;
  int flags = readUInt8(f);
  struct Matrix m;
  int notFirst = 0;

  if(flags == 0)
    return 0;

  character = readUInt16(f);
  layer = readUInt16(f);

  printf("\t$s%i->addShape($s%i, ", id, character);
  readMatrix(f, &m);

  if(flags & 0x08)
  {
    printf("SWFBUTTON_HIT");
    notFirst = 1;
  }
  if(flags & 0x04)
  {
    printf("%sSWFBUTTON_DOWN", (notFirst?" | ":""));
    notFirst = 1;
  }
  if(flags & 0x02)
  {
    printf("%sSWFBUTTON_OVER", (notFirst?" | ":""));
    notFirst = 1;
  }
  if(flags & 0x01)
  {
    printf("%sSWFBUTTON_UP", (notFirst?" | ":""));
    notFirst = 1;
  }
  printf(");\n");

  printf("\t/* button character transforms not implemented, sorry. */\n");

  if(recordType == 2)
  {
    struct CXForm s;
    readCXForm(f, &s, true);
    printf("\t/* button cxforms not implemented, either.. */\n");
  }

  return 1;
}

void printDefineButton(FILE *f, int length)
{
  int offset = fileOffset;
  int id = readUInt16(f);

  printf("\n\t/* -- Shape %i -- */\n", id);
  printf("\t$s%i = new SWFButton();\n", id);

  while(printButtonRecord(f, 1, id)) ;

  printf("\t$a = new SWFAction(\"\n\n");
  decompileAction(f, length-(fileOffset-offset), 0);
  printf("\t\");\n");
  printf("\t$s%i->setAction($a);\n", id);
}

int printButton2ActionCondition(FILE *f, int end, int id)
{
  int offset = readUInt16(f);
  int condition = readUInt16(f);
  int notFirst = 0;

  printf("\t$a = new SWFAction(\"\n\n");

  if(offset == 0)
    decompileAction(f, end-fileOffset, 0);
  else
    decompileAction(f, offset-4, 0);

  printf("\t\");\n");
  printf("\t$s%i->addAction($a, ", id);

  if(condition & 0xfe00)
  {
    printf("SWFBUTTON_KEYPRESS(%c)", (condition&0xfe00)>>9);
    notFirst = 1;
  }
  if(condition & 0x100 || condition & 0x10)
  {
    printf("%sSWFBUTTON_DRAGOUT", (notFirst?" | ":""));
    notFirst = 1;
  }
  if(condition & 0x80 || condition & 0x20)
  {
    printf("%sSWFBUTTON_DRAGIN", (notFirst?" | ":""));
    notFirst = 1;
  }
  if(condition & 0x40)
  {
    printf("%sSWFBUTTON_MOUSEUPOUTSIDE", (notFirst?" | ":""));
    notFirst = 1;
  }
  if(condition & 0x08)
  {
    printf("%sSWFBUTTON_MOUSEUP", (notFirst?" | ":""));
    notFirst = 1;
  }
  if(condition & 0x04)
  {
    printf("%sSWFBUTTON_MOUSEDOWN", (notFirst?" | ":""));
    notFirst = 1;
  }
  if(condition & 0x02)
  {
    printf("%sSWFBUTTON_MOUSEOUT", (notFirst?" | ":""));
    notFirst = 1;
  }
  if(condition & 0x01)
  {
    printf("%sSWFBUTTON_MOUSEOVER", (notFirst?" | ":""));
    notFirst = 1;
  }

  printf(");\n");

  return offset;
}

void printDefineButton2(FILE *f, int length)
{
  int offset, end = fileOffset+length;
  int id = readUInt16(f);
  int flags = readUInt8(f);

  printf("\n\t/* -- Shape %i -- */\n", id);
  printf("\t$s%i = new SWFButton();\n", id);

  if(flags)
    printf("\t/* button should be tracked as menu item (whatever that means..) */\n");

  offset = readUInt16(f);

  while(printButtonRecord(f, 2, id)) ;

  if(offset>0)
    while(printButton2ActionCondition(f, end, id)) ;
}

void printCXForm(struct CXForm *c, char ch, int num)
{
  if(c->aMult != 0x100)
    printf("\t$%c%i->multColor(%f, %f, %f, %f);\n", ch, num,
	   c->rMult/256.0, c->gMult/256.0, c->bMult/256.0, c->aMult/256.0);
  else
    printf("\t$%c%i->multColor(%f, %f, %f);\n", ch, num,
	   c->rMult/256.0, c->gMult/256.0, c->bMult/256.0);

  if(c->aAdd != 0)
    printf("\t$%c%i->addColor(0x%02x, 0x%02x, 0x%02x, 0x%02x);\n", ch, num,
	   c->rAdd, c->gAdd, c->bAdd, c->aAdd);
  else
    printf("\t$%c%i->addColor(0x%02x, 0x%02x, 0x%02x);\n", ch, num,
	   c->rAdd, c->gAdd, c->bAdd);
}

void printPlaceObject(FILE *f, int length)
{
  int start = fileOffset;
  struct Matrix m;
  int id = readUInt16(f);
  int depth = readUInt16(f);

  readMatrix(f, &m);

  if(sprite == 0)
    printf("\t$i%i = $m->add($s%i);\n", depth, id);
  else
    printf("\t$j%i = $s%i->add($s%i);\n", depth, sprite, id);

  printTransform(&m, (sprite==0)?'i':'j', depth);

  if(fileOffset < start+length)
  {
    struct CXForm c;
    readCXForm(f, &c, false);
    printCXForm(&c, (sprite==0)?'i':'j', depth);
  }
}

#define PLACE_RESERVED		(1<<7)
#define PLACE_HASCLIP           (1<<6)
#define PLACE_HASNAME		(1<<5)
#define PLACE_HASRATIO		(1<<4)
#define PLACE_HASCXFORM		(1<<3)
#define PLACE_HASMATRIX		(1<<2)
#define PLACE_HASCHARACTER	(1<<1)
#define PLACE_HASMOVE		(1<<0)

void printPlaceObject2(FILE *f)
{
  int flags = readUInt8(f);
  int depth = readUInt16(f);

  if(flags & PLACE_HASCHARACTER)
  {
    if(sprite == 0)
      printf("\t$i%i = $m->add($s%i);\n", depth, readUInt16(f));
    else
      printf("\t$j%i = $s%i->add($s%i);\n", depth, sprite, readUInt16(f));
  }

  if(flags & PLACE_HASMATRIX)
  {
    struct Matrix m;
    readMatrix(f, &m);
    printTransform(&m, (sprite==0)?'i':'j', depth);
  }

  if(flags & PLACE_HASCXFORM)
  {
    struct CXForm c;
    readCXForm(f, &c, true);
    printCXForm(&c, (sprite==0)?'i':'j', depth);
  }

  if(flags & PLACE_HASRATIO)
    printf("\t$%c%i->setRatio(%f);\n",
	   (sprite==0)?'i':'j', depth, (float)readUInt16(f)/(1<<16));

  if(flags & PLACE_HASNAME)
    printf("\t$%c%i->setName('%s');\n",
	   (sprite==0)?'i':'j', depth, readString(f));

  if(flags & PLACE_HASCLIP)
    printf("\t/* clipDepth (%i) not implemented */\n", readUInt16(f));
}

void printRemoveObject(FILE *f)
{
  /* may be cases where id matters, not sure. */

  readUInt16(f); /* id */

  if(sprite == 0)
    printf("\t$m->remove($i%i);\n", readUInt16(f));
  else
    printf("\t$s%i->remove($j%i);\n", sprite, readUInt16(f));
}

void printRemoveObject2(FILE *f)
{
  if(sprite == 0)
    printf("\t$m->remove($i%i);\n", readUInt16(f));
  else
    printf("\t$s%i->remove($j%i);\n", sprite, readUInt16(f));
}

void printSetBackgroundColor(FILE *f)
{
  struct RGBA s;
  readRGB(f, &s);

  printf("\t$m->setBackground(0x%02x, 0x%02x, 0x%02x);\n", s.r, s.g, s.b);
}

void printFrameLabel(FILE *f)
{
  if(sprite == 0)
    printf("\t$m->labelFrame('%s');\n", readString(f));
  else
    printf("\t$s%i->labelFrame('%s');\n", sprite, readString(f));
}

#define FONTINFO2_HASLAYOUT		(1<<7)
#define FONTINFO2_SHIFTJIS		(1<<6)
#define FONTINFO2_UNICODE		(1<<5)
#define FONTINFO2_ANSI			(1<<4)
#define FONTINFO2_WIDEOFFSETS	        (1<<3)
#define FONTINFO2_WIDECODES		(1<<2)
#define FONTINFO2_ITALIC		(1<<1)
#define FONTINFO2_BOLD			(1<<0)

#define FONTINFO_RESERVED	(1<<6 | 1<<7)
#define FONTINFO_UNICODE	(1<<5)
#define FONTINFO_SHIFTJIS	(1<<4)
#define FONTINFO_ANSI		(1<<3)
#define FONTINFO_ITALIC		(1<<2)
#define FONTINFO_BOLD		(1<<1)
#define FONTINFO_WIDE		(1<<0)

#define TEXTRECORD_STATECHANGE	(1<<7)
#define TEXTRECORD_RESERVED		(1<<6 | 1<<5 | 1<<4)
#define TEXTRECORD_HASFONT		(1<<3)
#define TEXTRECORD_HASCOLOR		(1<<2)
#define TEXTRECORD_HASYOFF		(1<<1)
#define TEXTRECORD_HASXOFF		(1<<0)
#define TEXTRECORD_NUMGLYPHS	0x7f

int printTextRecord(FILE *f, int glyphBits, int advanceBits, int type)
{
  int i, numGlyphs, fontid;
  int flags = readUInt8(f);
  struct RGBA rgba;

  if(flags == 0)
    return 0;

  if(flags & TEXTRECORD_STATECHANGE)
  {
    if(flags & TEXTRECORD_HASFONT)
      fontid = readUInt16(f); /* fontid */

    if(flags & TEXTRECORD_HASCOLOR)
    {
      if(type == 2)
	readRGBA(f, &rgba);
      else
	readRGB(f, &rgba);
    }

    if(flags & TEXTRECORD_HASXOFF)
      readSInt16(f); /* x offset */

    if(flags & TEXTRECORD_HASYOFF)
      readSInt16(f); /* y offset */

    if(flags & TEXTRECORD_HASFONT)
      readUInt16(f); /* font height */
  }
  else /* it's a text entry */
  {
    numGlyphs = flags & TEXTRECORD_NUMGLYPHS;

    for(i=0; i<numGlyphs; ++i)
    {
      readBits(f, glyphBits); /* glyph index */
      readSBits(f, advanceBits); /* glyph advance */
    }
  }

  return 1;
}

void printDefineText(FILE *f, Blocktype type) /* type 2 allows transparency */
{
  int glyphBits, advanceBits;
  int id = readUInt16(f);
  struct Rect r;
  struct Matrix m;

  printf("\t$s%i = new SWFShape();  /* empty shape for text, for now */\n", id);

  readRect(f, &r);
  byteAlign();
  readMatrix(f, &m);

  glyphBits = readUInt8(f);
  advanceBits = readUInt8(f);

  while(printTextRecord(f, glyphBits, advanceBits, type)) ;
}

void silentSkipBytes(FILE *f, int length)
{
  for(; length>0; --length)
    readUInt8(f);
}

void printDoAction(FILE *f, int length)
{
  if(sprite == 0)
  {
    printf("\t$m->add(new SWFAction(\"\n\n");
    decompileAction(f, length, 0);
    printf("\t\"));\n");
  }
  else
  {
    printf("\t$s%i->add(new SWFAction(\"\n\n", sprite);
    decompileAction(f, length, 0);
    printf("\t\"));\n");
  }
}

void printSprite(FILE *f, int length)
{
  int start = fileOffset;
  int block, type, l;
  int nFrames, frame = 0;

  sprite = readUInt16(f);
  nFrames = readUInt16(f);

  printf("\n\t/* -- Shape %i -- */\n", sprite);
  printf("\t$s%i = new SWFSprite();  /* (%i frames) */\n", sprite, nFrames);

  while(fileOffset < start+length)
  {
    block = readUInt16(f);
    type = block>>6;

    l = block & ((1<<6)-1);

    if(l == 63) /* it's a long block */
      l = readUInt32(f);

    switch(type)
    {
      case PLACEOBJECT:		printPlaceObject(f, l);	        break;
      case PLACEOBJECT2:	printPlaceObject2(f);		break;
      case REMOVEOBJECT:	printRemoveObject(f);		break;
      case REMOVEOBJECT2:	printRemoveObject2(f);		break;
      case FRAMELABEL:		printFrameLabel(f);		break;
      case DOACTION:		printDoAction(f, l);	        break;
      case SHOWFRAME:           printf("\t$s%i->nextFrame();  /* (end of sprite frame %i) */\n\n", sprite, frame++); break;
      case END:                 putchar('\n');                  break;
      /*
      case SOUNDSTREAMHEAD:     printSoundStreamHead(f, 1);     break;
      case SOUNDSTREAMHEAD2:    printSoundStreamHead(f, 2);     break;
      case SOUNDSTREAMBLOCK:    printSoundStreamBlock(f, l);    break;
      */
      default:
	printf("\t/* %s, %i bytes */\n", blockName(type), l);
	silentSkipBytes(f, l);
    }
  }

  sprite = 0;
}

#define TEXTFIELD_HASLENGTH (1<<1)
#define TEXTFIELD_NOEDIT    (1<<3)
#define TEXTFIELD_PASSWORD  (1<<4)
#define TEXTFIELD_MULTILINE (1<<5)
#define TEXTFIELD_WORDWRAP  (1<<6)
#define TEXTFIELD_DRAWBOX   (1<<11)
#define TEXTFIELD_NOSELECT  (1<<12)

#define TEXTFIELD_JUSTIFY_LEFT    0
#define TEXTFIELD_JUSTIFY_RIGHT   1
#define TEXTFIELD_JUSTIFY_CENTER  2
#define TEXTFIELD_JUSTIFY_JUSTIFY 3

void printTextField(FILE *f, int length)
{
  int notFirst = 0, flags, num, end = fileOffset+length;
  struct Rect r;
  struct RGBA c;
  char *string;
  int id = readUInt16(f);

  readRect(f, &r);

  flags=readUInt16(f);

  printf("\n\t$s%i = new SWFTextField(", id);

  if(flags & TEXTFIELD_HASLENGTH)
  {
    puts("TEXTFIELD_HASLENGTH");
    notFirst = 1;
  }
  if(flags & TEXTFIELD_NOEDIT)
  {
    printf("%sTEXTFIELD_NOEDIT", notFirst?" | ":"");
    notFirst = 1;
  }
  if(flags & TEXTFIELD_PASSWORD)
  {
    printf("%sTEXTFIELD_PASSWORD", notFirst?" | ":"");
    notFirst = 1;
  }
  if(flags & TEXTFIELD_MULTILINE)
  {
    printf("%sTEXTFIELD_MULTILINE", notFirst?" | ":"");
    notFirst = 1;
  }
  if(flags & TEXTFIELD_WORDWRAP)
  {
    printf("%sTEXTFIELD_WORDWRAP", notFirst?" | ":"");
    notFirst = 1;
  }
  if(flags & TEXTFIELD_DRAWBOX)
  {
    printf("%sTEXTFIELD_DRAWBOX", notFirst?" | ":"");
    notFirst = 1;
  }
  if(flags & TEXTFIELD_NOSELECT)
  {
    printf("%sTEXTFIELD_NOSELECT", notFirst?" | ":"");
    notFirst = 1;
  }

  puts(");\n");

  printf("\t$s%i->setBounds(%i, %i);\n", id, r.xMax, r.yMax);

  printf("\t$s%i->setFont(new SWFFont('_serif')); /* temp hack, should be $f%i */\n", id, readUInt16(f));
  printf("\t$s%i->setHeight(%i);\n", id, readUInt16(f));

  readRGBA(f, &c);
  printf("\t$s%i->setColor(0x%02x, 0x%02x, 0x%02x, 0x%02x);\n", id, c.r, c.g, c.b, c.a);

  if(flags & TEXTFIELD_HASLENGTH)
    printf("\t$s%i->setLength(%i);\n", id, readUInt16(f));

  printf("\t$s%i->align(", id);

  switch(num = readUInt8(f))
  {
    case TEXTFIELD_JUSTIFY_LEFT:    puts("TEXTFIELD_JUSTIFY_LEFT");    break;
    case TEXTFIELD_JUSTIFY_RIGHT:   puts("TEXTFIELD_JUSTIFY_RIGHT");   break;
    case TEXTFIELD_JUSTIFY_CENTER:  puts("TEXTFIELD_JUSTIFY_CENTER");  break;
    case TEXTFIELD_JUSTIFY_JUSTIFY: puts("TEXTFIELD_JUSTIFY_JUSTIFY"); break;
    default: error("unexpected justification: %i\n", num);
  }

  puts(");\n");

  if((num = readUInt16(f)) != 0)
    printf("\t$s%i->setLeftMargin(%i);\n", id, num);

  if((num = readUInt16(f)) != 0)
    printf("\t$s%i->setRightMargin(%i);\n", id, num);

  if((num = readUInt16(f)) != 0)
    printf("\t$s%i->setIndentation(%i);\n", id, num);

  if((num = readUInt16(f)) != 40)
    printf("\t$s%i->setLineSpacing(%i);\n", id, num);

  string = readString(f);

  if(string[0] != '\0')
    printf("\t$s%i->setName('%s');\n", id, string);

  if(fileOffset<end)
  {
    string = readString(f);

    if(string[0] != '\0')
      printf("\t$s%i->addString('%s');\n", id, string);
  }

  if(fileOffset<end)
  {
    puts("/* extra garbage (i.e., we goofed):\n");
    dumpBytes(f, end-fileOffset);
    puts("*/\n");
  }
}

void skipBytes(FILE *f, int length)
{
  ++gIndent;
  printf("%s<%i bytes skipped>\n\n", indent(), length);
  --gIndent;

  for(; length>0; --length)
    readUInt8(f);
}

int main(int argc, char *argv[])
{
  struct Movie m;
  FILE *f;
  int block, type, length, frame = 0;

  if(argc<2)
    error("Give me a filename.\n");

  if(!(f = fopen(argv[1],"rb")))
    error("Sorry, can't seem to read that file.\n");

  if(!(readUInt8(f)=='F' && readUInt8(f)=='W' && readUInt8(f)=='S'))
    error("Doesn't look like a swf file to me..\n");

  m.version = readUInt8(f);
  m.size = readUInt32(f);

  readRect(f, &(m.frame));

  m.rate = readUInt8(f)/256.0+readUInt8(f);
  m.nFrames = readUInt16(f);

  printf("<?\n");
  printf("\t$m = new SWFMovie();\n");
  printf("\t$m->setRate(%f);\n", m.rate);
  printf("\t$m->setDimension(%i, %i);\n", m.frame.xMax, m.frame.yMax);
  printf("\t$m->setFrames(%i);\n", m.nFrames);

  for(;;)
  {
    block = readUInt16(f);
    type = block>>6;

    length = block & ((1<<6)-1);

    if(length == 63) /* it's a long block. */
      length = readUInt32(f);

    if(type == 0 || fileOffset >= m.size)
      break;

    switch(type)
    {
      case DEFINESHAPE3:
      case DEFINESHAPE2:
      case DEFINESHAPE:		printShape(f, length);	        break;
      case SETBACKGROUNDCOLOR:	printSetBackgroundColor(f);	break;
      case SHOWFRAME:           printf("\t$m->nextFrame();  /* (end of frame %i) */\n\n", frame++); break;
      case PLACEOBJECT2:	printPlaceObject2(f);		break;
      case REMOVEOBJECT2:	printRemoveObject2(f);		break;
      case DOACTION:		printDoAction(f, length);	break;
      case DEFINESPRITE:        printSprite(f, length);         break;
      case DEFINEBUTTON:        printDefineButton(f, length);   break;
      case DEFINEBUTTON2:       printDefineButton2(f, length);  break;
      case FRAMELABEL:		printFrameLabel(f);		break;
      case DEFINETEXT:
      case DEFINETEXT2:		printDefineText(f, type);	break;
      case TEXTFIELD:           printTextField(f, length);      break;
      case DEFINEMORPHSHAPE:	printMorphShape(f, length);	break; 
      case PLACEOBJECT:		printPlaceObject(f, length);	break;
      case REMOVEOBJECT:	printRemoveObject(f);		break;
	/*
      case DEFINEFONT:		printDefineFont(f, length);	break;
      case DEFINEFONT2:		printDefineFont2(f, length);	break;
      case DEFINEFONTINFO:	printFontInfo(f, length);	break;
      case DEFINESOUND:         printDefineSound(f, length);    break;
      case SOUNDSTREAMHEAD:     printSoundStreamHead(f, 1);     break;
      case SOUNDSTREAMHEAD2:    printSoundStreamHead(f, 2);     break;
      case SOUNDSTREAMBLOCK:    printSoundStreamBlock(f, length); break;
      case JPEGTABLES:          printJpegStream(f, length);     break;
      case DEFINEBITS:
      case DEFINEBITSJPEG2:     printDefineBitsJpeg(f,length);  break;

      case DEFINEBITSJPEG3:
	*/
      default:
	printf("\t/* %s, %i bytes */\n", blockName(type), length);
	silentSkipBytes(f, length);
    }
  }

  putchar('\n');

  if(fileOffset < m.size)
  {
    printf("/* extra garbage (i.e., we messed up):\n");
    dumpBytes(f, m.size-fileOffset);
    printf("\n*/\n");
  }

  printf("\n\theader('Content-type: application/x-shockwave-flash');\n");
  printf("\t$m->output();\n");
  printf("?>\n");

  exit(0);
}
