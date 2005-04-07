#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decompile.h"
#include "parser.h"
#include "read.h"

extern struct Movie m;

#define PAR_BEGIN(block) \
	struct block *parserrec;\
	parserrec=(struct block *)calloc(1,sizeof(struct block)); \


#define PAR_END \
	return (SWF_Parserstruct *)parserrec;

/* Parse Basic Flash types */

void
parseSWF_RGB (FILE * f, struct SWF_RGBA *rgb)
{
  rgb->red = readUInt8 (f);
  rgb->green = readUInt8 (f);
  rgb->blue = readUInt8 (f);
  rgb->alpha = 255;
}

void
parseSWF_RGBA (FILE * f, struct SWF_RGBA *rgb)
{
  rgb->red = readUInt8 (f);
  rgb->green = readUInt8 (f);
  rgb->blue = readUInt8 (f);
  rgb->alpha = readUInt8 (f);
}

void
parseSWF_RECT (FILE * f, struct SWF_RECT *rect)
{
  byteAlign ();

  rect->Nbits = readBits (f, 5);
  rect->Xmin = readSBits (f, rect->Nbits);
  rect->Xmax = readSBits (f, rect->Nbits);
  rect->Ymin = readSBits (f, rect->Nbits);
  rect->Ymax = readSBits (f, rect->Nbits);
}

void
parseSWF_MATRIX (FILE * f, struct SWF_MATRIX *matrix)
{
  byteAlign ();

  matrix->HasScale = readBits (f, 1);
  if (matrix->HasScale)
    {
      matrix->NScaleBits = readBits (f, 5);
      matrix->ScaleX = (float) readSBits (f, matrix->NScaleBits) / 0x10000;
      matrix->ScaleY = (float) readSBits (f, matrix->NScaleBits) / 0x10000;
    }
  matrix->HasRotate = readBits (f, 1);
  if (matrix->HasRotate)
    {
      matrix->NRotateBits = readBits (f, 5);
      matrix->RotateSkew0 =
	(float) readSBits (f, matrix->NRotateBits) / 0x10000;
      matrix->RotateSkew1 =
	(float) readSBits (f, matrix->NRotateBits) / 0x10000;
    }
  matrix->NTranslateBits = readBits (f, 5);
  matrix->TranslateX = readSBits (f, matrix->NTranslateBits);
  matrix->TranslateY = readSBits (f, matrix->NTranslateBits);
}

void
parseSWF_CXFORMWITHALPHA (FILE * f, struct SWF_CXFORMWITHALPHA *cxform)
{
  byteAlign ();

  cxform->HasAddTerms = readBits (f, 1);
  cxform->HasMultTerms = readBits (f, 1);
  cxform->Nbits = readBits (f, 4);
}

int
parseSWF_CLIPEVENTFLAGS (FILE * f, struct SWF_CLIPEVENTFLAGS *cflags)
{
  byteAlign ();

  cflags->ClipEventKeyUp = readBits (f, 1);
  cflags->ClipEventKeyDown = readBits (f, 1);
  cflags->ClipEventMouseUp = readBits (f, 1);
  cflags->ClipEventMouseDown = readBits (f, 1);
  cflags->ClipEventMouseMove = readBits (f, 1);
  cflags->ClipEventUnload = readBits (f, 1);
  cflags->ClipEventEnterFrame = readBits (f, 1);
  cflags->ClipEventLoad = readBits (f, 1);
  cflags->ClipEventDragOver = readBits (f, 1);
  cflags->ClipEventRollOut = readBits (f, 1);
  cflags->ClipEventRollOver = readBits (f, 1);
  cflags->ClipEventReleaseOutside = readBits (f, 1);
  cflags->ClipEventRelease = readBits (f, 1);
  cflags->ClipEventPress = readBits (f, 1);
  cflags->ClipEventInitialize = readBits (f, 1);
  cflags->ClipEventData = readBits (f, 1);
  if( m.version >= 6 ) {
    cflags->Reserved = readBits (f, 5);
    cflags->ClipEventConstruct = readBits (f, 1);
    cflags->ClipEventKeyPress = readBits (f, 1);
    cflags->ClipEventDragOut = readBits (f, 1);
    cflags->Reserved2 = readBits (f, 8);
  }

  return cflags->ClipEventKeyUp|cflags->ClipEventKeyDown|cflags->ClipEventMouseUp|cflags->ClipEventMouseDown|cflags->ClipEventMouseMove|cflags->ClipEventUnload|cflags->ClipEventEnterFrame|cflags->ClipEventLoad|cflags->ClipEventDragOver|cflags->ClipEventRollOut|cflags->ClipEventRollOver|cflags->ClipEventReleaseOutside|cflags->ClipEventRelease|cflags->ClipEventPress|cflags->ClipEventInitialize|cflags->ClipEventData|cflags->ClipEventConstruct|cflags->ClipEventKeyPress|cflags->ClipEventDragOut;
}

int
parseSWF_CLIPACTIONRECORD (FILE * f, struct SWF_CLIPACTIONRECORD *carec)
{
  int length;
  byteAlign ();

  if( parseSWF_CLIPEVENTFLAGS( f, &(carec->EventFlag) ) == 0 )
	  return 0;
  carec->ActionRecordSize = readUInt32 (f);
  if( carec->EventFlag.ClipEventKeyPress ) {
  	carec->KeyCode = readUInt8 (f);
	length = carec->ActionRecordSize-1;
  } else {
	length = carec->ActionRecordSize;
  }
  carec->Actions = decompile5Action (f, length, 1);

  return 1;
}

void
parseSWF_CLIPACTIONS (FILE * f, struct SWF_CLIPACTIONS *clipact)
{
  byteAlign ();

  clipact->Reserved = readUInt16 (f);
  parseSWF_CLIPEVENTFLAGS( f, &(clipact->AllEventFlags) );
  /* parseSWF_CLIPACTIONRECORD( f, &(clipact->ClipActionRecords) ); */
  clipact->ClipActionRecords =
    (SWF_CLIPACTIONRECORD *) calloc (1, sizeof (SWF_CLIPACTIONRECORD));
  clipact->NumClipRecords = 0;
  while (parseSWF_CLIPACTIONRECORD
	 (f, &(clipact->ClipActionRecords[clipact->NumClipRecords++]) ) )
    {
      clipact->ClipActionRecords = (SWF_CLIPACTIONRECORD *) realloc (clipact->ClipActionRecords,
							 (clipact->
							  NumClipRecords +
							  1) *
							 sizeof
							 (SWF_CLIPACTIONRECORD));
    }
  /*
  clipact->ClipActionEndFlag = readUInt32 (f);
  */
}

void
parseSWF_GRADIENTRECORD (FILE * f, struct SWF_GRADIENTRECORD *gradientrec)
{
  gradientrec->Ratio = readUInt8 (f);
  parseSWF_RGBA (f, &gradientrec->Color);
}

void
parseSWF_GRADIENT (FILE * f, struct SWF_GRADIENT *gradient)
{
  int i;
  gradient->NumGradients = readUInt8 (f);
  for (i = 0; i < gradient->NumGradients; i++)
    parseSWF_GRADIENTRECORD (f, &(gradient->GradientRecords[i]));
}


int
parseSWF_SHAPERECORD (FILE * f, SWF_SHAPERECORD * shape, int *fillBits,
		      int *lineBits, int level)
{
  UI16 tmpbits;

  memset (shape, 0, sizeof (SWF_SHAPERECORD));
  shape->EndShape.TypeFlag = readBits (f, 1);
  if (shape->EndShape.TypeFlag)
    {
      /* An Edge Record */
      shape->StraightEdge.StraightEdge = readBits (f, 1);
      if (shape->StraightEdge.StraightEdge == 1)
	{
	  /* A Straight Edge Record */
	  shape->StraightEdge.NumBits = readBits (f, 4);
	  shape->StraightEdge.GeneralLineFlag = readBits (f, 1);
	  if (shape->StraightEdge.GeneralLineFlag)
	    {
	      shape->StraightEdge.DeltaX =
		readSBits (f, shape->StraightEdge.NumBits + 2);
	      shape->StraightEdge.DeltaY =
		readSBits (f, shape->StraightEdge.NumBits + 2);
	    }
	  else
	    {
	      shape->StraightEdge.VertLineFlag = readBits (f, 1);
	      if (shape->StraightEdge.VertLineFlag)
		{
		  shape->StraightEdge.VLDeltaY =
		    readSBits (f, shape->StraightEdge.NumBits + 2);
		}
	      else
		{
		  shape->StraightEdge.VLDeltaX =
		    readSBits (f, shape->StraightEdge.NumBits + 2);
		}
	    }
	}
      else
	{
	  /* A Curved Edge Record */
	  shape->CurvedEdge.NumBits = readBits (f, 4);
	  shape->CurvedEdge.ControlDeltaX =
	    readSBits (f, shape->CurvedEdge.NumBits + 2);
	  shape->CurvedEdge.ControlDeltaY =
	    readSBits (f, shape->CurvedEdge.NumBits + 2);
	  shape->CurvedEdge.AnchorDeltaX =
	    readSBits (f, shape->CurvedEdge.NumBits + 2);
	  shape->CurvedEdge.AnchorDeltaY =
	    readSBits (f, shape->CurvedEdge.NumBits + 2);
	}
    }
  else
    {
      /* A Non-Edge Record */
      tmpbits = readBits (f, 5);
      if (tmpbits == 0)
	{
	  /* EndShapeRecord */
	  shape->EndShape.EndOfShape = 0;
	  return 0;
	}
      /* StyleChangeRecord - ie one or more of the next 5 bits are set */

      if (tmpbits & (1 << 4))
	shape->StyleChange.StateNewStyles = 1;
      if (tmpbits & (1 << 3))
	shape->StyleChange.StateLineStyle = 1;
      if (tmpbits & (1 << 2))
	shape->StyleChange.StateFillStyle1 = 1;
      if (tmpbits & (1 << 1))
	shape->StyleChange.StateFillStyle0 = 1;
      if (tmpbits & (1 << 0))
	shape->StyleChange.StateMoveTo = 1;

      if (shape->StyleChange.StateMoveTo)
	{
	  shape->StyleChange.MoveBits = readBits (f, 5);
	  shape->StyleChange.MoveDeltaX =
	    readSBits (f, shape->StyleChange.MoveBits);
	  shape->StyleChange.MoveDeltaY =
	    readSBits (f, shape->StyleChange.MoveBits);
	}
      if (shape->StyleChange.StateFillStyle0)
	{
	  shape->StyleChange.FillStyle0 = readBits (f, *fillBits);
	}
      if (shape->StyleChange.StateFillStyle1)
	{
	  shape->StyleChange.FillStyle1 = readBits (f, *fillBits);
	}
      if (shape->StyleChange.StateLineStyle)
	{
	  shape->StyleChange.LineStyle = readBits (f, *lineBits);
	}
      if (shape->StyleChange.StateNewStyles)
	{
	  parseSWF_FILLSTYLEARRAY (f, &(shape->StyleChange.FillStyles),
				   level);
	  parseSWF_LINESTYLEARRAY (f, &(shape->StyleChange.LineStyles),
				   level);
	  shape->StyleChange.NumFillBits = *fillBits = readBits (f, 4);
	  shape->StyleChange.NumLineBits = *lineBits = readBits (f, 4);
	}

    }
  return 1;
}

void
parseSWF_FILLSTYLE (FILE * f, SWF_FILLSTYLE * fillstyle, int level)
{
  fillstyle->FillStyleType = readUInt8 (f);
  switch (fillstyle->FillStyleType)
    {
    case 0x00:			/* Solid Fill */
      if (level < 3)
	parseSWF_RGB (f, &fillstyle->Color);
      else
	parseSWF_RGBA (f, &fillstyle->Color);
      break;
    case 0x10:			/* Linear Gradient Fill */
    case 0x12:			/* Radial Gradient Fill */
      parseSWF_MATRIX (f, &fillstyle->GradientMatrix);
      parseSWF_GRADIENT (f, &fillstyle->Gradient);
      break;
    case 0x40:			/* Repeating Bitmap Fill */
    case 0x41:			/* Clipped Bitmap Fill */
    case 0x42:			/* Non-smoothed Repeating Bitmap Fill */
    case 0x43:			/* Non-smoothed Clipped Bitmap Fill */
      fillstyle->BitmapId = readUInt16 (f);
      parseSWF_MATRIX (f, &fillstyle->BitmapMatrix);
      break;
    }
}

void
parseSWF_FILLSTYLEARRAY (FILE * f, SWF_FILLSTYLEARRAY * fillstyle, int level)
{
  int count, i;
  fillstyle->FillStyleCount = readUInt8 (f);
  count = fillstyle->FillStyleCount;
  if (fillstyle->FillStyleCount == 0xff)
    {
      fillstyle->FillStyleCountExtended = readUInt16 (f);
      count = fillstyle->FillStyleCountExtended;
    }
  fillstyle->FillStyles =
    (SWF_FILLSTYLE *) calloc (count, sizeof (SWF_FILLSTYLE));
  for (i = 0; i < count; i++)
    {
      parseSWF_FILLSTYLE (f, &(fillstyle->FillStyles[i]), level);
    }
}

void
parseSWF_LINESTYLE (FILE * f, SWF_LINESTYLE * linestyle, int level)
{
  linestyle->Width = readUInt16 (f);
  if (level < 3)
    parseSWF_RGB (f, &linestyle->Color);
  else
    parseSWF_RGBA (f, &linestyle->Color);
}

void
parseSWF_LINESTYLEARRAY (FILE * f, SWF_LINESTYLEARRAY * linestyle, int level)
{
  int count, i;

  linestyle->LineStyleCount = readUInt8 (f);
  count = linestyle->LineStyleCount;
  if (linestyle->LineStyleCount == 0xff)
    {
      linestyle->LineStyleCountExtended = readUInt16 (f);
      count = linestyle->LineStyleCountExtended;
    }
  linestyle->LineStyles =
    (SWF_LINESTYLE *) malloc (count * sizeof (SWF_LINESTYLE));
  for (i = 0; i < count; i++)
    {
      parseSWF_LINESTYLE (f, &(linestyle->LineStyles[i]), level);
    }
}

void
parseSWF_SHAPE (FILE * f, SWF_SHAPE * shape, int level)
{
  int fillBits, lineBits;

  byteAlign ();

  shape->NumFillBits = fillBits = readBits (f, 4);
  shape->NumLineBits = lineBits = readBits (f, 4);
  shape->ShapeRecords =
    (SWF_SHAPERECORD *) calloc (1, sizeof (SWF_SHAPERECORD));
  shape->NumShapeRecords = 0;
  while (parseSWF_SHAPERECORD
	 (f, &(shape->ShapeRecords[shape->NumShapeRecords++]), &fillBits,
	  &lineBits, level))
    {
      shape->ShapeRecords = (SWF_SHAPERECORD *) realloc (shape->ShapeRecords,
							 (shape->
							  NumShapeRecords +
							  1) *
							 sizeof
							 (SWF_SHAPERECORD));
    }
}

void
parseSWF_SHAPEWITHSTYLE (FILE * f, SWF_SHAPEWITHSTYLE * shape, int level)
{
  int fillBits, lineBits;
  memset (shape, 0, sizeof (SWF_SHAPEWITHSTYLE));

  parseSWF_FILLSTYLEARRAY (f, &shape->FillStyles, level);
  parseSWF_LINESTYLEARRAY (f, &shape->LineStyles, level);

  byteAlign ();

  shape->NumFillBits = fillBits = readBits (f, 4);
  shape->NumLineBits = lineBits = readBits (f, 4);

  shape->ShapeRecords =
    (SWF_SHAPERECORD *) calloc (1, sizeof (SWF_SHAPERECORD));
  shape->NumShapeRecords = 0;
  while (parseSWF_SHAPERECORD
	 (f, &(shape->ShapeRecords[shape->NumShapeRecords++]), &fillBits,
	  &lineBits, level))
    {
      shape->ShapeRecords = (SWF_SHAPERECORD *) realloc (shape->ShapeRecords,
							 (shape->
							  NumShapeRecords +
							  1) *
							 sizeof
							 (SWF_SHAPERECORD));
    }
}

/* Parse Block types */

SWF_Parserstruct *
parseSWF_CHARACTERSET (FILE * f, int length)
{
  PAR_BEGIN (SWF_CHARACTERSET);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEBITS (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEBITS);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEBITSJPEG2 (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEBITSJPEG2);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEBITSJPEG3 (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEBITSJPEG3);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEBITSPTR (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEBITSPTR);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEBUTTON (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEBUTTON);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEBUTTON2 (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEBUTTON2);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEBUTTONCXFORM (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEBUTTONCXFORM);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEBUTTONSOUND (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEBUTTONSOUND);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINECOMMANDOBJ (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINECOMMANDOBJ);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEEDITTEXT (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEEDITTEXT);

  parserrec->CharacterID = readUInt16 (f);
  parseSWF_RECT (f, &(parserrec->Bounds));
  byteAlign ();
  parserrec->HasText = readBits (f, 1);
  parserrec->WordWrap = readBits (f, 1);
  parserrec->Multiline = readBits (f, 1);
  parserrec->Password = readBits (f, 1);
  parserrec->ReadOnly = readBits (f, 1);
  parserrec->HasTextColor = readBits (f, 1);
  parserrec->HasMaxLength = readBits (f, 1);
  parserrec->HasFont = readBits (f, 1);
  parserrec->Reserved = readBits (f, 1);
  parserrec->AutoSize = readBits (f, 1);
  parserrec->HasLayout = readBits (f, 1);
  parserrec->NoSelect = readBits (f, 1);
  parserrec->Border = readBits (f, 1);
  parserrec->Reserved2 = readBits (f, 1);
  parserrec->HTML = readBits (f, 1);
  parserrec->UseOutlines = readBits (f, 1);
  if (parserrec->HasFont)
    {
      parserrec->FontID = readUInt16 (f);
      parserrec->FontHeight = readUInt16 (f);
    }
  if (parserrec->HasTextColor)
    {
      parseSWF_RGBA (f, &parserrec->TextColor);
    }
  if (parserrec->HasMaxLength)
    {
      parserrec->MaxLength = readUInt16 (f);
    }
  if (parserrec->HasLayout)
    {
      parserrec->Align = readUInt8 (f);
      parserrec->LeftMargin = readUInt16 (f);
      parserrec->RightMargin = readUInt16 (f);
      parserrec->Indent = readUInt16 (f);
      parserrec->Leading = readUInt16 (f);
    }
  parserrec->VariableName = readString (f);
  if (parserrec->HasText)
    {
      parserrec->InitialText = readString (f);
    }

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEFONT (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEFONT);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEFONT2 (FILE * f, int length)
{
  int i;
  PAR_BEGIN (SWF_DEFINEFONT2);

  byteAlign ();

  parserrec->FontID = readUInt16 (f);
  parserrec->FontFlagsHasLayout = readBits (f, 1);
  parserrec->FontFlagsShiftJis = readBits (f, 1);
  parserrec->FontFlagsSmallText = readBits (f, 1);
  parserrec->FontFlagsFlagANSI = readBits (f, 1);
  parserrec->FontFlagsWideOffsets = readBits (f, 1);
  parserrec->FontFlagsWideCodes = readBits (f, 1);
  parserrec->FontFlagsFlagsItalics = readBits (f, 1);
  parserrec->FontFlagsFlagsBold = readBits (f, 1);
  parserrec->LanguageCode = readUInt8 (f);
  parserrec->FontNameLen = readUInt8 (f);
  parserrec->FontName = readSizedString (f, parserrec->FontNameLen);
  parserrec->NumGlyphs = readUInt16 (f);
  if (parserrec->FontFlagsWideOffsets)
    {
      parserrec->OffsetTable.UI32 =
	(UI32 *) malloc (parserrec->NumGlyphs * sizeof (UI32));
      for (i = 0; i < parserrec->NumGlyphs; i++)
	{
	  parserrec->OffsetTable.UI32[i] = readUInt32 (f);
	}
    }
  else
    {
      parserrec->OffsetTable.UI16 =
	(UI16 *) malloc (parserrec->NumGlyphs * sizeof (UI16));
      for (i = 0; i < parserrec->NumGlyphs; i++)
	{
	  parserrec->OffsetTable.UI16[i] = readUInt16 (f);
	}
    }
  if (parserrec->FontFlagsWideOffsets)
    {
      parserrec->OffsetTable.UI32 = readUInt32 (f);
    }
  else
    {
      parserrec->OffsetTable.UI16 = readUInt16 (f);
    }
  parserrec->GlyphShapeTable = (SWF_SHAPE *)
    malloc (parserrec->NumGlyphs * sizeof (SWF_SHAPE));
  for (i = 0; i < parserrec->NumGlyphs; i++)
    {
      parseSWF_SHAPE (f, &parserrec->GlyphShapeTable[i], 3);
    }

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEFONTINFO (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEFONTINFO);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINELOSSLESS (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINELOSSLESS);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINELOSSLESS2 (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINELOSSLESS2);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEMORPHSHAPE (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEMORPHSHAPE);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINESHAPE (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINESHAPE);

  parserrec->ShapeID = readUInt16 (f);
  parseSWF_RECT (f, &(parserrec->ShapeBounds));
  parseSWF_SHAPEWITHSTYLE (f, &(parserrec->Shapes), 1);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINESHAPE2 (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINESHAPE2);

  parserrec->ShapeID = readUInt16 (f);
  parseSWF_RECT (f, &(parserrec->ShapeBounds));
  parseSWF_SHAPEWITHSTYLE (f, &(parserrec->Shapes), 2);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINESHAPE3 (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINESHAPE3);

  parserrec->ShapeID = readUInt16 (f);
  parseSWF_RECT (f, &(parserrec->ShapeBounds));
  parseSWF_SHAPEWITHSTYLE (f, &(parserrec->Shapes), 3);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINESOUND (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINESOUND);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINESPRITE (FILE * f, int length)
{
  int block, type, splength, blockstart, nextFrame, i;
  int numblocks, start;
  PAR_BEGIN (SWF_DEFINESPRITE);

  numblocks=0;
  start = fileOffset;
  parserrec->SpriteId = readUInt16 (f);
  parserrec->FrameCount = readUInt16 (f);
  parserrec->tagTypes = NULL;
  parserrec->Tags = NULL;
  while( fileOffset < start+length ) {
	  /*
	  printf ("Block offset: %d\n", fileOffset);
	  */
	  block = readUInt16 (f);
	  type = block >> 6;
	  splength = block & ((1 << 6) - 1);
	  if (splength == 63)         /* it's a long block. */
		    splength = readUInt32 (f);
	  blockstart = fileOffset;
	  nextFrame = fileOffset+splength;
	  /*
	  printf ("Found Block: %s, %i bytes\n",
			  blockName (type), splength );
	  */

	  parserrec->tagTypes = (UI16 *)
	  	realloc(parserrec->tagTypes, ((numblocks+1)*sizeof(UI16)));
	  parserrec->Tags = (SWF_Parserstruct **)
	  	realloc(parserrec->Tags,
				((numblocks+1)*sizeof(SWF_Parserstruct *)));

	  parserrec->tagTypes[numblocks] = type;
	  parserrec->Tags[numblocks++]=blockParse(f,splength,type);
	  if( ftell(f) != nextFrame ) {
	    printf(" Sprite Stream out of sync...\n");
	    printf(" %d but expecting %d\n", ftell(f),nextFrame);
	    fseek(f,blockstart,SEEK_SET);
	    silentSkipBytes (f, (nextFrame-ftell(f)));
	    fileOffset=ftell(f);
	  }

  }
  parserrec->BlockCount = numblocks;

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINETEXT (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINETEXT);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINETEXT2 (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINETEXT2);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINETEXTFORMAT (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINETEXTFORMAT);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEVIDEO (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEVIDEO);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEVIDEOSTREAM (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEVIDEOSTREAM);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DOACTION (FILE * f, int length)
{
  PAR_BEGIN (SWF_DOACTION);

  parserrec->AScript = decompile5Action (f, length, 1);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_ENABLEDEBUGGER (FILE * f, int length)
{
  PAR_BEGIN (SWF_ENABLEDEBUGGER);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_END (FILE * f, int length)
{
  PAR_BEGIN (SWF_END);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_EXPORTASSETS (FILE * f, int length)
{
  PAR_BEGIN (SWF_EXPORTASSETS);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_FONTREF (FILE * f, int length)
{
  PAR_BEGIN (SWF_FONTREF);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_FRAMELABEL (FILE * f, int length)
{
  PAR_BEGIN (SWF_FRAMELABEL);

  parserrec->Name = readString (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_FRAMETAG (FILE * f, int length)
{
  PAR_BEGIN (SWF_FRAMETAG);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_FREEALL (FILE * f, int length)
{
  PAR_BEGIN (SWF_FREEALL);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_FREECHARACTER (FILE * f, int length)
{
  PAR_BEGIN (SWF_FREECHARACTER);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_GENCOMMAND (FILE * f, int length)
{
  PAR_BEGIN (SWF_GENCOMMAND);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_IMPORTASSETS (FILE * f, int length)
{
  PAR_BEGIN (SWF_IMPORTASSETS);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_JPEGTABLES (FILE * f, int length)
{
  PAR_BEGIN (SWF_JPEGTABLES);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_NAMECHARACTER (FILE * f, int length)
{
  PAR_BEGIN (SWF_NAMECHARACTER);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_PATHSAREPOSTSCRIPT (FILE * f, int length)
{
  PAR_BEGIN (SWF_PATHSAREPOSTSCRIPT);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_PLACEOBJECT (FILE * f, int length)
{
  PAR_BEGIN (SWF_PLACEOBJECT);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_PLACEOBJECT2 (FILE * f, int length)
{
  PAR_BEGIN (SWF_PLACEOBJECT2);

  byteAlign();

  parserrec->PlaceFlagHasClipActions = readBits (f, 1);
  parserrec->PlaceFlagHasClipDepth   = readBits (f, 1);
  parserrec->PlaceFlagHasName        = readBits (f, 1);
  parserrec->PlaceFlagHasRatio       = readBits (f, 1);
  parserrec->PlaceFlagHasColorTransform = readBits (f, 1);
  parserrec->PlaceFlagHasMatrix      = readBits (f, 1);
  parserrec->PlaceFlagHasCharacter   = readBits (f, 1);
  parserrec->PlaceFlagMove           = readBits (f, 1);
  parserrec->Depth = readUInt16 (f);
  if( parserrec->PlaceFlagHasCharacter ) {
    parserrec->CharacterId = readUInt16 (f);
  }
  if( parserrec->PlaceFlagHasMatrix ) {
    parseSWF_MATRIX( f, &(parserrec->Matrix) ); 
  }
  if( parserrec->PlaceFlagHasColorTransform ) {
    parseSWF_CXFORMWITHALPHA( f, &(parserrec->ColorTransform) ); 
  }
  if( parserrec->PlaceFlagHasRatio ) {
    parserrec->Ratio = readUInt16 (f);
  }
  if( parserrec->PlaceFlagHasName ) {
    parserrec->Name = readString (f);
  }
  if( parserrec->PlaceFlagHasClipDepth ) {
    parserrec->ClipDepth = readUInt16 (f);
  }
  if( parserrec->PlaceFlagHasClipActions ) {
    parseSWF_CLIPACTIONS( f, &(parserrec->ClipActions) ); 
  }

  PAR_END;
}

SWF_Parserstruct *
parseSWF_PREBUILT (FILE * f, int length)
{
  PAR_BEGIN (SWF_PREBUILT);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_PREBUILTCLIP (FILE * f, int length)
{
  PAR_BEGIN (SWF_PREBUILTCLIP);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_PROTECT (FILE * f, int length)
{
  PAR_BEGIN (SWF_PROTECT);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_REMOVEOBJECT (FILE * f, int length)
{
  PAR_BEGIN (SWF_REMOVEOBJECT);

  parserrec->CharacterId = readUInt16 (f);
  parserrec->Depth = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_REMOVEOBJECT2 (FILE * f, int length)
{
  PAR_BEGIN (SWF_REMOVEOBJECT2);

  parserrec->Depth = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_SERIALNUMBER (FILE * f, int length)
{
  PAR_BEGIN (SWF_SERIALNUMBER);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_SETBACKGROUNDCOLOR (FILE * f, int length)
{
  PAR_BEGIN (SWF_SETBACKGROUNDCOLOR);

  parseSWF_RGB (f, &parserrec->rgb);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_SHOWFRAME (FILE * f, int length)
{
  PAR_BEGIN (SWF_SHOWFRAME);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_SOUNDSTREAMBLOCK (FILE * f, int length)
{
  PAR_BEGIN (SWF_SOUNDSTREAMBLOCK);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_SOUNDSTREAMHEAD (FILE * f, int length)
{
  PAR_BEGIN (SWF_SOUNDSTREAMHEAD);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_SOUNDSTREAMHEAD2 (FILE * f, int length)
{
  PAR_BEGIN (SWF_SOUNDSTREAMHEAD2);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_STARTSOUND (FILE * f, int length)
{
  PAR_BEGIN (SWF_STARTSOUND);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_SYNCFRAME (FILE * f, int length)
{
  PAR_BEGIN (SWF_SYNCFRAME);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_VIDEOFRAME (FILE * f, int length)
{
  PAR_BEGIN (SWF_VIDEOFRAME);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}
