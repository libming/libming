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

/* mask the c type names so that we can replace them with classes.
   weird, but it works.  (on gcc, anyway..) */

extern "C"
{
  #define SWFShape        c_SWFShape
  #define SWFMovie        c_SWFMovie
  #define SWFDisplayItem  c_SWFDisplayItem
  #define SWFFill         c_SWFFill
  #define SWFCharacter    c_SWFCharacter
  #define SWFBlock        c_SWFBlock
  #define SWFSprite       c_SWFSprite
  #define SWFMovieClip    c_SWFMovieClip
  #define SWFBitmap       c_SWFBitmap
  #define SWFGradient     c_SWFGradient
  #define SWFMorph        c_SWFMorph
  #define SWFText         c_SWFText
  #define SWFFont         c_SWFFont
  #define SWFBrowserFont  c_SWFBrowserFont
  #define SWFTextField    c_SWFTextField
  #define SWFAction       c_SWFAction
  #define SWFButton       c_SWFButton
  #define SWFSound        c_SWFSound
  #define SWFInput        c_SWFInput

  #include <ming.h>

  #undef SWFShape
  #undef SWFMovie
  #undef SWFDisplayItem
  #undef SWFFill
  #undef SWFCharacter
  #undef SWFBlock
  #undef SWFSprite
  #undef SWFMovieClip
  #undef SWFBitmap
  #undef SWFGradient
  #undef SWFMorph
  #undef SWFFont
  #undef SWFText
  #undef SWFBrowserFont
  #undef SWFTextField
  #undef SWFAction
  #undef SWFButton
  #undef SWFSound
  #undef SWFInput
}


/*  SWFInput  */

class SWFInput
{
 public:
  c_SWFInput input;

  SWFInput() { }

  SWFInput(FILE *f)
    { this->input = newSWFInput_file(f); }

  SWFInput(unsigned char *buffer, int length)
    { this->input = newSWFInput_buffer(buffer, length); }

  SWFInput(unsigned char *buffer, int length, int alloced)
  {
    if(alloced)
      this->input = newSWFInput_allocedBuffer(buffer, length);
    else
      this->input = newSWFInput_buffer(buffer, length);
  }

  virtual ~SWFInput() { destroySWFInput(this->input); }
};


/*  SWFBlock  */

class SWFBlock
{
 public:
  virtual c_SWFBlock getBlock()
    { return NULL; }
};


/*  SWFCharacter  */

class SWFCharacter : public SWFBlock
{
 public:
  c_SWFCharacter character;

  float getWidth()
    { return SWFCharacter_getWidth(this->character); }

  float getHeight()
    { return SWFCharacter_getHeight(this->character); }

  virtual c_SWFBlock getBlock()
    { return NULL; }
};


/*  SWFDisplayItem  */

class SWFDisplayItem
{
 public:
  c_SWFDisplayItem item;

  SWFDisplayItem(c_SWFDisplayItem item)
    { this->item = item; }

  virtual ~SWFDisplayItem()
    { }

  void rotate(float degrees)
    { SWFDisplayItem_rotate(this->item, degrees); }

  void rotateTo(float degrees)
    { SWFDisplayItem_rotateTo(this->item, degrees); }

  void move(float x, float y)
    { SWFDisplayItem_move(this->item, x, y); }

  void moveTo(float x, float y)
    { SWFDisplayItem_moveTo(this->item, x, y); }

  void scale(float xScale, float yScale)
    { SWFDisplayItem_scale(this->item, xScale, yScale); }

  void scale(float scale)
    { SWFDisplayItem_scale(this->item, scale, scale); }

  void scaleTo(float xScale, float yScale)
    { SWFDisplayItem_scaleTo(this->item, xScale, yScale); }

  void scaleTo(float scale)
    { SWFDisplayItem_scaleTo(this->item, scale, scale); }

  void skewX(float skew)
    { SWFDisplayItem_skewX(this->item, skew); }

  void skewXTo(float skew)
    { SWFDisplayItem_skewXTo(this->item, skew); }

  void skewY(float skew)
    { SWFDisplayItem_skewY(this->item, skew); }

  void skewYTo(float skew)
    { SWFDisplayItem_skewYTo(this->item, skew); }

  int getDepth()
    { return SWFDisplayItem_getDepth(this->item); }

  void setDepth(int depth)
    { SWFDisplayItem_setDepth(this->item, depth); }

  void remove()
    { SWFDisplayItem_remove(this->item); }

  void setName(const char *name)
    { SWFDisplayItem_setName(this->item, name); }

  void setRatio(float ratio)
    { SWFDisplayItem_setRatio(this->item, ratio); }

  void addColor(int r, int g, int b, int a=0)
    { SWFDisplayItem_setColorAdd(this->item, r, g, b, a); }

  void multColor(float r, float g, float b, float a=1.0)
    { SWFDisplayItem_setColorMult(this->item, r, g, b, a); }
};


/*  SWFSound  */

class SWFSound
{
 public:
  c_SWFSound sound;

  SWFSound(FILE *file)
    { this->sound = newSWFSound(file); }

  SWFSound(char *filename)
    { this->sound = newSWFSound(fopen(filename, "rb")); }

  virtual ~SWFSound()
    { destroySWFSound(this->sound); }
};


/*  SWFMovie  */

class SWFMovie
{
 public:
  c_SWFMovie movie;

  SWFMovie()
    { this->movie = newSWFMovie(); }

  virtual ~SWFMovie()
    { destroySWFMovie(this->movie); }

  void setRate(float rate)
    { SWFMovie_setRate(this->movie, rate); }

  void setDimension(float x, float y)
    { SWFMovie_setDimension(this->movie, x, y); }

  void setNumberOfFrames(int nFrames)
    { SWFMovie_setNumberOfFrames(this->movie, nFrames); }

  /* aka */
  void setFrames(int nFrames)
    { SWFMovie_setNumberOfFrames(this->movie, nFrames); }

  void setBackground(int r, int g, int b)
    { SWFMovie_setBackground(this->movie, r, g, b); }

  void setSoundStream(SWFSound *sound)
    { SWFMovie_setSoundStream(this->movie, sound->sound); }

  SWFDisplayItem *add(SWFBlock *character)
    { return new SWFDisplayItem(SWFMovie_add(this->movie, character->getBlock())); }

  void remove(SWFDisplayItem *item)
    { SWFMovie_remove(this->movie, item->item); }

  void nextFrame()
    { SWFMovie_nextFrame(this->movie); }

  void labelFrame(char *label)
    { SWFMovie_labelFrame(this->movie, label); }

  int output()
    { return SWFMovie_output(this->movie, fileOutputMethod, stdout); }

  int save(const char *filename)
  {
    FILE *fp = fopen(filename, "wb");
    const int result = SWFMovie_output(this->movie, fileOutputMethod, fp);
    fclose(fp);
    return result;
  }
};


/*  SWFFill  */

class SWFFill
{
 public:
  c_SWFFill fill;

  SWFFill(c_SWFFill fill)
    { this->fill = fill; }

  // shape destroys c_SWFFill object
  virtual ~SWFFill() {}

  void skewX(float x)
    { SWFFill_skewX(this->fill, x); }

  void skewXTo(float x)
    { SWFFill_skewXTo(this->fill, x); }

  void skewY(float y)
    { SWFFill_skewY(this->fill, y); }

  void skewYTo(float y)
    { SWFFill_skewYTo(this->fill, y); }

  void scaleX(float x)
    { SWFFill_scaleX(this->fill, x); }

  void scaleXTo(float x)
    { SWFFill_scaleXTo(this->fill, x); }

  void scaleY(float y)
    { SWFFill_scaleY(this->fill, y); }

  void scaleYTo(float y)
    { SWFFill_scaleYTo(this->fill, y); }

  void scale(float x, float y)
    { SWFFill_scaleXY(this->fill, x, y); }

  void scale(float scale)
    { SWFFill_scaleXY(this->fill, scale, scale); }

  void scaleTo(float x, float y)
    { SWFFill_scaleXYTo(this->fill, x, y); }

  void scaleTo(float scale)
    { SWFFill_scaleXYTo(this->fill, scale, scale); }

  void rotate(float degrees)
    { SWFFill_rotate(this->fill, degrees); }

  void rotateTo(float degrees)
    { SWFFill_rotateTo(this->fill, degrees); }

  void move(float x, float y)
    { SWFFill_move(this->fill, x, y); }

  void moveTo(float x, float y)
    { SWFFill_move(this->fill, x, y); }
};


/*  SWFGradient  */

class SWFGradient
{
 public:
  c_SWFGradient gradient;

  SWFGradient()
    { this->gradient = newSWFGradient(); }

  virtual ~SWFGradient()
    { destroySWFGradient(this->gradient); }

  void addEntry(float ratio, int r, int g, int b, int a=0xff)
    { SWFGradient_addEntry(this->gradient, ratio, r, g, b, a); }
};


/*  SWFBitmap  */

class SWFBitmap : public SWFBlock
{
 public:
  c_SWFBitmap bitmap;

  SWFBitmap(char *filename, char *alpha=NULL)
  {
    if(strlen(filename) > 4)
    {
      if(strcmp(filename+strlen(filename)-4, ".dbl") == 0)
	this->bitmap = newSWFDBLBitmap(fopen(filename, "rb"));

      else if(strcmp(filename+strlen(filename)-4, ".jpg") == 0)
      {
	if(alpha != NULL)
	  this->bitmap = newSWFJpegWithAlpha(fopen(filename, "rb"),
					     fopen(alpha, "rb"));
	else
	  this->bitmap = newSWFJpegBitmap(fopen(filename, "rb"));
      }

      else
	; // XXX - throw exception
    }
  }

  SWFBitmap(SWFInput input)
    { this->bitmap = newSWFBitmap_fromInput(input); }

  virtual ~SWFBitmap()
    { destroySWFBitmap(this->bitmap); }

  float getWidth()
    { return SWFBitmap_getWidth(this->bitmap); }

  float getHeight()
    { return SWFBitmap_getHeight(this->bitmap); }
};


/*  SWFFont  */

class SWFFont : public SWFBlock
{
 public:
  c_SWFFont font;

  SWFFont()
    { this->font = newSWFFont(); }

  SWFFont(FILE *file)
    { this->font = loadSWFFontFromFile(file); }

  SWFFont(char *name)
  {
    if(strlen(name) > 4 &&
       strcmp(name + strlen(name) - 4, ".fdb") == 0)
      this->font = loadSWFFontFromFile(fopen(name, "rb"));
    else
      this->font = newSWFBrowserFont(name);
  }

  virtual ~SWFFont()
    { destroySWFFont(this->font); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->font; }

  float getStringWidth(char *string)
    { return SWFFont_getStringWidth(this->font, string); }

  float getWidth(char *string)
    { return SWFFont_getStringWidth(this->font, string); }

  float getAscent()
    { return SWFFont_getAscent(this->font); }

  float getDescent()
    { return SWFFont_getDescent(this->font); }

  float getLeading()
    { return SWFFont_getLeading(this->font); }
};


/*  SWFShape  */

class SWFShape : public SWFCharacter
{
 public:
  c_SWFShape shape;

  SWFShape()
    { this->shape = newSWFShape(); }

  SWFShape(c_SWFShape shape)
    { this->shape = shape; }

  virtual ~SWFShape()
    { destroySWFShape(this->shape); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->shape; }

  void movePen(float x, float y)
    { SWFShape_movePen(this->shape, x, y); }

  void movePenTo(float x, float y)
    { SWFShape_movePenTo(this->shape, x, y); }

  void drawLine(float x, float y)
    { SWFShape_drawLine(this->shape, x, y); }

  void drawLineTo(float x, float y)
    { SWFShape_drawLineTo(this->shape, x, y); }

  void drawCurve(float cx, float cy, float ax, float ay)
    { SWFShape_drawCurve(this->shape, cx, cy, ax, ay); }

  void drawCurveTo(float cx, float cy, float ax, float ay)
    { SWFShape_drawCurveTo(this->shape, cx, cy, ax, ay); }

  void drawCubic(float ax, float ay, float bx, float by, float cx, float cy)
    { SWFShape_drawCubic(this->shape, ax, ay, bx, by, cx, cy); }

  void drawCubicTo(float ax, float ay, float bx, float by, float cx, float cy)
    { SWFShape_drawCubicTo(this->shape, ax, ay, bx, by, cx, cy); }

  void end()
    { SWFShape_end(this->shape); }

  SWFFill *addSolidFill(int r, int g, int b, int a=0xff)
    { return new SWFFill(SWFShape_addSolidFill(this->shape, r, g, b, a)); }

  SWFFill *addGradientFill(SWFGradient *gradient, byte flags=0)
    { return new SWFFill(SWFShape_addGradientFill(this->shape, gradient->gradient, flags)); }

  SWFFill *addBitmapFill(SWFBitmap *bitmap, byte flags=0)
    { return new SWFFill(SWFShape_addBitmapFill(this->shape, bitmap->bitmap, flags)); }

  void setLeftFill(SWFFill *fill)
    { SWFShape_setLeftFill(this->shape, fill->fill); }

  void setRightFill(SWFFill *fill)
    { SWFShape_setRightFill(this->shape, fill->fill); }

  void setLine(unsigned short width, int r, int g, int b, int a=0xff)
    { SWFShape_setLine(this->shape, width, r, g, b, a); }

  void drawArc(float r, float startAngle, float endAngle)
    { SWFShape_drawArc(this->shape, r, startAngle, endAngle); }

  void drawCircle(float r)
    { SWFShape_drawCircle(this->shape, r); }

  void drawGlyph(SWFFont *font, unsigned char c)
    { SWFShape_drawFontGlyph(this->shape, font->font, c); }

  // deprecated?
  void drawFontGlyph(SWFFont *font, unsigned char c)
    { SWFShape_drawFontGlyph(this->shape, font->font, c); }
};


/*  SWFSprite  */

class SWFSprite : public SWFCharacter
{
 public:
  c_SWFMovieClip clip;

  SWFSprite()
    { this->clip = newSWFMovieClip(); }

  virtual ~SWFSprite()
    { destroySWFMovieClip(this->clip); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->clip; }

  void setNumberOfFrames(int nFrames)
    { SWFMovieClip_setNumberOfFrames(this->clip, nFrames); }

  SWFDisplayItem *add(SWFBlock *character)
    { return new SWFDisplayItem(SWFMovieClip_add(this->clip, character->getBlock())); }

  void remove(SWFDisplayItem *item)
    { SWFMovieClip_remove(this->clip, item->item); }

  void nextFrame()
    { SWFMovieClip_nextFrame(this->clip); }

  void labelFrame(char *label)
    { SWFMovieClip_labelFrame(this->clip, label); }
};


/*  SWFMorph  */

class SWFMorph : public SWFCharacter
{
 public:
  c_SWFMorph morph;

  SWFMorph()
    { this->morph = newSWFMorphShape(); }

  virtual ~SWFMorph()
    { destroySWFMorph(this->morph); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->morph; }

  SWFShape *getShape1()
    { return new SWFShape(SWFMorph_getShape1(this->morph)); }

  SWFShape *getShape2()
    { return new SWFShape(SWFMorph_getShape2(this->morph)); }
};


/*  SWFText  */

class SWFText : public SWFCharacter
{
 public:
  c_SWFText text;

  SWFText()
    { this->text = newSWFText2(); }

  virtual ~SWFText()
    { destroySWFText(this->text); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->text; }

  void setFont(SWFBlock *font)
    { SWFText_setFont(this->text, font->getBlock()); }

  void setHeight(float height)
    { SWFText_setHeight(this->text, height); }

  // I don't like this..  it's too easy to confuse with displayitem::moveTo
  void moveTo(float x, float y)
    { SWFText_moveTo(this->text, x, y); }

  void setColor(unsigned char r, unsigned char g, unsigned char b, int a=0xff)
    { SWFText_setColor(this->text, r, g, b, a); }

  void addString(const char *string, float *advance=NULL)
    { SWFText_addString(this->text, string, advance); }

  void setSpacing(float spacing)
    { SWFText_setSpacing(this->text, spacing); }

  float getStringWidth(const char *string)
    { return SWFText_getStringWidth(this->text, string); }

  float getWidth(const char *string)
    { return SWFText_getStringWidth(this->text, string); }
};


/*  SWFBrowserFont  */

class SWFBrowserFont : public SWFBlock
{
 public:
  c_SWFBrowserFont browserFont;

  SWFBrowserFont(char *name)
    { this->browserFont = newSWFBrowserFont(name); }

  virtual ~SWFBrowserFont()
    { destroySWFBrowserFont(this->browserFont); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->browserFont; }
};


/*  SWFTextField  */

class SWFTextField : public SWFCharacter
{
 public:
  c_SWFTextField textField;

  SWFTextField()
    { this->textField = newSWFTextField(); }

  virtual ~SWFTextField()
    { destroySWFTextField(this->textField); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->textField; }

  void setFont(SWFBlock *font)
    { SWFTextField_setFont(this->textField, font->getBlock()); }

  void setBounds(float width, float height)
    { SWFTextField_setBounds(this->textField, width, height); }

  void setFlags(int flags)
    { SWFTextField_setFlags(this->textField, flags); }

  void setColor(int r, int g, int b, int a=0xff)
    { SWFTextField_setColor(this->textField, r, g, b, a); }

  void setVariableName(char *name)
    { SWFTextField_setVariableName(this->textField, name); }

  void addString(char *string)
    { SWFTextField_addString(this->textField, string); }

  void setHeight(float height)
    { SWFTextField_setHeight(this->textField, height); }

  void setLeftMargin(float margin)
    { SWFTextField_setLeftMargin(this->textField, margin); }

  void setRightMargin(float margin)
    { SWFTextField_setRightMargin(this->textField, margin); }

  void setIndentation(float indentation)
    { SWFTextField_setIndentation(this->textField, indentation); }

  void setLineSpacing(float lineSpacing)
    { SWFTextField_setLineSpacing(this->textField, lineSpacing); }

  void setAlignment(SWFTextFieldAlignment alignment)
    { SWFTextField_setAlignment(this->textField, alignment); }

  void align(SWFTextFieldAlignment alignment)
    { SWFTextField_setAlignment(this->textField, alignment); }

  void setLength(int length)
    { SWFTextField_setLength(this->textField, length); }
};


/*  SWFAction  */

class SWFAction : public SWFBlock
{
 public:
  c_SWFAction action;

  SWFAction(char *script)
    { this->action = compileSWFActionCode(script); }

  // movies, buttons, etc. destroy the c_SWFAction..
  virtual ~SWFAction() {}

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->action; }
};


/*  SWFButton  */

class SWFButton : public SWFCharacter
{
 public:
  c_SWFButton button;

  SWFButton()
    { this->button = newSWFButton(); }

  virtual ~SWFButton()
    { destroySWFButton(this->button); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->button; }

  void addShape(SWFCharacter *character, byte flags)
    { SWFButton_addShape(this->button, (c_SWFCharacter)character->getBlock(), flags); }

  void addAction(SWFAction *action, int flags)
    { SWFButton_addAction(this->button, action->action, flags); }
};
