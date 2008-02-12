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

#ifndef SWF_MINGPP_H_INCLUDED
#define SWF_MINGPP_H_INCLUDED

/* why would we need to include these ??
#include <stdio.h>
*/
#include <cstring> /* for strlen used in SWFBitmap costructor */
#include <stdexcept>
#include <iostream>
#include <string>
/* mask the c type names so that we can replace them with classes.
   weird, but it works.  (on gcc, anyway..) */

extern "C"
{
  #define SWFShape        c_SWFShape
  #define SWFMovie        c_SWFMovie
  #define SWFDisplayItem  c_SWFDisplayItem
  #define SWFFill         c_SWFFill
  #define SWFFillStyle    c_SWFFillStyle
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
  #define SWFSoundStream  c_SWFSoundStream
  #define SWFInput        c_SWFInput
  #define SWFSound        c_SWFSound
  #define SWFVideoStream  c_SWFVideoStream
  #define SWFFilter       c_SWFFilter
  #define SWFBlur         c_SWFBlur
  #define SWFShadow       c_SWFShadow
  #define SWFFilterMatrix c_SWFFilterMatrix
  #define SWFInitAction   c_SWFInitAction
  #define SWFButtonRecord c_SWFButtonRecord

// begin minguts 2004/08/31 ((((
  #define SWFFontCharacter c_SWFFontCharacter
  #define SWFPrebuiltClip c_SWFPrebuiltClip
// )))) end minguts 2004/08/31

  #include <ming.h>

/* 
 * declaration from src/blocks/fdbfont.h. 
 * internal function to maintain behavior of older ming-version
 */
SWFFont loadSWFFont_fromFdbFile(FILE *file);

  #undef SWFShape
  #undef SWFMovie
  #undef SWFDisplayItem
  #undef SWFFill
  #undef SWFFillStyle
  #undef SWFCharacter
  #undef SWFBlock
  #undef SWFSprite
  #undef SWFMovieClip
  #undef SWFBitmap
  #undef SWFGradient
  #undef SWFMorph
  #undef SWFFont
  #undef SWFBrowserFont
  #undef SWFText
  #undef SWFTextField
  #undef SWFAction
  #undef SWFButton
  #undef SWFSoundStream
  #undef SWFInput
  #undef SWFSound
// begin minguts 2004/08/31 ((((
  #undef SWFFontCharacter
  #undef SWFPrebuiltClip 
// )))) end minguts 2004/08/31
  #undef SWFVideoStream
  #undef SWFFilter
  #undef SWFBlur
  #undef SWFShadow
  #undef SWFFilterMatrix
  #undef SWFInitAction
  #undef SWFButtonRecord
} // extern C

#define SWF_DECLAREONLY(classname) \
	private: \
	classname(const classname&); \
	const classname& operator=(const classname&)


class SWFException : public std::exception
{
public:
	SWFException(const char *m)
	{ 
		this->message = m;
	}

	virtual ~SWFException() throw () 
	{ }
		
	virtual const char *what()
	{
		return this->message.c_str();
	}	
private:
	std::string message;
};


/*  SWFInput  */

class SWFInput
{
 public:
  c_SWFInput input;

  SWFInput(FILE *f)
  { 
    this->input = newSWFInput_file(f); 
    if(this->input == NULL) 
      throw SWFException("SWFInput(FILE *f)\n");
  }

  SWFInput(unsigned char *buffer, int length)
  { 
    this->input = newSWFInput_buffer(buffer, length); 
    if(this->input == NULL)
      throw SWFException("SWFInput(unsigned char *buffer, int length)\n");
  }

  SWFInput(unsigned char *buffer, int length, int alloced)
  {
    if(alloced)
      this->input = newSWFInput_allocedBuffer(buffer, length);
    else
      this->input = newSWFInput_buffer(buffer, length);

    if(this->input == NULL)
      SWFException("SWFInput(unsigned char *buffer, int length, int alloced)\n");
  }

  virtual ~SWFInput() { destroySWFInput(this->input); }

  SWF_DECLAREONLY(SWFInput);
  SWFInput();
};


/*  SWFBlock  */

class SWFBlock
{
 public:
  virtual c_SWFBlock getBlock() = 0;
};

/*  SWFCharacter  */

class SWFCharacter : public SWFBlock
{
 public:
  c_SWFCharacter character;

  SWFCharacter()
  { character = NULL; }
  
  SWFCharacter(c_SWFCharacter c)
  { character = c; }

  float getWidth()
    { return SWFCharacter_getWidth(this->character); }

  float getHeight()
    { return SWFCharacter_getHeight(this->character); }

  virtual c_SWFBlock getBlock()
    { return (c_SWFBlock)character; }

  SWF_DECLAREONLY(SWFCharacter);
};
/*  SWFFontCharacter */
class SWFFontCharacter : public SWFCharacter
{
 friend class SWFMovie;
 public:
  c_SWFFontCharacter fontcharacter;
  
  void addChars(const char *str)
    { SWFFontCharacter_addChars(this->fontcharacter, str); }

  void addUTF8Chars(const char *str)
    { SWFFontCharacter_addUTF8Chars(this->fontcharacter, str); }

  void addAllChars()
    { SWFFontCharacter_addAllChars(this->fontcharacter); }

 private:
  SWFFontCharacter(c_SWFFontCharacter fontcharacter)
  { 
	this->fontcharacter = fontcharacter; 
	this->character = (c_SWFCharacter)fontcharacter;
  }

  virtual ~SWFFontCharacter()
    { }

  SWF_DECLAREONLY(SWFFontCharacter);
  SWFFontCharacter();
};


/*  SWFPrebuiltClip */

class SWFPrebuiltClip : public SWFBlock
{
 public:
  c_SWFPrebuiltClip prebuiltclip;

  SWFPrebuiltClip(c_SWFPrebuiltClip prebuiltclip)
  { 
    if(prebuiltclip == NULL)
      throw SWFException("new SWFPrebuiltClip: prebuiltclip == NULL)");
    this->prebuiltclip = prebuiltclip; 
  }

  virtual ~SWFPrebuiltClip()
    { }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->prebuiltclip; }


  SWFPrebuiltClip(const char *name)
  {
    if(strlen(name) > 4 &&
       strcmp(name + strlen(name) - 4, ".swf") == 0)
      this->prebuiltclip = newSWFPrebuiltClip_fromFile(name);
    else
      this->prebuiltclip = NULL;
   
    if(this->prebuiltclip == NULL)
      throw SWFException("SWFPrebuiltClip(const char *name)");
  }
  SWF_DECLAREONLY(SWFPrebuiltClip);
  SWFPrebuiltClip();
};
// )))) end minguts 2004/08/31


/*  SWFAction  */

class SWFAction : public SWFBlock
{
 public:
  c_SWFAction action;

  SWFAction(const char *script)
  { 
    this->action = newSWFAction(script); 
    if(this->action == NULL)
      throw SWFException("SWFAction(const char *script)");
  }

  // movies, buttons, etc. destroy the c_SWFAction..
  virtual ~SWFAction() {}

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->action; }

  SWF_DECLAREONLY(SWFAction);
  SWFAction();
};

/*  SWFInitAction  */

class SWFInitAction : public SWFBlock
{
 public:
  c_SWFInitAction init;

  SWFInitAction(SWFAction *action)
  { 
    this->init = newSWFInitAction(action->action); 
    if(this->init == NULL)
      throw SWFException("SWFInitAction(SWFAction *action)");
  }

  SWFInitAction(SWFAction *action, int id)
  { 
    this->init = newSWFInitAction_withId(action->action, id); 
    if(this->init == NULL)
      throw SWFException("SWFInitAction(SWFAction *action, int id)");
  }

  virtual ~SWFInitAction() {}

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->init; }

  SWF_DECLAREONLY(SWFInitAction);
  SWFInitAction();
};


/*  SWFGradient  */

class SWFGradient
{
 public:
  c_SWFGradient gradient;

  SWFGradient()
  { 
    this->gradient = newSWFGradient(); 
    if(this->gradient == NULL)
      throw SWFGradient();
  }

  virtual ~SWFGradient()
    { destroySWFGradient(this->gradient); }

  void addEntry(float ratio, byte r, byte g, byte b, byte a=0xff)
    { SWFGradient_addEntry(this->gradient, ratio, r, g, b, a); }

  void setSpreadMode(GradientSpreadMode mode)
    { SWFGradient_setSpreadMode(this->gradient, mode); }

  void setInterpolationMode(GradientInterpolationMode mode)
    { SWFGradient_setInterpolationMode(this->gradient, mode); }

  void setFocalPoint(float focalPoint)
    { SWFGradient_setFocalPoint(this->gradient, focalPoint); }

  SWF_DECLAREONLY(SWFGradient);
};

/* SWFFilter */
class SWFBlur
{
 public:
  c_SWFBlur blur;
  
  SWFBlur(float blurX, float blurY, int passes)
  {  
    this->blur = newSWFBlur(blurX, blurY, passes); 
    if(this->blur == NULL)
      throw SWFException("SWFBlur(float blurX, float blurY, int passes)");
  }

  ~SWFBlur()
    { destroySWFBlur(blur); }

  SWF_DECLAREONLY(SWFBlur);
  SWFBlur();
};

class SWFShadow
{
 public:
  c_SWFShadow shadow;

  SWFShadow(float angle, float distance, float strength)
  { 
    this->shadow = newSWFShadow(angle, distance, strength); 
    if(this->shadow == NULL)
      throw SWFException("SWFShadow(float angle, float distance, float strength)");
  }

  ~SWFShadow()
    { destroySWFShadow(shadow); }

  SWF_DECLAREONLY(SWFShadow);
  SWFShadow();
};

class SWFFilterMatrix 
{
 public:
  c_SWFFilterMatrix matrix;

  SWFFilterMatrix(int cols, int rows, float *vals)
  { 
    this->matrix = newSWFFilterMatrix(cols, rows, vals); 
    if(this->matrix == NULL)
      throw SWFException("SWFFilterMatrix(int cols, int rows, float *vals)");
  }

  ~SWFFilterMatrix()
    { destroySWFFilterMatrix(matrix); }

  SWF_DECLAREONLY(SWFFilterMatrix);
  SWFFilterMatrix();
};

class SWFFilter
{
 public:
  c_SWFFilter filter;

  virtual ~SWFFilter() {}

  static SWFFilter *BlurFilter(SWFBlur *blur)
    { return new SWFFilter(newBlurFilter(blur->blur)); }

  static SWFFilter *DropShadowFilter(SWFColor color, SWFBlur *blur, 
                                    SWFShadow *shadow, int flags)
    { return new SWFFilter(newDropShadowFilter(color, blur->blur, shadow->shadow, flags)); } 

  static SWFFilter *GlowFilter(SWFColor color, SWFBlur *blur, 
                              float strength, int flags)
    { return new SWFFilter(newGlowFilter(color, blur->blur, strength, flags));}

  static SWFFilter *BevelFilter(SWFColor sColor, SWFColor hColor,
                               SWFBlur *blur, SWFShadow *shadow, int flags)
    { return new SWFFilter(newBevelFilter(sColor, hColor, blur->blur, shadow->shadow, flags)); }

  static SWFFilter *GradientGlowFilter(SWFGradient *gradient, SWFBlur *blur, SWFShadow *shadow, int flags)
    { return new SWFFilter(newGradientGlowFilter(gradient->gradient, blur->blur, shadow->shadow, flags)); }

  static SWFFilter *GradientBevelFilter(SWFGradient *gradient, SWFBlur *blur,
                                       SWFShadow *shadow, int flags)
    { return new SWFFilter(newGradientBevelFilter(gradient->gradient, blur->blur, shadow->shadow, flags)); }

  static SWFFilter *ConvolutionFilter(SWFFilterMatrix *matrix, float divisor,
                                     float bias, SWFColor color, int flags)
    { return new SWFFilter(newConvolutionFilter(matrix->matrix, divisor, bias, color, flags)); }

  static SWFFilter *ColorMatrixFilter(SWFFilterMatrix *matrix)
    { return new SWFFilter(newColorMatrixFilter(matrix->matrix)); }

private:
  SWFFilter(c_SWFFilter filter)
  {  
    this->filter = filter;
    if(this->filter == NULL)
      throw SWFException("SWFFilter(c_SWFFilter filter)");
  } 
  SWF_DECLAREONLY(SWFFilter);
  SWFFilter();
};



/*  SWFDisplayItem  */

class SWFDisplayItem
{
 friend class SWFMovie;
 friend class SWFMovieClip;
 friend class SWFSprite;
 public:
  c_SWFDisplayItem item;
 
  void rotate(float degrees)
    { SWFDisplayItem_rotate(this->item, degrees); }

  void rotateTo(float degrees)
    { SWFDisplayItem_rotateTo(this->item, degrees); }

  void getRotation(float *degrees)
    { SWFDisplayItem_getRotation(this->item, degrees); }

  void move(float x, float y)
    { SWFDisplayItem_move(this->item, x, y); }

  void moveTo(float x, float y)
    { SWFDisplayItem_moveTo(this->item, x, y); }

  void getPosition(float *x, float *y)
    { SWFDisplayItem_getPosition(this->item, x, y); }

  void scale(float xScale, float yScale)
    { SWFDisplayItem_scale(this->item, xScale, yScale); }

  void scale(float scale)
    { SWFDisplayItem_scale(this->item, scale, scale); }

  void scaleTo(float xScale, float yScale)
    { SWFDisplayItem_scaleTo(this->item, xScale, yScale); }

  void scaleTo(float scale)
    { SWFDisplayItem_scaleTo(this->item, scale, scale); }

  void getScale(float *xScale, float *yScale)
    { SWFDisplayItem_getScale(this->item, xScale, yScale); }

  void skewX(float skew)
    { SWFDisplayItem_skewX(this->item, skew); }

  void skewXTo(float skew)
    { SWFDisplayItem_skewXTo(this->item, skew); }

  void skewY(float skew)
    { SWFDisplayItem_skewY(this->item, skew); }

  void skewYTo(float skew)
    { SWFDisplayItem_skewYTo(this->item, skew); }

  void getSkew(float *xSkew, float *ySkew)
    { SWFDisplayItem_getSkew(this->item, xSkew, ySkew); }

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

  void addAction(SWFAction *action, int flags)
    { SWFDisplayItem_addAction(this->item, action->action, flags); }

  void addFilter(SWFFilter *filter)
    { SWFDisplayItem_addFilter(this->item, filter->filter); }

  void cacheAsBitmap(int flag)
    { SWFDisplayItem_cacheAsBitmap(this->item, flag); }

  void setBlendMode(int mode)
    { SWFDisplayItem_setBlendMode(this->item, mode); }
  
  void setMatrix(float a, float b, float c, float d, float x, float y)
    { SWFDisplayItem_setMatrix(this->item, a, b, c, d, x, y); }

  void setMaskLevel(int level)
    { SWFDisplayItem_setMaskLevel(this->item, level); }
  
  void flush()
    { SWFDisplayItem_flush(this->item); }
  
  // FIXME: http://bugs.libming.net/show_bug.cgi?id=48
  virtual ~SWFDisplayItem()
    { }

 private:
  SWFDisplayItem(c_SWFDisplayItem item)
  { 
    this->item = item; 
    if(this->item == NULL)
      throw SWFException("SWFDisplayItem()");
  }

  SWF_DECLAREONLY(SWFDisplayItem);
  SWFDisplayItem();
};


/*  SWFSoundStream  */

class SWFSoundStream
{
 public:
  c_SWFSoundStream sound;

  SWFSoundStream(FILE *file)
  { 
    this->sound = newSWFSoundStream(file); 
    if(this->sound == NULL)
      throw SWFException("SWFSoundStream(FILE *file)");
  }

  SWFSoundStream(SWFInput *input)
  { 
    this->sound = newSWFSoundStream_fromInput(input->input); 
    if(this->sound == NULL)
      throw SWFException("SWFSoundStream(SWFInput *input)");
  }

  SWFSoundStream(char *filename)
  { 
    this->sound = newSWFSoundStream(fopen(filename, "rb")); 
    if(this->sound == NULL)
      throw SWFException("SWFSoundStream(char *filename)");
  }

  virtual ~SWFSoundStream()
    { destroySWFSoundStream(this->sound); }
  SWF_DECLAREONLY(SWFSoundStream);
  SWFSoundStream();
};




class SWFSound
{
 public:
  c_SWFSound sound;

  SWFSound(FILE *file, int flags)
  { 
	filep = NULL;
	this->sound = newSWFSound(file, flags);
	if(this->sound == NULL)
		throw SWFException("SWFSound(FILE *file, int flags)");
  }

  SWFSound(SWFInput *input, int flags)
  {
	this->sound = newSWFSound_fromInput(input->input, flags); 
	filep = NULL;
	if(this->sound == NULL)
		throw SWFException("SWFSound(SWFInput *input, int flags)");
  }

  SWFSound(char *filename, int flags)
  { 
	filep = fopen(filename, "rb");
	this->sound = newSWFSound(filep, flags);
	if(this->sound == NULL)
	{
		fclose(filep);
		throw SWFException("SWFSound(char *filename, int flags)");
	}
  }
  
  SWFSound(SWFSoundStream *stream)
  { 
	this->sound = newSWFSound_fromSoundStream(stream->sound); 
	filep = NULL;
	if(this->sound == NULL)
		throw SWFException("SWFSound(SWFSoundStream *stream)");
  }

  virtual ~SWFSound()
  {
	if(filep)
		fclose(filep); 
	destroySWFSound(this->sound); 
  }
 
 private:
    FILE *filep;

  SWF_DECLAREONLY(SWFSound);
  SWFSound();
};


/*  SWFMovie  */

class SWFMovie
{
 public:
  c_SWFMovie movie;

  SWFMovie()
  { 
    this->movie = newSWFMovie();
    if(this->movie == NULL)
      throw SWFException("SWFMovie()");
  }

  SWFMovie(int version)
  { 
    this->movie = newSWFMovieWithVersion(version);
    if(this->movie == NULL)
      throw SWFException("SWFMovie(int version)");
  }

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

  void setBackground(byte r, byte g, byte b)
    { SWFMovie_setBackground(this->movie, r, g, b); }

  void setSoundStream(SWFSoundStream *sound)
    { SWFMovie_setSoundStream(this->movie, sound->sound); /* wogl */ }

  SWFDisplayItem *add(SWFBlock *character)
  { 
    c_SWFDisplayItem item = SWFMovie_add_internal(this->movie, 
		(SWFMovieBlockType){character->getBlock()});
    if(item == NULL)
      return NULL;
    return new SWFDisplayItem(item); 
  }

  void addExport(SWFBlock *exp, char *name)
    {  SWFMovie_addExport(this->movie, exp->getBlock(), name); }

  void remove(SWFDisplayItem *item)
    { SWFMovie_remove(this->movie, item->item); }

  bool replace(SWFDisplayItem *item, SWFBlock *character)
    { return SWFMovie_replace_internal(this->movie, item->item, 
		(SWFMovieBlockType){character->getBlock()}); }

  void nextFrame()
    { SWFMovie_nextFrame(this->movie); }

  void labelFrame(const char *label)
    { SWFMovie_labelFrame(this->movie, label); }

  void namedAnchor(const char *label)
    { SWFMovie_namedAnchor(this->movie, label); }

  int output(int level=-1)
  {
    int oldlevel = Ming_setSWFCompression(level);
    int ret = SWFMovie_output_to_stream(this->movie, stdout);
    Ming_setSWFCompression(oldlevel);
    return ret;
  }

  int save(const char *filename, int level=-1)
  {
    int oldlevel = Ming_setSWFCompression(level);
    int result = SWFMovie_save(this->movie,filename);
    Ming_setSWFCompression(oldlevel);
    return result;
  }

  void startSound(SWFSound *sound)
    { SWFMovie_startSound(this->movie, sound->sound); }
  void stopSound(SWFSound *sound)
    { SWFMovie_stopSound(this->movie, sound->sound); }

// begin minguts 2004/08/31 ((((
	SWFCharacter *importCharacter(const char *filename, const char *name)
	{ return new SWFCharacter(SWFMovie_importCharacter(this->movie, filename, name));}
	SWFFontCharacter *importFont(const char *filename, const char *name)
	{ return new SWFFontCharacter(SWFMovie_importFont(this->movie, filename, name)); }
// )))) end minguts 2004/08/31

  void protect()
	{
		SWFMovie_protect(this->movie, NULL);
	}

  void protect(char *password)
	{
		SWFMovie_protect(this->movie,password);
	}
  
  void addMetadata(char *xml)
    { SWFMovie_addMetadata(this->movie, xml); }
  
  void setNetworkAccess(int flag)
    { SWFMovie_setNetworkAccess(this->movie, flag); }
  
  void setScriptLimits(int maxRecursion, int timeout)
    { SWFMovie_setScriptLimits(this->movie, maxRecursion, timeout); }

  void setTabIndex(int depth, int index)
    { SWFMovie_setTabIndex(this->movie, depth, index); }

  void assignSymbol(SWFCharacter *character, char *name)
    { SWFMovie_assignSymbol(this->movie, (c_SWFCharacter)character->getBlock(), name); }

  void defineScene(unsigned int offset, const char *name)
    { SWFMovie_defineScene(this->movie, offset, name); }

  SWF_DECLAREONLY(SWFMovie);
};


/*  SWFFill  */

class SWFFill
{
 public:
  c_SWFFill fill;

  SWFFill(c_SWFFill fill)
  { 
    this->fill = fill; 
    if(this->fill == NULL)
      throw SWFException("SWFFill");
  }

  // shape destroys c_SWFFill object
  virtual ~SWFFill() 
	{ destroySWFFill(this->fill); }

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
  SWF_DECLAREONLY(SWFFill);
  SWFFill();
};




/*  SWFBitmap  */

class SWFBitmap : public SWFCharacter
{
 public:
  c_SWFBitmap bitmap;

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->bitmap; }
  
  SWFBitmap(const char *filename, const char *alpha=NULL)
  {
    if(strlen(filename) > 4)
    {
      if(strcasecmp(filename+strlen(filename)-4, ".dbl") == 0)
	this->bitmap = (c_SWFBitmap) newSWFDBLBitmap(fopen(filename, "rb"));

      else if(strcasecmp(filename+strlen(filename)-4, ".gif") == 0)
	this->bitmap = (c_SWFBitmap) newSWFDBLBitmapData_fromGifFile(filename);

      else if(strcasecmp(filename+strlen(filename)-4, ".png") == 0)
 		this->bitmap =   (c_SWFBitmap) newSWFDBLBitmapData_fromPngFile( filename );

      else if(strcasecmp(filename+strlen(filename)-4, ".jpg") == 0 ||
	(strlen(filename) > 5 && (strcasecmp(filename+strlen(filename)-5, ".jpeg") == 0)))
      {
	if(alpha != NULL)
	  this->bitmap = (c_SWFBitmap) newSWFJpegWithAlpha(fopen(filename, "rb"),
					     fopen(alpha, "rb"));
	else
	  this->bitmap = (c_SWFBitmap) newSWFJpegBitmap(fopen(filename, "rb"));
      }

      else
	bitmap = NULL;
    }
    if ( ! this->bitmap ) 
      throw SWFException("SWFBitmap(const char *filename, const char *alpha=NULL)");

    this->character = (c_SWFCharacter)bitmap;
  }

  SWFBitmap(SWFInput *input)
  { 
    this->bitmap = newSWFBitmap_fromInput(input->input);
    if(this->bitmap == NULL)
      throw SWFException("SWFBitmap(SWFInput *input)");
    this->character = (c_SWFCharacter)bitmap;
  }

  virtual ~SWFBitmap()
    { destroySWFBitmap(this->bitmap); }
  
  int getWidth()
    { return SWFBitmap_getWidth(this->bitmap); }

  int getHeight()
    { return SWFBitmap_getHeight(this->bitmap); }

  SWF_DECLAREONLY(SWFBitmap);
  SWFBitmap();
};


class SWFFillStyle
{
 public:
  c_SWFFillStyle fill;
  
  virtual ~SWFFillStyle() { }

  static SWFFillStyle *SolidFillStyle(byte r, byte g, byte b, byte a=255)
    { return new SWFFillStyle(newSWFSolidFillStyle(r, g, b, a)); }

  static SWFFillStyle *GradientFillStyle(SWFGradient *gradient, byte flags)
    { return new SWFFillStyle(newSWFGradientFillStyle(gradient->gradient, flags)); }

  static SWFFillStyle *BitmapFillStyle(SWFBitmap *bitmap, byte flags)
    { return new SWFFillStyle(newSWFBitmapFillStyle(bitmap->bitmap, flags)); }

 private:
  SWFFillStyle(c_SWFFillStyle fill)
  { 
    this->fill = fill; 
    if(this->fill == NULL)
      throw SWFException("SWFFillStyle");
  }
  SWF_DECLAREONLY(SWFFillStyle);
  SWFFillStyle();
};   


/*  SWFFont  */

class SWFFont : public SWFBlock
{
 public:
  c_SWFFont font;

  SWFFont()
  { 
    this->font = newSWFFont(); 
    if(this->font == NULL)
      throw SWFException("SWFFont()");
  }

  SWFFont(FILE *file) // deprecated 
  {
	std::cerr << "SWFFont(FILE *file) is deprecated and will be removed in future releases." << std::endl;
	this->font = loadSWFFont_fromFdbFile(file);
	if(this->font == NULL)
		throw SWFException("SWFFont(FILE *file)");
  }

  SWFFont(char *path)
  { 
    this->font = newSWFFont_fromFile(path); 
    if(this->font == NULL)
      throw SWFException("SWFFont(char *path)");
  }

  virtual ~SWFFont()
    { destroySWFFont(/*(c_SWFBlock)*/this->font); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->font; }

  float getStringWidth(const char *string)
    { return SWFFont_getStringWidth(this->font, string); }

  float getWidth(const char *string)
    { return SWFFont_getStringWidth(this->font, string); }

  float getAscent()
    { return SWFFont_getAscent(this->font); }

  float getDescent()
    { return SWFFont_getDescent(this->font); }

  float getLeading()
    { return SWFFont_getLeading(this->font); }
  SWF_DECLAREONLY(SWFFont);
};

/* SWFBrowserFont */
class SWFBrowserFont : public SWFBlock
{
 public:
  c_SWFBrowserFont bfont;
  
  SWFBrowserFont(char *name)
  { 
    this->bfont = newSWFBrowserFont(name); 
    if(this->bfont == NULL)
      throw SWFException("SWFBrowserFont(char *name)");
  }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->bfont; }

  virtual ~SWFBrowserFont()
    { destroySWFBrowserFont(this->bfont); }
  
  SWF_DECLAREONLY(SWFBrowserFont);
};

/*  SWFShape  */
class SWFShape : public SWFCharacter
{
 public:
  c_SWFShape shape;

  SWFShape()
  { 
    this->shape = newSWFShape();
    if(this->shape == NULL)
      throw SWFException("SWFShape()");

    this->character = (c_SWFCharacter)shape;
  }

  SWFShape(c_SWFShape shape)
  { 
    this->shape = shape;
    if(this->shape == NULL)
      throw SWFException("SWFShape(c_SWFShape shape)");
  }

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

  SWFFill *addSolidFill(byte r, byte g, byte b, byte a=0xff)
    { return new SWFFill(SWFShape_addSolidFill(this->shape, r, g, b, a)); }

  SWFFill *addGradientFill(SWFGradient *gradient, byte flags=0)
    { return new SWFFill(SWFShape_addGradientFill(this->shape, gradient->gradient, flags)); }

  SWFFill *addBitmapFill(SWFBitmap *bitmap, byte flags=0)
    { return new SWFFill(SWFShape_addBitmapFill(this->shape, bitmap->bitmap, flags)); }

  void setLeftFillStyle(SWFFillStyle *fill)
    { SWFShape_setLeftFillStyle(this->shape, fill->fill); }

  void setRightFillStyle(SWFFillStyle *fill)
    { SWFShape_setRightFillStyle(this->shape, fill->fill); }

  void setLeftFill(SWFFill *fill)
    { SWFShape_setLeftFill(this->shape, fill->fill); }

  void setRightFill(SWFFill *fill)
    { SWFShape_setRightFill(this->shape, fill->fill); }

  void setLine(unsigned short width, byte r, byte g, byte b, byte a=0xff)
    { SWFShape_setLine(this->shape, width, r, g, b, a); }

// begin minguts 2004/08/31 ((((
  void drawCharacterBounds(SWFCharacter *character)
	{ SWFShape_drawCharacterBounds(this->shape, character->character); }

  void setLineStyle(unsigned short width, byte r, byte g, byte b, byte a=0xff) // alias for setline
	{ setLine(width, r, g, b, a); }
// )))) end minguts 2004/08/31

  void setLine2(unsigned short width, SWFFillStyle *fill, int flags, float miterLimit)
    {  SWFShape_setLine2Filled(this->shape, width, fill->fill, flags, miterLimit); }
  
  void setLine2(unsigned short width, byte r, byte g, byte b, byte a, int flags, float miterLimit)
    {  SWFShape_setLine2(this->shape, width, r, g, b, a, flags, miterLimit); }

  void drawArc(float r, float startAngle, float endAngle)
    { SWFShape_drawArc(this->shape, r, startAngle, endAngle); }

  void drawCircle(float r)
    { SWFShape_drawCircle(this->shape, r); }

  void drawGlyph(SWFFont *font, unsigned short c, int size=0)
    { SWFShape_drawSizedGlyph(this->shape, font->font, c, size); }

  void useVersion(int version)
    { SWFShape_useVersion(this->shape, version); }

  // deprecated?
  void drawFontGlyph(SWFFont *font, unsigned short c)
    { SWFShape_drawGlyph(this->shape, font->font, c); }

  SWF_DECLAREONLY(SWFShape);
};


/*  SWFSprite  */

class SWFSprite : public SWFCharacter
{
 public:
  c_SWFMovieClip clip;

  SWFSprite()
  { 
    this->clip = newSWFMovieClip();
    if(this->clip == NULL)
      throw SWFException("SWFSprite()");
 
    this->character = (c_SWFCharacter)clip;
  }

  virtual ~SWFSprite()
    { destroySWFMovieClip(this->clip); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->clip; }

  void setNumberOfFrames(int nFrames)
    { SWFMovieClip_setNumberOfFrames(this->clip, nFrames); }

  SWFDisplayItem *add(SWFBlock *character)
  { 
    c_SWFDisplayItem item = SWFMovieClip_add(this->clip, character->getBlock());
    if(item == NULL)
      return NULL;
    return new SWFDisplayItem(item); 
  }

  void remove(SWFDisplayItem *item)
    { SWFMovieClip_remove(this->clip, item->item); }

  void nextFrame()
    { SWFMovieClip_nextFrame(this->clip); }

  void labelFrame(char *label)
    { SWFMovieClip_labelFrame(this->clip, label); }

  void setScalingGrid(int x, int y, int w, int h)
    { SWFMovieClip_setScalingGrid(this->clip, x, y, w, h); }

  void removeScalingGrid()
    { SWFMovieClip_removeScalingGrid(this->clip); }

  void addInitAction(SWFAction* a)
    { SWFMovieClip_addInitAction(this->clip, a->action); }

  SWF_DECLAREONLY(SWFSprite);
};

/*  SWFMovieClip  */

class SWFMovieClip : public SWFCharacter
{
 public:
  c_SWFMovieClip clip;

  SWFMovieClip()
  { 
    this->clip = newSWFMovieClip();
    if(this->clip == NULL)
      throw SWFException("SWFMovieClip()");

    this->character = (c_SWFCharacter)clip;
  }

  virtual ~SWFMovieClip()
    { destroySWFMovieClip(this->clip); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->clip; }

  void setNumberOfFrames(int nFrames)
    { SWFMovieClip_setNumberOfFrames(this->clip, nFrames); }

  SWFDisplayItem *add(SWFBlock *character)
  {
    c_SWFDisplayItem item = SWFMovieClip_add(this->clip, character->getBlock());
    if(item == NULL)
      return NULL;
    else
      return new SWFDisplayItem(item); 
  }

  void remove(SWFDisplayItem *item)
    { SWFMovieClip_remove(this->clip, item->item); }

  void nextFrame()
    { SWFMovieClip_nextFrame(this->clip); }

  void labelFrame(char *label)
    { SWFMovieClip_labelFrame(this->clip, label); }

  void addInitAction(SWFAction* a)
    { SWFMovieClip_addInitAction(this->clip, a->action); }

  SWF_DECLAREONLY(SWFMovieClip);
};


/*  SWFMorph  */

class SWFMorph : public SWFCharacter
{
 public:
  c_SWFMorph morph;

  SWFMorph()
  { 
    this->morph = newSWFMorphShape();
    if(this->morph == NULL)
      throw SWFException("SWFMorph()");

    this->character = (c_SWFCharacter)morph;
  }

  virtual ~SWFMorph()
    { destroySWFMorph(this->morph); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->morph; }

  SWFShape *getShape1()
    { return new SWFShape(SWFMorph_getShape1(this->morph)); }

  SWFShape *getShape2()
    { return new SWFShape(SWFMorph_getShape2(this->morph)); }
  SWF_DECLAREONLY(SWFMorph);
};


/*  SWFText  */

class SWFText : public SWFCharacter
{
 public:
  c_SWFText text;

  SWFText(int version = 2)
  { 
      if(version == 2)
        this->text = newSWFText2();
      else 
        this->text = newSWFText();
      if(this->text == NULL)
        throw SWFException("SWFText()");
      this->character = (c_SWFCharacter)text;
  }

  virtual ~SWFText()
    { destroySWFText(this->text); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->text; }

  void setFont(SWFFont *font)
    { SWFText_setFont(this->text, font->font); }

  void setHeight(float height)
    { SWFText_setHeight(this->text, height); }

  // I don't like this..  it's too easy to confuse with displayitem::moveTo
  void moveTo(float x, float y)
    { SWFText_moveTo(this->text, x, y); }

  void setColor(byte r, byte g, byte b, byte a=0xff)
    { SWFText_setColor(this->text, r, g, b, a); }

  void addString(const char *string, int *advance=NULL)
    { SWFText_addString(this->text, string, advance); }

  void addUTF8String(const char *string, int *advance=NULL)
    { SWFText_addUTF8String(this->text, string, advance); }

  void setSpacing(float spacing)
    { SWFText_setSpacing(this->text, spacing); }

  float getStringWidth(const char *string)
    { return SWFText_getStringWidth(this->text, string); }

  float getWidth(const char *string)
    { return SWFText_getStringWidth(this->text, string); }

  float getUTF8Width(const char *string)
    { return SWFText_getUTF8StringWidth(this->text, string); }
  SWF_DECLAREONLY(SWFText);
};

/*  SWFTextField  */

class SWFTextField : public SWFCharacter
{
 public:
  c_SWFTextField textField;

  SWFTextField()
  { 
    this->textField = newSWFTextField();
    if(this->textField == NULL)
      throw SWFException("SWFTextField()");
    this->character = (c_SWFCharacter)textField;
  }

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

  void setColor(byte r, byte g, byte b, byte a=0xff)
    { SWFTextField_setColor(this->textField, r, g, b, a); }

  void setVariableName(const char *name)
    { SWFTextField_setVariableName(this->textField, name); }

  void addString(const char *string)
    { SWFTextField_addString(this->textField, string); }

  void addUTF8String(const char *string)
    { SWFTextField_addUTF8String(this->textField, string); }

  void setHeight(float height)
    { SWFTextField_setHeight(this->textField, height); }

  void setLeftMargin(float margin)
    { SWFTextField_setLeftMargin(this->textField, margin); }

  void setRightMargin(float margin)
    { SWFTextField_setRightMargin(this->textField, margin); }

  void setIndentation(float indentation)
    { SWFTextField_setIndentation(this->textField, indentation); }

  void setPadding(float padding)
    { SWFTextField_setPadding(this->textField, padding); }

  void setLineSpacing(float lineSpacing)
    { SWFTextField_setLineSpacing(this->textField, lineSpacing); }

  void setAlignment(SWFTextFieldAlignment alignment)
    { SWFTextField_setAlignment(this->textField, alignment); }

  void align(SWFTextFieldAlignment alignment)
    { SWFTextField_setAlignment(this->textField, alignment); }

  void setLength(int length)
    { SWFTextField_setLength(this->textField, length); }
  SWF_DECLAREONLY(SWFTextField);
};

class SWFButtonRecord
{
 friend class SWFButton;
 public:
  c_SWFButtonRecord record;

  void addFilter(SWFFilter *f)
    { SWFButtonRecord_addFilter(this->record, f->filter); }

  void setDepth(int depth)
    { SWFButtonRecord_setDepth(this->record, depth); }

  void setBlendMode(int mode)
    { SWFButtonRecord_setBlendMode(this->record, mode); }

  void move(float x, float y)
    { SWFButtonRecord_move(this->record, x, y); }

  void moveTo(float x, float y)
    { SWFButtonRecord_moveTo(this->record, x, y); }

  void rotate(float deg)
    { SWFButtonRecord_rotate(this->record, deg); }

  void rotateTo(float deg)
    { SWFButtonRecord_rotateTo(this->record, deg); }

  void scale(float scaleX, float scaleY)
    { SWFButtonRecord_scale(this->record, scaleX, scaleY); }

  void scaleTo(float scaleX, float scaleY)
    { SWFButtonRecord_scaleTo(this->record, scaleX, scaleY); }

  void skewX(float skewX)
    { SWFButtonRecord_skewX(this->record, skewX); }

  void skewY(float skewY)
    { SWFButtonRecord_skewY(this->record, skewY); }

  void skewXTo(float skewX)
    { SWFButtonRecord_skewXTo(this->record, skewX); }

  void skewYTo(float skewY)
    { SWFButtonRecord_skewYTo(this->record, skewY); }

 private:
  SWFButtonRecord(c_SWFButtonRecord record)
  { 
    this->record = record; 
    if(this->record == NULL)
      throw SWFException("ButtonRecord");
  }
  SWF_DECLAREONLY(SWFButtonRecord);	
};

/*  SWFButton  */

class SWFButton : public SWFCharacter
{
 public:
  c_SWFButton button;

  SWFButton()
  { 
    this->button = newSWFButton();
    if(this->button == NULL)
      throw SWFException("SWFButton()");
    this->character = (c_SWFCharacter)button;
  }

  virtual ~SWFButton()
    { destroySWFButton(this->button); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->button; }

  SWFButtonRecord* addShape(SWFCharacter *character, byte flags)
    { SWFButton_addCharacter(this->button, (c_SWFCharacter)character->getBlock(), flags); }

  void addAction(SWFAction *action, int flags)
    { SWFButton_addAction(this->button, action->action, flags); }

  void setMenu(int flag=0)
    { SWFButton_setMenu(this->button, flag); }

  void addSound(SWFSound *sound, int flags)
    { SWFButton_addSound(this->button, sound->sound, flags); }

  void setScalingGrid(int x, int y, int w, int h)
    { SWFButton_setScalingGrid(this->button, x, y, w, h); }

  void removeScalingGrid()
    { SWFButton_removeScalingGrid(this->button); }

  SWFButtonRecord* addCharacter(SWFCharacter *character, byte flags)
    { return new SWFButtonRecord(
        SWFButton_addCharacter(this->button, (c_SWFCharacter)character->getBlock(), flags)); }

  SWF_DECLAREONLY(SWFButton);
};


/* SWFVideoStream */
class SWFVideoStream : public SWFCharacter
{
 public:
  c_SWFVideoStream stream;

  SWFVideoStream()
  { 
    this->stream = newSWFVideoStream();
    if(this->stream == NULL) 
      throw SWFException("newSWFVideoStream()");
    this->character = (c_SWFCharacter)stream;
  }

  SWFVideoStream(const char *path)
  { 
    this->stream = newSWFVideoStream_fromFile(fopen(path, "rb")); 
    if(this->stream == NULL)
      throw SWFException("SWFVideoStream(const char *path)");
  }

  SWFVideoStream(FILE *file)
  { 
    this->stream = newSWFVideoStream_fromFile(file); 
    if(this->stream == NULL)
      throw SWFException(" SWFVideoStream(FILE *file)");
  }

  virtual ~SWFVideoStream()
    { destroySWFVideoStream(this->stream); }

  void setDimension(int width, int height)
    { SWFVideoStream_setDimension(this->stream, width, height); }

  int getNumFrames()
    { return SWFVideoStream_getNumFrames(this->stream); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->stream; }

  SWF_DECLAREONLY(SWFVideoStream);

};

#endif /* SWF_MINGPP_H_INCLUDED */
