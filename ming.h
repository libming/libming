/*
    Ming, an SWF output library
    Copyright (C) 2000  Opaque Industries - http://www.opaque.net/

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

#include <stdio.h>

#define MING_VERSION        0.0.9c
#define MING_VERSION_TEXT  "0.0.9c"

int Ming_init();


typedef enum
{
  SWF_DEFINESHAPE		=  2,
  SWF_DEFINESHAPE2		= 22,
  SWF_DEFINESHAPE3		= 32,
  SWF_DEFINEMORPHSHAPE	        = 46,
  SWF_DEFINEBITS		=  6,
  SWF_DEFINEBITSJPEG2		= 21,
  SWF_DEFINEBITSJPEG3		= 35,
  SWF_DEFINELOSSLESS		= 20,
  SWF_DEFINELOSSLESS2		= 36,
  SWF_JPEGTABLES		=  8,
  SWF_DEFINEBUTTON		=  7,
  SWF_DEFINEBUTTON2		= 34,
  SWF_DEFINEBUTTONCXFORM	= 23,
  SWF_DEFINEBUTTONSOUND	        = 17,
  SWF_DEFINEFONT		= 10,
  SWF_DEFINEFONT2		= 48,
  SWF_DEFINEFONTINFO		= 13,
  SWF_DEFINETEXT		= 11,
  SWF_DEFINETEXT2		= 33,
  SWF_DEFINESOUND		= 14,
  SWF_SOUNDSTREAMBLOCK	        = 19,
  SWF_SOUNDSTREAMHEAD		= 18,
  SWF_SOUNDSTREAMHEAD2	        = 45,
  SWF_DEFINESPRITE		= 39,
  SWF_PLACEOBJECT		=  4,
  SWF_PLACEOBJECT2		= 26,
  SWF_REMOVEOBJECT		=  5,
  SWF_REMOVEOBJECT2		= 28,
  SWF_SHOWFRAME			=  1,
  SWF_SETBACKGROUNDCOLOR	=  9,
  SWF_FRAMELABEL		= 43,
  SWF_PROTECT			= 24,
  SWF_STARTSOUND		= 15,
  SWF_END			=  0,
  SWF_DOACTION		        = 12
} SWFBlocktype;


typedef unsigned char byte;


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

SWFRect SWFCharacter_getBounds(SWFCharacter character);
byte SWFBlock_isCharacter(SWFBlock block);


  /* SWFBitmap */

typedef void *SWFBitmap;

void destroySWFBitmap(SWFBlock block);
int SWFBitmap_getWidth(SWFBitmap b);
int SWFBitmap_getHeight(SWFBitmap b);


  /* SWFDBLBitmap */

typedef void *SWFDBLBitmap;

SWFDBLBitmap newSWFDBLBitmap(FILE *f);


  /* SWFJpegBitmap */

typedef void *SWFJpegBitmap, *SWFJpegWithAlpha;

SWFJpegBitmap newSWFJpegBitmap(FILE *f);
SWFJpegWithAlpha newSWFJpegWithAlpha(FILE *f, FILE *alpha);


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

void SWFShape_movePenTo(SWFShape shape, int x, int y);
void SWFShape_movePen(SWFShape shape, int x, int y);

int SWFShape_getPenX(SWFShape shape);
int SWFShape_getPenY(SWFShape shape);

void SWFShape_drawLineTo(SWFShape shape, int x, int y);
void SWFShape_drawLine(SWFShape shape, int dx, int dy);
void SWFShape_drawCurveTo(SWFShape shape,
			  int controlx, int controly,
			  int anchorx, int anchory);
void SWFShape_drawCurve(SWFShape shape,
			int controldx, int controldy,
			int anchordx, int anchordy);

/* deprecated: */

#define SWFShape_moveTo SWFShape_movePenTo
#define SWFShape_moveToRelative SWFShape_movePen
#define SWFShape_lineTo SWFShape_drawLineTo
#define SWFShape_lineToRelative SWFShape_drawLine
#define SWFShape_curveTo SWFShape_drawCurveTo
#define SWFShape_curveToRelative SWFShape_drawCurve

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


  /* SWFFont */

typedef void *SWFFont;

SWFFont newSWFFont();
SWFFont loadSWFFontFromFile(FILE *file);
void destroySWFFont(SWFBlock block);

byte *SWFFont_findCharacterGlyph(SWFFont font, int c);

int SWFFont_getStringWidth(SWFFont font, const char *string);

/* XXX */
#define SWFFont_getWidth SWFFont_getStringWidth

short SWFFont_getAscent(SWFFont font);
short SWFFont_getDescent(SWFFont font);
short SWFFont_getLeading(SWFFont font);


  /* SWFText */

typedef void *SWFText;

SWFText newSWFText();
SWFText newSWFText2();
void destroySWFText(SWFBlock block);

void SWFText_setFont(SWFText text, SWFFont font);
void SWFText_setHeight(SWFText text, int height);
void SWFText_moveTo(SWFText text, int x, int y);
void SWFText_setColor(SWFText text, byte r, byte g, byte b, byte a);
void SWFText_addString(SWFText text, const char *string, int *advance);
void SWFText_setSpacing(SWFText text, float spacing);

int SWFText_getStringWidth(SWFText text, const char *string);

/* XXX */
#define SWFText_getWidth SWFText_getStringWidth

short SWFText_getAscent(SWFText text);
short SWFText_getDescent(SWFText text);
short SWFText_getLeading(SWFText text);

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
void SWFTextField_setBounds(SWFTextField field, int width, int height);
void SWFTextField_setFlags(SWFTextField field, int flags);
void SWFTextField_setColor(SWFTextField field, byte r, byte g, byte b, byte a);
void SWFTextField_setVariableName(SWFTextField field, char *name);
void SWFTextField_addString(SWFTextField field, char *string);

void SWFTextField_setHeight(SWFTextField field, int height);
void SWFTextField_setLeftMargin(SWFTextField field, int leftMargin);
void SWFTextField_setRightMargin(SWFTextField field, int rightMargin);
void SWFTextField_setIndentation(SWFTextField field, int indentation);
void SWFTextField_setLineSpacing(SWFTextField field, int lineSpacing);
void SWFTextField_setAlignment(SWFTextField field,
			       SWFTextFieldAlignment alignment);
void SWFTextField_setLength(SWFTextField field, int length);


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


/* position.h */

struct _swfPosition
{
  int x;
  int y;
  float xScale;
  float yScale;
  float xSkew;
  float ySkew;
  float rot;
  SWFMatrix matrix;
};

typedef struct _swfPosition *SWFPosition;

#define SWFPOSITION_SIZE sizeof(struct _swfPosition)

void destroySWFPosition(SWFPosition position);
SWFPosition newSWFPosition(SWFMatrix matrix);

void SWFPosition_skewX(SWFPosition position, float x);
void SWFPosition_skewXTo(SWFPosition position, float x);
void SWFPosition_skewY(SWFPosition position, float y);
void SWFPosition_skewYTo(SWFPosition position, float y);
void SWFPosition_scaleX(SWFPosition position, float x);
void SWFPosition_scaleXTo(SWFPosition position, float x);
void SWFPosition_scaleY(SWFPosition position, float y);
void SWFPosition_scaleYTo(SWFPosition position, float y);
void SWFPosition_scaleXY(SWFPosition position, float x, float y);
void SWFPosition_scaleXYTo(SWFPosition position, float x, float y);
void SWFPosition_setMatrix(SWFPosition p, float a, float b, float c, float d,
			   int x, int y);

void SWFPosition_rotate(SWFPosition position, float degrees);
void SWFPosition_rotateTo(SWFPosition position, float degrees);
void SWFPosition_move(SWFPosition position, int x, int y);
void SWFPosition_moveTo(SWFPosition position, int x, int y);


/* blocklist.h */

/* we have to note which blocks are characters b/c characters may be destroyed
   before we have a chance to look at them */

struct _swfBlockListEntry
{
  SWFBlock block;
  byte isCharacter;
};
typedef struct _swfBlockListEntry swfBlockEntry;

struct _swfBlockList
{
  swfBlockEntry *blocks;
  int nBlocks;
};
typedef struct _swfBlockList *SWFBlockList;

#define SWFBLOCKLIST_SIZE sizeof(struct _swfBlockList)

#define SWFBLOCKLIST_INCREMENT 16

SWFBlockList newSWFBlockList();
void SWFBlockList_addBlock(SWFBlockList blocklist, SWFBlock block);

int SWFBlockList_completeBlocks(SWFBlockList list);
int SWFBlockList_writeBlocksToMethod(SWFBlockList list,
				     SWFByteOutputMethod method, void *data);

void destroySWFBlockList(SWFBlockList list);


/* displaylist.h */

#define ITEM_NEW            (1<<0)
#define ITEM_REMOVED        (1<<1)
#define ITEM_DISPLACED      (1<<2)
#define ITEM_TRANSFORMED    (1<<3)
#define ITEM_CXFORMCHANGED  (1<<4)
#define ITEM_RATIOCHANGED   (1<<5)

struct _swfDisplayItem
{
  int flags;
  struct _swfDisplayItem *next;
  int depth;
  SWFPosition position;
  SWFMatrix matrix;
  SWFCharacter character;
  int ratio;
  SWFCXform cXform;
  char *name;
};
typedef struct _swfDisplayItem *SWFDisplayItem;

#define SWFDISPLAYITEM_SIZE sizeof(struct _swfDisplayItem)

void SWFDisplayItem_rotate(SWFDisplayItem item, float degrees);
void SWFDisplayItem_rotateTo(SWFDisplayItem item, float degrees);
void SWFDisplayItem_move(SWFDisplayItem item, int x, int y);
void SWFDisplayItem_moveTo(SWFDisplayItem item, int x, int y);
void SWFDisplayItem_scale(SWFDisplayItem item, float xScale, float yScale);
void SWFDisplayItem_scaleTo(SWFDisplayItem item, float xScale, float yScale);
void SWFDisplayItem_skewX(SWFDisplayItem item, float x);
void SWFDisplayItem_skewXTo(SWFDisplayItem item, float x);
void SWFDisplayItem_skewY(SWFDisplayItem item, float y);
void SWFDisplayItem_skewYTo(SWFDisplayItem item, float y);
void SWFDisplayItem_setMatrix(SWFDisplayItem i,
			      float a, float b, float c, float d, int x, int y);

int SWFDisplayItem_getDepth(SWFDisplayItem item);
void SWFDisplayItem_setDepth(SWFDisplayItem item, int depth);
void SWFDisplayItem_remove(SWFDisplayItem item);
void SWFDisplayItem_setName(SWFDisplayItem item, const char *name);
void SWFDisplayItem_setRatio(SWFDisplayItem item, float ratio);
void SWFDisplayItem_setCXform(SWFDisplayItem item, SWFCXform cXform);
void SWFDisplayItem_setColorAdd(SWFDisplayItem item,
				int r, int g, int b, int a);
void SWFDisplayItem_setColorMult(SWFDisplayItem item,
				 float r, float g, float b, float a);

#define SWFDisplayItem_addColor SWFDisplayItem_setColorAdd
#define SWFDisplayItem_multColor SWFDisplayItem_setColorMult

struct _swfDisplayList
{
  SWFSound soundStream;
  SWFDisplayItem head;
  SWFDisplayItem tail;
  byte isSprite;
  int depth;
};
typedef struct _swfDisplayList *SWFDisplayList;

#define SWFDISPLAYLIST_SIZE sizeof(struct _swfDisplayList)

void destroySWFDisplayList(SWFDisplayList list);
SWFDisplayList newSWFDisplayList();
SWFDisplayList newSWFSpriteDisplayList();
void SWFDisplayList_nextFrame(SWFDisplayList list);

SWFDisplayItem SWFDisplayList_add(SWFDisplayList list, SWFCharacter shape);
void SWFDisplayList_writeBlocks(SWFDisplayList list, SWFBlockList blocklist);

void SWFDisplayList_setSoundStream(SWFDisplayList list, SWFSound sound);


/* SWFFill adds a position object to manipulate SWFFillStyle's matrix. */

struct _swfFill
{
  SWFFillStyle fillstyle;
  SWFPosition position;
};
typedef struct _swfFill *SWFFill;

#define SWFFILL_SIZE (sizeof(struct _swfFill))

static inline SWFFill newSWFFill(SWFFillStyle fillstyle)
{
  SWFFill fill = (SWFFill)malloc(SWFFILL_SIZE);
  fill->fillstyle = fillstyle;
  fill->position = newSWFPosition(SWFFillStyle_getMatrix(fill->fillstyle));
  return fill;
}
static inline void destroySWFFill(SWFFill fill)
{
  destroySWFPosition(fill->position);
  free(fill);
}

static inline void SWFFill_skewX(SWFFill fill, float x)
{
  SWFPosition_skewX(fill->position, x);
}
static inline void SWFFill_skewXTo(SWFFill fill, float x)
{
  SWFPosition_skewXTo(fill->position, x);
}
static inline void SWFFill_skewY(SWFFill fill, float y)
{
  SWFPosition_skewY(fill->position, y);
}
static inline void SWFFill_skewYTo(SWFFill fill, float y)
{
  SWFPosition_skewYTo(fill->position, y);
}
static inline void SWFFill_scaleX(SWFFill fill, float x)
{
  SWFPosition_scaleX(fill->position, x);
}
static inline void SWFFill_scaleXTo(SWFFill fill, float x)
{
  SWFPosition_scaleXTo(fill->position, x);
}
static inline void SWFFill_scaleY(SWFFill fill, float y)
{
  SWFPosition_scaleY(fill->position, y);
}
static inline void SWFFill_scaleYTo(SWFFill fill, float y)
{
  SWFPosition_scaleYTo(fill->position, y);
}

/* deprecated: */
static inline void SWFFill_scaleXY(SWFFill fill, float x, float y)
{
  SWFPosition_scaleXY(fill->position, x, y);
}
static inline void SWFFill_scaleXYTo(SWFFill fill, float x, float y)
{
  SWFPosition_scaleXYTo(fill->position, x, y);
}

static inline void SWFFill_scale(SWFFill fill, float x, float y)
{
  SWFPosition_scaleXY(fill->position, x, y);
}
static inline void SWFFill_scaleTo(SWFFill fill, float x, float y)
{
  SWFPosition_scaleXYTo(fill->position, x, y);
}
static inline void SWFFill_rotate(SWFFill fill, float degrees)
{
  SWFPosition_rotate(fill->position, degrees);
}
static inline void SWFFill_rotateTo(SWFFill fill, float degrees)
{
  SWFPosition_rotateTo(fill->position, degrees);
}
static inline void SWFFill_move(SWFFill fill, int x, int y)
{
  SWFPosition_move(fill->position, x, y);
}
static inline void SWFFill_moveTo(SWFFill fill, int x, int y)
{
  SWFPosition_moveTo(fill->position, x, y);
}
static inline void SWFFill_setMatrix(SWFFill fill, float a, float b, float c, float d, int x, int y)
{
  SWFPosition_setMatrix(fill->position, a, b, c, d, x, y);
}


/* shape_util.h */

static inline void SWFShape_setLine(SWFShape shape, unsigned short width,
				    byte r, byte g, byte b, byte a)
{
  SWFShape_setLineStyle(shape, width, r, g, b, a);
}

static inline SWFFill SWFShape_addSolidFill(SWFShape shape,
					    byte r, byte g, byte b, byte a)
{
  return newSWFFill(SWFShape_addSolidFillStyle(shape, r, g, b, a));
}
static inline SWFFill SWFShape_addGradientFill(SWFShape shape,
					       SWFGradient gradient,
					       byte flags)
{
  return newSWFFill(SWFShape_addGradientFillStyle(shape, gradient, flags));
}
static inline SWFFill SWFShape_addBitmapFill(SWFShape shape,
					     SWFBitmap bitmap, byte flags)
{
  return newSWFFill(SWFShape_addBitmapFillStyle(shape, bitmap, flags));
}

static inline void SWFShape_setLeftFill(SWFShape shape, SWFFill fill)
{
  SWFShape_setLeftFillStyle(shape, fill==NULL ? NULL : fill->fillstyle);
}
static inline void SWFShape_setRightFill(SWFShape shape, SWFFill fill)
{
  SWFShape_setRightFillStyle(shape, fill==NULL ? NULL : fill->fillstyle);
}

static inline void SWFShape_drawRect(SWFShape shape, SWFRect rect)
{
  SWFShape_lineToRelative(shape, SWFRect_getWidth(rect), 0);
  SWFShape_lineToRelative(shape, 0, SWFRect_getHeight(rect));
  SWFShape_lineToRelative(shape, -SWFRect_getWidth(rect), 0);
  SWFShape_lineToRelative(shape, 0, -SWFRect_getHeight(rect));
}

void SWFShape_setLine(SWFShape shape,
		      unsigned short width, byte r, byte g, byte b, byte a);

void SWFShape_drawArc(SWFShape shape, int r, float startAngle, float endAngle);

static inline void SWFShape_drawCircle(SWFShape shape, int r)
{
  SWFShape_drawArc(shape, r, 0, 360);
}

/* draw character c from font font into shape shape */
void SWFShape_drawGlyph(SWFShape shape, SWFFont font, int c);

/* approximate a cubic bezier with quadratic segments */
/* returns the number of segments used */
int SWFShape_drawCubic(SWFShape shape, int bx, int by, int cx, int cy, int dx, int dy);
int SWFShape_drawCubicTo(SWFShape shape, int bx, int by, int cx, int cy, int dx, int dy);

/* sets the threshold error for drawing cubic beziers.  Lower is more
   accurate, hence larger file size. */

void Ming_setCubicThreshold(int num);


/* deprecated: */

#define SWFShape_drawFontGlyph SWFShape_drawGlyph


typedef void *SWFMovieClip;

void destroySWFMovieClip(SWFMovieClip clip);
SWFMovieClip newSWFMovieClip();

void SWFMovieClip_setNumberOfFrames(SWFMovieClip clip, int frames);
SWFDisplayItem SWFMovieClip_add(SWFMovieClip clip, SWFBlock block);
void SWFMovieClip_remove(SWFMovieClip clip, SWFDisplayItem item);
void SWFMovieClip_nextFrame(SWFMovieClip clip);
void SWFMovieClip_labelFrame(SWFMovieClip clip, char *label);

int SWFMovieClip_output(SWFMovieClip clip, SWFByteOutputMethod method, void *data);


/* movie.h */

struct _swfMovie
{
  SWFBlockList blockList;
  SWFDisplayList displayList;
  float rate;
  SWFRect bounds;
  unsigned short nFrames;
  unsigned short totalFrames;
  byte version;
};
typedef struct _swfMovie *SWFMovie;

#define SWFMOVIE_SIZE sizeof(struct _swfMovie)

void destroySWFMovie(SWFMovie movie);
SWFMovie newSWFMovie();
SWFMovie newSWFMovieWithVersion(int version);

void SWFMovie_setRate(SWFMovie movie, float rate);
void SWFMovie_setDimension(SWFMovie movie, int x, int y);
void SWFMovie_setNumberOfFrames(SWFMovie movie, int frames);

/* XXX - 0.1 name: */
#define SWFMovie_setFrames SWFMovie_setNumberOfFrames

void SWFMovie_setBackground(SWFMovie movie, int r, int g, int b);

void SWFMovie_setSoundStream(SWFMovie movie, SWFSound sound);

SWFDisplayItem SWFMovie_add(SWFMovie movie, SWFBlock block);
void SWFMovie_remove(SWFMovie movie, SWFDisplayItem item);
void SWFMovie_nextFrame(SWFMovie movie);
void SWFMovie_labelFrame(SWFMovie movie, char *label);

int SWFMovie_output(SWFMovie movie, SWFByteOutputMethod method, void *data);

static inline int SWFMovie_save(SWFMovie movie, char *filename)
{
  FILE *f = fopen(filename, "wb");
  int count;

  if(f == NULL)
    return -1;

  count = SWFMovie_output(movie, fileOutputMethod, f);
  fclose(f);
  return count;
}
