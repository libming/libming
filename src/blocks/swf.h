/*
    Ming, an SWF output library
    Copyright (C) 2001  Opaque Industries - http://www.opaque.net/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SWF_H_INCLUDED
#define SWF_H_INCLUDED

#include <stdio.h>

#include "blocktypes.h"

typedef unsigned char byte;


extern void (*SWF_warn)(char *msg, ...);
extern void (*SWF_error)(char *msg, ...);

void setSWFWarnFunction(void (*error)(char *msg, ...));
void setSWFErrorFunction(void (*error)(char *msg, ...));

void SWF_assert(int c);


  /* a generic output method.  specific instances dump output to file,
     send to stdout, etc. */

typedef void (*SWFByteOutputMethod)(byte b, void *data);

void methodWriteUInt16(int i, SWFByteOutputMethod method, void *data);
void methodWriteUInt32(int i, SWFByteOutputMethod method, void *data);

void fileOutputMethod(byte b, void *data);


/* we dig opaque types */


  /* SWFBlock is the parent for all classes in this directory */

typedef void *SWFBlock;

int completeSWFBlock(SWFBlock block);
void destroySWFBlock(SWFBlock block);
int writeSWFBlockToMethod(SWFBlock block,
			  SWFByteOutputMethod method, void *data);

void SWFBlock_setDefined(SWFBlock block);
byte SWFBlock_isDefined(SWFBlock block);

SWFBlocktype SWFBlock_getType(SWFBlock block);


  /* SWFInput */

typedef void *SWFInput;

int SWFInput_length(SWFInput input);
void SWFInput_rewind(SWFInput input);
int SWFInput_tell(SWFInput input);
void SWFInput_seek(SWFInput input, long offset, int whence);
int SWFInput_eof(SWFInput input);

SWFInput newSWFInput_file(FILE *f);
SWFInput newSWFInput_stream(FILE *f);
SWFInput newSWFInput_buffer(unsigned char *buffer, int length);
SWFInput newSWFInput_allocedBuffer(unsigned char *buffer, int length);


  /* SWFOutput */

typedef void *SWFOutput;

SWFOutput newSWFOutput();
SWFOutput newSizedSWFOutput(int size);
void destroySWFOutput(SWFOutput out);

int SWFOutput_length(SWFOutput out);
void SWFOutput_byteAlign(SWFOutput out);
void SWFOutput_writeUInt16(SWFOutput out, int data);
void SWFOutput_writeToMethod(SWFOutput out,
			     SWFByteOutputMethod method, void *data);


  /* SWFRect */

typedef void *SWFRect;

SWFRect newSWFRect(int minX, int maxX, int minY, int maxY);
void destroySWFRect(SWFRect rect);
void SWFOutput_writeRect(SWFOutput output, SWFRect rect);

int SWFRect_getWidth(SWFRect r);
int SWFRect_getHeight(SWFRect r);


  /* SWFCharacter */

/* everything with a character ID is an SWFCharacter */

typedef void *SWFCharacter;

/* this is a silly hack to track blocks which are dependent on others.
   Would be nice to have this in the ming layer above instead */

void SWFCharacter_addDependency(SWFCharacter character, SWFBlock dependency);
SWFBlock *SWFCharacter_getDependencies(SWFCharacter character);
int SWFCharacter_getNDependencies(SWFCharacter character);
void SWFCharacter_clearDependencies(SWFCharacter character);

int SWFCharacter_getScaledWidth(SWFCharacter character);
int SWFCharacter_getScaledHeight(SWFCharacter character);

int SWFBlock_isCharacter(SWFBlock block);


  /* SWFBitmap */

typedef void *SWFBitmap;

void destroySWFBitmap(SWFBlock block);
int SWFBitmap_getWidth(SWFBitmap b);
int SWFBitmap_getHeight(SWFBitmap b);

SWFBitmap newSWFBitmap_fromInput(SWFInput input);


  /* SWFDBLBitmap */

typedef void *SWFDBLBitmap;

SWFDBLBitmap newSWFDBLBitmap(FILE *f);
SWFDBLBitmap newSWFDBLBitmap_fromInput(SWFInput input);


  /* SWFJpegBitmap */

typedef void *SWFJpegBitmap, *SWFJpegWithAlpha;

SWFJpegBitmap newSWFJpegBitmap(FILE *f);
SWFJpegBitmap newSWFJpegBitmap_fromInput(SWFInput input);
SWFJpegWithAlpha newSWFJpegWithAlpha(FILE *f, FILE *alpha);
SWFJpegWithAlpha newSWFJpegWithAlpha_fromInput(SWFInput input, SWFInput alpha);


  /* SWFGradient */

typedef void *SWFGradient;

SWFGradient newSWFGradient();
void destroySWFGradient(SWFGradient gradient);
void SWFGradient_addEntry(SWFGradient gradient,
			  float ratio, byte r, byte g, byte b, byte a);


  /* SWFMatrix */

typedef void *SWFMatrix;

SWFMatrix newSWFMatrix(int a, int b, int c, int d, int x, int y);
void destroySWFMatrix(SWFMatrix matrix);

SWFMatrix SWFMatrix_set(SWFMatrix m,
			float a, float b, float c, float d, int x, int y);
void SWFMatrix_clearTranslate(SWFMatrix m);
void SWFMatrix_clearTransform(SWFMatrix m);

void SWFMatrix_copy(SWFMatrix src, SWFMatrix dest);


  /* SWFFont */

typedef void *SWFFont;

SWFFont newSWFFont();
SWFFont loadSWFFontFromFile(FILE *file);
void destroySWFFont(SWFBlock block);

int SWFFont_getScaledStringWidth(SWFFont font, const char *string);

short SWFFont_getScaledAscent(SWFFont font);
short SWFFont_getScaledDescent(SWFFont font);
short SWFFont_getScaledLeading(SWFFont font);


  /* SWFText */

typedef void *SWFText;

SWFText newSWFText();
SWFText newSWFText2();
void destroySWFText(SWFBlock block);

void SWFText_setFont(SWFText text, SWFFont font);
void SWFText_setScaledHeight(SWFText text, int height);
void SWFText_scaledMoveTo(SWFText text, int x, int y);
void SWFText_setColor(SWFText text, byte r, byte g, byte b, byte a);
void SWFText_addString(SWFText text, const char *string, int *advance);
void SWFText_setSpacing(SWFText text, float spacing);

int SWFText_getScaledStringWidth(SWFText text, const char *string);

short SWFText_getScaledAscent(SWFText text);
short SWFText_getScaledDescent(SWFText text);
short SWFText_getScaledLeading(SWFText text);


/* deprecated: */
#define SWFText_setXY(t,x,y) SWFText_moveTo((t),(x),(y))


  /* SWFBrowserFont */

typedef void *SWFBrowserFont;

SWFBrowserFont newSWFBrowserFont(char *name);
void destroySWFBrowserFont(SWFBlock block);


  /* SWFTextField */

typedef void *SWFTextField;

#define SWFTEXTFIELD_ONMASK  0x2085 /* on bits */
#define SWFTEXTFIELD_OFFMASK 0x38FF /* off bits */

#define SWFTEXTFIELD_HASLENGTH (1<<1)
#define SWFTEXTFIELD_NOEDIT    (1<<3)
#define SWFTEXTFIELD_PASSWORD  (1<<4)
#define SWFTEXTFIELD_MULTILINE (1<<5)
#define SWFTEXTFIELD_WORDWRAP  (1<<6)
#define SWFTEXTFIELD_DRAWBOX   (1<<11)
#define SWFTEXTFIELD_NOSELECT  (1<<12)
#define SWFTEXTFIELD_HTML      (1<<9) /* 0x0200 */

typedef enum
{
  SWFTEXTFIELD_ALIGN_LEFT    = 0,
  SWFTEXTFIELD_ALIGN_RIGHT   = 1,
  SWFTEXTFIELD_ALIGN_CENTER  = 2,
  SWFTEXTFIELD_ALIGN_JUSTIFY = 3
} SWFTextFieldAlignment;

SWFTextField newSWFTextField();
void destroySWFTextField(SWFBlock block);

void SWFTextField_setFont(SWFTextField field, SWFBlock font);
void SWFTextField_setScaledBounds(SWFTextField field, int width, int height);
void SWFTextField_setFlags(SWFTextField field, int flags);
void SWFTextField_setColor(SWFTextField field, byte r, byte g, byte b, byte a);
void SWFTextField_setVariableName(SWFTextField field, char *name);
void SWFTextField_addString(SWFTextField field, char *string);

void SWFTextField_setScaledFontHeight(SWFTextField field, int height);
void SWFTextField_setScaledFieldHeight(SWFTextField field, int height);
void SWFTextField_setScaledLeftMargin(SWFTextField field, int leftMargin);
void SWFTextField_setScaledRightMargin(SWFTextField field, int rightMargin);
void SWFTextField_setScaledIndentation(SWFTextField field, int indentation);
void SWFTextField_setScaledLineSpacing(SWFTextField field, int lineSpacing);
void SWFTextField_setAlignment(SWFTextField field,
			       SWFTextFieldAlignment alignment);
void SWFTextField_setLength(SWFTextField field, int length);


  /* SWFFillStyle */

typedef void *SWFFillStyle;

#define SWFFILL_SOLID		0x00
#define SWFFILL_GRADIENT	0x10
#define SWFFILL_LINEAR_GRADIENT 0x10
#define SWFFILL_RADIAL_GRADIENT 0x12
#define SWFFILL_BITMAP		0x40
#define SWFFILL_TILED_BITMAP	0x40
#define SWFFILL_CLIPPED_BITMAP	0x41

SWFFillStyle newSWFSolidFillStyle(byte r, byte g, byte b, byte a);
SWFFillStyle newSWFGradientFillStyle(SWFGradient gradient, byte radial);
SWFFillStyle newSWFBitmapFillStyle(SWFCharacter bitmap, byte flags);

SWFMatrix SWFFillStyle_getMatrix(SWFFillStyle fill);


/* SWFLineStyle */

typedef void *SWFLineStyle;

SWFLineStyle newSWFLineStyle(int width, int r, int g, int b, int a);
byte SWFLineStyle_equals(SWFLineStyle line, unsigned short width,
			 byte r, byte g, byte b, byte a);


  /* SWFShape */

typedef void *SWFShape;

SWFShape newSWFShape();
void destroySWFShape(SWFBlock block);

int SWFShape_getScaledPenX(SWFShape shape);
int SWFShape_getScaledPenY(SWFShape shape);

void SWFShape_moveScaledPenTo(SWFShape shape, int x, int y);
void SWFShape_moveScaledPen(SWFShape shape, int x, int y);

void SWFShape_drawScaledLineTo(SWFShape shape, int x, int y);
void SWFShape_drawScaledLine(SWFShape shape, int dx, int dy);
void SWFShape_drawScaledCurveTo(SWFShape shape, int controlx, int controly,
				int anchorx, int anchory);
void SWFShape_drawScaledCurve(SWFShape shape, int controldx, int controldy,
			      int anchordx, int anchordy);

void SWFShape_drawGlyph(SWFShape shape, SWFFont font, unsigned char c);


/* deprecated: */

#define SWFShape_moveTo SWFShape_moveScaledPenTo
#define SWFShape_moveToRelative SWFShape_moveScaledPen
#define SWFShape_lineTo SWFShape_drawScaledLineTo
#define SWFShape_lineToRelative SWFShape_drawScaledLine
#define SWFShape_curveTo SWFShape_drawScaledCurveTo
#define SWFShape_curveToRelative SWFShape_drawScaledCurve

void SWFShape_end(SWFShape shape);

int SWFShape_setLineStyle(SWFShape shape, unsigned short width,
			  byte r, byte g, byte b, byte a);

SWFFillStyle SWFShape_addSolidFillStyle(SWFShape shape,
					byte r, byte g, byte b, byte a);

SWFFillStyle SWFShape_addGradientFillStyle(SWFShape shape,
					   SWFGradient gradient, byte flags);

SWFFillStyle SWFShape_addBitmapFillStyle(SWFShape shape,
					 SWFBitmap bitmap, byte flags);

void SWFShape_setLeftFillStyle(SWFShape shape, SWFFillStyle fill);
void SWFShape_setRightFillStyle(SWFShape shape, SWFFillStyle fill);


  /* SWFMorph */

typedef void *SWFMorph;

SWFMorph newSWFMorphShape();
void destroySWFMorph(SWFBlock block);
SWFShape SWFMorph_getShape1(SWFMorph morph);
SWFShape SWFMorph_getShape2(SWFMorph morph);


  /* sound - only mp3 streaming implemented */

typedef void *SWFSound;

#define SWF_SOUND_COMPRESSION      0xf0
#define SWF_SOUND_NOT_COMPRESSED   (0<<4)
#define SWF_SOUND_ADPCM_COMPRESSED (1<<4)

#define SWF_SOUND_RATE             0x0c
#define SWF_SOUND_5KHZ             (0<<2)
#define SWF_SOUND_11KHZ            (1<<2)
#define SWF_SOUND_22KHZ            (2<<2)
#define SWF_SOUND_44KHZ            (3<<2)

#define SWF_SOUND_BITS             0x02
#define SWF_SOUND_8BIT             (0<<1)
#define SWF_SOUND_16BIT            (1<<1)

#define SWF_SOUND_CHANNELS         0x01
#define SWF_SOUND_MONO             (0<<0)
#define SWF_SOUND_STEREO           (1<<0)

SWFSound newSWFSound(FILE *file);
SWFSound newSWFSound_fromInput(SWFInput input);
void destroySWFSound(SWFSound sound);

SWFBlock SWFSound_getStreamHead(SWFSound sound, float frameRate);
SWFBlock SWFSound_getStreamBlock(SWFSound sound);
void SWFSound_rewind(SWFSound sound);


  /* SWFCXform */

typedef void *SWFCXform;

SWFCXform newSWFCXform(int rAdd, int gAdd, int bAdd, int aAdd,
		       float rMult, float gMult, float bMult, float aMult);

SWFCXform newSWFAddCXform(int rAdd, int gAdd, int bAdd, int aAdd);

SWFCXform newSWFMultCXform(float rMult, float gMult, float bMult, float aMult);

void SWFCXform_setColorAdd(SWFCXform cXform,
			   int rAdd, int gAdd, int bAdd, int aAdd);

void SWFCXform_setColorMult(SWFCXform cXform,
			    float rMult, float gMult, float bMult, float aMult);

void destroySWFCXform(SWFCXform cXform);


  /* random blocks */

typedef void *SWFOutputBlock;

SWFBlock newSWFPlaceObjectBlock(SWFCharacter character, int depth,
				SWFMatrix matrix, SWFCXform cXform);
SWFBlock newSWFPlaceObject2Block(int depth, char *name, int ratio,
				 SWFCXform cXform, SWFMatrix matrix,
				 SWFCharacter character, int move);

SWFOutputBlock newSWFRemoveObjectBlock(SWFCharacter character, int depth);
SWFOutputBlock newSWFRemoveObject2Block(int depth);
SWFOutputBlock newSWFFrameLabelBlock(char *string);
SWFBlock newSWFSetBackgroundBlock(byte r, byte g, byte b);
SWFBlock newSWFShowFrameBlock();
SWFBlock newSWFEndBlock();
SWFBlock newSWFProtectBlock();


  /* SWFAction */

typedef void *SWFAction;

SWFAction newSWFAction();
SWFAction newSWFAction_fromOutput(SWFOutput out);
SWFAction compileSWFActionCode(char *script);
void destroySWFAction(SWFAction action);


  /* SWFButton */

typedef void *SWFButton;

#define SWFBUTTON_HIT    (1<<3)
#define SWFBUTTON_DOWN   (1<<2)
#define SWFBUTTON_OVER   (1<<1)
#define SWFBUTTON_UP     (1<<0)

/* deprecated: */

  #define SWFBUTTONRECORD_HITSTATE    (1<<3)
  #define SWFBUTTONRECORD_DOWNSTATE   (1<<2)
  #define SWFBUTTONRECORD_OVERSTATE   (1<<1)
  #define SWFBUTTONRECORD_UPSTATE     (1<<0)


#define SWFBUTTON_KEYPRESS(c)     (((c)&0x7f)<<9)
#define SWFBUTTON_ONKEYPRESS(c)     (((c)&0x7f)<<9)

#define SWFBUTTON_OVERDOWNTOIDLE    (1<<8)
#define SWFBUTTON_IDLETOOVERDOWN    (1<<7)
#define SWFBUTTON_OUTDOWNTOIDLE     (1<<6)
#define SWFBUTTON_OUTDOWNTOOVERDOWN (1<<5)
#define SWFBUTTON_OVERDOWNTOOUTDOWN (1<<4)
#define SWFBUTTON_OVERDOWNTOOVERUP  (1<<3)
#define SWFBUTTON_OVERUPTOOVERDOWN  (1<<2)
#define SWFBUTTON_OVERUPTOIDLE      (1<<1)
#define SWFBUTTON_IDLETOOVERUP      (1<<0)

/* easier to remember: */
#define SWFBUTTON_MOUSEUPOUTSIDE  SWFBUTTON_OUTDOWNTOIDLE
#define SWFBUTTON_DRAGOVER        (SWFBUTTON_OUTDOWNTOOVERDOWN | SWFBUTTON_IDLETOOVERDOWN)
#define SWFBUTTON_DRAGOUT         (SWFBUTTON_OVERDOWNTOOUTDOWN | SWFBUTTON_OVERDOWNTOIDLE)
#define SWFBUTTON_MOUSEUP         SWFBUTTON_OVERDOWNTOOVERUP
#define SWFBUTTON_MOUSEDOWN       SWFBUTTON_OVERUPTOOVERDOWN
#define SWFBUTTON_MOUSEOUT        SWFBUTTON_OVERUPTOIDLE
#define SWFBUTTON_MOUSEOVER       SWFBUTTON_IDLETOOVERUP


SWFButton newSWFButton();
void destroySWFButton(SWFBlock block);

void SWFButton_addShape(SWFButton button, SWFCharacter character, byte flags);
void SWFButton_addAction(SWFButton button, SWFAction action, int flags);

SWFBlock newSWFButtonCXform(SWFButton button, SWFCXform *cXforms);
SWFBlock newSWFButtonSound(SWFButton button, SWFSound sound);


  /* SWFSprite */

typedef void *SWFSprite;

SWFSprite newSWFSprite();
void destroySWFSprite(SWFBlock block);

SWFSprite newSWFSprite_fromArray(SWFBlock *blocks, int nBlocks);
void SWFSprite_setBlocks(SWFSprite sprite, SWFBlock *blocks, int nBlocks);
void SWFSprite_addBlock(SWFSprite sprite, SWFBlock block);

#endif /* SWF_H_INCLUDED */
