#include <math.h>
#include "blocks/blocktypes.h"
#include "parser.h"
#include "swfoutput.h"

/*
 * This file contains output functions that can convert the different SWF block
 * types into libming API calls that can recreate the block. Because the Ming
 * API usage is the same independent of the language being used, this file
 * can be used for multiple language bindings (OK, maybe not LISP). The
 * difference in syntax can be paramterized so the code in #ifdefs is not
 * a lot of duplicated code.
 */
#if !defined(SWFPHP) && !defined(SWFPERL) && !defined(SWFPYTHON)
#error "You must define SWFPHP or SWFPERL or SWFPYTHON when building this file"
#endif

#ifdef SWFPERL
#define COMMSTART "#"
#define COMMEND   ""
#define VAR       "$"
#define MEMBER    "->"
#define OBJPREF   "SWF::"
#define NEWOP     "new"
#endif
#ifdef SWFPHP
#define COMMSTART "/* "
#define COMMEND   "*/"
#define VAR       "$"
#define MEMBER    "->"
#define OBJPREF   "SWF"
#define NEWOP     "new"
#endif
#ifdef SWFPYTHON
#define COMMSTART "/* "
#define COMMEND   "*/"
#define VAR       ""
#define MEMBER    "."
#define OBJPREF   "SWF"
#define NEWOP     ""
#endif

static int framenum = 1;
static int spframenum = 1;
static int spritenum = 0;
static char spritename[64];

#define OUT_BEGIN(block) \
	struct block *sblock = (struct block *)pblock; \
	printf( "\n\t" COMMSTART " " #block " " COMMEND "\n");

static struct SWFOutput outputs[] = {
  {SWF_CHARACTERSET, outputSWF_CHARACTERSET},
  {SWF_DEFINEBITS, outputSWF_DEFINEBITS},
  {SWF_DEFINEBITSJPEG2, outputSWF_DEFINEBITSJPEG2},
  {SWF_DEFINEBITSJPEG3, outputSWF_DEFINEBITSJPEG3},
  {SWF_DEFINEBITSPTR, outputSWF_DEFINEBITSPTR},
  {SWF_DEFINEBUTTON, outputSWF_DEFINEBUTTON},
  {SWF_DEFINEBUTTON2, outputSWF_DEFINEBUTTON2},
  {SWF_DEFINEBUTTONCXFORM, outputSWF_DEFINEBUTTONCXFORM},
  {SWF_DEFINEBUTTONSOUND, outputSWF_DEFINEBUTTONSOUND},
  {SWF_DEFINECOMMANDOBJ, outputSWF_DEFINECOMMANDOBJ},
  {SWF_DEFINEEDITTEXT, outputSWF_DEFINEEDITTEXT},
  {SWF_DEFINEFONT, outputSWF_DEFINEFONT},
  {SWF_DEFINEFONT2, outputSWF_DEFINEFONT2},
  {SWF_DEFINEFONTINFO, outputSWF_DEFINEFONTINFO},
  {SWF_DEFINELOSSLESS, outputSWF_DEFINELOSSLESS},
  {SWF_DEFINELOSSLESS2, outputSWF_DEFINELOSSLESS2},
  {SWF_DEFINEMORPHSHAPE, outputSWF_DEFINEMORPHSHAPE},
  {SWF_DEFINESHAPE, outputSWF_DEFINESHAPE},
  {SWF_DEFINESHAPE2, outputSWF_DEFINESHAPE2},
  {SWF_DEFINESHAPE3, outputSWF_DEFINESHAPE3},
  {SWF_DEFINESOUND, outputSWF_DEFINESOUND},
  {SWF_DEFINESPRITE, outputSWF_DEFINESPRITE},
  {SWF_DEFINETEXT, outputSWF_DEFINETEXT},
  {SWF_DEFINETEXT2, outputSWF_DEFINETEXT2},
  {SWF_DEFINETEXTFORMAT, outputSWF_DEFINETEXTFORMAT},
  {SWF_DEFINEVIDEO, outputSWF_DEFINEVIDEO},
  {SWF_DEFINEVIDEOSTREAM, outputSWF_DEFINEVIDEOSTREAM},
  {SWF_DOACTION, outputSWF_DOACTION},
  {SWF_ENABLEDEBUGGER, outputSWF_ENABLEDEBUGGER},
  {SWF_END, outputSWF_END},
  {SWF_EXPORTASSETS, outputSWF_EXPORTASSETS},
  {SWF_FONTREF, outputSWF_FONTREF},
  {SWF_FRAMELABEL, outputSWF_FRAMELABEL},
  {SWF_FRAMETAG, outputSWF_FRAMETAG},
  {SWF_FREEALL, outputSWF_FREEALL},
  {SWF_FREECHARACTER, outputSWF_FREECHARACTER},
  {SWF_GENCOMMAND, outputSWF_GENCOMMAND},
  {SWF_IMPORTASSETS, outputSWF_IMPORTASSETS},
  {SWF_JPEGTABLES, outputSWF_JPEGTABLES},
  {SWF_NAMECHARACTER, outputSWF_NAMECHARACTER},
  {SWF_PATHSAREPOSTSCRIPT, outputSWF_PATHSAREPOSTSCRIPT},
  {SWF_PLACEOBJECT, outputSWF_PLACEOBJECT},
  {SWF_PLACEOBJECT2, outputSWF_PLACEOBJECT2},
  {SWF_PREBUILT, outputSWF_PREBUILT},
  {SWF_PREBUILTCLIP, outputSWF_PREBUILTCLIP},
  {SWF_PROTECT, outputSWF_PROTECT},
  {SWF_REMOVEOBJECT, outputSWF_REMOVEOBJECT},
  {SWF_REMOVEOBJECT2, outputSWF_REMOVEOBJECT2},
  {SWF_SERIALNUMBER, outputSWF_SERIALNUMBER},
  {SWF_SETBACKGROUNDCOLOR, outputSWF_SETBACKGROUNDCOLOR},
  {SWF_SHOWFRAME, outputSWF_SHOWFRAME},
  {SWF_SOUNDSTREAMBLOCK, outputSWF_SOUNDSTREAMBLOCK},
  {SWF_SOUNDSTREAMHEAD, outputSWF_SOUNDSTREAMHEAD},
  {SWF_SOUNDSTREAMHEAD2, outputSWF_SOUNDSTREAMHEAD2},
  {SWF_STARTSOUND, outputSWF_STARTSOUND},
  {SWF_SYNCFRAME, outputSWF_SYNCFRAME},
  {SWF_VIDEOFRAME, outputSWF_VIDEOFRAME},
};

static int numOutputs = sizeof (outputs) / sizeof (struct SWFOutput);

/* Handle language syntax differnces with these function */

char *
methodcall (char *varname, char *method)
{
  static char buf[256];

  sprintf (buf, VAR "%s" MEMBER "%s", varname, method);

  return buf;
}

char *
newobj (char *varname, char *obj)
{
  static char buf[256];

  if (varname)
    sprintf (buf, VAR "%s = " NEWOP " " OBJPREF "%s", varname, obj);
  else
    sprintf (buf, NEWOP " " OBJPREF "%s", obj);

  return buf;
}

/* Output basic Flash Types */

void
outputSWF_RGBA (SWF_RGBA * color, char *pname)
{
  printf ("\t" VAR "%s_red   = 0x%2.2x;\n", pname, color->red);
  printf ("\t" VAR "%s_green = 0x%2.2x;\n", pname, color->green);
  printf ("\t" VAR "%s_blue  = 0x%2.2x;\n", pname, color->blue);
  printf ("\t" VAR "%s_alpha = 0x%2.2x;\n", pname, color->alpha);
}

void
outputSWF_MATRIX (SWF_MATRIX * matrix, char *fname)
{
  float a, b, c, d;
  float angle, xScale, yScale, skew;

  a = matrix->ScaleX;
  b = matrix->RotateSkew0;
  c = matrix->RotateSkew1;
  d = matrix->ScaleY;

  xScale = sqrt (a * a + c * c);
  yScale = (a * d - b * c) / xScale;
  skew = (a * b + c * d) / (a * a + c * c);

  if (a == 0)
    {
      if (c < 0)
	angle = -90;
      else
	angle = 90;
    }
  else
    {
      angle = atan (c / a) * 180 / M_PI;

      if (a < 0)
	{
	  if (angle < 0)
	    angle += 180;
	  else
	    angle -= 180;
	}
    }

#define TOLERANCE 0.02

  if (skew < -TOLERANCE || skew > TOLERANCE)
    printf ("\t$%s->skewXTo(%f);\n", fname, skew);

  if (xScale > 1.0 - TOLERANCE && xScale < 1.0 + TOLERANCE)
    xScale = 1.0;

  if (yScale > 1.0 - TOLERANCE && yScale < 1.0 + TOLERANCE)
    yScale = 1.0;

  if (xScale != 1.0 || yScale != 1.0)
    {
      if (xScale == yScale)
	printf ("\t%s(%f);\n", methodcall (fname, "scaleTo"), xScale);
      else
	printf ("\t%s(%f, %f);\n", methodcall (fname, "scaleTo"), xScale,
		yScale);
    }

  if (angle < -TOLERANCE || angle > TOLERANCE)
    printf ("\t%s(%f);\n", methodcall (fname, "rotateTo"), angle);

  if (matrix->TranslateX != 0 || matrix->TranslateY != 0)
    printf ("\t%s(%ld, %ld);\n", methodcall (fname, "moveTo"),
	    matrix->TranslateX, matrix->TranslateY);
}

void
outputSWF_CLIPACTIONS (SWF_CLIPACTIONS * clipactions, char *sname)
{
	printf( "/* %d clip actions */\n", clipactions->NumClipRecords );
}
void
outputSWF_GRADIENT (SWF_GRADIENT * gradient, char *gname)
{
  int i;
  printf ("\t%s();\n", newobj (gname, "Gradient"));
  for (i = 0; i < gradient->NumGradients; i++)
    printf ("\t%s(%f,0x%2.2x,0x%2.2x,0x%2.2x,0x%2.2x);\n",
	    methodcall (gname, "addEntry"),
	    (gradient->GradientRecords[i].Ratio / 255.0),
	    gradient->GradientRecords[i].Color.red,
	    gradient->GradientRecords[i].Color.green,
	    gradient->GradientRecords[i].Color.blue,
	    gradient->GradientRecords[i].Color.alpha);
}

void
outputSWF_FILLSTYLE (SWF_FILLSTYLE * fillstyle, char *parentname, int i)
{
  char fname[64];
  char gname[64];

  switch (fillstyle->FillStyleType)
    {
    case 0x00:			/* Solid Fill */
      sprintf (fname, "%s_f%d", parentname, i);
      outputSWF_RGBA (&fillstyle->Color, fname);
      printf ("\t" VAR "%s = %s(" VAR "%s_red, "
	      VAR "%s_green, "
	      VAR "%s_blue, "
	      VAR "%s_alpha "
	      "); " COMMSTART "SWFFILL_SOLID" COMMEND "\n",
	      fname,
	      methodcall (parentname, "addFill"), fname, fname, fname, fname);
      break;
    case 0x10:			/* Linear Gradient Fill */
      sprintf (gname, "%s_g%d", parentname, i);
      outputSWF_GRADIENT (&fillstyle->Gradient, gname);
      sprintf (fname, "%s_f%d", parentname, i);
      printf ("\t" VAR "%s = %s(" VAR "%s,SWFFILL_LINEAR_GRADIENT);\n",
	      fname, methodcall (parentname, "addFill"), gname);
      outputSWF_MATRIX (&fillstyle->GradientMatrix, fname);
      break;
    case 0x12:			/* Radial Gradient Fill */
      sprintf (gname, "%s_g%d", parentname, i);
      outputSWF_GRADIENT (&fillstyle->Gradient, gname);
      sprintf (fname, "%s_f%d", parentname, i);
      printf ("\t" VAR "%s = %s(" VAR "%s,SWFFILL_RADIAL_GRADIENT);\n",
	      fname, methodcall (parentname, "addFill"), gname);
      outputSWF_MATRIX (&fillstyle->GradientMatrix, fname);
      break;
    case 0x40:			/* Repeating Bitmap Fill */
    case 0x41:			/* Clipped Bitmap Fill */
    case 0x42:			/* Non-smoothed Repeating Bitmap Fill */
    case 0x43:			/* Non-smoothed Clipped Bitmap Fill */
      printf (" BitmapID: %d\n", fillstyle->BitmapId);
      sprintf (fname, "%s_f%d", parentname, i);
      printf ("\t" VAR "%s = %s(" VAR "%s,SWFFILL_RADIAL_GRADIENT);\n",
	      fname, methodcall (parentname, "addFill"), gname);
      outputSWF_MATRIX (&fillstyle->BitmapMatrix, fname);
      break;
    }

}

void
outputSWF_FILLSTYLEARRAY (SWF_FILLSTYLEARRAY * fillstylearray,
			  char *parentname)
{
  int i, count;

  count = (fillstylearray->FillStyleCount != 0xff) ?
    fillstylearray->FillStyleCount : fillstylearray->FillStyleCountExtended;

  printf ("\t" COMMSTART "%d fillstyle(s)" COMMEND "\n", count);

  for (i = 0; i < count; i++)
    {
      outputSWF_FILLSTYLE (&(fillstylearray->FillStyles[i]), parentname, i);
    }
}

void
outputSWF_LINESTYLE (SWF_LINESTYLE * linestyle, char *parentname, int i)
{
  char lname[64];
  sprintf (lname, "%s_l%d", parentname, i);
  printf ("\t" VAR "%s_width = %d;\n", lname, linestyle->Width);
  outputSWF_RGBA (&linestyle->Color, lname);

}

void
outputSWF_LINESTYLEARRAY (SWF_LINESTYLEARRAY * linestylearray,
			  char *parentname)
{
  int i, count;

  count = (linestylearray->LineStyleCount != 0xff) ?
    linestylearray->LineStyleCount : linestylearray->LineStyleCountExtended;

  printf ("\t" COMMSTART "%d linestyles(s)" COMMEND "\n", count);
  for (i = 0; i < count; i++)
    {
      outputSWF_LINESTYLE (&(linestylearray->LineStyles[i]), parentname, i);
    }
}

void
outputSWF_SHAPERECORD (SWF_SHAPERECORD * shaperec, char *parentname)
{
  if (shaperec->EndShape.TypeFlag)
    {
      /* An Edge Record */
      if (shaperec->StraightEdge.StraightEdge == 1)
	{
	  /* A Straight Edge Record */
	  if (shaperec->StraightEdge.GeneralLineFlag)
	    {
	      /* The general case */
	      printf ("\t%s(%ld, %ld);\n", methodcall (parentname, "drawLine"),
		      shaperec->StraightEdge.DeltaX,
		      shaperec->StraightEdge.DeltaY);
	    }
	  else
	    {
	      /* The Horizontal or Verticle case */
	      printf ("\t%s(%ld, %ld);\n", methodcall (parentname, "drawLine"),
		      shaperec->StraightEdge.VLDeltaX,
		      shaperec->StraightEdge.VLDeltaY);
	    }
	}
      else
	{
	  /* A Curved Edge Record */
	  printf ("\t%s(%ld, %ld, %ld, %ld);\n",
		  methodcall (parentname, "drawCurve"),
		  shaperec->CurvedEdge.ControlDeltaX,
		  shaperec->CurvedEdge.ControlDeltaY,
		  shaperec->CurvedEdge.AnchorDeltaX,
		  shaperec->CurvedEdge.AnchorDeltaY);
	}
    }
  else
    {
      /* A Non-Edge Record */
      if (shaperec->EndShape.EndOfShape == 0)
	{
	  return;
	}

      if (shaperec->StyleChange.StateLineStyle)
	{
	  printf (" StateLineStyle: %ld\n", shaperec->StyleChange.LineStyle);
	  if (shaperec->StyleChange.LineStyle == 0)
	    {
	      printf ("\t%s(0);\n", methodcall (parentname, "setLine"));
	    }
	  else
	    {
	      /*
	       * We use the variable names that were output by
	       * outputSWF_LINESTYLE()
	       */
	      printf ("\t%s(" VAR "%s_l%ld_width, " VAR "%s_l%ld_red, "
		      VAR "%s_l%ld_green, " VAR "%s_l%ld_blue, "
		      VAR "%s_l%ld_alpha);\n",
		      methodcall (parentname, "setLine"),
		      parentname, shaperec->StyleChange.LineStyle - 1,
		      parentname, shaperec->StyleChange.LineStyle - 1,
		      parentname, shaperec->StyleChange.LineStyle - 1,
		      parentname, shaperec->StyleChange.LineStyle - 1,
		      parentname, shaperec->StyleChange.LineStyle - 1);
	    }
	}
      if (shaperec->StyleChange.StateFillStyle1)
	{
	  printf ("\t%s(", methodcall (parentname, "setRightFill"));
	  if (shaperec->StyleChange.FillStyle1)
	    {
/* This is supposed tocome from the parent SHAPE record
		  SWF_FILLSTYLE *fill;
		  fill=&(shaperec->StyleChange.FillStyles.FillStyles[shaperec->StyleChange.FillStyle1-1]);
		  if( fill->FillStyleType == 0 ) {
			  printf("%d, %d, %d, %d ",
				fill->Color.red,
				fill->Color.green,
				fill->Color.blue,
				fill->Color.alpha );

		  } else {
*/
	      printf ("$%s_f%ld", parentname,
		      shaperec->StyleChange.FillStyle1 - 1);
/*
		  }
*/

	    }
	  printf (");\n");
	}
      if (shaperec->StyleChange.StateFillStyle0)
	{
	  printf (" FillStyle0: %ld\n", shaperec->StyleChange.FillStyle0);
	}
      if (shaperec->StyleChange.StateMoveTo)
	{
	  printf ("\t%s(%ld, %ld);\n", methodcall (parentname, "movePenTo"),
		  shaperec->StyleChange.MoveDeltaX,
		  shaperec->StyleChange.MoveDeltaY);
	}
    }

}

void
outputSWF_SHAPE (SWF_SHAPE * shape, char *name)
{
  int i;
  for (i = 0; i < shape->NumShapeRecords; i++)
    {
      outputSWF_SHAPERECORD (&(shape->ShapeRecords[i]), name);
    }
}

void
outputSWF_SHAPEWITHSTYLE (SWF_SHAPEWITHSTYLE * shape, int level, char *name)
{
  int i;
  outputSWF_FILLSTYLEARRAY (&(shape->FillStyles), name);
  outputSWF_LINESTYLEARRAY (&(shape->LineStyles), name);
  for (i = 0; i < shape->NumShapeRecords; i++)
    {
      outputSWF_SHAPERECORD (&(shape->ShapeRecords[i]), name);
    }
}

/* Output Flash Blocks */
void
outputSWF_CHARACTERSET (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_CHARACTERSET);

}

void
outputSWF_DEFINEBITS (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEBITS);

}

void
outputSWF_DEFINEBITSJPEG2 (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEBITSJPEG2);

}

void
outputSWF_DEFINEBITSJPEG3 (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEBITSJPEG3);

}

void
outputSWF_DEFINEBITSPTR (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEBITSPTR);

}

void
outputSWF_DEFINEBUTTON (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEBUTTON);

}

void
outputSWF_DEFINEBUTTON2 (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEBUTTON2);

}

void
outputSWF_DEFINEBUTTONCXFORM (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEBUTTONCXFORM);

}

void
outputSWF_DEFINEBUTTONSOUND (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEBUTTONSOUND);

}

void
outputSWF_DEFINECOMMANDOBJ (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINECOMMANDOBJ);

}

void
outputSWF_DEFINEEDITTEXT (SWF_Parserstruct * pblock)
{
  int notFirst = 0;
  char tname[64];
  OUT_BEGIN (SWF_DEFINEEDITTEXT);

  sprintf (tname, "s%d", sblock->CharacterID);
  printf ("\t%s(", newobj (tname, "TextField"));
  if (sblock->HasText)
    {
      printf ("SWFTEXTFIELD_HASTEXT");
      notFirst = 1;
    }
  if (sblock->WordWrap)
    {
      if (notFirst)
	printf (" | ");
      printf ("SWFTEXTFIELD_WORDWRAP");
      notFirst = 1;
    }
  if (sblock->Multiline)
    {
      if (notFirst)
	printf (" | ");
      printf ("SWFTEXTFIELD_MULTILINE");
      notFirst = 1;
    }
  if (sblock->Password)
    {
      if (notFirst)
	printf (" | ");
      printf ("SWFTEXTFIELD_PASSWORD");
      notFirst = 1;
    }
  if (sblock->ReadOnly)
    {
      if (notFirst)
	printf (" | ");
      printf ("SWFTEXTFIELD_NOEDIT");
      notFirst = 1;
    }
  if (sblock->HasMaxLength)
    {
      if (notFirst)
	printf (" | ");
      printf ("SWFTEXTFIELD_HASLENGTH");
      notFirst = 1;
    }
  if (sblock->AutoSize)
    {
      if (notFirst)
	printf (" | ");
      printf ("SWFTEXTFIELD_AUTOSIZE");
      notFirst = 1;
    }
/*
if( sblock->HasLayout ) {
	if( notFirst ) printf("|");
	printf("SWFTEXTFIELD_HASLAYOUT");
	notFirst=1;
}
*/
  if (sblock->NoSelect)
    {
      if (notFirst)
	printf (" | ");
      printf ("SWFTEXTFIELD_NOSELECT");
      notFirst = 1;
    }
  if (sblock->Border)
    {
      if (notFirst)
	printf (" | ");
      printf ("SWFTEXTFIELD_DRAWBOX");
      notFirst = 1;
    }
  if (sblock->HTML)
    {
      if (notFirst)
	printf (" | ");
      printf ("SWFTEXTFIELD_HTML");
      notFirst = 1;
    }
  if (sblock->UseOutlines)
    {
      if (notFirst)
	printf (" | ");
      printf ("SWFTEXTFIELD_OUTLINES");
      notFirst = 1;
    }
  printf (");\n");

  printf ("\t%s(%ld, %ld);\n", methodcall (tname, "setBounds"),
	  sblock->Bounds.Xmax, sblock->Bounds.Ymax);
  if (sblock->HasFont)
    {
      printf ("\t%s(" VAR "f%d);\n", methodcall (tname, "setFont"),
	      sblock->FontID);
      printf ("\t%s(%d);\n", methodcall (tname, "setHeight"),
	      sblock->FontHeight);
    }
  if (sblock->HasTextColor)
    {
      printf ("\t%s(0x%02x, 0x%02x, 0x%02x, 0x%02x);\n",
	      methodcall (tname, "setColor"),
	      sblock->TextColor.red,
	      sblock->TextColor.green,
	      sblock->TextColor.blue, sblock->TextColor.alpha);
    }
  if (sblock->HasMaxLength)
    {
      printf ("\t%s(%d);\n", methodcall (tname, "setLength"),
	      sblock->MaxLength);
    }
  if (sblock->HasLayout)
    {
      printf ("\t%s(", methodcall (tname, "align"));
      switch (sblock->Align)
	{
	case 0:
	  printf ("SWFTEXTFIELD_ALIGN_LEFT");
	  break;
	case 1:
	  printf ("SWFTEXTFIELD_ALIGN_RIGHT");
	  break;
	case 2:
	  printf ("SWFTEXTFIELD_ALIGN_CENTER");
	  break;
	case 3:
	  printf ("SWFTEXTFIELD_ALIGN_JUSTIFY");
	  break;
	}
      printf (");\n");
      printf ("\t%s(%d);\n", methodcall (tname, "setLeftMargin"),
	      sblock->LeftMargin);
      printf ("\t%s(%d);\n", methodcall (tname, "setRightMargin"),
	      sblock->RightMargin);
      printf ("\t%s(%d);\n", methodcall (tname, "setIndention"),
	      sblock->Indent);
      printf ("\t%s(%d);\n", methodcall (tname, "setLineSpacing"),
	      sblock->Leading);
    }
  printf ("\t%s('%s');\n", methodcall (tname, "setName"),
	  sblock->VariableName);
  if (sblock->HasText)
    {
      printf ("\t%s('%s');\n", methodcall (tname, "addString"),
	      sblock->InitialText);
    }

}

void
outputSWF_DEFINEFONT (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEFONT);

}

void
outputSWF_DEFINEFONT2 (SWF_Parserstruct * pblock)
{
  char fname[64];
  OUT_BEGIN (SWF_DEFINEFONT2);

  sprintf (fname, "f%d", sblock->FontID);
  printf ("\t%s(\"%s.fdb\" );\n", newobj (fname, "Font"), sblock->FontName);

}

void
outputSWF_DEFINEFONTINFO (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEFONTINFO);

}

void
outputSWF_DEFINELOSSLESS (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINELOSSLESS);

}

void
outputSWF_DEFINELOSSLESS2 (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINELOSSLESS2);

}

void
outputSWF_DEFINEMORPHSHAPE (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEMORPHSHAPE);

}

void
outputSWF_DEFINESHAPE (SWF_Parserstruct * pblock)
{
  char name[8];
  OUT_BEGIN (SWF_DEFINESHAPE);
  sprintf (name, "s%d", sblock->ShapeID);

  printf ("\n\t/*  Shape %d */\n", sblock->ShapeID);
  printf ("\t%s();\n", newobj (name, "Shape"));
  /* There doesn't seem to be a way to use this in the API 
   * it is calculated internal to teh shape object, but I'm not
   * sure it will come up with the same answer.
   outputSWF_RECT(&sblock->ShapeBounds);
   */
  outputSWF_SHAPEWITHSTYLE (&sblock->Shapes, 1, name);

}

void
outputSWF_DEFINESHAPE2 (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINESHAPE2);

}

void
outputSWF_DEFINESHAPE3 (SWF_Parserstruct * pblock)
{
  char name[8];
  OUT_BEGIN (SWF_DEFINESHAPE3);
  sprintf (name, "s%d", sblock->ShapeID);

  printf ("\n\t/*  Shape %d */\n", sblock->ShapeID);
  printf ("\t%s();\n", newobj (name, "Shape"));
  /* There doesn't seem to be a way to use this in the API 
   * it is calculated internal to teh shape object, but I'm not
   * sure it will come up with the same answer.
   outputSWF_RECT(&sblock->ShapeBounds);
   */
  outputSWF_SHAPEWITHSTYLE (&sblock->Shapes, 3, name);

}

void
outputSWF_DEFINESOUND (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINESOUND);

}

void
outputSWF_DEFINESPRITE (SWF_Parserstruct * pblock)
{
  int i;
  OUT_BEGIN (SWF_DEFINESPRITE);

  spritenum = sblock->SpriteId;
  spframenum = 1;
  sprintf(spritename,"sp%d",sblock->SpriteId);
  printf ("\n\t" COMMSTART "  MovieClip %d " COMMEND "\n", sblock->SpriteId);
  printf ("\t%s();  # %d frames\n",
		  newobj (spritename, "MovieClip"), sblock->FrameCount);
  for(i=0;i<sblock->BlockCount;i++) {
	  outputBlock( sblock->tagTypes[i], sblock->Tags[i], 0, 0 );
  }
  spritenum = 0;

}

void
outputSWF_DEFINETEXT (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINETEXT);

}

void
outputSWF_DEFINETEXT2 (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINETEXT2);

}

void
outputSWF_DEFINETEXTFORMAT (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINETEXTFORMAT);

}

void
outputSWF_DEFINEVIDEO (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEVIDEO);

}

void
outputSWF_DEFINEVIDEOSTREAM (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINEVIDEOSTREAM);

}

void
outputSWF_DOACTION (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DOACTION);

  printf ("\t%s(%s(\"\n%s\") );\n", methodcall (spritenum?spritename:"m", "add"),
	  newobj (NULL, "Action"), sblock->AScript);
}

void
outputSWF_ENABLEDEBUGGER (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_ENABLEDEBUGGER);

}

void
outputSWF_END (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_END);

}

void
outputSWF_EXPORTASSETS (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_EXPORTASSETS);

}

void
outputSWF_FONTREF (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_FONTREF);

}

void
outputSWF_FRAMELABEL (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_FRAMELABEL);

  printf ("\t%s(\'%s\');\n",
	  methodcall (spritenum?spritename:"m", "lableFrame"), sblock->Name );

}

void
outputSWF_FRAMETAG (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_FRAMETAG);

}

void
outputSWF_FREEALL (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_FREEALL);

}

void
outputSWF_FREECHARACTER (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_FREECHARACTER);

}

void
outputSWF_GENCOMMAND (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_GENCOMMAND);

}

void
outputSWF_IMPORTASSETS (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_IMPORTASSETS);

}

void
outputSWF_JPEGTABLES (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_JPEGTABLES);

}

void
outputSWF_NAMECHARACTER (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_NAMECHARACTER);

}

void
outputSWF_PATHSAREPOSTSCRIPT (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_PATHSAREPOSTSCRIPT);

}

void
outputSWF_PLACEOBJECT (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_PLACEOBJECT);

}

void
outputSWF_PLACEOBJECT2 (SWF_Parserstruct * pblock)
{
  char cname[64];
  OUT_BEGIN (SWF_PLACEOBJECT2);

  if( sblock->PlaceFlagHasCharacter ) {
      printf(COMMSTART " PlaceFlagHasCharacter " COMMEND "\n");
    sprintf(cname, "s%d", sblock->CharacterId );
    printf ("\t%s(" VAR "%s);\n", methodcall (spritename, "add"),
	      cname);
  }
  if( sblock->PlaceFlagHasMatrix ) {
      printf(COMMSTART " PlaceFlagHasMatrix " COMMEND "\n");
      outputSWF_MATRIX (&sblock->Matrix, spritename);
  }
  if( sblock->PlaceFlagHasColorTransform ) {
      printf(COMMSTART " PlaceFlagHasColorTransform " COMMEND "\n");
  }
  if( sblock->PlaceFlagHasRatio ) {
      printf(COMMSTART " PlaceFlagHasRatio " COMMEND "\n");
  }
  if( sblock->PlaceFlagHasName ) {
      printf(COMMSTART " PlaceFlagHasName " COMMEND "\n");
  }
  if( sblock->PlaceFlagHasClipDepth ) {
      printf(COMMSTART " PlaceFlagHasClipDepth " COMMEND "\n");
  }
  if( sblock->PlaceFlagHasClipActions ) {
      printf(COMMSTART " PlaceFlagHasClipActions " COMMEND "\n");
      outputSWF_CLIPACTIONS (&sblock->ClipActions, spritename);
  }

}

void
outputSWF_PREBUILT (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_PREBUILT);

}

void
outputSWF_PREBUILTCLIP (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_PREBUILTCLIP);

}

void
outputSWF_PROTECT (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_PROTECT);

}

void
outputSWF_REMOVEOBJECT (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_REMOVEOBJECT);
  printf ("\t%s(" VAR "c%d);\n",
	  methodcall (spritenum?spritename:"m", "remove"), sblock->CharacterId);

}

void
outputSWF_REMOVEOBJECT2 (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_REMOVEOBJECT2);
  printf ("\t%s(" VAR "i%d);\n",
	  methodcall (spritenum?spritename:"m", "remove"), sblock->Depth);

}

void
outputSWF_SERIALNUMBER (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_SERIALNUMBER);

}

void
outputSWF_SETBACKGROUNDCOLOR (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_SETBACKGROUNDCOLOR);

  printf ("\t%s(0x%02x, 0x%02x, 0x%02x);\n",
	  methodcall ("m", "setBackground"), sblock->rgb.red,
	  sblock->rgb.green, sblock->rgb.blue);

}

void
outputSWF_SHOWFRAME (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_SHOWFRAME);

  printf ("\t%s();  # end of %sframe %d\n",
	  methodcall (spritenum?spritename:"m", "nextFrame"),
	  spritenum?"clip ":"",
	  spritenum?spframenum++:framenum++);
}

void
outputSWF_SOUNDSTREAMBLOCK (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_SOUNDSTREAMBLOCK);

}

void
outputSWF_SOUNDSTREAMHEAD (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_SOUNDSTREAMHEAD);

}

void
outputSWF_SOUNDSTREAMHEAD2 (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_SOUNDSTREAMHEAD2);

}

void
outputSWF_STARTSOUND (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_STARTSOUND);

}

void
outputSWF_SYNCFRAME (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_SYNCFRAME);

}

void
outputSWF_VIDEOFRAME (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_VIDEOFRAME);

}


void
outputHeader (struct Movie *m)
{
#ifdef SWFPHP
  printf ("<?php\n");
#endif
#ifdef SWFPERL
  printf ("#!/usr/bin/perl -w\n");
  printf
    ("# Generated by swftoperl converter included with ming. Have fun. \n\n");
  printf
    ("# Change this to your needs. If you installed perl-ming global you don't need this.\n");
  printf ("#use lib(\"/home/peter/mystuff/lib/site_perl\");\n\n");

  printf
    ("# We import all because our converter is not so clever to select only needed. ;-)\n");
  printf ("use SWF qw(:ALL);\n");
  printf
    ("# Just copy from a sample, needed to use Constants like SWFFILL_RADIAL_GRADIENT\n");
  printf ("use SWF::Constants qw(:Text :Button :DisplayItem :Fill);\n\n");
  printf ("SWF::setScale(1);\n");
#endif
  printf ("\t%s(%d);\n\n", newobj ("m", "Movie"), m->version);
  printf ("\t%s(%f);\n", methodcall ("m", "setRate"), m->rate);
  printf ("\t%s(%i, %i);\n", methodcall ("m", "setDimension"), m->frame.xMax,
	  m->frame.yMax);
  printf ("\t%s(%i);\n", methodcall ("m", "setFrames"), m->nFrames);
}

void
outputTrailer ()
{
#ifdef SWFPHP
  printf ("\n\theader('Content-type: application/x-shockwave-flash');\n");
#endif
#ifdef SWFPERL
  printf ("\t#print('Content-type: application/x-shockwave-flash\\n\\n');\n");
#endif
#if 0
  printf ("\t$m->output(%i);\n", (compressed) ? '9' : '0');
  printf ("\t$m->save(\"$0.swf\",%i);\n", (compressed) ? '9' : '0');
#endif
#ifdef SWFPHP
  printf ("?>");
#endif
}

void
outputBlock (int type, SWF_Parserstruct * blockp, int offset, int length)
{
  int i;

  if (type < 0 || type > numOutputs)
    return;

  for (i = 0; i < numOutputs; i++)
    {
      if (outputs[i].type == type)
	{
	  return outputs[i].output (blockp);
	}
    }
  return;
}
