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

#include <stdio.h>

#define MING_VERSION        0.1.1
#define MING_VERSION_TEXT  "0.1.1"

int Ming_init();

/* sets the threshold error for drawing cubic beziers.  Lower is more
   accurate, hence larger file size. */

void Ming_setCubicThreshold(int num);

/* sets the overall scale, default is 20 */
void Ming_setScale(float scale);

/* change the error/warn behavior.  Default prints message and exits. */
void Ming_setWarnFunction(void (*warn)(char *msg, ...));
void Ming_setErrorFunction(void (*error)(char *msg, ...));


typedef unsigned char byte;


  /* a generic output method.  specific instances dump output to file,
     send to stdout, etc. */

typedef void (*SWFByteOutputMethod)(byte b, void *data);

void fileOutputMethod(byte b, void *data);


/* we dig opaque types */

typedef void *SWFBlock, *SWFMatrix;


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

void destroySWFInput(SWFInput input);


  /* SWFCharacter */

/* everything with a character ID is an SWFCharacter */

typedef void *SWFCharacter;

float SWFCharacter_getWidth(SWFCharacter character);
float SWFCharacter_getHeight(SWFCharacter character);


  /* SWFBitmap */

typedef void *SWFBitmap;

void destroySWFBitmap(SWFBitmap b);
int SWFBitmap_getWidth(SWFBitmap b);
int SWFBitmap_getHeight(SWFBitmap b);

SWFBitmap newSWFBitmap_fromInput(SWFInput input);


  /* SWFDBLBitmap extends SWFBitmap */

typedef void *SWFDBLBitmap;

SWFDBLBitmap newSWFDBLBitmap(FILE *f);
SWFDBLBitmap newSWFDBLBitmap_fromInput(SWFInput input);


  /* SWFJpegBitmap extends SWFBitmap */

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


  /* SWFShape */

typedef void *SWFShape;

SWFShape newSWFShape();
void destroySWFShape(SWFBlock block);

void SWFShape_movePenTo(SWFShape shape, float x, float y);
void SWFShape_movePen(SWFShape shape, float x, float y);

float SWFShape_getPenX(SWFShape shape);
float SWFShape_getPenY(SWFShape shape);

void SWFShape_drawLineTo(SWFShape shape, float x, float y);
void SWFShape_drawLine(SWFShape shape, float dx, float dy);
void SWFShape_drawCurveTo(SWFShape shape, float controlx, float controly,
			  float anchorx, float anchory);
void SWFShape_drawCurve(SWFShape shape, float controldx, float controldy,
			float anchordx, float anchordy);

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

float SWFFont_getStringWidth(SWFFont font, const char *string);

/* XXX */
#define SWFFont_getWidth SWFFont_getStringWidth

float SWFFont_getAscent(SWFFont font);
float SWFFont_getDescent(SWFFont font);
float SWFFont_getLeading(SWFFont font);


  /* SWFText */

typedef void *SWFText;

SWFText newSWFText();
SWFText newSWFText2();
void destroySWFText(SWFBlock block);

void SWFText_setFont(SWFText text, SWFFont font);
void SWFText_setHeight(SWFText text, float height);
void SWFText_moveTo(SWFText text, float x, float y);
void SWFText_setColor(SWFText text, byte r, byte g, byte b, byte a);
void SWFText_addString(SWFText text, const char *string, float *advance);
void SWFText_setSpacing(SWFText text, float spacing);

float SWFText_getStringWidth(SWFText text, const char *string);

/* XXX */
#define SWFText_getWidth SWFText_getStringWidth

float SWFText_getAscent(SWFText text);
float SWFText_getDescent(SWFText text);
float SWFText_getLeading(SWFText text);

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
void SWFTextField_setBounds(SWFTextField field, float width, float height);
void SWFTextField_setFlags(SWFTextField field, int flags);
void SWFTextField_setColor(SWFTextField field, byte r, byte g, byte b, byte a);
void SWFTextField_setVariableName(SWFTextField field, char *name);
void SWFTextField_addString(SWFTextField field, char *string);

void SWFTextField_setHeight(SWFTextField field, float height);
void SWFTextField_setFieldHeight(SWFTextField field, float height);
void SWFTextField_setLeftMargin(SWFTextField field, float leftMargin);
void SWFTextField_setRightMargin(SWFTextField field, float rightMargin);
void SWFTextField_setIndentation(SWFTextField field, float indentation);
void SWFTextField_setLineSpacing(SWFTextField field, float lineSpacing);
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
SWFSound newSWFSound_fromInput(SWFInput input);
void destroySWFSound(SWFSound sound);


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


  /* SWFAction */

typedef void *SWFAction;

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


  /* SWFSprite */

typedef void *SWFSprite;

SWFSprite newSWFSprite();
void destroySWFSprite(SWFBlock block);

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
			   float x, float y);

void SWFPosition_rotate(SWFPosition position, float degrees);
void SWFPosition_rotateTo(SWFPosition position, float degrees);
void SWFPosition_move(SWFPosition position, float x, float y);
void SWFPosition_moveTo(SWFPosition position, float x, float y);


typedef void *SWFDisplayItem;

void SWFDisplayItem_rotate(SWFDisplayItem item, float degrees);
void SWFDisplayItem_rotateTo(SWFDisplayItem item, float degrees);
void SWFDisplayItem_move(SWFDisplayItem item, float x, float y);
void SWFDisplayItem_moveTo(SWFDisplayItem item, float x, float y);
void SWFDisplayItem_scale(SWFDisplayItem item, float xScale, float yScale);
void SWFDisplayItem_scaleTo(SWFDisplayItem item, float xScale, float yScale);
void SWFDisplayItem_skewX(SWFDisplayItem item, float x);
void SWFDisplayItem_skewXTo(SWFDisplayItem item, float x);
void SWFDisplayItem_skewY(SWFDisplayItem item, float y);
void SWFDisplayItem_skewYTo(SWFDisplayItem item, float y);
void SWFDisplayItem_setMatrix(SWFDisplayItem i, float a, float b,
			      float c, float d, float x, float y);

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


/* SWFFill adds a position object to manipulate SWFFillStyle's matrix. */

typedef void *SWFFill;

SWFFill newSWFFill(SWFFillStyle fillstyle);
void destroySWFFill(SWFFill fill);

void SWFFill_skewX(SWFFill fill, float x);
void SWFFill_skewXTo(SWFFill fill, float x);
void SWFFill_skewY(SWFFill fill, float y);
void SWFFill_skewYTo(SWFFill fill, float y);
void SWFFill_scaleX(SWFFill fill, float x);
void SWFFill_scaleXTo(SWFFill fill, float x);
void SWFFill_scaleY(SWFFill fill, float y);
void SWFFill_scaleYTo(SWFFill fill, float y);

/* deprecated: */
void SWFFill_scaleXY(SWFFill fill, float x, float y);
void SWFFill_scaleXYTo(SWFFill fill, float x, float y);

void SWFFill_scale(SWFFill fill, float x, float y);
void SWFFill_scaleTo(SWFFill fill, float x, float y);
void SWFFill_rotate(SWFFill fill, float degrees);
void SWFFill_rotateTo(SWFFill fill, float degrees);
void SWFFill_move(SWFFill fill, float x, float y);
void SWFFill_moveTo(SWFFill fill, float x, float y);
void SWFFill_setMatrix(SWFFill fill, float a, float b,
		       float c, float d, float x, float y);


/* shape_util.h */

void SWFShape_setLine(SWFShape shape, unsigned short width,
		      byte r, byte g, byte b, byte a);

SWFFill SWFShape_addSolidFill(SWFShape shape, byte r, byte g, byte b, byte a);
SWFFill SWFShape_addGradientFill(SWFShape shape, SWFGradient gradient,
				 byte flags);
SWFFill SWFShape_addBitmapFill(SWFShape shape, SWFBitmap bitmap, byte flags);

void SWFShape_setLeftFill(SWFShape shape, SWFFill fill);
void SWFShape_setRightFill(SWFShape shape, SWFFill fill);

void SWFShape_setLine(SWFShape shape,
		      unsigned short width, byte r, byte g, byte b, byte a);

void SWFShape_drawArc(SWFShape shape, float r,
		      float startAngle, float endAngle);

void SWFShape_drawCircle(SWFShape shape, float r);


/* draw character c from font font into shape shape */
void SWFShape_drawGlyph(SWFShape shape, SWFFont font, unsigned char c);

/* approximate a cubic bezier with quadratic segments */
/* returns the number of segments used */
int SWFShape_drawCubic(SWFShape shape, float bx, float by,
		       float cx, float cy, float dx, float dy);
int SWFShape_drawCubicTo(SWFShape shape, float bx, float by,
			 float cx, float cy, float dx, float dy);

void SWFShape_drawCharacterBounds(SWFShape shape, SWFCharacter character);


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


/* movie.h */

typedef void *SWFMovie;

void destroySWFMovie(SWFMovie movie);
SWFMovie newSWFMovie();
SWFMovie newSWFMovieWithVersion(int version);

void SWFMovie_setRate(SWFMovie movie, float rate);
void SWFMovie_setDimension(SWFMovie movie, float x, float y);
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

static inline int SWFMovie_save(SWFMovie movie, const char *filename)
{
  FILE *f = fopen(filename, "wb");
  int count;

  if(f == NULL)
    return -1;

  count = SWFMovie_output(movie, fileOutputMethod, f);
  fclose(f);
  return count;
}
