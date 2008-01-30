/****************************************************************************
 *
 *  Copyright (C) 2005-2006 "Stuart R. Anderson" <anderson@netsweng.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************/

#include "blocks/blocktypes.h"
#include "action.h"
#include "parser.h"
#include "read.h"
#include "decompile.h"
#include "swfoutput.h"
#include "abctypes.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

extern const char *blockName (SWFBlocktype header);

extern int verbose;

extern struct Movie m;
/*
 * This file contains output functions that can display the different SWF block
 * types in a human readable format.
 */

#define OUT_BEGIN(block) \
	struct block *sblock = (struct block *)pblock;

static struct SWFBlockOutput outputs[] = {
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
  {SWF_DEFINEFONT3, outputSWF_DEFINEFONT3},
  {SWF_DEFINEFONTINFO, outputSWF_DEFINEFONTINFO},
  {SWF_DEFINEFONTINFO2, outputSWF_DEFINEFONTINFO2},
  {SWF_CSMTEXTSETTINGS, outputSWF_CSMTEXTSETTINGS},
  {SWF_DEFINEFONTNAME, outputSWF_DEFINEFONTNAME},
  {SWF_DEFINEFONTALIGNZONES, outputSWF_DEFINEFONTALIGNZONES},
  {SWF_DEFINELOSSLESS, outputSWF_DEFINELOSSLESS},
  {SWF_DEFINELOSSLESS2, outputSWF_DEFINELOSSLESS2},
  {SWF_DEFINEMORPHSHAPE, outputSWF_DEFINEMORPHSHAPE},
  {SWF_DEFINEMORPHSHAPE2, outputSWF_DEFINEMORPHSHAPE2},
  {SWF_DEFINESHAPE, outputSWF_DEFINESHAPE},
  {SWF_DEFINESHAPE2, outputSWF_DEFINESHAPE2},
  {SWF_DEFINESHAPE3, outputSWF_DEFINESHAPE3},
  {SWF_DEFINESHAPE4, outputSWF_DEFINESHAPE4},
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
  {SWF_IMPORTASSETS2, outputSWF_IMPORTASSETS2},
  {SWF_JPEGTABLES, outputSWF_JPEGTABLES},
  {SWF_NAMECHARACTER, outputSWF_NAMECHARACTER},
  {SWF_PATHSAREPOSTSCRIPT, outputSWF_PATHSAREPOSTSCRIPT},
  {SWF_PLACEOBJECT, outputSWF_PLACEOBJECT},
  {SWF_PLACEOBJECT2, outputSWF_PLACEOBJECT2},
  {SWF_PLACEOBJECT3, outputSWF_PLACEOBJECT3},
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
  {SWF_STARTSOUND2, outputSWF_STARTSOUND2},
  {SWF_SYNCFRAME, outputSWF_SYNCFRAME},
  {SWF_INITACTION, outputSWF_INITACTION},
  {SWF_VIDEOFRAME, outputSWF_VIDEOFRAME},
  {SWF_REFLEX, outputSWF_REFLEX},
  {SWF_FILEATTRIBUTES, outputSWF_FILEATTRIBUTES},
  {SWF_METADATA, outputSWF_METADATA},
  {SWF_SCRIPTLIMITS, outputSWF_SCRIPTLIMITS},
  {SWF_DEFINESCALINGGRID, outputSWF_DEFINESCALINGGRID},
  {SWF_SETTABINDEX, outputSWF_SETTABINDEX},
  {SWF_DOABC, outputSWF_DOABC},
  {SWF_SYMBOLCLASS, outputSWF_SYMBOLCLASS},
  {SWF_DEFINESCENEANDFRAMEDATA, outputSWF_DEFINESCENEANDFRAMEDATA},
};

static int numOutputs = sizeof (outputs) / sizeof (struct SWFBlockOutput);

/* Indented output function */

static unsigned INDENT=0;

static void
iprintf(const char* fmt, ...)
{
	va_list ap;
	unsigned ii=INDENT;

	/* do intenting */
	while(ii--) printf("  ");

	va_start (ap, fmt);
	vprintf(fmt, ap);
	return;
}

/* Output basic Flash Types */

void
outputSWF_RGBA (SWF_RGBA * rgb, char *pname)
{
  iprintf (" RGBA: (");
  iprintf ("%2x,", rgb->red);
  iprintf ("%2x,", rgb->green);
  iprintf ("%2x,", rgb->blue);
  iprintf ("%2x)\n", rgb->alpha);
}

void
outputSWF_RECT (SWF_RECT * rect)
{
  iprintf (" RECT: ");
  iprintf (" (%ld,", rect->Xmin);
  iprintf ("%ld)x", rect->Ymin);
  iprintf ("(%ld,", rect->Xmax);
  iprintf ("%ld)", rect->Ymax);
  iprintf (":%d\n", rect->Nbits);
}

void
outputSWF_MATRIX (SWF_MATRIX * matrix, char *name)
{
  iprintf ("  Matrix:\n");
  if (matrix->HasScale)
    {
      iprintf ("   ScaleX %f ", matrix->ScaleX);
      iprintf ("ScaleY %f\n", matrix->ScaleY);
    }
  if (matrix->HasRotate)
    {
      iprintf ("   RotateSkew0 %f ", matrix->RotateSkew0);
      iprintf ("RotateSkew1 %f\n", matrix->RotateSkew1);
    }
  iprintf ("   TranslateX %6ld ", matrix->TranslateX);
  iprintf ("TranslateY %6ld\n", matrix->TranslateY);
}

void
outputSWF_CXFORM(SWF_CXFORM * cxform){
	iprintf("  ColorTransForm:\n");
	if (cxform->HasMultTerms){
		iprintf("   Mult:");
		iprintf("%2x,", cxform->RedMultTerm);
		iprintf("%2x,", cxform->GreenMultTerm);
		iprintf("%2x,", cxform->BlueMultTerm);
		/*
		if (cxform->hasAlpha){
			iprintf("%2x", cxform->AlphaMultTerm);	
		}
		*/
		iprintf("\n");
	}
	
	if (cxform->HasAddTerms){
		iprintf("   Add:");	
		iprintf("%2x,", cxform->RedAddTerm);
		iprintf("%2x,", cxform->GreenAddTerm);
		iprintf("%2x,", cxform->BlueAddTerm);
		/*
		if (cxform->hasAlpha){
			iprintf("%2x", cxform->AlphaAddTerm);	
		}
		*/
		iprintf("\n");
	}
}

/* alpha could be handled in SWF_CXFORM / outputSWF_CXFORM too
*  or is there a reason to make 
* 2 parsefunctions /
* 2 cxform structures
* 2 outputfunctions
* for that?
*/
void
outputSWF_CXFORMWITHALPHA(SWF_CXFORMWITHALPHA * cxform, char *name){
	iprintf("  ColorTransForm:\n");
	if (cxform->HasMultTerms){
		iprintf("   Mult:");
		iprintf("%2x,", cxform->RedMultTerm);
		iprintf("%2x,", cxform->GreenMultTerm);
		iprintf("%2x,", cxform->BlueMultTerm);
		iprintf("%2x",  cxform->AlphaMultTerm);
		iprintf("\n");
	}
	if (cxform->HasAddTerms){
		iprintf("   Add:");
		iprintf("%2x,", cxform->RedAddTerm);
		iprintf("%2x,", cxform->GreenAddTerm);
		iprintf("%2x,", cxform->BlueAddTerm);
		iprintf("%2x",  cxform->AlphaAddTerm);
		iprintf("\n");
	}	
}

void 
outputSWF_FILTER(SWF_FILTER *filter);

void
outputSWF_BUTTONRECORD (SWF_BUTTONRECORD *brec)
{
  iprintf (" BUTTONRECORD: ");
  iprintf ("  ButtonHasBlendMode %d ", brec->ButtonHasBlendMode);
  iprintf ("  ButtonHasFilterList %d ", brec->ButtonHasFilterList);
  iprintf ("  ButtonStateHitTest: %d ", brec->ButtonStateHitTest);
  iprintf ("  ButtonStateDown: %d ", brec->ButtonStateDown);
  iprintf ("  ButtonStateOver: %d ", brec->ButtonStateOver);
  iprintf ("  ButtonStateUp: %d\n", brec->ButtonStateUp);
  iprintf ("  CharacterID: %d\n", brec->CharacterId);
  iprintf ("  PlaceDepth: %d\n", brec->PlaceDepth);

	outputSWF_MATRIX(&brec->PlaceMatrix,"");
	outputSWF_CXFORMWITHALPHA(&brec->ColorTransform,"");
  if( brec->ButtonHasBlendMode )
	  iprintf("  BlendMode %d\n", brec->BlendMode );
  if( brec->ButtonHasFilterList )
  {
	  int i;
	  SWF_FILTERLIST *filterList = &brec->FilterList;
	  
	  iprintf("  NumberOfFilters %d\n", filterList->NumberOfFilters);
	  
	  for(i = 0; i < filterList->NumberOfFilters; i++)
	    outputSWF_FILTER(filterList->Filter + i);
  }
}

void
outputSWF_BUTTONCONDACTION (SWF_BUTTONCONDACTION *bcarec)
{
#ifdef NODECOMPILE
  int i;
#endif
#if !defined(ACTIONONLY)
  iprintf (" BUTTONCONDACTION: ");
  iprintf ("  CondActionSize: %d\n", bcarec->CondActionSize);
  iprintf ("  CondIdleToOverDown: %d ", bcarec->CondIdleToOverDown);
  iprintf ("  CondOutDownToIdle: %d ", bcarec->CondOutDownToIdle);
  iprintf ("  CondOutDownToOverDown: %d ", bcarec->CondOutDownToOverDown);
  iprintf ("  CondOverDownToOutDown: %d ", bcarec->CondOverDownToOutDown);
  iprintf ("  CondOverDownToOverUp: %d ", bcarec->CondOverDownToOverUp);
  iprintf ("  CondOverUpToOverDown: %d ", bcarec->CondOverUpToOverDown);
  iprintf ("  CondOverUpToIdle: %d ", bcarec->CondOverUpToIdle);
  iprintf ("  CondIdleToOverUp: %d ", bcarec->CondIdleToOverUp);
  iprintf ("  CondKeyPress: %d ", bcarec->CondKeyPress);
  iprintf ("  CondOverDownToIdle: %d ", bcarec->CondOverDownToIdle);
  iprintf ("\n");
#endif
#ifdef NODECOMPILE
  iprintf(" %d Actions\n", bcarec->numActions);
  for(i=0;i<bcarec->numActions;i++)
  outputSWF_ACTION(i,&(bcarec->Actions[i]));
#else
  iprintf (" %s\n", decompile5Action(bcarec->numActions,bcarec->Actions,0));
#endif
}

void
outputSWF_CLIPEVENTFLAGS (SWF_CLIPEVENTFLAGS * clipevflags )
{
  if ( clipevflags->ClipEventKeyUp ) printf (" ClipEventKeyUp");
  if ( clipevflags->ClipEventKeyDown ) printf (" ClipEventKeyDown");
  if ( clipevflags->ClipEventMouseUp ) printf (" ClipEventMouseUp");
  if ( clipevflags->ClipEventMouseDown ) printf (" ClipEventMouseDown");
  if ( clipevflags->ClipEventMouseMove ) printf (" ClipEventMouseMove");
  if ( clipevflags->ClipEventUnload ) printf (" ClipEventUnload");
  if ( clipevflags->ClipEventEnterFrame ) printf (" ClipEventEnterFrame");
  if ( clipevflags->ClipEventLoad ) printf (" ClipEventLoad");
  if ( clipevflags->ClipEventDragOver ) printf (" ClipEventDragOver");
  if ( clipevflags->ClipEventRollOut ) printf (" ClipEventRollOut");
  if ( clipevflags->ClipEventRollOver ) printf (" ClipEventRollOver");
  if ( clipevflags->ClipEventReleaseOutside ) printf (" ClipEventReleaseOutside");
  if ( clipevflags->ClipEventRelease ) iprintf (" ClipEventRelease");
  if ( clipevflags->ClipEventPress ) iprintf (" ClipEventPress");
  if ( clipevflags->ClipEventInitialize ) iprintf (" ClipEventInitialize");
  if ( clipevflags->ClipEventData ) iprintf (" ClipEventData");
  if ( clipevflags->ClipEventConstruct ) iprintf (" ClipEventConstruct");
  if ( clipevflags->ClipEventKeyPress ) iprintf (" ClipEventKeyPress");
  if ( clipevflags->ClipEventDragOut ) iprintf (" ClipEventDragOut");
}

void
outputSWF_CLIPACTIONRECORD (SWF_CLIPACTIONRECORD * carec )
{
#ifdef NODECOMPILE
  int i;
#endif
#if !defined(ACTIONONLY)
  iprintf(" onClipEvents("); outputSWF_CLIPEVENTFLAGS (&carec->EventFlag); printf(" ):\n");
  /*iprintf(" ActionRecordSize %ld\n", carec->ActionRecordSize);*/
  if ( carec->KeyCode) iprintf(" EventKeyCode %d\n", carec->KeyCode);
#endif
#ifdef NODECOMPILE
  ++INDENT;
  /*iprintf(" %d Actions\n", carec->numActions);*/
  for(i=0;i<carec->numActions;i++)
     outputSWF_ACTION(i,&(carec->Actions[i]));
  --INDENT;
#else
  ++INDENT;
  iprintf (" %s\n", decompile5Action(carec->numActions,carec->Actions,0));
  --INDENT;
#endif
}

void
outputSWF_CLIPACTIONS (SWF_CLIPACTIONS * clipactions )
{
  int i;
  for(i=0;i<clipactions->NumClipRecords-1;i++)
    outputSWF_CLIPACTIONRECORD(&(clipactions->ClipActionRecords[i]));
}

void
outputSWF_GRADIENTRECORD (SWF_GRADIENTRECORD * gradientrec, char *gname)
{
  iprintf (" Ratio: %d\n", gradientrec->Ratio);
  outputSWF_RGBA (&gradientrec->Color, "");
}

void
outputSWF_MORPHGRADIENTRECORD (SWF_MORPHGRADIENTRECORD * gradientrec, 
                               char *gname)
{
  iprintf (" StartRatio: %d\n", gradientrec->StartRatio);
  outputSWF_RGBA (&gradientrec->StartColor, "");
  iprintf (" EndRatio: %d\n", gradientrec->EndRatio);
  outputSWF_RGBA (&gradientrec->EndColor, "");
}

void 
outputFIXED(FIXED fixed, const char *prefix)
{
	float f;
	
	f = fixed * 1.0 / (1<<16);
	iprintf("%s%f\n", prefix, f);
}

void 
outputFIXED8(FIXED fixed, const char *prefix)
{
	float f;
	
	f = fixed * 1.0 / (1<<8);
	iprintf("%s%f\n", prefix, f);
}

void
outputSWF_FOCALGRADIENT (SWF_FOCALGRADIENT * gradient, char *name)
{
  int i;
  iprintf (" Gradient: ");
  iprintf (" SpreadMode: %d\n", gradient->SpreadMode);
  iprintf (" InterpolationMode: %d\n", gradient->InterpolationMode);
  iprintf (" NumGradients: %d\n", gradient->NumGradients);
  for (i = 0; i < gradient->NumGradients; i++)
    outputSWF_GRADIENTRECORD (&(gradient->GradientRecords[i]),"");
  outputFIXED8(gradient->FocalPoint, "  FocalPoint: ");
}

void
outputSWF_GRADIENT (SWF_GRADIENT * gradient, char *name)
{
  int i;
  iprintf (" Gradient: ");
  iprintf (" SpreadMode: %d\n", gradient->SpreadMode);
  iprintf (" InterpolationMode: %d\n", gradient->InterpolationMode);
  iprintf (" NumGradients: %d\n", gradient->NumGradients);
  for (i = 0; i < gradient->NumGradients; i++)
    outputSWF_GRADIENTRECORD (&(gradient->GradientRecords[i]),"");
}

void
outputSWF_MORPHGRADIENT (SWF_MORPHGRADIENT * gradient, char *name)
{
  int i;
  iprintf (" MorphGradient: ");
  iprintf (" NumGradients: %d\n", gradient->NumGradients);
  for (i = 0; i < gradient->NumGradients; i++)
    outputSWF_MORPHGRADIENTRECORD (&(gradient->GradientRecords[i]),"");
}


void
outputSWF_FILLSTYLE (SWF_FILLSTYLE * fillstyle, char *name, int i)
{
  iprintf (" FillStyle: ");
  iprintf (" FillStyleType: %x\n", fillstyle->FillStyleType);
  switch (fillstyle->FillStyleType)
    {
    case 0x00:			/* Solid Fill */
      outputSWF_RGBA (&fillstyle->Color, "");
      break;
    case 0x10:			/* Linear Gradient Fill */
    case 0x12:			/* Radial Gradient Fill */
      outputSWF_MATRIX (&fillstyle->GradientMatrix,"");
      outputSWF_GRADIENT (&fillstyle->Gradient,"");
      break;
    case 0x13:
      outputSWF_MATRIX (&fillstyle->GradientMatrix,"");
      outputSWF_FOCALGRADIENT(&fillstyle->FocalGradient, "");
    case 0x40:			/* Repeating Bitmap Fill */
    case 0x41:			/* Clipped Bitmap Fill */
    case 0x42:			/* Non-smoothed Repeating Bitmap Fill */
    case 0x43:			/* Non-smoothed Clipped Bitmap Fill */
      iprintf (" BitmapID: %d\n", fillstyle->BitmapId);
      outputSWF_MATRIX (&fillstyle->BitmapMatrix,"");
      break;
    }
}

void
outputSWF_FILLSTYLEARRAY (SWF_FILLSTYLEARRAY * fillstylearray, char *name)
{
  int count, i;

  iprintf (" FillStyleArray: ");
  iprintf (" FillStyleCount: %6d ", fillstylearray->FillStyleCount);
  iprintf (" FillStyleCountExtended: %6d\n",
	  fillstylearray->FillStyleCountExtended);
  count =
    (fillstylearray->FillStyleCount !=
     0xff) ? fillstylearray->FillStyleCount : fillstylearray->
    FillStyleCountExtended;
  for (i = 0; i < count; i++)
    {
      outputSWF_FILLSTYLE (&(fillstylearray->FillStyles[i]),"",0);
    }
}

void
outputSWF_MORPHFILLSTYLE (SWF_MORPHFILLSTYLE * fillstyle, char *name, 
                          int i)
{
  iprintf (" MorphFillStyle: ");
  iprintf (" FillStyleType: %x\n", fillstyle->FillStyleType);
  switch (fillstyle->FillStyleType)
    {
    case 0x00:			/* Solid Fill */
      outputSWF_RGBA (&fillstyle->StartColor, "");
      outputSWF_RGBA (&fillstyle->EndColor, "");
      break;
    case 0x10:			/* Linear Gradient Fill */
    case 0x12:			/* Radial Gradient Fill */
      outputSWF_MATRIX (&fillstyle->StartGradientMatrix,"");
      outputSWF_MATRIX (&fillstyle->EndGradientMatrix,"");
      outputSWF_MORPHGRADIENT (&fillstyle->Gradient,"");
      break;
    case 0x40:			/* Repeating Bitmap Fill */
    case 0x41:			/* Clipped Bitmap Fill */
    case 0x42:			/* Non-smoothed Repeating Bitmap Fill */
    case 0x43:			/* Non-smoothed Clipped Bitmap Fill */
      iprintf (" BitmapID: %d\n", fillstyle->BitmapId);
      outputSWF_MATRIX (&fillstyle->StartBitmapMatrix,"");
      outputSWF_MATRIX (&fillstyle->EndBitmapMatrix,"");
      break;
    }
}

void
outputSWF_MORPHFILLSTYLES( SWF_MORPHFILLSTYLES *fillstylearray)
{
  int count, i;

  if( !verbose ) 
	return;
  iprintf (" MorphFillStyleArray: ");
  iprintf (" FillStyleCount: %6d ", fillstylearray->FillStyleCount);
  iprintf (" FillStyleCountExtended: %6d\n",
          fillstylearray->FillStyleCountExtended);
  count =
    (fillstylearray->FillStyleCount !=
     0xff) ? fillstylearray->FillStyleCount : fillstylearray->
    FillStyleCountExtended;
  for (i = 0; i < count; i++)
    { 
      outputSWF_MORPHFILLSTYLE (&(fillstylearray->FillStyles[i]),"",0);
    }
}


void
outputSWF_LINESTYLE (SWF_LINESTYLE * fillstyle, char *name, int i)
{
  iprintf (" LineStyle: ");
  iprintf (" Width: %d\n", fillstyle->Width);
  outputSWF_RGBA (&fillstyle->Color, "");
}

void
outputSWF_LINESTYLE2 (SWF_LINESTYLE2 * fillstyle, char *name, int i)
{
  iprintf (" LineStyle2: ");
  iprintf (" Width: %d\n", fillstyle->Width);
  iprintf (" StartCapStyle: %d\n", fillstyle->StartCapStyle);
  iprintf (" JoinStyle: %d\n", fillstyle->JoinStyle);
  iprintf (" HasFillFlag: %d\n", fillstyle->HasFillFlag);
  iprintf (" NoHScaleFlag: %d\n", fillstyle->NoHScaleFlag);
  iprintf (" NoVScaleFlag: %d\n", fillstyle->NoVScaleFlag);
  iprintf (" PixelHintingFlag %d\n", fillstyle->PixelHintingFlag);
  iprintf (" NoClose %d\n", fillstyle->NoClose);
  iprintf (" EndCapStyle %d\n", fillstyle->EndCapStyle);
  if(fillstyle->JoinStyle == 2)
    iprintf (" MiterLimitFactor %d\n", fillstyle->MiterLimitFactor);
  if(fillstyle->HasFillFlag == 0)
    outputSWF_RGBA (&fillstyle->Color, "");
  else
    outputSWF_FILLSTYLE (&fillstyle->FillType, "", 0);
}

void
outputSWF_LINESTYLEARRAY (SWF_LINESTYLEARRAY * linestylearray, char *name)
{

  int count, i;

  iprintf (" LineStyleArray: ");
  iprintf (" LineStyleCount: %6d ", linestylearray->LineStyleCount);
  iprintf (" LineStyleCountExtended: %6d\n",
	  linestylearray->LineStyleCountExtended);
  count =
    (linestylearray->LineStyleCount !=
     0xff) ? linestylearray->LineStyleCount : linestylearray->
    LineStyleCountExtended;
  for (i = 0; i < count; i++)
  {
    if(linestylearray->LineStyles != NULL)   
      outputSWF_LINESTYLE (&(linestylearray->LineStyles[i]),"",0);
    else if(linestylearray->LineStyles2 != NULL)
      outputSWF_LINESTYLE2 (&(linestylearray->LineStyles2[i]),"",0);
    else
      iprintf("LineStyleArray: parser error\n");
  }
}

void
outputSWF_MORPHLINESTYLE (SWF_MORPHLINESTYLE * linestyle, char *name)
{
  iprintf (" MorphLineStyle: ");
  iprintf (" StartWidth: %d\n", linestyle->StartWidth);
  iprintf (" EndWidth: %d\n", linestyle->EndWidth);
  outputSWF_RGBA (&linestyle->StartColor, "");
  outputSWF_RGBA (&linestyle->EndColor, "");
}

void
outputSWF_MORPHLINESTYLE2 (SWF_MORPHLINESTYLE2 * linestyle, char *name)
{
  iprintf (" MorphLineStyle2: ");
  iprintf (" StartWidth: %d\n", linestyle->StartWidth);
  iprintf (" EndWidth: %d\n", linestyle->EndWidth);
  iprintf (" StartCapStyle: %d\n", linestyle->StartCapStyle);
  iprintf (" JoinStyle: %d\n", linestyle->JoinStyle);
  iprintf (" HasFillFlag: %d\n", linestyle->HasFillFlag);
  iprintf (" NoHScaleFlag: %d\n", linestyle->NoHScaleFlag);
  iprintf (" NoVScaleFlag: %d\n", linestyle->NoVScaleFlag);
  iprintf (" PixelHintingFlag %d\n", linestyle->PixelHintingFlag);
  iprintf (" NoClose %d\n", linestyle->NoClose);
  iprintf (" EndCapStyle %d\n", linestyle->EndCapStyle);
  if(linestyle->JoinStyle == 2)
    iprintf (" MiterLimitFactor %d\n", linestyle->MiterLimitFactor);
  if(linestyle->HasFillFlag == 0) {
    outputSWF_RGBA (&linestyle->StartColor, "");
    outputSWF_RGBA (&linestyle->EndColor, "");
  }
  else
    outputSWF_MORPHFILLSTYLE (&linestyle->FillType, "", 0);
}

void
outputSWF_MORPHLINESTYLES (SWF_MORPHLINESTYLES * linestylearray)
{

  int count, i;

  if( !verbose ) return;
  iprintf (" MorphLineStyleArray: ");
  iprintf (" LineStyleCount: %6d ", linestylearray->LineStyleCount);
  iprintf (" LineStyleCountExtended: %6d\n",
	  linestylearray->LineStyleCountExtended);
  count =
    (linestylearray->LineStyleCount !=
     0xff) ? linestylearray->LineStyleCount : linestylearray->
    LineStyleCountExtended;
  for (i = 0; i < count; i++)
  {
    if(linestylearray->LineStyles != NULL)   
      outputSWF_MORPHLINESTYLE (&(linestylearray->LineStyles[i]),"");
    else if(linestylearray->LineStyles2 != NULL)
      outputSWF_MORPHLINESTYLE2 (&(linestylearray->LineStyles2[i]),"");
    else
      iprintf("LineStyleArray: parser error\n");
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
	  iprintf (" Straight EdgeRecord: (%d)",
		  shaperec->StraightEdge.NumBits);
	  if( shaperec->StraightEdge.GeneralLineFlag ) {
		  iprintf(" - (%ld, %ld)\n",shaperec->StraightEdge.DeltaX,shaperec->StraightEdge.DeltaY);
	  } else {
	  	if( shaperec->StraightEdge.VertLineFlag ) 
		  iprintf(" - (0, %ld)\n",shaperec->StraightEdge.VLDeltaY);
		else
		  iprintf(" - (%ld, 0)\n",shaperec->StraightEdge.VLDeltaX);
	  }
	}
      else
	{
	  /* A Curved Edge Record */
	  iprintf (" Curved EdgeRecord: %d", shaperec->CurvedEdge.NumBits);
	  iprintf (" Control(%ld,%ld)", shaperec->CurvedEdge.ControlDeltaX,
		  shaperec->CurvedEdge.ControlDeltaY);
	  iprintf (" Anchor(%ld,%ld)\n", shaperec->CurvedEdge.AnchorDeltaX,
		  shaperec->CurvedEdge.AnchorDeltaY);
	}
    }
  else
    {
      /* A Non-Edge Record */
      if (shaperec->EndShape.EndOfShape == 0)
	{
	  iprintf ("  ENDSHAPE\n");
	  return;
	}
      iprintf (" StyleChangeRecord:\n");
      iprintf ("  StateNewStyles: %d", shaperec->StyleChange.StateNewStyles);
      iprintf (" StateLineStyle: %d ", shaperec->StyleChange.StateLineStyle);
      iprintf (" StateFillStyle1: %d\n",
	      shaperec->StyleChange.StateFillStyle1);
      iprintf ("  StateFillStyle0: %d",
	      shaperec->StyleChange.StateFillStyle0);
      iprintf (" StateMoveTo: %d\n", shaperec->StyleChange.StateMoveTo);

      if (shaperec->StyleChange.StateLineStyle) {
	  iprintf ("   LineStyle: %ld\n", shaperec->StyleChange.LineStyle);
      }
      if (shaperec->StyleChange.StateFillStyle1) {
	  iprintf ("   FillStyle1: %ld\n", shaperec->StyleChange.FillStyle1);
      }
      if (shaperec->StyleChange.StateFillStyle0) {
	  iprintf ("   FillStyle0: %ld\n", shaperec->StyleChange.FillStyle0);
      }
      if (shaperec->StyleChange.StateMoveTo)
	{
	  iprintf ("   MoveBits: %d ", shaperec->StyleChange.MoveBits);
	  iprintf (" MoveDeltaX: %ld ", shaperec->StyleChange.MoveDeltaX);
	  iprintf (" MoveDeltaY: %ld\n", shaperec->StyleChange.MoveDeltaY);
	}
    }
}

void
outputSWF_SHAPE (SWF_SHAPE * shape, char *name)
{
  int i;
  iprintf (" %s\n", name );
  iprintf (" NumFillBits: %d\n", shape->NumFillBits);
  iprintf (" NumLineBits: %d\n", shape->NumLineBits);
  for (i = 0; i < shape->NumShapeRecords; i++)
    {
      outputSWF_SHAPERECORD (&(shape->ShapeRecords[i]), name);
    }
}

void
outputSWF_SHAPEWITHSTYLE (SWF_SHAPEWITHSTYLE * shape, int level, char *name)
{
  int i;

  outputSWF_FILLSTYLEARRAY (&(shape->FillStyles),"");
  outputSWF_LINESTYLEARRAY (&(shape->LineStyles),"");
  iprintf (" NumFillBits: %d\n", shape->NumFillBits);
  iprintf (" NumLineBits: %d\n", shape->NumLineBits);
  for (i = 0; i < shape->NumShapeRecords; i++)
    {
      outputSWF_SHAPERECORD (&(shape->ShapeRecords[i]),name);
    }
}

void
outputSWF_GLYPHENTRY (SWF_GLYPHENTRY *gerec)
{
	iprintf("   GlyphIndex[0] = %4.4lx ", gerec->GlyphIndex[0] );
	iprintf("   GlyphAdvance[0] = %4.4lx\n", gerec->GlyphAdvance[0] );
}

void
outputSWF_TEXTRECORD (SWF_TEXTRECORD *trec, int level)
{
  int i;
  iprintf (" TEXTRECORD: ");
  iprintf ("  TextRecordType: %d ", trec->TextRecordType);
  iprintf ("  StyleFlagsReserved: %d ", trec->StyleFlagsReserved);
  iprintf ("  StyleFlagHasFont: %d ", trec->StyleFlagHasFont);
  iprintf ("  StyleFlagHasColor: %d ", trec->StyleFlagHasColor);
  iprintf ("  StyleFlagHasYOffset: %d ", trec->StyleFlagHasYOffset);
  iprintf ("  StyleFlagHasXOffset: %d\n", trec->StyleFlagHasXOffset);

  if ( trec->TextRecordType == 0 )
  {
  	/*
	 * parser doesn't initialize any other
	 * member when TextRecordType == 0,
	 * see parseSWF_TEXTRECORD in parser.c
	 */
  	return;
  }

  if( trec->StyleFlagHasFont )
    iprintf ("  FontID: %d\n", trec->FontID);
  if( trec->StyleFlagHasColor ) {
    outputSWF_RGBA(&trec->TextColor, "" );
  }
  if( trec->StyleFlagHasYOffset || trec->StyleFlagHasXOffset ) {
    iprintf ("  XOffset: %d ", trec->XOffset);
    iprintf ("  YOffset: %d\n", trec->YOffset);
  }
  if( trec->StyleFlagHasFont )
    iprintf ("  TextHeight: %d\n", trec->TextHeight);
  iprintf ("  GlyphCount: %d\n", trec->GlyphCount);
  for(i=0;i<trec->GlyphCount;i++)
	  outputSWF_GLYPHENTRY( &(trec->GlyphEntries[i]) );
}

void 
outputSWF_BLURFILTER(SWF_BLURFILTER *filter)
{
	outputFIXED(filter->BlurX, "    BlurX: ");
        outputFIXED(filter->BlurY, "    BlurY: ");
	iprintf("    Passes %d\n", filter->Passes);
}

void 
outputSWF_BEVELFILTER(SWF_BEVELFILTER *filter)
{
	outputSWF_RGBA (&filter->ShadowColor, "    ShadowColor:");
	outputSWF_RGBA (&filter->HighlightColor, "    HighLightColor:");
	outputFIXED(filter->BlurX, "    BlurX: ");
        outputFIXED(filter->BlurY, "    BlurY: ");
        outputFIXED(filter->Angle, "    Angle: ");
        outputFIXED(filter->Distance, "    Distance: ");
        outputFIXED8(filter->Strength, "    Strength: ");
        iprintf("    InnerShadow: %d\n", filter->InnerShadow);
        iprintf("    Kockout %d\n", filter->Kockout);
        iprintf("    CompositeSource %d\n", filter->CompositeSource);
        iprintf("    OnTop: %d\n", filter->OnTop);
        iprintf("    Passes %d\n", filter->Passes);
}

void
outputSWF_GRADIENTFILTER(SWF_GRADIENTFILTER *filter)
{
	int i;
	iprintf("    NumColor %d\n", filter->NumColors);
	for(i = 0; i < filter->NumColors; i++)
	{
		outputSWF_RGBA (filter->GradientColors + i, "    ");
		iprintf("    Ratio: %d\n", filter->GradientRatio[i]);
	}
	outputFIXED(filter->BlurX, "    BlurX: ");
        outputFIXED(filter->BlurY, "    BlurY: ");
        outputFIXED(filter->Angle, "    Angle: ");
        outputFIXED(filter->Distance, "    Distance: ");
        outputFIXED8(filter->Strength, "    Strength: ");
	iprintf("    InnerShadow: %d\n", filter->InnerShadow);
        iprintf("    Kockout %d\n", filter->Kockout);
        iprintf("    CompositeSource %d\n", filter->CompositeSource);
	iprintf("    OnTop: %d\n", filter->OnTop);
        iprintf("    Passes %d\n", filter->Passes);
}

void 
outputSWF_DROPSHADOWFILTER(SWF_DROPSHADOWFILTER *filter)
{
	outputSWF_RGBA (&filter->DropShadowColor, "    DropShadowColor:");
	outputFIXED(filter->BlurX, "    BlurX: ");
	outputFIXED(filter->BlurY, "    BlurY: ");
	outputFIXED(filter->Angle, "    Angle: ");
	outputFIXED(filter->Distance, "    Distance: ");
	outputFIXED8(filter->Strength, "    Strength: ");
	iprintf("    InnerShadow: %d\n", filter->InnerShadow);
	iprintf("    Kockout %d\n", filter->Kockout);
	iprintf("    CompositeSource %d\n", filter->CompositeSource);
	iprintf("    Passes %d\n", filter->Passes);
}

void 
outputSWF_GLOWFILTER(SWF_GLOWFILTER *filter)
{
	outputSWF_RGBA (&filter->GlowColor, "");
	outputFIXED(filter->BlurX, "    BlurX: ");
	outputFIXED(filter->BlurY, "    BlurY: ");
	outputFIXED8(filter->Strength, "    Strength: ");
	iprintf("    InnerGlow: %d\n", filter->InnerGlow);
	iprintf("    Kockout %d\n", filter->Kockout);
	iprintf("    CompositeSource %d\n", filter->CompositeSource);
	iprintf("    Passes %d\n", filter->Passes);
}

void 
outputSWF_CONVOLUTIONFILTER(SWF_CONVOLUTIONFILTER *filter)
{
	int y, x;

	iprintf("    Matrix %dx%d\n", filter->MatrixX, filter->MatrixY);
	iprintf("      Bias %f, Divisor %f\n", filter->Bias, filter->Divisor);
	for(y = 0; y < filter->MatrixY; y++)
	{
		iprintf("    ");
		for(x = 0; x < filter->MatrixX; x++)
		{
			FLOAT val = filter->Matrix[y * filter->MatrixX + x];
			iprintf("%f ", val);
		}
		iprintf("\n");
	}
	outputSWF_RGBA (&filter->DefaultColor, "     efault Color: ");
	iprintf("    Clamp: %d\n", filter->Clamp);
	iprintf("    PreserveAlpha: %d\n", filter->PreserveAlpha);
}

void 
outputSWF_COLORMATRIXFILTER(SWF_COLORMATRIXFILTER *filter)
{
	int y, x;

	for(y = 0; y < 4; y++)
        {
                iprintf("    ");
                for(x = 0; x < 5; x++)
                {
                        FLOAT val = filter->Matrix[y * 5 + x];
                        iprintf("%f ", val);
                }
                iprintf("\n");
        }
}

void 
outputSWF_FILTER(SWF_FILTER *filter)
{
	switch(filter->FilterId)
	{
		case FILTER_DROPSHADOW:
			iprintf("  Filter: DropShadow\n");
			outputSWF_DROPSHADOWFILTER(&filter->filter.dropShadow);
			break;
		case FILTER_BLUR:
			iprintf("  Filter: Blur\n");
			outputSWF_BLURFILTER(&filter->filter.blur);
			break;
		case FILTER_GLOW:
			iprintf("  Filter: Glow\n");
			outputSWF_GLOWFILTER(&filter->filter.glow);
			break;
		case FILTER_BEVEL:
			iprintf("  Filter: Bevel\n");
			outputSWF_BEVELFILTER(&filter->filter.bevel);
			break;
		case FILTER_GRADIENTGLOW:
			iprintf("  Filter: GradientGlow\n");
			outputSWF_GRADIENTFILTER(&filter->filter.gradientGlow);
			break;
		case FILTER_CONVOLUTION:
			iprintf("  Filter: Convolution\n");
			outputSWF_CONVOLUTIONFILTER(&filter->filter.convolution);
			break;
		case FILTER_COLORMATRIX:
			iprintf("  Filter: ColorMatrix\n");
			outputSWF_COLORMATRIXFILTER(&filter->filter.colorMatrix);
			break;
		case FILTER_GRADIENTBEVEL:
			iprintf("  Filter: GradientBevel\n");
			outputSWF_GRADIENTFILTER(&filter->filter.gradientBevel);
			break;
		default:
			iprintf("  Filter: Unknown %d\n", filter->FilterId);
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
  OUT_BEGIN (SWF_DEFINEBITS);
  iprintf(" CharacterID: %d\n", sblock->CharacterID);
}

void
outputSWF_DEFINEBITSJPEG2 (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINEBITSJPEG2);
  iprintf(" CharacterID: %d\n", sblock->CharacterID);
}

void
outputSWF_DEFINEBITSJPEG3 (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINEBITSJPEG3);
  iprintf(" CharacterID: %d\n", sblock->CharacterID);
  iprintf(" AlphaDataOffset %d\n", sblock->AlphaDataOffset);
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
  int i;
  OUT_BEGIN (SWF_DEFINEBUTTON2);

#if !defined(ACTIONONLY)
  iprintf (" CharacterID: %d\n", sblock->Buttonid);
  iprintf (" TrackAsMenu: %d\n", sblock->TrackAsMenu);
  iprintf (" ActionOffset: %d\n", sblock->ActionOffset);
  for(i=0;i<sblock->numCharacters;i++) {
	  outputSWF_BUTTONRECORD( &(sblock->Characters[i]) );
  }
#endif
  for(i=0;i<sblock->numActions;i++) {
	  outputSWF_BUTTONCONDACTION( &(sblock->Actions[i]) );
  }

}

void
outputSWF_DEFINEBUTTONCXFORM (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINEBUTTONCXFORM);
  iprintf(" ButtonId %d\n", sblock->ButtonId);
  outputSWF_CXFORM(&sblock->ButtonColorTransform);
}

void 
outputSWF_SOUNDINFO (SWF_SOUNDINFO *info);

void
outputSWF_DEFINEBUTTONSOUND (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINEBUTTONSOUND);
  iprintf(" CharacterID: %d\n", sblock->CharacterID);
  iprintf(" ButtonSoundChar0 %d\n", sblock->ButtonSoundChar0);
  if(sblock->ButtonSoundChar0)
    outputSWF_SOUNDINFO (&sblock->ButtonSoundInfo0);
  
  iprintf(" ButtonSoundChar1 %d\n", sblock->ButtonSoundChar1);
  if(sblock->ButtonSoundChar1)
    outputSWF_SOUNDINFO (&sblock->ButtonSoundInfo1);
  
  iprintf(" ButtonSoundChar2 %d\n", sblock->ButtonSoundChar2);
  if(sblock->ButtonSoundChar2)
    outputSWF_SOUNDINFO (&sblock->ButtonSoundInfo2);
  
  iprintf(" ButtonSoundChar3 %d\n", sblock->ButtonSoundChar3);
  if(sblock->ButtonSoundChar3)
    outputSWF_SOUNDINFO (&sblock->ButtonSoundInfo3);
}

void
outputSWF_DEFINECOMMANDOBJ (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_DEFINECOMMANDOBJ);

}

void
outputSWF_DEFINEEDITTEXT (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINEEDITTEXT);

  iprintf (" CharacterID: %d\n", sblock->CharacterID);
  outputSWF_RECT (&(sblock->Bounds));
  iprintf (" Flags: ");
  iprintf (" HasText: %d ", sblock->HasText);
  iprintf (" WordWrap: %d ", sblock->WordWrap);
  iprintf (" Multiline: %d ", sblock->Multiline);
  iprintf (" Password: %d ", sblock->Password);
  iprintf (" ReadOnly: %d\n", sblock->ReadOnly);
  iprintf ("        ");
  iprintf (" HasTextColor: %d ", sblock->HasTextColor);
  iprintf (" HasMaxLength: %d ", sblock->HasMaxLength);
  iprintf (" HasFont: %d ", sblock->HasFont);
  iprintf (" HasFontClass: %d ", sblock->HasFontClass);
  iprintf (" AutoSize: %d ", sblock->AutoSize);
  iprintf (" HasLayout: %d\n", sblock->HasLayout);
  iprintf ("        ");
  iprintf (" NoSelect: %d ", sblock->NoSelect);
  iprintf (" Border: %d ", sblock->Border);
  iprintf (" WasStatic: %d ", sblock->WasStatic);
  iprintf (" HTML: %d ", sblock->HTML);
  iprintf (" UseOutlines: %d\n", sblock->UseOutlines);
  if (sblock->HasFont)
    {
      iprintf (" Font: ");
      iprintf (" FontID: %d ", sblock->FontID);
      iprintf (" FontHeight: %d\n", sblock->FontHeight);
    }

  if (sblock->HasFontClass)
    iprintf(" FontClass: %s\n", sblock->FontClass);

  if (sblock->HasTextColor)
    {
      outputSWF_RGBA (&sblock->TextColor,"");
    }
  if (sblock->HasLayout)
    {
      iprintf (" Layout:: ");
      iprintf (" Align: %d ", sblock->Align);
      iprintf (" LeftMargin: %d ", sblock->LeftMargin);
      iprintf (" RightMargin: %d ", sblock->RightMargin);
      iprintf (" Indent: %d ", sblock->Indent);
      iprintf (" Leading: %d\n", sblock->Leading);
    }
  iprintf (" VariableName: %s\n", sblock->VariableName);
  if (sblock->HasText)
    {
      iprintf (" InitialText: %s\n", sblock->InitialText);
    }
}

void
outputSWF_DEFINEFONT (SWF_Parserstruct * pblock)
{
  int i;
  OUT_BEGIN (SWF_DEFINEFONT);
  iprintf (" FontID: %d\n", sblock->FontID);
  for (i = 0; i < sblock->NumGlyphs; i++)
    iprintf (" OffsetTable[%3.3d]: %x\n", i, sblock->OffsetTable[i]);
  
  for (i = 0; i < sblock->NumGlyphs; i++)
    {
	char shapename[32];
	sprintf(shapename,"Shape[%3.3d]",i);
	outputSWF_SHAPE (&(sblock->GlyphShapeTable[i]), shapename);
    }
}

void
outputSWF_DEFINEFONT2 (SWF_Parserstruct * pblock)
{
  int i;
  OUT_BEGIN (SWF_DEFINEFONT2);

  iprintf (" FontID: %d\n", sblock->FontID);
  iprintf (" FontFlagsHasLayout: %d\n", sblock->FontFlagsHasLayout);
  iprintf (" FontFlagsShiftJis: %d\n", sblock->FontFlagsShiftJis);
  iprintf (" FontFlagsSmallText: %d\n", sblock->FontFlagsSmallText);
  iprintf (" FontFlagsFlagANSI: %d\n", sblock->FontFlagsFlagANSI);
  iprintf (" FontFlagsWideOffsets: %d\n", sblock->FontFlagsWideOffsets);
  iprintf (" FontFlagsWideCodes: %d\n", sblock->FontFlagsWideCodes);
  iprintf (" FontFlagsFlagsItalics: %d\n", sblock->FontFlagsFlagsItalics);
  iprintf (" FontFlagsFlagsBold: %d\n", sblock->FontFlagsFlagsBold);
  iprintf (" LanguageCode: %d\n", sblock->LanguageCode);
  iprintf (" FontNameLen: %d\n", sblock->FontNameLen);
  iprintf (" FontName: %s\n", sblock->FontName);
  iprintf (" NumGlyphs: %d\n", sblock->NumGlyphs);
  for (i = 0; i < sblock->NumGlyphs; i++)
    {
      if (sblock->FontFlagsWideOffsets)
	{
	  iprintf (" OffsetTable[%3.3d]: %lx\n", i,
		  sblock->OffsetTable.UI32[i]);
	}
      else
	{
	  iprintf (" OffsetTable[%3.3d]: %x\n", i,
		  sblock->OffsetTable.UI16[i]);
	}
    }
  if (sblock->FontFlagsWideOffsets)
    {
      iprintf (" CodeTableOffset: %lx\n", sblock->CodeTableOffset.UI32);
    }
  else
    {
      iprintf (" CodeTableOffset: %x\n", sblock->CodeTableOffset.UI16);
    }

  for (i = 0; i < sblock->NumGlyphs; i++)
    {
	char shapename[32];
	sprintf(shapename,"Shape[%3.3d]",i);
	outputSWF_SHAPE (&(sblock->GlyphShapeTable[i]), shapename);
    }

  for (i = 0; i < sblock->NumGlyphs; i++)
    {
	if( sblock->FontFlagsWideCodes )
	  {
		iprintf (" CodeTable[%3.3d]: %4.4x\n", i,
		  	sblock->CodeTable[i]);
	  }
	else
	  {
		iprintf (" CodeTable[%3.3d]: %2.2x\n", i,
		  	sblock->CodeTable[i]);
	  }
    }

  if( sblock->FontFlagsHasLayout ) {
    iprintf (" FontAscent: %d\n", sblock->FontAscent);
    iprintf (" FontDecent: %d\n", sblock->FontDecent);
    iprintf (" FontLeading: %d\n", sblock->FontLeading);
    for (i = 0; i < sblock->NumGlyphs; i++)
      {
	iprintf (" FontAdvanceTable[%3.3d]: %x\n", i,
		  sblock->FontAdvanceTable[i]);
      }
    iprintf (" FontBoundsable: (not used)\n");
    for (i = 0; i < sblock->NumGlyphs; i++)
      {
	outputSWF_RECT (&(sblock->FontBoundsTable[i]));
      }
    iprintf (" KerningCount: %d\n", sblock->KerningCount);
    for (i = 0; i < sblock->KerningCount; i++)
      {
	iprintf (" FontKerningTable[%3.3d]: %d,%d %d\n", i,
		  sblock->FontKerningTable[i].FontKerningCode1,
		  sblock->FontKerningTable[i].FontKerningCode2,
		  sblock->FontKerningTable[i].FontKerningAdjustment);
      }
  }

}

void
outputSWF_DEFINEFONT3 (SWF_Parserstruct * pblock)
{
  int i;
  OUT_BEGIN (SWF_DEFINEFONT3);

  iprintf (" FontID: %d\n", sblock->FontID);
  iprintf (" FontFlagsHasLayout: %d\n", sblock->FontFlagsHasLayout);
  iprintf (" FontFlagsShiftJis: %d\n", sblock->FontFlagsShiftJis);
  iprintf (" FontFlagsSmallText: %d\n", sblock->FontFlagsSmallText);
  iprintf (" FontFlagsFlagANSI: %d\n", sblock->FontFlagsFlagANSI);
  iprintf (" FontFlagsWideOffsets: %d\n", sblock->FontFlagsWideOffsets);
  iprintf (" FontFlagsWideCodes: %d\n", sblock->FontFlagsWideCodes);
  iprintf (" FontFlagsFlagsItalics: %d\n", sblock->FontFlagsFlagsItalics);
  iprintf (" FontFlagsFlagsBold: %d\n", sblock->FontFlagsFlagsBold);
  iprintf (" LanguageCode: %d\n", sblock->LanguageCode);
  iprintf (" FontNameLen: %d\n", sblock->FontNameLen);
  iprintf (" FontName: %s\n", sblock->FontName);
  iprintf (" NumGlyphs: %d\n", sblock->NumGlyphs);
  for (i = 0; i < sblock->NumGlyphs; i++)
    {
      if (sblock->FontFlagsWideOffsets)
	{
	  iprintf (" OffsetTable[%3.3d]: %lx\n", i,
		  sblock->OffsetTable.UI32[i]);
	}
      else
	{
	  iprintf (" OffsetTable[%3.3d]: %x\n", i,
		  sblock->OffsetTable.UI16[i]);
	}
    }
  if (sblock->FontFlagsWideOffsets)
    {
      iprintf (" CodeTableOffset: %lx\n", sblock->CodeTableOffset.UI32);
    }
  else
    {
      iprintf (" CodeTableOffset: %x\n", sblock->CodeTableOffset.UI16);
    }

  for (i = 0; i < sblock->NumGlyphs; i++)
    {
	char shapename[32];
	sprintf(shapename,"Shape[%3.3d]",i);
	outputSWF_SHAPE (&(sblock->GlyphShapeTable[i]), shapename);
    }

  for (i = 0; i < sblock->NumGlyphs; i++)
    {
	if( sblock->FontFlagsWideCodes )
	  {
		iprintf (" CodeTable[%3.3d]: %4.4x\n", i,
		  	sblock->CodeTable[i]);
	  }
	else
	  {
		iprintf (" CodeTable[%3.3d]: %2.2x\n", i,
		  	sblock->CodeTable[i]);
	  }
    }

  if( sblock->FontFlagsHasLayout ) {
    iprintf (" FontAscent: %d\n", sblock->FontAscent);
    iprintf (" FontDecent: %d\n", sblock->FontDecent);
    iprintf (" FontLeading: %d\n", sblock->FontLeading);
    for (i = 0; i < sblock->NumGlyphs; i++)
      {
	iprintf (" FontAdvanceTable[%3.3d]: %x\n", i,
		  sblock->FontAdvanceTable[i]);
      }
    iprintf (" FontBoundsable: (not used)\n");
    for (i = 0; i < sblock->NumGlyphs; i++)
      {
	outputSWF_RECT (&(sblock->FontBoundsTable[i]));
      }
    iprintf (" KerningCount: %d\n", sblock->KerningCount);
    for (i = 0; i < sblock->KerningCount; i++)
      {
	iprintf (" FontKerningTable[%3.3d]: %d,%d %d\n", i,
		  sblock->FontKerningTable[i].FontKerningCode1,
		  sblock->FontKerningTable[i].FontKerningCode2,
		  sblock->FontKerningTable[i].FontKerningAdjustment);
      }
  }

}

void
outputSWF_DEFINEFONTINFO (SWF_Parserstruct * pblock)
{
  int i;
  OUT_BEGIN (SWF_DEFINEFONTINFO);
  iprintf("FontID: %d\n", sblock->FontID);
  iprintf("FontNameLen %d\n", sblock->FontNameLen);
  iprintf("FontName %s\n", sblock->FontName);
  iprintf("FontFlagsSmallText %d\n", sblock->FontFlagsSmallText);
  iprintf("FontFlagsShiftJIS %d\n", sblock->FontFlagsShiftJIS);
  iprintf("FontFlagsANSI %d\n", sblock->FontFlagsANSI);
  iprintf("FontFlagsItalic %d\n", sblock->FontFlagsItalic);
  iprintf("FontFlagsBold %d\n", sblock->FontFlagsBold);
  iprintf("FontFlagsWideCodes %d\n", sblock->FontFlagsWideCodes);
  
  if(!verbose)
	return;

  for (i = 0; i < sblock->nGlyph; i++)
  	iprintf("code table mapping: %i -> %i\n", i, sblock->CodeTable[i]);
}

void
outputSWF_DEFINEFONTINFO2 (SWF_Parserstruct * pblock)
{
  int i;
  OUT_BEGIN (SWF_DEFINEFONTINFO2);
  iprintf("FontID: %d\n", sblock->FontID);
  iprintf("FontNameLen %d\n", sblock->FontNameLen);
  iprintf("FontName %s\n", sblock->FontName);
  iprintf("FontFlagsSmallText %d\n", sblock->FontFlagsSmallText);
  iprintf("FontFlagsShiftJIS %d\n", sblock->FontFlagsShiftJIS);
  iprintf("FontFlagsANSI %d\n", sblock->FontFlagsANSI);
  iprintf("FontFlagsItalic %d\n", sblock->FontFlagsItalic);
  iprintf("FontFlagsBold %d\n", sblock->FontFlagsBold);
  iprintf("FontFlagsWideCodes %d\n", sblock->FontFlagsWideCodes);
  iprintf("LanguageCode %d\n", sblock->LanguageCode); 
 
  if(!verbose)
	return;

  for (i = 0; i < sblock->nGlyph; i++)
  	iprintf("code table mapping: %i -> %i\n", i, sblock->CodeTable[i]);
}

void 
outputSWF_CSMTEXTSETTINGS (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_CSMTEXTSETTINGS);
  iprintf("TextID: %d\n", sblock->TextID);
  iprintf("UseFlashType %d\n", sblock->UseFlashType);
  iprintf("GridFit %d\n", sblock->GridFit);
  iprintf("Thickness %d\n", sblock->Thickness);
  iprintf("Sharpness %d\n", sblock->Sharpness);
}

void 
outputSWF_ZONEDATA(int j, struct SWF_ZONEDATA *data)
{
  iprintf("  ZoneData: %i\n", j);
  iprintf("    AlignmentCoordinate %d\n", data->AlignmentCoordinate);
  iprintf("    Range %d\n", data->Range);
}

void 
outputSWF_ZONERECORD(int i, struct SWF_ZONERECORD *zone)
{
	int j;
	iprintf("ZoneRecord %d\n", i);
	iprintf("  NumZoneData %d\n", zone->NumZoneData);
	for(j = 0; j < zone->NumZoneData; j++)
		outputSWF_ZONEDATA(j, zone->ZoneData + j);
	
	iprintf("  ZoneMask X %d, Y %d\n", zone->ZoneMaskX, zone->ZoneMaskY);
}

void 
outputSWF_DEFINEFONTALIGNZONES (SWF_Parserstruct * pblock)
{
  int i;
  OUT_BEGIN (SWF_DEFINEFONTALIGNZONES);

  iprintf("  FontID: %d\n", sblock->FontID);
  iprintf("  CSMTableHint %d\n", sblock->CSMTableHint);
  iprintf("  GlyphCount %d\n", sblock->GlyphCount);
  for(i = 0; i < sblock->GlyphCount; i++)
    outputSWF_ZONERECORD(i, sblock->ZoneTable + i);
}

void
outputSWF_DEFINEFONTNAME (SWF_Parserstruct *pblock)
{
  OUT_BEGIN(SWF_DEFINEFONTNAME);
  iprintf(" FontId: %d\n", sblock->FontId);
  iprintf(" FontName: %s\n", sblock->FontName);
  iprintf(" FontCopyright %s\n", sblock->FontCopyright);
}

void
outputSWF_DEFINELOSSLESS (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINELOSSLESS);
  iprintf(" CharacterID: %d\n", sblock->CharacterID);
  iprintf(" Bitmap format %d\n", sblock->BitmapFormat);
  iprintf(" Bitmap width %d x height %d\n", sblock->BitmapWidth, sblock->BitmapHeight);
  
  if(sblock->BitmapFormat == 3)
  	iprintf(" BitmapColorTableSize %d\n", sblock->BitmapColorTableSize);
}

void
outputSWF_DEFINELOSSLESS2 (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINELOSSLESS2);
  iprintf(" CharacterID: %d\n", sblock->CharacterID);
  iprintf(" Bitmap format %d\n", sblock->BitmapFormat);
  iprintf(" Bitmap width %d x height %d\n", sblock->BitmapWidth, sblock->BitmapHeight);
  if(sblock->BitmapFormat == 3)                                                                                                                                                    
        iprintf(" BitmapColorTableSize %d\n", sblock->BitmapColorTableSize);
}

void
outputSWF_DEFINEMORPHSHAPE (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINEMORPHSHAPE);
  iprintf(" CharacterID: %d\n", sblock->CharacterID);
  outputSWF_RECT(&(sblock->StartBounds));
  outputSWF_RECT(&(sblock->EndBounds));
  iprintf("  Offset %d\n", sblock->Offset);
  outputSWF_MORPHFILLSTYLES(&(sblock->MorphFillStyles));
  outputSWF_MORPHLINESTYLES(&(sblock->MorphLineStyles));
  outputSWF_SHAPE(&(sblock->StartEdges), "");
  outputSWF_SHAPE(&(sblock->EndEdges), "");
}

void
outputSWF_DEFINEMORPHSHAPE2 (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINEMORPHSHAPE2);
  iprintf(" CharacterID: %d\n", sblock->CharacterID);
  outputSWF_RECT(&(sblock->StartBounds));
  outputSWF_RECT(&(sblock->EndBounds));
  outputSWF_RECT(&(sblock->StartEdgeBounds));
  outputSWF_RECT(&(sblock->EndEdgeBounds));
  iprintf("  UsesNonScalingStrokes %d\n", 
    sblock->UsesNonScalingStrokes);
  iprintf("  UsesScalinStrokes %d\n",
    sblock->UsesScalingStrokes);
  iprintf("  Offset %d\n", sblock->Offset);
  outputSWF_MORPHFILLSTYLES(&(sblock->MorphFillStyles));
  outputSWF_MORPHLINESTYLES(&(sblock->MorphLineStyles));
  outputSWF_SHAPE(&(sblock->StartEdges), "");
  outputSWF_SHAPE(&(sblock->EndEdges), "");
}

void
outputSWF_DEFINESHAPE (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINESHAPE);

  iprintf (" CharacterID: %d\n", sblock->ShapeID);
  outputSWF_RECT (&(sblock->ShapeBounds));
  outputSWF_SHAPEWITHSTYLE (&(sblock->Shapes),1,"");
}

void
outputSWF_DEFINESHAPE2 (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINESHAPE2);

  iprintf (" CharacterID: %d\n", sblock->ShapeID);
  outputSWF_RECT (&(sblock->ShapeBounds));
  outputSWF_SHAPEWITHSTYLE (&(sblock->Shapes),2,"");

}

void
outputSWF_DEFINESHAPE3 (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINESHAPE3);

  iprintf (" CharacterID: %d\n", sblock->ShapeID);
  outputSWF_RECT (&(sblock->ShapeBounds));
  outputSWF_SHAPEWITHSTYLE (&(sblock->Shapes),2,"");

}

void
outputSWF_DEFINESHAPE4 (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINESHAPE4);

  iprintf (" CharacterID: %d\n", sblock->ShapeID);
  outputSWF_RECT (&(sblock->ShapeBounds));
  outputSWF_RECT (&(sblock->EdgeBounds));
  iprintf("   UsesNonScalingStrokes: %d\n", sblock->UsesNonScalingStrokes);
  iprintf("   UsesScalingStrokes: %d\n", sblock->UsesScalingStrokes);
  outputSWF_SHAPEWITHSTYLE (&(sblock->Shapes),2,"");
}

void
outputSWF_DEFINESOUND (SWF_Parserstruct * pblock)
{
  
	OUT_BEGIN (SWF_DEFINESOUND);
	iprintf(" CharacterID: %d\n", sblock->SoundId);
	iprintf(" SoundFormat: %d\n", sblock->SoundFormat);
	iprintf(" SoundRate: %d\n", sblock->SoundRate);
	iprintf(" SoundSize: %d\n", sblock->SoundSize);
	iprintf(" SoundType: %d\n", sblock->SoundType);
	iprintf(" SoundSampleCount: %d\n", sblock->SoundSampleCount);
	//iprintf(" SoundData: ");
}

void
outputSWF_DEFINESPRITE (SWF_Parserstruct * pblock)
{
  int i;
  OUT_BEGIN (SWF_DEFINESPRITE);

  iprintf(" SpriteId: %d\n", sblock->SpriteId );
  iprintf(" FrameCount: %d\n", sblock->FrameCount );
  iprintf(" BlockCount: %d\n", sblock->BlockCount );
  ++INDENT;
  for(i=0;i<sblock->BlockCount;i++) {
       outputBlock(sblock->tagTypes[i], sblock->Tags[i], NULL);
  }
  --INDENT;

}

void
outputSWF_DEFINETEXT (SWF_Parserstruct * pblock)
{
  int i;
  OUT_BEGIN (SWF_DEFINETEXT);

  iprintf(" CharacterID: %d\n", sblock->CharacterID );
  outputSWF_RECT( &sblock->TextBounds );
  outputSWF_MATRIX( &sblock->TextMatrix, "" );
  iprintf(" GlyphBits: %d\n", sblock->GlyphBits );
  iprintf(" AdvanceBits: %d\n", sblock->AdvanceBits );
  iprintf(" TextRecords: %d\n", sblock->numTextRecords );
  for(i=0;i<sblock->numTextRecords;i++) {
	  outputSWF_TEXTRECORD(&(sblock->TextRecords[i]), 1 );
  }

}

void
outputSWF_DEFINETEXT2 (SWF_Parserstruct * pblock)
{
  int i;
  OUT_BEGIN (SWF_DEFINETEXT2);

  iprintf(" CharacterID: %d\n", sblock->CharacterID );
  outputSWF_RECT( &sblock->TextBounds );
  outputSWF_MATRIX( &sblock->TextMatrix, "" );
  iprintf(" GlyphBits: %d\n", sblock->GlyphBits );
  iprintf(" AdvanceBits: %d\n", sblock->AdvanceBits );
  iprintf(" TextRecords: %d\n", sblock->numTextRecords );
  for(i=0;i<sblock->numTextRecords;i++) {
	  outputSWF_TEXTRECORD(&(sblock->TextRecords[i]), 2 );
  }
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
  OUT_BEGIN (SWF_DEFINEVIDEOSTREAM);
  iprintf("  CharacterID: %d\n", sblock->CharacterID);
  iprintf("  NumFrames: %d\n", sblock->NumFrames);
  iprintf("  Width: %d; Height %d\n", sblock->Width, sblock->Height);
  iprintf("  Flag deblocking: %x\n", sblock->VideoFlagsDeblocking);
  iprintf("  Flag smoothing: %x\n", sblock->VideoFlagsSmoothing);
  iprintf("  Codec ID: %d\n", sblock->CodecID);
}

void
outputSWF_DOACTION (SWF_Parserstruct * pblock)
{
#ifdef NODECOMPILE
	int i;
#endif
	OUT_BEGIN (SWF_DOACTION);

#ifdef NODECOMPILE
	iprintf(" %d Actions\n", sblock->numActions);
	for(i=0;i<sblock->numActions;i++)
	outputSWF_ACTION(i,&(sblock->Actions[i]));
#else
	iprintf ("%s\n", decompile5Action(sblock->numActions, sblock->Actions, 0));
#endif

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
  int i;
  OUT_BEGIN (SWF_EXPORTASSETS);

  iprintf (" num assets: %d\n", sblock->Count );
  for (i = 0; i < sblock->Count; i++)
    {
	iprintf (" Asset[%3.3d]: %s\n", sblock->Tags[i],
		  sblock->Names[i]);
    }

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

  iprintf (" Name: %s\n", sblock->Name );
  iprintf (" IsAnchor: %d\n", sblock->IsAnchor );
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
  int i;
  OUT_BEGIN (SWF_IMPORTASSETS);

  iprintf (" URL: %s\n", sblock->URL );
  iprintf (" num assets: %d\n", sblock->Count );
  for (i = 0; i < sblock->Count; i++)
    {
	iprintf (" Asset[%3.3d]: %s\n", sblock->Tags[i],
		  sblock->Names[i]);
    }

}

void
outputSWF_IMPORTASSETS2 (SWF_Parserstruct * pblock)
{
  int i;
  OUT_BEGIN (SWF_IMPORTASSETS2);

  iprintf (" URL: %s\n", sblock->URL );
  iprintf (" num assets: %d\n", sblock->Count );
  for (i = 0; i < sblock->Count; i++)
    {
	iprintf (" Asset[%3.3d]: %s\n", sblock->Tags[i],
		  sblock->Names[i]);
    }

}

void
outputSWF_JPEGTABLES (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_JPEGTABLES);

}

void
outputSWF_NAMECHARACTER (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_NAMECHARACTER);
  iprintf("  Id: %d\n", sblock->Id);
  iprintf("  Name: %s\n", sblock->Name);
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
  OUT_BEGIN (SWF_PLACEOBJECT2);

#if !defined(ACTIONONLY)
  iprintf(" PlaceFlagHasClipActions %d\n", sblock->PlaceFlagHasClipActions);
  iprintf(" PlaceFlagHasClipDepth %d\n", sblock->PlaceFlagHasClipDepth);
  iprintf(" PlaceFlagHasName %d\n", sblock->PlaceFlagHasName);
  iprintf(" PlaceFlagHasRatio %d\n", sblock->PlaceFlagHasRatio);
  iprintf(" PlaceFlagHasColorTransform %d\n", sblock->PlaceFlagHasColorTransform);
  iprintf(" PlaceFlagHasMatrix %d\n", sblock->PlaceFlagHasMatrix);
  iprintf(" PlaceFlagHasCharacter %d\n", sblock->PlaceFlagHasCharacter);
  iprintf(" PlaceFlagMove %d\n", sblock->PlaceFlagMove);
  iprintf(" Depth %d\n", sblock->Depth);
  if( sblock->PlaceFlagHasCharacter )
	  iprintf( " CharacterID: %d\n", sblock->CharacterId );
  if( sblock->PlaceFlagHasMatrix )
	outputSWF_MATRIX (&(sblock->Matrix), "");
/*
  if( sblock->PlaceFlagHasColorTransform )
	outputSWF_CXFORMWITHALPHA (&(sblock->ColorTransform), "");
*/
  if( sblock->PlaceFlagHasRatio )
	  iprintf( " Ratio: %d\n", sblock->Ratio );
  if( sblock->PlaceFlagHasName )
	  iprintf( " Name: %s\n", sblock->Name );
  if( sblock->PlaceFlagHasClipDepth )
	  iprintf( " ClipDepth: %d\n", sblock->ClipDepth );
#endif
  if( sblock->PlaceFlagHasClipActions )
	outputSWF_CLIPACTIONS (&(sblock->ClipActions));
}

void
outputSWF_PLACEOBJECT3 (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_PLACEOBJECT3);
#if !defined(ACTIONONLY)
  iprintf(" PlaceFlagHasClipActions %d\n", sblock->PlaceFlagHasClipActions);
  iprintf(" PlaceFlagHasClipDepth %d\n", sblock->PlaceFlagHasClipDepth);
  iprintf(" PlaceFlagHasName %d\n", sblock->PlaceFlagHasName);
  iprintf(" PlaceFlagHasRatio %d\n", sblock->PlaceFlagHasRatio);
  iprintf(" PlaceFlagHasColorTransform %d\n", sblock->PlaceFlagHasColorTransform);
  iprintf(" PlaceFlagHasMatrix %d\n", sblock->PlaceFlagHasMatrix);
  iprintf(" PlaceFlagHasCharacter %d\n", sblock->PlaceFlagHasCharacter);
  iprintf(" PlaceFlagMove %d\n", sblock->PlaceFlagMove);
  iprintf(" PlaceFlagHasImage %d\n", sblock->PlaceFlagHasImage);
  iprintf(" PlaceFlagHasClassName %d\n", sblock->PlaceFlagHasClassName);
  iprintf(" PlaceFlagHasCacheAsbitmap %d\n", sblock->PlaceFlagHasCacheAsBitmap);
  iprintf(" PlaceFlagHasBlendMode %d\n", sblock->PlaceFlagHasBlendMode);
  iprintf(" PlaceFlagHasFilterList %d\n", sblock->PlaceFlagHasFilterList); 
  iprintf(" Depth %d\n", sblock->Depth);
 
  if( sblock->PlaceFlagHasClassName ||
      (sblock->PlaceFlagHasImage && sblock->PlaceFlagHasCharacter))
    iprintf(" ClassName %s\n", sblock->ClassName);
  
  if( sblock->PlaceFlagHasCharacter )
	  iprintf( " CharacterID: %d\n", sblock->CharacterId );
  if( sblock->PlaceFlagHasMatrix )
	outputSWF_MATRIX (&(sblock->Matrix), "");
/*
  if( sblock->PlaceFlagHasColorTransform )
	outputSWF_CXFORMWITHALPHA (&(sblock->ColorTransform), "");
*/
  if( sblock->PlaceFlagHasRatio )
	  iprintf( " Ratio: %d\n", sblock->Ratio );
  if( sblock->PlaceFlagHasName )
	  iprintf( " Name: %s\n", sblock->Name );
  if( sblock->PlaceFlagHasClipDepth )
	  iprintf( " ClipDepth: %d\n", sblock->ClipDepth );
  if( sblock->PlaceFlagHasBlendMode )
	  iprintf("  BlendMode %d\n", sblock->BlendMode );
  if( sblock->PlaceFlagHasFilterList )
  {
	  int i;
	  SWF_FILTERLIST *filterList = &sblock->SurfaceFilterList;
	  
	  iprintf("  NumberOfFilters %d\n", filterList->NumberOfFilters);
	  
	  for(i = 0; i < filterList->NumberOfFilters; i++)
	    outputSWF_FILTER(filterList->Filter + i);
  }
#endif
  if( sblock->PlaceFlagHasClipActions )
	outputSWF_CLIPACTIONS (&(sblock->ClipActions));

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
  OUT_BEGIN (SWF_PROTECT);

  if( sblock->Password )
    iprintf(" Password: %s\n", sblock->Password);

}

void
outputSWF_REMOVEOBJECT (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_REMOVEOBJECT);

  iprintf(" CharacterID: %d\n", sblock->CharacterId);
  iprintf(" Depth: %d\n", sblock->Depth);

}

void
outputSWF_REMOVEOBJECT2 (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_REMOVEOBJECT2);
  iprintf(" Depth: %d\n", sblock->Depth);

}

void
outputSWF_SERIALNUMBER (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_SERIALNUMBER);
  iprintf("Version %d.%d.%d.%d\n", sblock->Id, sblock->Edition, 
	sblock->Major, sblock->Minor);
  iprintf("Build: %lu\n", (((long long)sblock->BuildH) << 32) + sblock->BuildL);
  iprintf("Timestamp: %lu\n", 
	(((long long)sblock->TimestampH) << 32) + sblock->TimestampL);
}

void
outputSWF_SETBACKGROUNDCOLOR (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_SETBACKGROUNDCOLOR);

  outputSWF_RGBA (&sblock->rgb, "");

}

void
outputSWF_SHOWFRAME (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_SHOWFRAME);

}


void
outputSWF_SOUNDSTREAMBLOCK (SWF_Parserstruct * pblock)
{
  if(m.soundStreamFmt == 2)
  {
    OUT_BEGIN (SWF_SOUNDSTREAMBLOCK);
    iprintf("  Mp3: SampleCount %i\n", 
           sblock->StreamData.mp3.SampleCount);
    iprintf("  Mp3: SeekSamples %i\n", 
           sblock->StreamData.mp3.SeekSamples);
  }
}

void
outputSWF_SOUNDSTREAMHEAD (SWF_Parserstruct * pblock)
{
  char *tmp;

  OUT_BEGIN (SWF_SOUNDSTREAMHEAD);
  switch(sblock->PlaybackSoundRate)
  {
    case 0: tmp = "5.5 kHz"; break;
    case 1: tmp = "11 kHz"; break;
    case 2: tmp = "22 kHz"; break;
    case 3: tmp = "44 kHz"; break;
    default: tmp = "error";
  }
  iprintf("  PlaybackSoundRate %s\n", tmp);

  switch(sblock->PlaybackSoundSize)
  {
    case 1: tmp = "16 bit"; break;
    default: tmp = "error";
  }
  iprintf("  PlaybackSoundSize %s\n", tmp);

  switch(sblock->PlaybackSoundType)
  {
    case 0: tmp = "mono"; break;
    case 1: tmp = "stereo"; break;
    default: tmp = "error";
  }
  iprintf("  PlaybackSoundType %s\n", tmp);

  switch(sblock->StreamSoundCompression)
  {
    case 1: tmp = "ADPCM"; break;
    case 2: tmp = "MP3"; break;
    default: tmp ="error";
  }
  iprintf("  StreamSoundCompression %s\n", tmp);

  switch(sblock->StreamSoundRate)
  {
    case 0: tmp = "5.5 kHz"; break;
    case 1: tmp = "11 kHz"; break;
    case 2: tmp = "22 kHz"; break;
    case 3: tmp = "44 kHz"; break;
    default: tmp = "error";
  }
  iprintf("  StreamSoundRate %s\n", tmp);

  switch(sblock->StreamSoundSize)
  {
    case 1: tmp = "16 bit"; break;
    default: tmp = "error";
  }
  iprintf("  StreamSoundSize %s\n", tmp);

  switch(sblock->StreamSoundType)
  {
    case 0: tmp = "mono"; break;
    case 1: tmp = "stereo"; break;
    default: tmp = "error";
  }
  iprintf("  StreamSoundType %s\n", tmp);
  iprintf("  StreamSoundSampleCount %i\n", sblock->StreamSoundSampleCount);
  if(sblock->StreamSoundCompression == 2)
    iprintf("  LatencySeek %i\n", sblock->LatencySeek);  
}

void
outputSWF_SOUNDSTREAMHEAD2 (SWF_Parserstruct * pblock)
{
  char *tmp;
  OUT_BEGIN (SWF_SOUNDSTREAMHEAD2);
  switch(sblock->PlaybackSoundRate)
  {
    case 0: tmp = "5.5 kHz"; break;
    case 1: tmp = "11 kHz"; break;
    case 2: tmp = "22 kHz"; break;
    case 3: tmp = "44 kHz"; break;
    default: tmp = "error";
  }
  iprintf("  PlaybackSoundRate %s\n", tmp);

  switch(sblock->PlaybackSoundSize)
  {
    case 0: tmp = "8 bit"; break;
    case 1: tmp = "16 bit"; break;
    default: tmp = "error";
  }
  iprintf("  PlaybackSoundSize %s\n", tmp);

  switch(sblock->PlaybackSoundType)
  {
    case 0: tmp = "mono"; break;
    case 1: tmp = "stereo"; break;
    default: tmp = "error";
  }
  iprintf("  PlaybackSoundType %s\n", tmp);

  switch(sblock->StreamSoundCompression)
  {
    case 0: tmp = "uncompressed"; break;
    case 1: tmp = "ADPCM"; break;
    case 2: tmp = "MP3"; break;
    case 3: tmp = "uncompressed"; break;
    case 6: tmp = "Nellymoser"; break;
    default: tmp ="error";
  }
  iprintf("  StreamSoundCompression %s\n", tmp);

  switch(sblock->StreamSoundRate)
  {
    case 0: tmp = "5.5 kHz"; break;
    case 1: tmp = "11 kHz"; break;
    case 2: tmp = "22 kHz"; break;
    case 3: tmp = "44 kHz"; break;
    default: tmp = "error";
  }
  iprintf("  StreamSoundRate %s\n", tmp);

  switch(sblock->StreamSoundSize)
  {
    case 0: tmp = "8 bit"; break;
    case 1: tmp = "16 bit"; break;
    default: tmp = "error";
  }
  iprintf("  StreamSoundSize %s\n", tmp);

  switch(sblock->StreamSoundType)
  {
    case 0: tmp = "mono"; break;
    case 1: tmp = "stereo"; break;
    default: tmp = "error";
  }
  iprintf("  StreamSoundType %s\n", tmp);
  iprintf("  StreamSoundSampleCount %i\n", sblock->StreamSoundSampleCount);
  if(sblock->StreamSoundCompression == 2)
    iprintf("  LatencySeek %i\n", sblock->LatencySeek);
}

void
outputSWF_SOUNDENVELOPE (SWF_SOUNDENVELOPE *env)
{
  iprintf("    SoundEnvelope:");
  iprintf(" Pos44 %d, LeftLevel %d, RightLevel %d\n",
    env->Pos44, env->LeftLevel, env->RightLevel);
}

void 
outputSWF_SOUNDINFO (SWF_SOUNDINFO *info)
{
  iprintf("  SoundInfo:");
  
  if(info->SyncStop)
    iprintf(" SyncStop");

  if(info->SyncNoMultiple)
    iprintf(" SyncNoMultiple");
  
  if(info->HasEnvelope)
    iprintf(" HasEnvelope");

  if(info->HasLoops)
    iprintf(" Loops: %d", info->LoopCount);

  if(info->HasOutPoint)
    iprintf(" HasOutPoint: %d", info->OutPoint);

  if(info->HasInPoint)
    iprintf(" HasInPoint: %d", info->InPoint);

  iprintf("\n");

  if(info->HasEnvelope)
  {
    int i;
    iprintf("    EnvPoints %d\n", info->EnvPoints);
    for(i = 0; i < info->EnvPoints; i++)
      outputSWF_SOUNDENVELOPE(info->EnvelopeRecords + i);
  }
} 

void
outputSWF_STARTSOUND (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_STARTSOUND);
  iprintf(" SoundId %d\n", sblock->SoundId);
  outputSWF_SOUNDINFO(&sblock->SoundInfo);
}

void
outputSWF_STARTSOUND2 (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_STARTSOUND2);
  iprintf(" SoundClassName %s\n", sblock->SoundClassName);
  outputSWF_SOUNDINFO(&sblock->SoundInfo);
}

void
outputSWF_SYNCFRAME (SWF_Parserstruct * pblock)
{
  //OUT_BEGIN (SWF_SYNCFRAME);

}

void
outputSWF_INITACTION (SWF_Parserstruct * pblock)
{
#ifdef NODECOMPILE
	int i;
#endif
	OUT_BEGIN (SWF_INITACTION);

#ifdef NODECOMPILE
	iprintf(" %d Init actions for character %u\n", sblock->numActions, sblock->SpriteId);
	for(i=0;i<sblock->numActions;i++)
		outputSWF_ACTION(i,&(sblock->Actions[i]));
#else
	iprintf ("%s\n", decompile5Action(sblock->numActions,sblock->Actions,0));
#endif

}
void
outputSWF_VIDEOFRAME (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_VIDEOFRAME);
  iprintf("  StreamID %i\n", sblock->StreamID);
  iprintf("  FrameNum %i\n", sblock->FrameNum);
}

void
outputSWF_REFLEX (SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_REFLEX);

  iprintf(" Reflex: \"%c%c%c\"\n", sblock->rfx[0], sblock->rfx[1], sblock->rfx[2]);
}

void 
outputSWF_FILEATTRIBUTES(SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_FILEATTRIBUTES);
  
  iprintf(" FileAttributes: HasMetaData %d, UseNetwork %d, HasAS3 %d\n", 
          sblock->HasMetadata, sblock->UseNetwork, sblock->ActionScript3);
}

void 
outputSWF_METADATA(SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_METADATA);
  
  iprintf(" Metadata: \n%s\n\n", 
          sblock->Metadata);
}

void 
outputSWF_SCRIPTLIMITS(SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_SCRIPTLIMITS);
  iprintf(" MaxRecursionDepth %d\n", sblock->MaxRecursionDepth);
  iprintf(" ScriptTimeoutSeconds %d\n", sblock->ScriptTimeoutSeconds);
}

void 
outputSWF_DEFINESCALINGGRID(SWF_Parserstruct * pblock)
{
  OUT_BEGIN (SWF_DEFINESCALINGGRID);
  iprintf(" CharacterId %d\n", sblock->CharacterId);
  outputSWF_RECT(&sblock->Splitter);
}

void 
outputSWF_SETTABINDEX(SWF_Parserstruct *pblock)
{
  OUT_BEGIN (SWF_SETTABINDEX);
  iprintf(" Depth: %d\n", sblock->Depth);
  iprintf(" TabIndex: %d\n", sblock->TabIndex);
}

static void 
outputNSSetConstant(struct ABC_FILE *abc, unsigned int index);
static void 
outputNamespaceConstant(struct ABC_FILE *abc, unsigned int index);
static void 
outputMultinameConstant(struct ABC_FILE *abc, unsigned int index);
static void 
outputStringConstant(struct ABC_FILE *abc, unsigned int strIndex);

static void 
outputABC_STRING_INFO(struct ABC_STRING_INFO *si)
{
  char *buffer, *bufp;
  int i;
  if(si->Size == 0)
  {
    iprintf("     ** empty ** (0)\n");
    return;
  }

  // we don't deal with utf8 yet
  buffer = malloc(si->Size+1);
  memset(buffer, 0, si->Size+1);
  bufp = buffer;
  for(i = 0; i < si->Size; i++)
  {
    if(si->UTF8String[i] < 128)
      *bufp++ = si->UTF8String[i];
  }
  iprintf("   '%s' (%i)\n", buffer, si->Size);
  free(buffer);

}

static void 
outputABC_QNAME(struct ABC_FILE *abc, struct ABC_QNAME *qn)
{
  outputNamespaceConstant(abc, qn->NS);
  outputStringConstant(abc, qn->Name);
}

static void 
outputABC_RTQNAME(struct ABC_FILE *abc, struct ABC_RTQNAME *rtq)
{
  outputStringConstant(abc, rtq->Name);
}

static void
outputABC_MULTINAME(struct ABC_FILE *abc, struct ABC_MULTINAME *mn)
{
  outputStringConstant(abc, mn->Name);
  outputNSSetConstant(abc, mn->NSSet);
}

static void
outputABC_MULTINAME_L(struct ABC_FILE *abc, struct ABC_MULTINAME_L *ml)
{
  outputNSSetConstant(abc, ml->NSSet);
}

static void outputABC_MULTINAME_INFO(struct ABC_FILE *abc, 
                                     struct ABC_MULTINAME_INFO *mi)
{
  switch(mi->Kind)
  {
    case ABC_CONST_QNAME:
    case ABC_CONST_QNAME_A:
      iprintf("    Multiname ABC_CONST_QNAME(A)");
      outputABC_QNAME(abc, &mi->Data.QName);
      break; 
    case ABC_CONST_RTQNAME:
    case ABC_CONST_RTQNAME_A:
      iprintf("    Multiname ABC_CONST_RTQNAME(A)");
      outputABC_RTQNAME(abc, &mi->Data.RTQName); 
      break;
    case ABC_CONST_RTQNAME_L:
    case ABC_CONST_RTQNAME_LA:
      iprintf("    Multiname ABC_CONST_MULTINAME(A)");
      break;
    case ABC_CONST_MULTINAME:
    case ABC_CONST_MULTINAME_A:
      iprintf("    Multiname ABC_CONST_MULTINAME(A)");
      outputABC_MULTINAME(abc, &mi->Data.Multiname); 
      break;
    case ABC_CONST_MULTINAME_L:
    case ABC_CONST_MULTINAME_LA:
      iprintf("    Multiname ABC_CONST_MULTINAME(A)");
      outputABC_MULTINAME_L(abc, &mi->Data.MultinameL); 
      break;
   }
}

static void 
outputABC_NS_INFO(struct ABC_FILE *abc, struct ABC_NS_INFO *ns)
{
  iprintf("    Namespace Kind %x\n", ns->Kind);
  outputStringConstant(abc, ns->Name);
}

static void 
outputABC_NS_SET_INFO(struct ABC_FILE *abc, struct ABC_NS_SET_INFO *set)
{
  int i;
  for(i = 0; i < set->Count; i++)
  {
    unsigned int index = set->NS[i];
    outputNamespaceConstant(abc, index);
  }
}

static void 
outputNSSetConstant(struct ABC_FILE *abc, unsigned int index)
{
  struct ABC_CONSTANT_POOL *cp = &abc->ConstantPool;
  if(index >= cp->NamespaceSetCount)
  {
    iprintf("ConstantPool NamespaceSetCount %u <= index %u\n",
      cp->NamespaceSetCount, index);
    return;
  }

  if(index == 0)
  {
    iprintf("*\n");
    return;
  }
  outputABC_NS_SET_INFO(abc, cp->NsSets + index);
}


static void 
outputNamespaceConstant(struct ABC_FILE *abc, unsigned int index)
{
  struct ABC_CONSTANT_POOL *cp = &abc->ConstantPool;
  if(index >= cp->NamespaceCount)
  {
    iprintf("ConstantPool NamespaceCount %u <= index %u\n",
      cp->NamespaceCount, index);
    return;
  }

  if(index == 0)
  {
    iprintf("*\n");
    return;
  }
  outputABC_NS_INFO(abc, cp->Namespaces + index);
}

static void 
outputMultinameConstant(struct ABC_FILE *abc, unsigned int index)
{
  struct ABC_CONSTANT_POOL *cp = &abc->ConstantPool;
  if(index >= cp->MultinameCount)
  {
    iprintf("ConstantPool MultinameCount %u <= index %u\n",
      cp->MultinameCount, index);
    return;
  }

  if(index == 0)
  {
    iprintf("Multiname index 0 is not allowed\n");
    return;
  }
  outputABC_MULTINAME_INFO(abc, cp->Multinames + index);
}

static void 
outputIntConstant(struct ABC_FILE *abc, unsigned int index)
{
  struct ABC_CONSTANT_POOL *cp = &abc->ConstantPool;
  if(index >= cp->IntCount)
  {
    iprintf("ConstantPool IntCount %u <= index %u\n",
      cp->IntCount, index);
    return;
  }

  if(index == 0)
  {
    iprintf("Integer index 0 is not allowed\n");
    return;
  }
  iprintf("Int %i\n", cp->Integers[index]);
}

static void 
outputUIntConstant(struct ABC_FILE *abc, unsigned int index)
{
  struct ABC_CONSTANT_POOL *cp = &abc->ConstantPool;
  if(index >= cp->UIntCount)
  {
    iprintf("ConstantPool UIntCount %u <= index %u\n",
      cp->UIntCount, index);
    return;
  }

  if(index == 0)
  {
    iprintf("UInteger index 0 is not allowed\n");
    return;
  }
  iprintf("    UInt %u\n", cp->UIntegers[index]);
}

static void 
outputDoubleConstant(struct ABC_FILE *abc, unsigned int index)
{
  struct ABC_CONSTANT_POOL *cp = &abc->ConstantPool;
  if(index >= cp->DoubleCount)
  {
    iprintf("ConstantPool DoubleCount %u <= index %u\n",
      cp->DoubleCount, index);
    return;
  }

  if(index == 0)
  {
    iprintf("    NaN\n");
    return;
  }
  iprintf("    Double %f\n", cp->Doubles[index]);
}


static void 
outputStringConstant(struct ABC_FILE *abc, unsigned int strIndex)
{
  struct ABC_CONSTANT_POOL *cp = &abc->ConstantPool;
  if(strIndex >= cp->StringCount)
  {
    iprintf("ConstantPool StringCount %u <= strIndex %u\n",
      cp->StringCount, strIndex);
    return;
  }

  if(strIndex == 0)
  {
    iprintf("    *\n");
    return;
  }
  outputABC_STRING_INFO(cp->Strings + strIndex);
}

void outputABC_OPTION_INFO(struct ABC_FILE *abc, struct ABC_OPTION_INFO *o)
{
  int i;
  for (i = 0; i < o->OptionCount; i++)
  {
    unsigned int index = o->Option[i].Val;
    iprintf("   Option: ");
    switch(o->Option[i].Kind)
    {
      case ABC_INT:
        outputIntConstant(abc, index);
        break;
      case ABC_UINT:
        outputUIntConstant(abc, index);
        break;
      case ABC_DOUBLE:
        outputDoubleConstant(abc, index);
        break;
      case ABC_UTF8:
        outputStringConstant(abc, index);
        break;
      case ABC_TRUE:
        iprintf(" TRUE\n");
        break;
      case ABC_FALSE:
        iprintf(" FALSE\n");
        break;
      case ABC_NULL:
        iprintf(" NULL\n");
        break;
      case ABC_UNDEF:
        iprintf(" UNDEF\n");
        break;
      case ABC_NAMESPACE:
      case ABC_PACKAGE_NS:
      case ABC_PACKAGE_INTERNAL_NS:
      case ABC_PROTECTED_NS:
      case ABC_EXPLICIT_NS:
      case ABC_STATIC_PROTECTED_NS:
      case ABC_PRIVATE_NS:
        outputNamespaceConstant(abc, index);
        break;
      default:
        iprintf("Option type %x unknown\n", o->Option[i].Kind);
    }
  }
}

void outputABC_METHOD_INFO(struct ABC_FILE *abc, struct ABC_METHOD_INFO *minfo)
{
  int i;
  iprintf("   ParamCount %u\n", minfo->ParamCount);
  iprintf("   ReturnType \n   {\n");
  if(minfo->ReturnType)
    outputMultinameConstant(abc, minfo->ReturnType);
  else 
    iprintf("    void\n");
  iprintf("   }\n\n");
  for(i = 0; i < minfo->ParamCount; i++)
  {
    unsigned int index = minfo->ParamType[i];
    iprintf("    Parameter %i\n    {\n", i);
    outputMultinameConstant(abc, index);
    iprintf("    }\n");
  }

  iprintf("   Name (%u) ",  minfo->Name);
  if(minfo->Name)
    outputStringConstant(abc, minfo->Name);
  else
    iprintf("**no name**\n");

  iprintf("   Flags %x\n", minfo->Flags);
  if(minfo->Flags & ABC_METHOD_HAS_OPTIONAL)
    outputABC_OPTION_INFO(abc, &minfo->Options);
  if(minfo->Flags & ABC_METHOD_HAS_PARAM_NAMES)
  {
    int i;
    iprintf("    Parameter Names:\n");
    for(i = 0; i < minfo->ParamCount; i++)
    {
      int strIndex = minfo->ParamType[i];
      outputStringConstant(abc, strIndex);
    }
  }
}


void outputABC_CONSTANT_POOL(struct ABC_CONSTANT_POOL *cpool)
{
  iprintf("  ConstantPool: \n");
  iprintf("   Integers: %u, Unsigend %u Doubles %u\n", 
    cpool->IntCount, cpool->UIntCount, cpool->DoubleCount);
  iprintf("   Strings %u, Namespaces %u, NS-Sets %u, Multinames %u\n\n",
    cpool->StringCount, cpool->NamespaceCount, cpool->NamespaceSetCount,
    cpool->MultinameCount); 
}

void 
outputABC_METADATA_INFO(struct ABC_FILE *abc, struct ABC_METADATA_INFO *mi)
{
  unsigned int i;

  iprintf("    Name: ");
  outputStringConstant(abc, mi->Name);

  for(i = 0; i < mi->ItemCount; i++)
  {
    iprintf("    Key (%u) ", mi->Items[i].Key);
    outputStringConstant(abc, mi->Items[i].Key);
    iprintf("    Value (%u) ", mi->Items[i].Value);
    outputStringConstant(abc, mi->Items[i].Value);
    iprintf("\n");
  }
}

void 
outputABC_TRAIT_SLOT(struct ABC_FILE *abc, struct ABC_TRAIT_SLOT *ts)
{
  iprintf("   Trait Slot\n");
  iprintf("    SlotId %u\n", ts->SlotId);
  iprintf("    Type Name ");
  if(ts->TypeName)
    outputMultinameConstant(abc, ts->TypeName);
  else
    iprintf(" * ");
  iprintf("\n");
  
  iprintf("    VIndex %u\n", ts->VIndex);
  if(ts->VIndex)
    iprintf("    VKind %u\n", ts->VKind);
}

void
outputABC_TRAIT_CLASS(struct ABC_FILE *abc, struct ABC_TRAIT_CLASS *tc)
{
  iprintf("   Trait Class\n");
  iprintf("    SlotId %u\n", tc->SlotId);
  iprintf("    Class Index %u\n", tc->ClassIndex);
}

void
outputABC_TRAIT_FUNCTION(struct ABC_FILE *abc, struct ABC_TRAIT_FUNCTION *tf)
{
  iprintf("   Trait Function\n");
  iprintf("    SlotId %u\n", tf->SlotId);
  iprintf("    Method Index %u\n", tf->Function);
}

void
outputABC_TRAIT_METHOD(struct ABC_FILE *abc, struct ABC_TRAIT_METHOD *tm)
{
  iprintf("   Trait Method\n");
  iprintf("    DispId %u\n", tm->DispId);
  iprintf("    Method Index %u\n", tm->Method);
}


void 
outputABC_TRAITS_INFO(struct ABC_FILE *abc, struct ABC_TRAITS_INFO *ti)
{
  iprintf("    Name: ");
  outputMultinameConstant(abc, ti->Name);
  iprintf("\n");
  
  switch(ti->Kind & 0xf)
  {
    case ABC_CONST_TRAIT_SLOT:
    case ABC_CONST_TRAIT_CONST:
      outputABC_TRAIT_SLOT(abc, &ti->Data.Slot);
      break;
    case ABC_CONST_TRAIT_CLASS:
      outputABC_TRAIT_CLASS(abc, &ti->Data.Class);
      break;
    case ABC_CONST_TRAIT_FUNCTION:
      outputABC_TRAIT_FUNCTION(abc, &ti->Data.Function);
      break;
    case ABC_CONST_TRAIT_METHOD:
    case ABC_CONST_TRAIT_GETTER:
    case ABC_CONST_TRAIT_SETTER:
      outputABC_TRAIT_METHOD(abc, &ti->Data.Method);
      break;
    default:
      iprintf("unknown trait %x\n", ti->Kind);
  }
  
  iprintf("    Trait Attr %x\n", ti->Attr);
  if(ti->Attr & ABC_TRAIT_ATTR_METADATA)
  {
    unsigned int i;
    iprintf("    Trait Metadata Num %u\n", ti->MetadataCount);
    for(i = 0; i < ti->MetadataCount; i++)
    {
      iprintf("     Metadata[%u] -> %u\n", i, ti->Metadata[i]);
    }
  }
}

void 
outputABC_INSTANCE_INFO(struct ABC_FILE *abc, struct ABC_INSTANCE_INFO *ii)
{
  unsigned int i; 

  iprintf("    Name: ");
  outputStringConstant(abc, ii->Name);
  iprintf("    SuperName: ");
  outputStringConstant(abc, ii->SuperName);
  iprintf("    Flags %x\n", ii->Flags);
  
  if(ii->Flags & ABC_CLASS_PROTECTED_NS)
  { 
    iprintf("    Protected NS ");
    outputNamespaceConstant(abc, ii->ProtectedNs);
  }
  
  iprintf("    Interfaces: (%u)\n", ii->InterfaceCount);
  for(i = 0; i < ii->InterfaceCount; i++)
  {
    iprintf("    Interface (%u)", i);
    outputMultinameConstant(abc, ii->Interfaces[i]);
  }
  iprintf("    Init Method #%u\n", ii->IInit);

  iprintf("    Traits (%u):\n", ii->TraitCount);
  for(i = 0; i < ii->TraitCount; i++)
  {
    iprintf("    Trait %u:\n", i);
    outputABC_TRAITS_INFO(abc, ii->Traits + i);
  }
}

void 
outputABC_CLASS_INFO(struct ABC_FILE *abc, struct ABC_CLASS_INFO *ci)
{
  unsigned int i;

  iprintf("    Init Method #%u\n", ci->CInit);

  iprintf("    Traits (%u):\n", ci->TraitCount);
  for(i = 0; i < ci->TraitCount; i++)
  {
    iprintf("    Trait %u:\n", i);
    outputABC_TRAITS_INFO(abc, ci->Traits + i);
  }
}

void 
outputABC_SCRIPT_INFO(struct ABC_FILE *abc, struct ABC_SCRIPT_INFO *si)
{
  unsigned int i;

  iprintf("    Init Method #%u\n", si->Init);

  iprintf("    Traits (%u):\n", si->TraitCount);
  for(i = 0; i < si->TraitCount; i++)
  {
    iprintf("    Trait %u:\n", i);
    outputABC_TRAITS_INFO(abc, si->Traits + i);
  }
}

void
outputABC_EXCEPTION_INFO(struct ABC_FILE *abc, struct ABC_EXCEPTION_INFO *ei)
{
  iprintf("    From: %u\n", ei->From);
  iprintf("    To: %u\n", ei->To);
  iprintf("    Target: %u\n", ei->Target);
  iprintf("    ExcType: ");
  outputStringConstant(abc, ei->ExcType);
  iprintf("    VarName: ");
  outputStringConstant(abc, ei->VarName); 
}

void 
outputABC_METHOD_BODY_INFO(struct ABC_FILE *abc, struct ABC_METHOD_BODY_INFO *mb)
{
  unsigned int i;

  iprintf("    Method Index -> %u\n", mb->Method);
  iprintf("    Max Stack %u\n", mb->MaxStack);
  iprintf("    LocalCount %u\n", mb->LocalCount);
  iprintf("    InitScopeDepth %u\n", mb->InitScopeDepth);
  iprintf("    MaxScopeDepth %u\n", mb->CodeLength);
  iprintf("    CodeLength %u\n", mb->CodeLength);
  
  iprintf("    ExceptionCount %u\n", mb->ExceptionCount);
  for(i = 0; i < mb->ExceptionCount; i++)
  {
    iprintf("    Exception [%u]: \n", i);
    outputABC_EXCEPTION_INFO(abc, mb->Exceptions + i);
  }

  iprintf("    Traits (%u):\n", mb->TraitCount);
  for(i = 0; i < mb->TraitCount; i++)
  {
    iprintf("    Trait [%u]:\n", i);
    outputABC_TRAITS_INFO(abc, mb->Traits + i);
  }
}

void outputABC_FILE(struct ABC_FILE *abc)
{
  unsigned int i;

  iprintf(" Version %i.%i\n", abc->Major, abc->Minor);
  outputABC_CONSTANT_POOL(&abc->ConstantPool);

  iprintf(" MethodCount %u\n", abc->MethodCount);
  for(i = 0; i < abc->MethodCount; i++)
  {
    iprintf("  Method Info[%u]:\n", i);
    outputABC_METHOD_INFO(abc, abc->Methods + i);
    iprintf("  ### Method done ###\n\n");
  }
  iprintf(" ### Method Info done ###\n\n");

  iprintf(" MetadataCount %u\n", abc->MetadataCount);
  for(i = 0; i < abc->MetadataCount; i++)
  {
    iprintf("  Metadata [%u]:\n", i);
    outputABC_METADATA_INFO(abc, abc->Metadata + i);
    iprintf("  ### Metadata done ###\n\n");
  }
  iprintf(" ### Metadata Info done ###\n\n");

  iprintf(" InstanceCount %u\n", abc->ClassCount);
  for(i = 0; i < abc->ClassCount; i++)
  {
    iprintf("  Instance [%u]:\n", i);
    outputABC_INSTANCE_INFO(abc, abc->Instances + i);
    iprintf("  ### Instance done ###\n\n");
  }
  iprintf(" ### Instances Info done ###\n\n");
  
  iprintf(" ClassCount %u\n", abc->ClassCount);
  for(i = 0; i < abc->ClassCount; i++)
  {
    iprintf("  Class [%u]:\n", i);
    outputABC_CLASS_INFO(abc, abc->Classes + i);
    iprintf("  ### Class done ###\n\n");
  }
  iprintf(" ### Class Info done ###\n\n");

  iprintf(" ScriptCount %u\n", abc->ScriptCount);
  for(i = 0; i < abc->ScriptCount; i++)
  {
    iprintf("  Script [%u]:\n", i);
    outputABC_SCRIPT_INFO(abc, abc->Scripts + i);
    iprintf("  ### Script done ###\n\n");
  }
  iprintf(" ### Script Info done ###\n\n");

  iprintf(" MethodBodyCount %u\n", abc->MethodBodyCount);
  for(i = 0; i < abc->MethodBodyCount; i++)
  {
    iprintf("  Method Body [%u]:\n", i);
    outputABC_METHOD_BODY_INFO(abc, abc->MethodBodies + i);
    iprintf("  ### Method Body done ###\n\n");
  }
  iprintf(" ### Method Body Info done ###\n\n"); 
}

void
outputSWF_DOABC(SWF_Parserstruct *pblock)
{
  OUT_BEGIN (SWF_DOABC);
  iprintf(" ActionFlags: %x\n", sblock->Flags);
  iprintf(" Name %s\n", sblock->Name);
  outputABC_FILE(&sblock->AbcFile);
}

void 
outputSWF_SYMBOLCLASS(SWF_Parserstruct *pblock)
{
  int count, i;
  OUT_BEGIN(SWF_SYMBOLCLASS);
  count = sblock->SymbolCount;
  iprintf("SymbolCount %i\n", count);
  for(i = 0; i < count; i++)
  {
    iprintf(" Id: %i, Name: %s\n", 
      sblock->SymbolList[i].SymbolId, sblock->SymbolList[i].SymbolName);
  }
}

void 
outputSWF_DEFINESCENEANDFRAMEDATA(SWF_Parserstruct *pblock)
{
  int i;
  OUT_BEGIN(SWF_DEFINESCENEANDFRAMEDATA);
  iprintf(" SceneCount: %d\n", sblock->SceneCount);
  for(i = 0; i < sblock->SceneCount; i++)
    iprintf("  Scene #%d: Offset: %d, Name: %s\n", 
	i, sblock->Scenes[i].Offset, sblock->Scenes[i].Name);

  iprintf(" FrameLabelCount: %d\n", sblock->FrameLabelCount);
  for(i = 0; i < sblock->FrameLabelCount; i++)
    iprintf("  FrameLabel #%d: Frame: %d, Name: %s\n", 
	i, sblock->Frames[i].FrameNum, sblock->Frames[i].FrameLabel);
}



void
printRect(struct Rect *r)
{
	iprintf("(%i,%i)x(%i,%i)", r->xMin, r->xMax, r->yMin, r->yMax);
}

void
outputHeader (struct Movie *m)
{
	iprintf("File version: %i\n", m->version);
	iprintf("File size: %i\n", m->size);

	iprintf("Frame size: ");
	printRect(&(m->frame));
	putchar('\n');

	iprintf("Frame rate: %f / sec.\n", m->rate);
	iprintf("Total frames: %i\n", m->nFrames);
}

void
outputTrailer (struct Movie *m)
{
}

void
outputBlock (int type, SWF_Parserstruct * blockp, FILE* stream)
{
  int i;

  if(blockp == NULL)
	return;

  int offset = blockp->offset;
  int length = blockp->length;

  if (type < 0)
    return;

#if defined(ACTIONONLY)
  if( type != SWF_DOACTION &&
      type != SWF_INITACTION &&
      type != SWF_DEFINEBUTTON2 &&
      type != SWF_PLACEOBJECT2 ) return;
#endif

  putchar('\n');
  iprintf( "Offset: %d (0x%4.4x)\n", offset, offset );
  iprintf( "Block type: %d (%s)\n", type, blockName(type) );
  iprintf( "Block length: %d\n", length );
  putchar('\n');

  for (i = 0; i < numOutputs; i++)
    {
      if (outputs[i].type == type)
	{
	  return outputs[i].output (blockp);
	}
    }
  iprintf("Unknown block type %d\n", type );
  return;
}
