#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blocks/blocktypes.h"
#include "action.h"
#include "decompile.h"
#include "parser.h"
#include "read.h"

extern struct Movie m;
extern SWF_Parserstruct *blockParse (FILE *f, int length, SWFBlocktype header);
void silentSkipBytes(FILE *f, int length);

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

int
parseSWF_BUTTONRECORD (FILE * f, struct SWF_BUTTONRECORD *brec, int level)
{
  byteAlign ();

  brec->ButtonReserved = readBits (f, 4);
  brec->ButtonStateHitTest = readBits (f, 1);
  brec->ButtonStateDown = readBits (f, 1);
  brec->ButtonStateOver = readBits (f, 1);
  brec->ButtonStateUp = readBits (f, 1);
  if( brec->ButtonStateHitTest == 0 &&
      brec->ButtonStateDown == 0 &&
      brec->ButtonStateOver == 0 &&
      brec->ButtonStateUp == 0 )
	  return 0;
  brec->CharacterId = readUInt16 (f);
  brec->PlaceDepth = readUInt16 (f);
  parseSWF_MATRIX (f, &brec->PlaceMatrix);
  if( level > 1 )
  	parseSWF_CXFORMWITHALPHA (f, &brec->ColorTransform);

  return 1;
}

void
parseSWF_BUTTONCONDACTION (FILE * f, struct SWF_BUTTONCONDACTION *bcarec)
{
  int end;

  byteAlign ();

  end = fileOffset;
  bcarec->CondActionSize = readUInt16 (f);
  end += bcarec->CondActionSize;
  bcarec->CondIdleToOverDown = readBits (f, 1);
  bcarec->CondOutDownToIdle = readBits (f, 1);
  bcarec->CondOutDownToOverDown = readBits (f, 1);
  bcarec->CondOverDownToOutDown = readBits (f, 1);
  bcarec->CondOverDownToOverUp = readBits (f, 1);
  bcarec->CondOverUpToOverDown = readBits (f, 1);
  bcarec->CondOverUpToIdle = readBits (f, 1);
  bcarec->CondIdleToOverUp = readBits (f, 1);
  bcarec->CondKeyPress = readBits (f, 7);
  bcarec->CondOverDownToIdle = readBits (f, 1);

  bcarec->Actions =
    (SWF_ACTION *) calloc (1, sizeof (SWF_ACTION));
  bcarec->numActions = 0;
  while ( parseSWF_ACTIONRECORD (f, &(bcarec->numActions), bcarec->Actions) ) {
      bcarec->Actions = (SWF_ACTION *) realloc (bcarec->Actions,
							 (++bcarec->
							  numActions +
							  1) *
							 sizeof
							 (SWF_ACTION));
    }

}

void
parseSWF_CXFORM (FILE * f, struct SWF_CXFORM *cxform)
{
  byteAlign ();

  cxform->HasAddTerms = readBits (f, 1);
  cxform->HasMultTerms = readBits (f, 1);
  cxform->Nbits = readBits (f, 4);
  if( cxform->HasMultTerms ) {
    cxform->RedMultTerm = readBits(f, cxform->Nbits );
    cxform->GreenMultTerm = readBits(f, cxform->Nbits );
    cxform->BlueMultTerm = readBits(f, cxform->Nbits );
  }
  if( cxform->HasAddTerms ) {
    cxform->RedAddTerm = readBits(f, cxform->Nbits );
    cxform->GreenAddTerm = readBits(f, cxform->Nbits );
    cxform->BlueAddTerm = readBits(f, cxform->Nbits );
  }
}

void
parseSWF_CXFORMWITHALPHA (FILE * f, struct SWF_CXFORMWITHALPHA *cxform)
{
  byteAlign ();

  cxform->HasAddTerms = readBits (f, 1);
  cxform->HasMultTerms = readBits (f, 1);
  cxform->Nbits = readBits (f, 4);
  if( cxform->HasMultTerms ) {
    cxform->RedMultTerm = readBits(f, cxform->Nbits );
    cxform->GreenMultTerm = readBits(f, cxform->Nbits );
    cxform->BlueMultTerm = readBits(f, cxform->Nbits );
    cxform->AlphaMultTerm = readBits(f, cxform->Nbits );
  }
  if( cxform->HasAddTerms ) {
    cxform->RedAddTerm = readBits(f, cxform->Nbits );
    cxform->GreenAddTerm = readBits(f, cxform->Nbits );
    cxform->BlueAddTerm = readBits(f, cxform->Nbits );
    cxform->AlphaAddTerm = readBits(f, cxform->Nbits );
  }
}

void
parseSWF_GLYPHENTRY (FILE * f, SWF_GLYPHENTRY *gerec, int glyphbits, int advancebits)
{
  int i;

  gerec->GlyphIndex = malloc((glyphbits+31)/32 * sizeof(UI32) );
  for( i=0; glyphbits; i++ ) {
	  if( glyphbits > 32 ) {
	  	gerec->GlyphIndex[i] = readBits(f, 32);
	  	glyphbits -= 32;
  	} else {
	 	gerec->GlyphIndex[i] = readBits(f, glyphbits);
	 	glyphbits = 0;
  	}
  }

  gerec->GlyphAdvance = malloc((advancebits+31)/32 * sizeof(UI32) );
  for( i=0; advancebits; i++ ) {
	  if( advancebits > 32 ) {
	  	gerec->GlyphAdvance[i] = readBits(f, 32);
	  	advancebits -= 32;
  	} else {
	 	gerec->GlyphAdvance[i] = readBits(f, advancebits);
	 	advancebits = 0;
  	}
  }
}

int
parseSWF_TEXTRECORD (FILE * f, struct SWF_TEXTRECORD *brec, int glyphbits, int advancebits, int level)
{
  int i;

  byteAlign ();

  brec->TextRecordType = readBits (f, 1);
  brec->StyleFlagsReserved = readBits (f, 3);
  brec->StyleFlagHasFont = readBits (f, 1);
  brec->StyleFlagHasColor = readBits (f, 1);
  brec->StyleFlagHasYOffset = readBits (f, 1);
  brec->StyleFlagHasXOffset = readBits (f, 1);
  if( brec->TextRecordType == 0 )
	  return 0;
  if( brec->StyleFlagHasFont )
    brec->FontID = readUInt16 (f);
  if( brec->StyleFlagHasColor ) {
    if( level > 1 )
	    parseSWF_RGBA(f, &brec->TextColor );
    else
	    parseSWF_RGB(f, &brec->TextColor );
  }
  if( brec->StyleFlagHasXOffset )
    brec->XOffset = readSInt16 (f);
  if( brec->StyleFlagHasYOffset )
    brec->YOffset = readSInt16 (f);
  if( brec->StyleFlagHasFont )
    brec->TextHeight = readUInt16 (f);
  brec->GlyphCount = readUInt8 (f);
  brec->GlyphEntries = malloc(brec->GlyphCount * sizeof(SWF_GLYPHENTRY) );
  byteAlign ();
  for(i=0;i<brec->GlyphCount;i++)
	  parseSWF_GLYPHENTRY(f, &(brec->GlyphEntries[i]), glyphbits, advancebits );

  return 1;
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
  } else {
    cflags->Reserved = 0;
    cflags->ClipEventConstruct = 0;
    cflags->ClipEventKeyPress = 0;
    cflags->ClipEventDragOut = 0;
    cflags->Reserved2 = 0;
  }
 

  return cflags->ClipEventKeyUp|cflags->ClipEventKeyDown|cflags->ClipEventMouseUp|cflags->ClipEventMouseDown|cflags->ClipEventMouseMove|cflags->ClipEventUnload|cflags->ClipEventEnterFrame|cflags->ClipEventLoad|cflags->ClipEventDragOver|cflags->ClipEventRollOut|cflags->ClipEventRollOver|cflags->ClipEventReleaseOutside|cflags->ClipEventRelease|cflags->ClipEventPress|cflags->ClipEventInitialize|cflags->ClipEventData|cflags->ClipEventConstruct|cflags->ClipEventKeyPress|cflags->ClipEventDragOut;
}

int
parseSWF_CLIPACTIONRECORD (FILE * f, struct SWF_CLIPACTIONRECORD *carec)
{
  int length,end;
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
  end = fileOffset + length;
  /* carec->Actions = decompile5Action (f, length, 1); */
  carec->Actions =
    (SWF_ACTION *) calloc (1, sizeof (SWF_ACTION));
  carec->numActions = 0;

  while ( fileOffset < end ) {
      parseSWF_ACTIONRECORD (f, &(carec->numActions), carec->Actions);
      carec->Actions = (SWF_ACTION *) realloc (carec->Actions,
							 (++carec->
							  numActions +
							  1) *
							 sizeof
							 (SWF_ACTION));
    }

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
parseSWF_GRADIENTRECORD (FILE * f, struct SWF_GRADIENTRECORD *gradientrec, int level)
{
  gradientrec->Ratio = readUInt8 (f);
  if (level < 3)
     parseSWF_RGB (f, &gradientrec->Color);
  else
     parseSWF_RGBA (f, &gradientrec->Color);
}

void
parseSWF_GRADIENT (FILE * f, struct SWF_GRADIENT *gradient, int level)
{
  int i;
  gradient->NumGradients = readUInt8 (f);
  if( gradient->NumGradients > 8 ) {
	  fprintf(stderr,"Something is out of sync!!!\nNumGradient %d\n", gradient->NumGradients );
	  exit(1);
  }
  for (i = 0; i < gradient->NumGradients; i++)
    parseSWF_GRADIENTRECORD (f, &(gradient->GradientRecords[i]), level);
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
      parseSWF_GRADIENT (f, &fillstyle->Gradient, level);
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
parseSWF_MORPHLINESTYLE (FILE * f, SWF_MORPHLINESTYLE * linestyle)
{
  linestyle->StartWidth = readUInt16 (f);
  linestyle->EndWidth = readUInt16 (f);
  parseSWF_RGBA (f, &linestyle->StartColor);
  parseSWF_RGBA (f, &linestyle->EndColor);
}

void
parseSWF_MORPHLINESTYLES (FILE * f, SWF_MORPHLINESTYLES * linestyle)
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
    (SWF_MORPHLINESTYLE *) malloc (count * sizeof (SWF_MORPHLINESTYLE));
  for (i = 0; i < count; i++)
    {
      parseSWF_MORPHLINESTYLE (f, &(linestyle->LineStyles[i]));
    }
}

void
parseSWF_MORPHGRADIENTRECORD (FILE * f, struct SWF_MORPHGRADIENTRECORD *gradientrec)
{
  gradientrec->StartRatio = readUInt8 (f);
  parseSWF_RGBA (f, &gradientrec->StartColor);
  gradientrec->EndRatio = readUInt8 (f);
  parseSWF_RGBA (f, &gradientrec->EndColor);
}

void
parseSWF_MORPHGRADIENT (FILE * f, struct SWF_MORPHGRADIENT *gradient)
{
  int i;
  gradient->NumGradients = readUInt8 (f);
  if( gradient->NumGradients > 8 ) {
	  fprintf(stderr,"Something is out of sync!!!\nNumGradient %d\n", gradient->NumGradients );
	  exit(1);
  }
  for (i = 0; i < gradient->NumGradients; i++)
    parseSWF_MORPHGRADIENTRECORD (f, &(gradient->GradientRecords[i]));
}
void
parseSWF_MORPHFILLSTYLE (FILE * f, SWF_MORPHFILLSTYLE * fillstyle )
{
  fillstyle->FillStyleType = readUInt8 (f);
  switch (fillstyle->FillStyleType)
    {
    case 0x00:			/* Solid Fill */
	parseSWF_RGBA (f, &fillstyle->StartColor);
	parseSWF_RGBA (f, &fillstyle->EndColor);
      break;
    case 0x10:			/* Linear Gradient Fill */
    case 0x12:			/* Radial Gradient Fill */
      parseSWF_MATRIX (f, &fillstyle->StartGradientMatrix);
      parseSWF_MATRIX (f, &fillstyle->EndGradientMatrix);
      parseSWF_MORPHGRADIENT (f, &fillstyle->Gradient);
      break;
    case 0x40:			/* Repeating Bitmap Fill */
    case 0x41:			/* Clipped Bitmap Fill */
    case 0x42:			/* Non-smoothed Repeating Bitmap Fill */
    case 0x43:			/* Non-smoothed Clipped Bitmap Fill */
      fillstyle->BitmapId = readUInt16 (f);
      parseSWF_MATRIX (f, &fillstyle->StartBitmapMatrix);
      parseSWF_MATRIX (f, &fillstyle->EndBitmapMatrix);
      break;
    }
}
void
parseSWF_MORPHFILLSTYLES (FILE * f, SWF_MORPHFILLSTYLES * fillstyle )
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
    (SWF_MORPHFILLSTYLE *) calloc (count, sizeof (SWF_MORPHFILLSTYLE));
  for (i = 0; i < count; i++)
    {
      parseSWF_MORPHFILLSTYLE (f, &(fillstyle->FillStyles[i]));
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

/* Parse Action types */

#define ACT_BEGIN(acttype) \
	struct acttype *act;\
	act=(struct acttype *)action; \
	act->Length = readUInt16(f);

#define ACT_BEGIN_NOLEN(acttype) \
	struct acttype *act;\
	act=(struct acttype *)action;

int
parseSWF_ACTIONRECORD(FILE * f, int *thisactionp, SWF_ACTION *actions)
{
	int thisaction = *thisactionp;
	SWF_ACTION *action = &(actions[thisaction]);

	//fprintf(stderr,"ACTION[%d] Offset %d\n", thisaction, fileOffset );

	action->SWF_ACTIONRECORD.Offset = fileOffset; /* remember where it came from */
	if( (action->SWF_ACTIONRECORD.ActionCode = readUInt8(f)) == SWFACTION_END )
		return 0;
	/*
	 * Actions without the high bit set take no additional
	 * arguments, so we are done for these types.
	 */
	if( !(action->SWF_ACTIONRECORD.ActionCode&0x80) ) {
		action->SWF_ACTIONRECORD.Length = 1; /* Fill in the size for later use */
		return 1;
	}

	action->SWF_ACTIONRECORD.Length = 0; /* make valgrind happy */
	/*
	 * Actions with the high bit set take additional
	 * arguments, so we have to parse each one uniquely.
	 */
	switch( action->SWF_ACTIONRECORD.ActionCode ) {
		/* v3 actions */
	case SWFACTION_GOTOFRAME:
		{
		ACT_BEGIN(SWF_ACTIONGOTOFRAME)
		act->Frame = readUInt16(f);
		break;
		}
	case SWFACTION_GETURL:
		{
		ACT_BEGIN(SWF_ACTIONGETURL)
		act->UrlString = readString(f);
		act->TargetString = readString(f);
		break;
		}
	case SWFACTION_WAITFORFRAME:
		{
		ACT_BEGIN(SWF_ACTIONWAITFORFRAME)
		act->Frame = readUInt16(f);
		act->SkipCount = readUInt8(f);
		break;
		}
	case SWFACTION_SETTARGET:
		{
		ACT_BEGIN(SWF_ACTIONSETTARGET)
		act->TargetName = readString(f);
		break;
		}
	case SWFACTION_GOTOLABEL:
		{
		ACT_BEGIN(SWF_ACTIONGOTOLABEL)
		act->FrameLabel = readString(f);
		break;
		}


		/* v4 actions */
	case SWFACTION_PUSH:
		{
		int end;
		struct SWF_ACTIONPUSHPARAM *param;
		ACT_BEGIN(SWF_ACTIONPUSH)

		end = fileOffset + act->Length;
  		act->Params = (struct SWF_ACTIONPUSHPARAM *) calloc (1, sizeof (struct SWF_ACTIONPUSHPARAM));
  		act->NumParam = 0;
  		while ( fileOffset < end ) {
			param = &(act->Params[act->NumParam++]);
			param->Type = readUInt8(f);
			switch( param->Type ) {
			case 0: /* STRING */
				param->p.String = readString(f);
				break;
			case 1: /* FLOAT */
				param->p.Float = readUInt32(f);
				break;
			case 2: /* NULL */
			case 3: /* Undefined */
				break;
			case 4: /* Register */
				param->p.RegisterNumber = readUInt8(f);
				break;
			case 5: /* BOOLEAN */
				param->p.Boolean = readUInt8(f);
				break;
			case 6: /* DOUBLE */
				param->p.Double = readDouble(f);
				break;
			case 7: /* INTEGER */
				param->p.Integer = readSInt32(f);
				break;
			case 8: /* CONSTANT8 */
				param->p.Constant8 = readUInt8(f);
				break;
			case 9: /* CONSTANT16 */
				param->p.Constant16 = readUInt16(f);
				break;
			default:
				printf("Unknown data type to push %x\n", param->Type );
				exit(1);
			}
      			act->Params = (struct SWF_ACTIONPUSHPARAM *) realloc (act->Params,
							 (act->NumParam + 1) *
							 sizeof (struct SWF_ACTIONPUSHPARAM));
    		}
		break;
		}
	case SWFACTION_LOGICALNOT:
		{
		ACT_BEGIN_NOLEN(SWF_ACTIONNOT)
		act->Boolean = readUInt32(f);
		fprintf(stderr,"NOT param: %d\n", act->Boolean );
		break;
		}
	case SWFACTION_CALLFRAME:
		{
		ACT_BEGIN(SWF_ACTIONCALL)
		readUInt16(f);
		break;
		}
	case SWFACTION_JUMP:
		{
		ACT_BEGIN(SWF_ACTIONJUMP)
		act->BranchOffset = readUInt16(f);
		break;
		}
	case SWFACTION_IF:
		{
		int i,j,k, curroffset;
		ACT_BEGIN(SWF_ACTIONIF)

		act->BranchOffset = readUInt16(f);
		/*
		 * Set curroffset to point to the next action so that an
		 * offset of zero matches it.
		 */
		curroffset=(action->SWF_ACTIONRECORD.Offset-actions[0].SWF_ACTIONRECORD.Offset)+
			    action->SWF_ACTIONRECORD.Length+3; /* Action + Length bytes not included in the length */
		if( act->BranchOffset < 0 ) {
			/*
			 * We are branching to records that we already have in the array. Just
			 * allocate new space for the if clause, and copy the records there, and then
			 * fix the count of records in actions[], and put this record at the new
			 * end of actions[].
			 */
		    for(i=0;i<=thisaction;i++) {
			if( (actions[i].SWF_ACTIONRECORD.Offset-actions[0].SWF_ACTIONRECORD.Offset) == curroffset+act->BranchOffset ) break;
		    }
		    if( i>=thisaction ) {
			    fprintf(stderr,"Failed to find branch target!!!\n");
			    exit(2);
		    }
		    act->numActions = thisaction-i;
		    act->Actions = (union SWF_ACTION *) calloc (act->numActions, sizeof (SWF_ACTION));
		    for(j=i,k=0;j<thisaction;j++,k++)
			    act->Actions[k] = actions[j];
		    *thisactionp = i;
		} else {
			/*
			 * We are branching to records not yet parsed. Just handle this in the
			 * same manner used for with, try, etc.
			 */
		    act->Actions = (union SWF_ACTION *) calloc (1, sizeof (SWF_ACTION));
		    act->numActions = 0;
		    while ( (fileOffset-actions[0].SWF_ACTIONRECORD.Offset) < curroffset+act->BranchOffset ) {
			parseSWF_ACTIONRECORD (f, &(act->numActions), (SWF_ACTION *)act->Actions);
			act->Actions = (union SWF_ACTION *) realloc (act->Actions,
							 (++act->numActions + 1) *
							 sizeof (SWF_ACTION));
		    }
		}
		break;
		}
	case SWFACTION_GETURL2:
		{
		ACT_BEGIN(SWF_ACTIONGETURL2)
		act->f.FlagBits.SendVarsMethod = readBits(f,2);
		act->f.FlagBits.Reserved = readBits(f,4);
		act->f.FlagBits.LoadTargetFlag = readBits(f,1);
		act->f.FlagBits.LoadVariableFlag = readBits(f,1);
		break;
		}
	case SWFACTION_GOTOFRAME2:
		{
		ACT_BEGIN(SWF_ACTIONGOTOFRAME2)
		act->f.FlagBits.Reserved = readBits(f,6);
		act->f.FlagBits.SceneBiasFlag = readBits(f,1);
		act->f.FlagBits.PlayFlag = readBits(f,1);
		if( act->f.FlagBits.SceneBiasFlag ) {
			act->SceneBias = readUInt16(f);
		}
		break;
		}
	case SWFACTION_WAITFORFRAME2:
		{
		ACT_BEGIN(SWF_ACTIONWAITFORFRAME2)
		act->SkipCount = readUInt8(f);
		break;
		}


		/* v5 actions */
	case SWFACTION_CONSTANTPOOL:
		{
		int i;
		ACT_BEGIN(SWF_ACTIONCONSTANTPOOL)

		act->Count = readUInt16(f);
		act->ConstantPool = malloc(act->Count*sizeof(char *));
		for(i=0;i<act->Count;i++) {
			act->ConstantPool[i] = readString(f);
		}
		break;
		}
	case SWFACTION_DEFINEFUNCTION:
		{
		int i, end2;
		ACT_BEGIN(SWF_ACTIONDEFINEFUNCTION)

		act->FunctionName = readString(f);
		act->NumParams = readSInt16(f);
		act->Params = (STRING *)malloc(act->NumParams*sizeof(char *));
		for(i=0;i<act->NumParams;i++) {
			act->Params[i] = readString(f);
			/* printf("Read %s\n", act->ConstantPool[i] ); */
		}
		act->CodeSize = readSInt16(f);
		end2 = fileOffset + act->CodeSize;
		act->Actions = (union SWF_ACTION *) calloc (1, sizeof (SWF_ACTION));
		act->numActions = 0;
		while ( fileOffset < end2 ) {
			parseSWF_ACTIONRECORD (f, &(act->numActions), (SWF_ACTION *)act->Actions);
			act->Actions = (union SWF_ACTION *) realloc (act->Actions,
							 (++act->numActions + 1) *
							 sizeof (SWF_ACTION));
		    }
		break;
		}
	case SWFACTION_WITH:
		{
		int end;
		ACT_BEGIN(SWF_ACTIONWITH)

		act->Size = readUInt16(f);
		end = fileOffset + act->Size;
		act->Actions = (union SWF_ACTION *) calloc (1, sizeof (SWF_ACTION));
		act->numActions = 0;
		while ( fileOffset < end ) {
			parseSWF_ACTIONRECORD (f, &(act->numActions), (SWF_ACTION *)act->Actions);
			act->Actions = (union SWF_ACTION *) realloc (act->Actions,
							 (++act->numActions + 1) *
							 sizeof (SWF_ACTION));
		    }
		break;
		}
	case SWFACTION_STOREREGISTER:
		{
		ACT_BEGIN(SWF_ACTIONSTOREREGISTER)

		act->Register = readUInt8(f);
		break;
		}


		/* v6 actions */

		/* v7 actions */
	case SWFACTION_DEFINEFUNCTION2:
		{
		int i, end2;
		ACT_BEGIN(SWF_ACTIONDEFINEFUNCTION2)

		act->FunctionName = readString(f);
		act->NumParams = readSInt16(f);
		act->RegisterCount = readSInt8(f);
		act->PreloadParentFlag = readBits(f,1);
		act->PreloadRootFlag = readBits(f,1);
		act->SuppressSuperFlag = readBits(f,1);
		act->PreloadSuperFlag = readBits(f,1);
		act->SuppressArgumentsFlag = readBits(f,1);
		act->PreloadArgumentsFlag = readBits(f,1);
		act->SuppressThisFlag = readBits(f,1);
		act->PreloadThisFlag = readBits(f,1);
		act->Reserved = readBits(f,7);
		act->PreloadGlobalFlag = readBits(f,1);
		act->Params = (struct REGISTERPARAM *)malloc(act->NumParams*sizeof(struct REGISTERPARAM));
		for(i=0;i<act->NumParams;i++) {
			act->Params[i].Register = readUInt8(f);
			act->Params[i].ParamName = readString(f);
		}
		act->CodeSize = readSInt16(f);
		end2 = fileOffset + act->CodeSize;
		act->Actions = (union SWF_ACTION *) calloc (1, sizeof (SWF_ACTION));
		act->numActions = 0;
		while ( fileOffset < end2 ) {
			parseSWF_ACTIONRECORD (f, &(act->numActions), (SWF_ACTION *)act->Actions);
			act->Actions = (union SWF_ACTION *) realloc (act->Actions,
							 (++act->numActions + 1) *
							 sizeof (SWF_ACTION));
		    }
		break;
		}
	case SWFACTION_TRY:
		{
		int end2;
		ACT_BEGIN(SWF_ACTIONTRY)

		act->Reserved = readBits(f,5);
		act->CatchInRegisterFlag = readBits(f,1);
		act->FinallyBlockFlag = readBits(f,1);
		act->CatchBlockFlag = readBits(f,1);
		act->TrySize = readSInt16(f);
		act->CatchSize = readSInt16(f);
		act->FinallySize = readSInt16(f);
		if( act->CatchInRegisterFlag == 0 ) {
			act->CatchName = readString(f);
		} else {
			act->CatchRegister = readUInt8(f);
		}

		/* Try Body */
		end2 = fileOffset + act->TrySize;
		act->TryActs = (union SWF_ACTION *) calloc (1, sizeof (SWF_ACTION));
		act->numTryActs = 0;
		while ( fileOffset < end2 ) {
			parseSWF_ACTIONRECORD (f, &(act->numTryActs), (SWF_ACTION *)act->TryActs);
			act->TryActs = (union SWF_ACTION *) realloc (act->TryActs,
							 (++act->numTryActs + 1) *
							 sizeof (SWF_ACTION));
		    }

		/* Catch Body */
		end2 = fileOffset + act->CatchSize;
		act->CatchActs = (union SWF_ACTION *) calloc (1, sizeof (SWF_ACTION));
		act->numCatchActs = 0;
		while ( fileOffset < end2 ) {
			parseSWF_ACTIONRECORD (f, &(act->numCatchActs), (SWF_ACTION *)act->CatchActs);
			act->CatchActs = (union SWF_ACTION *) realloc (act->CatchActs,
							 (++act->numCatchActs + 1) *
							 sizeof (SWF_ACTION));
		    }

		/* Finally Body */
		end2 = fileOffset + act->FinallySize;
		act->FinallyActs = (union SWF_ACTION *) calloc (1, sizeof (SWF_ACTION));
		act->numFinallyActs = 0;
		while ( fileOffset < end2 ) {
			parseSWF_ACTIONRECORD (f, &(act->numFinallyActs), (SWF_ACTION *)act->FinallyActs);
			act->FinallyActs = (union SWF_ACTION *) realloc (act->FinallyActs,
							 (++act->numFinallyActs + 1) *
							 sizeof (SWF_ACTION));
		    }
		break;
		}
	default:
		printf("Not parsing action %x length %x\n", action->SWF_ACTIONRECORD.ActionCode, action->SWF_ACTIONRECORD.Length );
		peekBytes(f,100);
		exit(1);
	}
	return 1;
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
  int end = fileOffset + length;
  PAR_BEGIN (SWF_DEFINEBITS);

  parserrec->CharacterID = readUInt16 (f);
  parserrec->JPEGDataSize = end-fileOffset;
  parserrec->JPEGData = (UI8 *)readBytes(f,end-fileOffset);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEBITSJPEG2 (FILE * f, int length)
{
  int end = fileOffset + length;
  PAR_BEGIN (SWF_DEFINEBITSJPEG2);

  parserrec->CharacterID = readUInt16 (f);
  parserrec->JPEGDataSize = end-fileOffset;
  parserrec->JPEGData = (UI8 *)readBytes(f,end-fileOffset);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEBITSJPEG3 (FILE * f, int length)
{
  int end = fileOffset + length;
  PAR_BEGIN (SWF_DEFINEBITSJPEG3);

  parserrec->CharacterID = readUInt16 (f);
  parserrec->AlphaDataOffset = readUInt32 (f);
  parserrec->JPEGData = (UI8 *)readBytes(f,parserrec->AlphaDataOffset);
  parserrec->AlphaDataSize = end-fileOffset;
  parserrec->BitmapAlphaData = (UI8 *)readBytes(f,end-fileOffset);


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
  int stop;
  int end = fileOffset + length;
  PAR_BEGIN (SWF_DEFINEBUTTON2);

  byteAlign();

  parserrec->Buttonid = readUInt16 (f);
  parserrec->ReservedFlags = readBits (f, 7);
  parserrec->TrackAsMenu = readBits (f, 1);
  stop = fileOffset;
  parserrec->ActionOffset = readUInt16 (f);
  if( parserrec->ActionOffset )
    stop += parserrec->ActionOffset;
  else
    stop = end;
  parserrec->numCharacters = 0;
  parserrec->Characters =
    (SWF_BUTTONRECORD *) calloc (1, sizeof (SWF_BUTTONRECORD));

  while ( fileOffset < stop-1 ) {
    parseSWF_BUTTONRECORD (f, &(parserrec->Characters[parserrec->numCharacters++]), 2 );
    parserrec->Characters = (SWF_BUTTONRECORD *) realloc (parserrec->Characters,
							 (parserrec->numCharacters + 1) *
							 sizeof
							 (SWF_BUTTONRECORD));
    }

  parserrec->CharacterEndFlag = readUInt8 (f);

  parserrec->numActions = 0;
  parserrec->Actions =
    (SWF_BUTTONCONDACTION *) calloc (1, sizeof (SWF_BUTTONCONDACTION));
  while( fileOffset < end ) {
    parseSWF_BUTTONCONDACTION (f, &(parserrec->Actions[parserrec->numActions++]) );
    parserrec->Actions = (SWF_BUTTONCONDACTION *) realloc (parserrec->Actions,
							 (parserrec->numActions + 1) *
							 sizeof
							 (SWF_BUTTONCONDACTION));
    }

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
  int i;
  UI16  firstOffset;
  PAR_BEGIN (SWF_DEFINEFONT);

  parserrec->FontID = readUInt16 (f);
  firstOffset = readUInt16 (f);
  parserrec->OffsetTable = (UI16 *)malloc((firstOffset/2) * sizeof( UI16 ) );
  parserrec->OffsetTable[0] = firstOffset;
  for(i=1;i<firstOffset/2;i++) {
  	parserrec->OffsetTable[i] = readUInt16 (f);
  }
  parserrec->GlyphShapeTable = (SWF_SHAPE *)malloc(firstOffset/2 * sizeof( SWF_SHAPE ) );
  for(i=0;i<firstOffset/2;i++) {
  	parseSWF_SHAPE(f, &(parserrec->GlyphShapeTable[i]), 1 );
  }


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
	parserrec->CodeTableOffset.UI32 = readUInt32 (f);
    }
  else
    {
	parserrec->CodeTableOffset.UI16 = readUInt16 (f);
    }

  parserrec->GlyphShapeTable = (SWF_SHAPE *)
    malloc (parserrec->NumGlyphs * sizeof (SWF_SHAPE));
  for (i = 0; i < parserrec->NumGlyphs; i++)
    {
      parseSWF_SHAPE (f, &parserrec->GlyphShapeTable[i], 3);
    }

  parserrec->CodeTable =
	(int *) malloc (parserrec->NumGlyphs * sizeof (int));
  if (parserrec->FontFlagsWideCodes)
    {
      for (i = 0; i < parserrec->NumGlyphs; i++)
	{
	  parserrec->CodeTable[i] = readUInt16 (f);
	}
    }
  else
    {
      for (i = 0; i < parserrec->NumGlyphs; i++)
	{
	  parserrec->CodeTable[i] = readUInt8 (f);
	}
    }

  if( parserrec->FontFlagsHasLayout ) {
	  parserrec->FontAscent = readSInt16(f);
	  parserrec->FontDecent = readSInt16(f);
	  parserrec->FontLeading = readSInt16(f);
	  /* FontAdvanceTable */
	  parserrec->FontAdvanceTable =
	     (SI16 *) malloc (parserrec->NumGlyphs * sizeof (SI16));
	  for (i = 0; i < parserrec->NumGlyphs; i++)
	  {
	    parserrec->FontAdvanceTable[i] = readSInt16 (f);
	  }
	  /* FontBoundsTable */
	  parserrec->FontBoundsTable =
	     (SWF_RECT *) malloc (parserrec->NumGlyphs * sizeof (SWF_RECT));
	  for (i = 0; i < parserrec->NumGlyphs; i++)
	  {
	    parseSWF_RECT (f, &(parserrec->FontBoundsTable[i]));
	  }
	  parserrec->KerningCount = readUInt16(f);
	  /* FontKerningTable */
	  parserrec->FontKerningTable =
	     (struct SWF_KERNINGRECORD *) malloc (parserrec->KerningCount * sizeof (struct SWF_KERNINGRECORD));
	  for (i = 0; i < parserrec->KerningCount; i++)
	  {
	    if( parserrec->FontFlagsWideCodes ) {
		parserrec->FontKerningTable[i].FontKerningCode1 = readUInt16 (f);
		parserrec->FontKerningTable[i].FontKerningCode2 = readUInt16 (f);
	    } else {
		parserrec->FontKerningTable[i].FontKerningCode1 = readUInt8 (f);
		parserrec->FontKerningTable[i].FontKerningCode2 = readUInt8 (f);
	    }
	    parserrec->FontKerningTable[i].FontKerningADjustment = readSInt16 (f);
	  }
  }

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEFONTINFO (FILE * f, int length)
{
  int i, end = fileOffset + length;
  PAR_BEGIN (SWF_DEFINEFONTINFO);

  parserrec->FontID = readUInt16 (f);
  parserrec->FontNameLen = readUInt8 (f);
  parserrec->FontName = readSizedString (f, parserrec->FontNameLen);
  byteAlign ();
  parserrec->FontFlagsReserved = readBits (f, 2);
  parserrec->FontFlagsSmallText = readBits (f, 1);
  parserrec->FontFlagsShiftJIS = readBits (f, 1);
  parserrec->FontFlagsANSI = readBits (f, 1);
  parserrec->FontFlagsItalic = readBits (f, 1);
  parserrec->FontFlagsBold = readBits (f, 1);
  parserrec->FontFlagsWideCodes = readBits (f, 1);
  if( parserrec->FontFlagsWideCodes )
	  parserrec->nGlyph = end-(fileOffset/2);
  else
	  parserrec->nGlyph = end-fileOffset;

  parserrec->CodeTable = (UI16 *)malloc(parserrec->nGlyph*sizeof(UI16));
  for(i=0;i<parserrec->nGlyph;i++)
  if( parserrec->FontFlagsWideCodes )
	  parserrec->CodeTable[i] = readUInt16(f);
  else
	  parserrec->CodeTable[i] = readUInt8(f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINELOSSLESS (FILE * f, int length)
{
  int end = fileOffset + length;
  PAR_BEGIN (SWF_DEFINELOSSLESS);

  parserrec->CharacterID = readUInt16 (f);
  parserrec->BitmapFormat = readUInt8 (f);
  parserrec->BitmapWidth = readUInt16 (f);
  parserrec->BitmapHeight = readUInt16 (f);
  if( parserrec->BitmapFormat == 3 /* 8-bit */ ) {
      parserrec->BitmapColorTableSize = readUInt8 (f);
  }
  parserrec->ZlibBitmapData = (UI8 *)readBytes (f,end-fileOffset);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINELOSSLESS2 (FILE * f, int length)
{
  int end = fileOffset + length;
  PAR_BEGIN (SWF_DEFINELOSSLESS2);

  parserrec->CharacterID = readUInt16 (f);
  parserrec->BitmapFormat = readUInt8 (f);
  parserrec->BitmapWidth = readUInt16 (f);
  parserrec->BitmapHeight = readUInt16 (f);
  if( parserrec->BitmapFormat == 3 /* 8-bit */ ) {
      parserrec->BitmapColorTableSize = readUInt8 (f);
  }
  parserrec->ZlibBitmapData = (UI8 *)readBytes (f,end-fileOffset);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINEMORPHSHAPE (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINEMORPHSHAPE);

  parserrec->CharacterID = readUInt16 (f);
  parseSWF_RECT (f, &(parserrec->StartBounds));
  parseSWF_RECT (f, &(parserrec->EndBounds));
  parserrec->Offset = readUInt32 (f);
  parseSWF_MORPHFILLSTYLES (f, &(parserrec->MorphFillStyles));
  parseSWF_MORPHLINESTYLES (f, &(parserrec->MorphLineStyles));
  parseSWF_SHAPE (f, &(parserrec->StartEdges),0);
  parseSWF_SHAPE (f, &(parserrec->EndEdges),0);

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
  int end = fileOffset + length;
  PAR_BEGIN (SWF_DEFINESOUND);

  parserrec->SoundId = readUInt16 (f);
  byteAlign ();
  parserrec->SoundFormat = readBits (f, 4);
  parserrec->SoundRate = readBits (f, 2);
  parserrec->SoundSize = readBits (f, 1);
  parserrec->SoundType = readBits (f, 1);
  parserrec->SoundSampleCount = readUInt32 (f);
  parserrec->SoundData = (UI8 *)readBytes (f, end-fileOffset);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINESPRITE (FILE * f, int length)
{
  int block, type, splength, blockstart, nextFrame;
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
	    printf(" %ld but expecting %d\n", ftell(f),nextFrame);
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

  parserrec->CharacterID = readUInt16 (f);
  parseSWF_RECT (f, &(parserrec->TextBounds));
  parseSWF_MATRIX (f, &(parserrec->TextMatrix));
  parserrec->GlyphBits = readUInt8 (f);
  parserrec->AdvanceBits = readUInt8 (f);

  parserrec->TextRecords =
    (SWF_TEXTRECORD *) calloc (1, sizeof (SWF_TEXTRECORD));
  parserrec->numTextRecords = 0;
  while ( parseSWF_TEXTRECORD (f, &(parserrec->TextRecords[parserrec->numTextRecords++]), parserrec->GlyphBits, parserrec->AdvanceBits, 1 ) ) {
      parserrec->TextRecords = (SWF_TEXTRECORD *) realloc (parserrec->TextRecords,
							 (parserrec->
							  numTextRecords +
							  1) *
							 sizeof
							 (SWF_TEXTRECORD));
    }

  PAR_END;
}

SWF_Parserstruct *
parseSWF_DEFINETEXT2 (FILE * f, int length)
{
  PAR_BEGIN (SWF_DEFINETEXT2);

  parserrec->CharacterID = readUInt16 (f);
  parseSWF_RECT (f, &(parserrec->TextBounds));
  parseSWF_MATRIX (f, &(parserrec->TextMatrix));
  parserrec->GlyphBits = readUInt8 (f);
  parserrec->AdvanceBits = readUInt8 (f);

  parserrec->TextRecords =
    (SWF_TEXTRECORD *) calloc (1, sizeof (SWF_TEXTRECORD));
  parserrec->numTextRecords = 0;
  while ( parseSWF_TEXTRECORD (f, &(parserrec->TextRecords[parserrec->numTextRecords++]), parserrec->GlyphBits, parserrec->AdvanceBits, 2 ) ) {
      parserrec->TextRecords = (SWF_TEXTRECORD *) realloc (parserrec->TextRecords,
							 (parserrec->
							  numTextRecords +
							  1) *
							 sizeof
							 (SWF_TEXTRECORD));
    }

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
  int end = fileOffset + length;
  PAR_BEGIN (SWF_DOACTION);

  parserrec->Actions =
    (SWF_ACTION *) calloc (1, sizeof (SWF_ACTION));
  parserrec->numActions = 0;
  while ( fileOffset < end ) {
      parseSWF_ACTIONRECORD (f, &(parserrec->numActions), parserrec->Actions );
      parserrec->Actions = (SWF_ACTION *) realloc (parserrec->Actions,
							 (++parserrec->
							  numActions +
							  1) *
							 sizeof
							 (SWF_ACTION));
    }

  /* parserrec->AScript = decompile5Action (f, length, 1); */

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
  int i;
  PAR_BEGIN (SWF_EXPORTASSETS);

  parserrec->Count = readUInt16 (f);
  parserrec->Tags = (UI16 *)malloc(parserrec->Count*sizeof(UI16));
  parserrec->Names = (STRING *)malloc(parserrec->Count*sizeof(char *));
  for(i=0;i<parserrec->Count;i++) {
	parserrec->Tags[i] = readUInt16(f);
	parserrec->Names[i] = readString(f);
  }

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
  int i;
  PAR_BEGIN (SWF_IMPORTASSETS);

  parserrec->URL = readString (f);
  parserrec->Count = readUInt16 (f);
  parserrec->Tags = (UI16 *)malloc(parserrec->Count*sizeof(UI16));
  parserrec->Names = (STRING *)malloc(parserrec->Count*sizeof(char *));
  for(i=0;i<parserrec->Count;i++) {
	parserrec->Tags[i] = readUInt16(f);
	parserrec->Names[i] = readString(f);
  }

  PAR_END;
}

SWF_Parserstruct *
parseSWF_JPEGTABLES (FILE * f, int length)
{
  int end = fileOffset + length;
  PAR_BEGIN (SWF_JPEGTABLES);

  parserrec->JPEGDataSize = end-fileOffset;
  parserrec->JPEGData = (UI8 *)readBytes(f,end-fileOffset);

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

  byteAlign ();
  parserrec->Reserved = readBits (f, 4);
  parserrec->PlaybackSoundRate = readBits (f, 2);
  parserrec->PlaybackSoundSize = readBits (f, 1);
  parserrec->PlaybackSoundType = readBits (f, 1);
  parserrec->StreamSoundCompression = readBits (f, 4);
  parserrec->StreamSoundRate = readBits (f, 2);
  parserrec->StreamSoundSize = readBits (f, 1);
  parserrec->StreamSoundType = readBits (f, 1);
  parserrec->StreamSoundSampleCount = readUInt16 (f);
  if( parserrec->StreamSoundCompression == 2 /* MP3 */ )
    parserrec->LatencySeek = readUInt16 (f);

  PAR_END;
}

SWF_Parserstruct *
parseSWF_SOUNDSTREAMHEAD2 (FILE * f, int length)
{
  PAR_BEGIN (SWF_SOUNDSTREAMHEAD2);

  byteAlign ();
  parserrec->Reserved = readBits (f, 4);
  parserrec->PlaybackSoundRate = readBits (f, 2);
  parserrec->PlaybackSoundSize = readBits (f, 1);
  parserrec->PlaybackSoundType = readBits (f, 1);
  parserrec->StreamSoundCompression = readBits (f, 4);
  parserrec->StreamSoundRate = readBits (f, 2);
  parserrec->StreamSoundSize = readBits (f, 1);
  parserrec->StreamSoundType = readBits (f, 1);
  parserrec->StreamSoundSampleCount = readUInt16 (f);
  if( parserrec->StreamSoundCompression == 2 /* MP3 */ )
    parserrec->LatencySeek = readUInt16 (f);

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
parseSWF_INITACTION (FILE * f, int length)
{
  int end = fileOffset + length;
  PAR_BEGIN (SWF_INITACTION);

  parserrec->SpriteId = readUInt16 (f);
  parserrec->Actions =
    (SWF_ACTION *) calloc (1, sizeof (SWF_ACTION));
  parserrec->numActions = 0;
  while ( fileOffset < end ) {
      parseSWF_ACTIONRECORD (f, &(parserrec->numActions), parserrec->Actions);
      parserrec->Actions = (SWF_ACTION *) realloc (parserrec->Actions,
							 (++parserrec->
							  numActions +
							  1) *
							 sizeof
							 (SWF_ACTION));
    }

  /* parserrec->AScript = decompile5Action (f, length, 1); */

  PAR_END;
}

SWF_Parserstruct *
parseSWF_VIDEOFRAME (FILE * f, int length)
{
  PAR_BEGIN (SWF_VIDEOFRAME);

  parserrec->chid = readUInt16 (f);

  PAR_END;
}
