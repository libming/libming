
%module mingc
%include typemaps.i
%{
  #include "ming.h"
%}

%typemap(python,in) FILE * {
  if (!PyFile_Check($source)) {
    PyErr_SetString(PyExc_TypeError, "Need a file!");
    return NULL;
  }
  $target = PyFile_AsFile($source);
}

%typemap(python,in) char * {
  if (!PyString_Check($source)) {
    PyErr_SetString(PyExc_TypeError, "not a string");
    return NULL;
  }
  $target = PyString_AsString($source);
}

typedef unsigned char byte;

%typemap(python,in) byte * {
  if (!PyString_Check($source)) {
    PyErr_SetString(PyExc_TypeError, "not a string");
    return NULL;
  }
  $target = PyString_AsString($source);
}

%typemap(ruby,in) int * {
  if (NIL_P($source)) {
         $target = 0;
  } else {
    Check_Type($source, T_ARRAY);
    $target = (int *)RARRAY($source)->ptr;
  }
}

int Ming_init();
void Ming_setScale(float scale);
void Ming_setCubicThreshold(int num);


/* movie.h */

typedef void *SWFMovie, *SWFBlock, *SWFSound, *SWFDisplayItem, *SWFFill,
	     *SWFShape, *SWFAction, *SWFGradient, *SWFButton, *SWFButtonRecord,
	     *SWFBitmap, *SWFMovieClip, *SWFCharacter, *SWFMatrix, *SWFMorph,
	     *SWFFont, *SWFText, *SWFBrowserFont, *SWFTextField;

void destroySWFMovie(SWFMovie movie);
SWFMovie newSWFMovie();

void SWFMovie_setRate(SWFMovie movie, float rate);
void SWFMovie_setDimension(SWFMovie movie, float x, float y);
void SWFMovie_setNumberOfFrames(SWFMovie movie, int frames);
void SWFMovie_setBackground(SWFMovie movie, int r, int g, int b);

void SWFMovie_setSoundStream(SWFMovie movie, SWFSound sound);

SWFDisplayItem SWFMovie_add(SWFMovie movie, SWFBlock block);
void SWFMovie_remove(SWFMovie movie, SWFDisplayItem item);
void SWFMovie_nextFrame(SWFMovie movie);
void SWFMovie_labelFrame(SWFMovie movie, char *label);

int SWFMovie_output(SWFMovie movie, SWFByteOutputMethod method, void *data);

int SWFMovie_simpleOutput(SWFMovie movie);
int SWFMovie_save(SWFMovie movie, const char* filename);
void SWFMovie_saveToFileNo(SWFMovie movie, int fd);

%{
void my_fileno_write (unsigned char c, void * fd) {
  write((int) fd, &c, 1);
}

int SWFMovie_simpleOutput(SWFMovie movie) {
   return SWFMovie_output(movie, my_fileno_write, (void *)1);
}

int SWFMovie_saveToFileNo(SWFMovie movie, int fd) {
   return SWFMovie_output(movie, my_fileno_write, (void *)fd);
}

%}


/* displayitem.h */

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
void SWFDisplayItem_setMatrix(SWFDisplayItem item, float a, float b,
			      float c, float d, float x, float y);

int SWFDisplayItem_getDepth(SWFDisplayItem item);
void SWFDisplayItem_setDepth(SWFDisplayItem item, int depth);
void SWFDisplayItem_remove(SWFDisplayItem item);
void SWFDisplayItem_setName(SWFDisplayItem item, char *name);
void SWFDisplayItem_setRatio(SWFDisplayItem item, float ratio);
void SWFDisplayItem_setCXform(SWFDisplayItem item, SWFCXform cXform);
void SWFDisplayItem_setColorAdd(SWFDisplayItem item,
				int r, int g, int b, int a);
void SWFDisplayItem_setColorMult(SWFDisplayItem item,
				 float r, float g, float b, float a);

/* fill.h */

void destroySWFFill(SWFFill fill);

void SWFFill_skewX(SWFFill fill, float x);
void SWFFill_skewXTo(SWFFill fill, float x);
void SWFFill_skewY(SWFFill fill, float y);
void SWFFill_skewYTo(SWFFill fill, float y);
void SWFFill_scaleX(SWFFill fill, float x);
void SWFFill_scaleXTo(SWFFill fill, float x);
void SWFFill_scaleY(SWFFill fill, float y);
void SWFFill_scaleYTo(SWFFill fill, float y);
void SWFFill_scaleXY(SWFFill fill, float x, float y);
void SWFFill_scaleXYTo(SWFFill fill, float x, float y);
void SWFFill_rotate(SWFFill fill, float degrees);
void SWFFill_rotateTo(SWFFill fill, float degrees);
void SWFFill_move(SWFFill fill, float x, float y);
void SWFFill_moveTo(SWFFill fill, float x, float y);
void SWFFill_setMatrix(SWFFill fill, float a, float b,
		       float c, float d, float x, float y);


/* shape.h */

SWFFill SWFShape_addSolidFill(SWFShape shape, byte r, byte g, byte b, byte a);
SWFFill SWFShape_addGradientFill(SWFShape shape, SWFGradient gradient,
					         byte flags);
SWFFill SWFShape_addBitmapFill(SWFShape shape, SWFBitmap bitmap, byte flags);
void SWFShape_setLeftFill(SWFShape shape, SWFFill fill);
void SWFShape_setRightFill(SWFShape shape, SWFFill fill);
void SWFShape_setLine(SWFShape shape,
		      unsigned short width, byte r, byte g, byte b, byte a);
void SWFShape_drawArc(SWFShape shape, int r,
		      float startAngle, float endAngle);
void SWFShape_drawCircle(SWFShape shape, int r);

void SWFShape_movePenTo(SWFShape shape, float x, float y);
void SWFShape_movePen(SWFShape shape, float x, float y);
void SWFShape_drawLineTo(SWFShape shape, float x, float y);
void SWFShape_drawLine(SWFShape shape, float dx, float dy);
void SWFShape_drawCurveTo(SWFShape shape, float controlx, float controly,
			  float anchorx, float anchory);
void SWFShape_drawCurve(SWFShape shape, float controldx, float controldy,
			float anchordx, float anchordy);
void SWFShape_drawCubic(SWFShape shape, float bx, float by,
			float cx, float cy, float dx, float dy);
void SWFShape_drawCubicTo(SWFShape shape, float bx, float by,
			  float cx, float cy, float dx, float dy);
void SWFShape_drawCharacterBounds(SWFShape shape, SWFCharacter character);


/* movieclip.h */

SWFMovieClip newSWFMovieClip();
void destroySWFMovieClip(SWFMovieClip clip);

void SWFMovieClip_setNumberOfFrames(SWFMovieClip clip, int frames);
SWFDisplayItem SWFMovieClip_add(SWFMovieClip clip, SWFBlock block);
void SWFMovieClip_remove(SWFMovieClip clip, SWFDisplayItem item);
void SWFMovieClip_nextFrame(SWFMovieClip clip);
void SWFMovieClip_labelFrame(SWFMovieClip clip, char *label);


/* swf.h */

  /* SWFCharacter */

float SWFCharacter_getWidth(SWFCharacter character);
float SWFCharacter_getHeight(SWFCharacter character);


  /* SWFBitmap */

void destroySWFBitmap(SWFBitmap b);

float SWFBitmap_getWidth(SWFBitmap b);
float SWFBitmap_getHeight(SWFBitmap b);


  /* SWFDBLBitmap */

SWFBitmap newSWFDBLBitmap(FILE *READ);


  /* SWFJpegBitmap */

SWFBitmap newSWFJpegBitmap(FILE *READ);
SWFBitmap newSWFJpegWithAlpha(FILE *READ, FILE *READ);


  /* SWFGradient */

SWFGradient newSWFGradient();
void destroySWFGradient(SWFGradient gradient);
void SWFGradient_addEntry(SWFGradient gradient,
			  float ratio, byte r, byte g, byte b, byte a);


  /* SWFMatrix */

/*
SWFMatrix newSWFMatrix(int a, int b, int c, int d, int x, int y);
void destroySWFMatrix(SWFMatrix matrix);
SWFMatrix SWFMatrix_set(SWFMatrix m,
			float a, float b, float c, float d, float x, float y);
*/

  /* SWFFillStyle */

#define SWFFILL_SOLID		0x00
#define SWFFILL_GRADIENT	0x10
#define SWFFILL_LINEAR_GRADIENT 0x10
#define SWFFILL_RADIAL_GRADIENT 0x12
#define SWFFILL_BITMAP		0x40
#define SWFFILL_TILED_BITMAP	0x40
#define SWFFILL_CLIPPED_BITMAP	0x41


  /* SWFShape */

SWFShape newSWFShape();
void destroySWFShape(SWFShape shape);

void SWFShape_end(SWFShape shape);
void SWFShape_drawGlyph(SWFShape shape, SWFFont font, unsigned char c);

/* deprecated: */
void SWFShape_drawFontGlyph(SWFShape shape, SWFFont font, int c);


  /* SWFMorph */

SWFMorph newSWFMorphShape();
void destroySWFMorph(SWFMorph morph);
SWFShape SWFMorph_getShape1(SWFMorph morph);
SWFShape SWFMorph_getShape2(SWFMorph morph);


  /* SWFFont */

SWFFont newSWFFont();
void destroySWFFont(SWFFont font);
SWFFont loadSWFFontFromFile(FILE *READ);

float SWFFont_getStringWidth(SWFFont font, char *string);

float SWFFont_getAscent(SWFFont font);
float SWFFont_getDescent(SWFFont font);
float SWFFont_getLeading(SWFFont font);


  /* SWFText */

SWFText newSWFText();
SWFText newSWFText2();
void destroySWFText(SWFText text);

void SWFText_setFont(SWFText text, SWFFont font);
void SWFText_setHeight(SWFText text, float height);
void SWFText_moveTo(SWFText text, float x, float y);
void SWFText_setColor(SWFText text, byte r, byte g, byte b, byte a);
void SWFText_addString(SWFText text, char *string, float *advance);
void SWFText_setSpacing(SWFText text, float spacing);
float SWFText_getStringWidth(SWFText text, char *string);

float SWFText_getAscent(SWFText text);
float SWFText_getDescent(SWFText text);
float SWFText_getLeading(SWFText text);


  /* SWFBrowserFont */

SWFBrowserFont newSWFBrowserFont(char *name);
void destroySWFBrowserFont(SWFBrowserFont b);


  /* SWFTextField */

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
void destroySWFTextField(SWFTextField field);

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

SWFSound newSWFSound(FILE *READ);
void destroySWFSound(SWFSound sound);


  /* SWFAction */

SWFAction compileSWFActionCode(char *script);


  /* SWFButton */

#define SWFBUTTON_HIT    (1<<3)
#define SWFBUTTON_DOWN   (1<<2)
#define SWFBUTTON_OVER   (1<<1)
#define SWFBUTTON_UP     (1<<0)

int swfButton_keypress(char c);

%{
  int swfButton_keypress(char c)
  {
    return SWFBUTTON_KEYPRESS(c);
  }
%}



#define SWFBUTTON_OVERDOWNTOIDLE    (1<<8)
#define SWFBUTTON_IDLETOOVERDOWN    (1<<7)
#define SWFBUTTON_OUTDOWNTOIDLE     (1<<6)
#define SWFBUTTON_OUTDOWNTOOVERDOWN (1<<5)
#define SWFBUTTON_OVERDOWNTOOUTDOWN (1<<4)
#define SWFBUTTON_OVERDOWNTOOVERUP  (1<<3)
#define SWFBUTTON_OVERUPTOOVERDOWN  (1<<2)
#define SWFBUTTON_OVERUPTOIDLE      (1<<1)
#define SWFBUTTON_IDLETOOVERUP      (1<<0)

/* aka: */
#define SWFBUTTON_MOUSEUPOUTSIDE  SWFBUTTON_OUTDOWNTOIDLE
#define SWFBUTTON_DRAGOVER        (SWFBUTTON_OUTDOWNTOOVERDOWN | SWFBUTTON_IDLETOOVERDOWN)
#define SWFBUTTON_DRAGOUT         (SWFBUTTON_OVERDOWNTOOUTDOWN | SWFBUTTON_OVERDOWNTOIDLE)
#define SWFBUTTON_MOUSEUP         SWFBUTTON_OVERDOWNTOOVERUP
#define SWFBUTTON_MOUSEDOWN       SWFBUTTON_OVERUPTOOVERDOWN
#define SWFBUTTON_MOUSEOUT        SWFBUTTON_OVERUPTOIDLE
#define SWFBUTTON_MOUSEOVER       SWFBUTTON_IDLETOOVERUP

SWFButton newSWFButton();
void destroySWFButton(SWFButton button);

void SWFButton_addShape(SWFButton button, SWFCharacter character, byte flags);
void SWFButton_addAction(SWFButton button, SWFAction action, int flags);
