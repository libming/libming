/* ====================================================================
 * Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the same terms as Perl itself.
 * ====================================================================
 */

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "SWF.h"

//XXX Temporary solution. I hate this!!
void stdoutByteOutputMethod(byte b, void *data){
	fprintf(stdout, "%c", b);	
}
void fileByteOutputMethod(byte b, void *data)
{
  fprintf((FILE *)data, "%c", b);
}


MODULE = SWF		PACKAGE = SWF		
PROTOTYPES: ENABLE

void
import(pclass, ...)
    SV *pclass

    PREINIT:
    I32 i = 0;
    SV *caller = perl_eval_pv("scalar caller", TRUE);

    CODE:
    for(i=1; i<items; i++) {
        my_import(pclass, caller, ST(i));
    }

int
SWFBUTTON_KEYPRESS(c)
    char c

int
SWFBUTTON_ONKEYPRESS(c)
    char c

byte
SWFFILL_SOLID()
    CODE:
    RETVAL = SWFFILL_SOLID;
    OUTPUT:
    RETVAL

byte
SWFFILL_GRADIENT()
    CODE:
    RETVAL = SWFFILL_GRADIENT;
    OUTPUT:
    RETVAL

byte
SWFFILL_LINEAR_GRADIENT()
   CODE:
   RETVAL = SWFFILL_LINEAR_GRADIENT;
   OUTPUT:
   RETVAL

byte
SWFFILL_RADIAL_GRADIENT()
   CODE:
   RETVAL = SWFFILL_RADIAL_GRADIENT;
   OUTPUT:
   RETVAL


byte
SWFFILL_BITMAP()
    CODE:
    RETVAL = SWFFILL_BITMAP;
    OUTPUT:
    RETVAL


byte
SWFFILL_TILED_BITMAP()
    CODE:
    RETVAL = SWFFILL_TILED_BITMAP;
    OUTPUT:
    RETVAL

byte
SWFFILL_CLIPPED_BITMAP()
    CODE:
    RETVAL = SWFFILL_CLIPPED_BITMAP;
    OUTPUT:
    RETVAL

byte
SWFBUTTON_HIT()
    CODE:
    RETVAL = SWFBUTTON_HIT;
    OUTPUT:
    RETVAL

byte
SWFBUTTON_DOWN()
    CODE:
    RETVAL = SWFBUTTON_DOWN;
    OUTPUT:
    RETVAL

byte
SWFBUTTON_OVER()
    CODE:
    RETVAL = SWFBUTTON_OVER;
    OUTPUT:
    RETVAL

byte
SWFBUTTON_UP()
    CODE:
    RETVAL = SWFBUTTON_UP;
    OUTPUT:
    RETVAL

byte
SWFBUTTON_MOUSEOVER()
    CODE:
    RETVAL = SWFBUTTON_MOUSEOVER;
    OUTPUT:
    RETVAL

byte
SWFBUTTON_MOUSEOUT()
    CODE:
    RETVAL = SWFBUTTON_MOUSEOUT;
    OUTPUT:
    RETVAL

byte
SWFBUTTON_MOUSEDOWN()
    CODE:
    RETVAL = SWFBUTTON_MOUSEDOWN;
    OUTPUT:
    RETVAL

byte
SWFBUTTON_MOUSEUP()
    CODE:
    RETVAL = SWFBUTTON_MOUSEUP;
    OUTPUT:
    RETVAL

double
SWFBUTTON_DRAGOUT()
    CODE:
    RETVAL = SWFBUTTON_DRAGOUT;
    OUTPUT:
    RETVAL

byte
SWFBUTTON_DRAGOVER()
    CODE:
    RETVAL = SWFBUTTON_DRAGOVER;
    OUTPUT:
    RETVAL

byte
SWFBUTTON_MOUSEUPOUTSIDE()
    CODE:
    RETVAL = SWFBUTTON_MOUSEUPOUTSIDE;
    OUTPUT:
    RETVAL

int
SWFTEXTFIELD_NOEDIT()
   CODE:
   RETVAL= SWFTEXTFIELD_NOEDIT;
   OUTPUT:
   RETVAL

int
SWFTEXTFIELD_PASSWORD()
   CODE:
   RETVAL= SWFTEXTFIELD_PASSWORD;
   OUTPUT:
   RETVAL

int
SWFTEXTFIELD_DRAWBOX()
   CODE:
   RETVAL= SWFTEXTFIELD_DRAWBOX;
   OUTPUT:
   RETVAL

int
SWFTEXTFIELD_MULTILINE()
   CODE:
   RETVAL= SWFTEXTFIELD_MULTILINE;
   OUTPUT:
   RETVAL

int
SWFTEXTFIELD_WORDWRAP()
   CODE:
   RETVAL= SWFTEXTFIELD_WORDWRAP;
   OUTPUT:
   RETVAL

int
SWFTEXTFIELD_NOSELECT()
   CODE:
   RETVAL= SWFTEXTFIELD_NOSELECT;
   OUTPUT:
   RETVAL

int
SWFTEXTFIELD_ALIGN_LEFT()
   CODE:
   RETVAL= SWFTEXTFIELD_ALIGN_LEFT;
   OUTPUT:
   RETVAL

int
SWFTEXTFIELD_ALIGN_RIGHT()
   CODE:
   RETVAL= SWFTEXTFIELD_ALIGN_RIGHT;
   OUTPUT:
   RETVAL

int
SWFTEXTFIELD_ALIGN_CENTER()
   CODE:
   RETVAL= SWFTEXTFIELD_ALIGN_CENTER;
   OUTPUT:
   RETVAL

int
SWFTEXTFIELD_ALIGN_JUSTIFY()
   CODE:
   RETVAL= SWFTEXTFIELD_ALIGN_JUSTIFY;
   OUTPUT:
   RETVAL

void
fileOutputMethod(b, data)
	byte	b
	void *	data


MODULE = SWF		PACKAGE = SWF::Bitmap		PREFIX = SWFBitmap_

SWF::Bitmap
SWFBitmap_new(package="SWF::Bitmap", filename, mask=NULL)
	char    *package
	char    *filename = NO_INIT
        char    *mask
        PREINIT:
        CV      *cv;
        STRLEN  len;
        char   *my_sub;
	CODE:
        filename = (char *) SvPV(ST(1), len);
        if( strncasecmp(filename+len-4, ".jpg", 4) == 0 ||
            strncasecmp(filename+len-5, ".jpeg", 5) == 0)
            my_sub = mask ? "SWF::Bitmap::newSWFJpegWithAlpha" : "SWF::Bitmap::newSWFJpegBitmap";
        else if(strncasecmp(filename+len-4, ".dbl", 4) == 0)
            my_sub = "SWF::Bitmap::newSWFDBLBitmapFile";
        else
            croak("argument to SWF::Bitmap::New must be a JPEG or dbl filename");

        PUSHMARK(mark);
        cv = GvCV(gv_fetchpv(my_sub, FALSE, SVt_PVCV));
#ifdef PERL_OBJECT
        (void)(*CvXSUB(cv))(cv, pPerl);
#else
        (void)(*CvXSUB(cv))(aTHXo_ cv);
#endif


SWF::Bitmap
newSWFDBLBitmapFile(package="SWF::Bitmap", filename)
         char    *package
         char    *filename
         PREINIT: 
         FILE    *dbl;
         CODE:
         if ( !(dbl = fopen(filename, "rb")) ){
             fprintf(stderr, "Unable to open %s\n", filename);
             ST(0) = &sv_undef;
         }else{
                RETVAL = newSWFDBLBitmap(dbl);
                ST(0) = sv_newmortal();
                sv_setref_pv(ST(0), package, (void*)RETVAL);
         }

SWF::Bitmap
newSWFJpegWithAlpha(package="SWF::Bitmap", filename, mask)
        char    *package
        char    *filename
        char    *mask
        PREINIT:
        FILE    *f;
        FILE    *alpha;
        CODE:
        if ( !(f = fopen(filename, "rb")) ){
            fprintf(stderr, "Unable to open %s\n", filename);
            ST(0) = &sv_undef;
        }
        else{
            if ( !(alpha = fopen(mask, "rb")) ){
                fprintf(stderr, "Unable to open %s\n", mask);
                ST(0) = &sv_undef;
            }
            else{
                RETVAL = newSWFJpegWithAlpha(f, alpha);
                ST(0) = sv_newmortal();
                sv_setref_pv(ST(0), package, (void*)RETVAL);
           }
        }



SWF::Bitmap
newSWFJpegBitmap(package="SWF::Bitmap", filename)
        char    *package
        char    *filename
	PREINIT:
	FILE    *f;
	CODE:
	if (!(f = fopen(filename, "rb"))) {
		fprintf(stderr, "Unable to open %s\n", filename);
		ST(0) = &sv_undef;
	}else{
        	RETVAL = newSWFJpegBitmap(f);
	        ST(0) = sv_newmortal();
        	sv_setref_pv(ST(0), package, (void*)RETVAL);
	} 



int
SWFBitmap_getWidth(b)
	SWF::Bitmap	b

int
SWFBitmap_getHeight(b)
	SWF::Bitmap	b

MODULE = SWF		PACKAGE = SWF::Fill		PREFIX = SWFFill_

void 
SWFFill_moveTo(fill, x, y)
        SWF::Fill       fill
        int             x
        int             y

void
SWFFill_scaleTo(fill, x, ...)
	SWF::Fill       fill
        float           x
        float           y = NO_INIT
        CODE:
        y = (items > 2) ? (float)SvNV(ST(2)) : x; 
        SWFFill_scaleXYTo(fill, x, y);

void
SWFFill_rotateTo(fill, degrees)
        SWF::Fill       fill
        float           degrees

void
SWFFill_skewXTo(fill, x)
        SWF::Fill       fill
        float           x

void
SWFFill_skewYTo(fill, y)
        SWF::Fill       fill
        float           y


MODULE = SWF		PACKAGE = SWF::Gradient		PREFIX = SWFGradient_

SWF::Gradient
SWFGradient_new(package="SWF::Gradient")
	char *package
	CODE:
        RETVAL = newSWFGradient();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);



void
SWFGradient_addEntry(gradient, ratio, r, g, b, a=0xff)
	SWF::Gradient	gradient
	float		ratio
	byte		r
	byte		g
	byte		b
	byte		a


MODULE = SWF		PACKAGE = SWF::Movie		PREFIX = SWFMovie_

void
destroySWFMovie(movie)
	SWF::Movie	movie
        ALIAS:
	SWF::Movie::DESTROY = 1

SWF::Movie
SWFMovie_new(package="SWF::Movie")
	char *package
	CODE:
        RETVAL = newSWFMovie();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);


SWF::Movie
newSWFMovieWithVersion(version)
	int	version

void
SWFMovie_setRate(movie, rate)
	SWF::Movie	movie
	float	rate

void
SWFMovie_setDimension(movie, x, y)
	SWF::Movie	movie
	int	x
	int	y

void
SWFMovie_setNumberOfFrames(movie, frames)
	SWF::Movie	movie
	int	frames

void
SWFMovie_setBackground(movie, r, g, b)
	SWF::Movie	movie
	int	r
	int	g
	int	b

void
SWFMovie_nextFrame(movie)
	SWF::Movie	movie

void
SWFMovie_labelFrame(movie, label)
	SWF::Movie	movie
	char *	label

int 
SWFMovie_output(movie)
	SWF::Movie movie
	CODE:
	RETVAL = SWFMovie_output(movie, &stdoutByteOutputMethod, NULL);

int 
SWFMovie_save(movie, filename)
	SWF::Movie movie
        char *     filename
        PREINIT: 
        FILE    *f;
	CODE:
	if (!(f = fopen(filename, "wb"))) {
		fprintf(stderr, "Unable to open %s\n", filename);
		ST(0) = &sv_undef;
	}else{
                RETVAL = SWFMovie_output(movie, &fileByteOutputMethod, (void *)f);
                fclose(f);
	}
        OUTPUT:
        RETVAL


SWF::DisplayItem
SWFMovie_add(movie, block)
	SWF::Movie movie
	SWF::Block block = (SWF__Block) SvIV((SV*)SvRV(ST(1)));


MODULE = SWF		PACKAGE = SWF::DisplayItem	PREFIX = SWFDisplayItem_

void
SWFDisplayItem_moveTo(item, x, y)
	SWF::DisplayItem item
	int x 
	int y

void
SWFDisplayItem_move(item, x, y)
	SWF::DisplayItem item
	int x 
	int y


void 
SWFDisplayItem_scaleTo(item, x, ...)
        SWF::DisplayItem item
        float            x
        float            y   = NO_INIT
        CODE:
        y = (items > 2) ? (float)SvNV(ST(2)) : x; 
        SWFDisplayItem_scaleTo(item, x, y);

void 
SWFDisplayItem_scale(item, x, ...)
        SWF::DisplayItem item
        float            x
        float            y   = NO_INIT
        CODE:
        y = (items > 2) ? (float)SvNV(ST(2)) : x; 
        SWFDisplayItem_scale(item, x, y);


void 
SWFDisplayItem_rotate(item, degrees)
	SWF::DisplayItem item
	float degrees

void 
SWFDisplayItem_rotateTo(item, degrees)
	SWF::DisplayItem item
	float degrees

void 
SWFDisplayItem_skewX(item, x)
        SWF::DisplayItem item 
        float x


void
SWFDisplayItem_skewXTo(item, x)
       SWF::DisplayItem item
       float x

void 
SWFDisplayItem_skewY(item, y)
        SWF::DisplayItem item
        float y

void 
SWFDisplayItem_skewYTo(item, y)
        SWF::DisplayItem item
        float y

void
SWFDisplayItem_setDepth(item, depth)
        SWF::DisplayItem item
        int depth

void
SWFDisplayItem_setRatio(item, ratio)
        SWF::DisplayItem item
        float ratio

void
SWFDisplayItem_setColorAdd(item, r, g, b, a=0)
        SWF::DisplayItem item
        int r
        int g
        int b
        int a
        ALIAS:
        SWF::DisplayItem::addColor = 1

void
SWFDisplayItem_setColorMult(item, r, g, b, a=1.0)
        SWF::DisplayItem item
        float r
        float g
        float b
        float a
        ALIAS:
        SWF::DisplayItem::multColor = 1

void 
SWFDisplayItem_setName(item, name)
       SWF::DisplayItem item
       char *name


MODULE = SWF		PACKAGE = SWF::Shape		PREFIX = SWFShape_

SWF::Shape
SWFShape_new(package="SWF::Shape")
     	char *package
	CODE:
        RETVAL = newSWFShape();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);

#void
#destroySWFShape(block)
#	SWF::Block	block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
#        ALIAS:
#        SWF::Shape::DESTROY = 1
#        CODE:
#        fprintf(stderr, "Shape DESTROY CALLED\n");
#        destroySWFShape(block);

void
SWFShape_moveTo(shape, x, y)
	SWF::Shape	shape
	int	x
	int	y
        ALIAS:
        SWF::Shape::movePenTo = 1

void 
SWFShape_moveToRelative(shape, x, y)
        SWF::Shape shape
        int x
        int y
        ALIAS:
        SWF::Shape::movePen = 1

int
SWFShape_getPenX(shape)
	SWF::Shape	shape

int
SWFShape_getPenY(shape)
	SWF::Shape	shape

void
SWFShape_lineTo(shape, x, y)
	SWF::Shape	shape
	int	x
	int	y
	ALIAS:
	SWF::Shape::drawLineTo = 1


void
SWFShape_lineToRelative(shape, dx, dy)
	SWF::Shape	shape
	int	dx
	int	dy
        ALIAS:
        SWF::Shape::drawLine = 1


void
SWFShape_curveTo(shape, controlx, controly, anchorx, anchory)
	SWF::Shape	shape
	int		controlx
	int		controly
	int		anchorx
	int		anchory
        ALIAS:
        SWF::Shape::drawCurveTo = 1

void
SWFShape_curveToRelative(shape, controldx, controldy, anchordx, anchordy)
	SWF::Shape	shape
	int		controldx
	int		controldy
	int		anchordx
	int		anchordy
        ALIAS:
        SWF::Shape::drawCurve = 1

#void
#SWFShape_end(shape)
#	SWFShape	shape

int
SWFShape_setLineStyle(shape, width, r, g, b, a=0xff)
	SWF::Shape	shape
	unsigned short	width
	byte	r
	byte	g
	byte	b
	byte	a
	ALIAS:
	SWF::Shape::setLine = 1

SWF::Fill
SWFShape_addFill(shape, ...)
       SWF::Shape      shape
       PREINIT:
       CV        *cv;
       char      *my_sub;
       CODE:
       PUSHMARK(mark);
       if (sv_derived_from(ST(1), "SWF::Gradient"))
           my_sub = "SWF::Shape::addGradientFill";
       else if(sv_derived_from(ST(1), "SWF::Bitmap"))
           my_sub = "SWF::Shape::addBitmapFill";
       else
           my_sub = "SWF::Shape::addSolidFill";
       cv = GvCV(gv_fetchpv(my_sub, FALSE, SVt_PVCV));
#ifdef PERL_OBJECT
        (void)(*CvXSUB(cv))(cv, pPerl);
#else
        (void)(*CvXSUB(cv))(aTHXo_ cv);
#endif


SWF::Fill
SWFShape_addSolidFill(shape, r, g, b, a=0xff)
        SWF::Shape      shape
        byte            r
        byte            g
        byte            b
        byte            a

SWF::Fill
SWFShape_addGradientFill(shape, gradient, flags=SWFFILL_LINEAR_GRADIENT)
	SWF::Shape      shape
	SWF::Gradient   gradient
	byte		flags

SWF::Fill
SWFShape_addBitmapFill(shape, bitmap, flags=SWFFILL_TILED_BITMAP)
	SWF::Shape	shape
	SWF::Bitmap	bitmap
	byte	        flags

void
SWFShape_setLeftFill(shape, fill)
	SWF::Shape	shape
	SWF::Fill	fill

void
SWFShape_setRightFill(shape, fill)
	SWF::Shape	shape
	SWF::Fill	fill

void
SWFShape_drawFontGlyph(shape, font, c)
        SWF::Shape shape
        SWF::Font font
        int c
        ALIAS:
        SWF::Shape::drawGlyph = 1

void
SWFShape_drawArc(shape, r, startAngle, endAngle)
        SWF::Shape shape
        int r
        float startAngle
        float endAngle

void
SWFShape_drawCircle(shape, r)
        SWF::Shape shape
        int r

MODULE = SWF		PACKAGE = SWF::Morph		PREFIX = SWFMorph_

SWF::Morph
SWFMorph_new(package="SWF::Morph")
	char *package
	CODE:
        RETVAL = newSWFMorphShape();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);


#void
#destroySWFMorph(block)
#	SWF::Block	block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
#        ALIAS:
#        SWF::Morph::DESTROY = 1
#        CODE:
#        fprintf(stderr, "Morph DESTROY CALLED\n");
#        destroySWFMorph(block);


SWF::Shape
SWFMorph_getShape1(morph)
	SWF::Morph	morph

SWF::Shape
SWFMorph_getShape2(morph)
	SWF::Morph	morph


MODULE = SWF		PACKAGE = SWF::Font		PREFIX = SWFFont_

SWF::Font
SWFFont_new(package="SWF::Font", filename)
	char *package
	char *filename
	PREINIT:
	FILE *f;
	CODE:
	if (!(f = fopen(filename, "rb"))) {
		fprintf(stderr, "Unable to open %s\n", filename);
		ST(0) = &sv_undef;
	}else{
        	RETVAL = loadSWFFontFromFile(f);
                fclose(f);
	        ST(0) = sv_newmortal();
        	sv_setref_pv(ST(0), package, (void*)RETVAL);
	}


#void
#destroySWFFont(block)
#        SWF::Block      block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
#        ALIAS:
#        SWF::Font::DESTROY = 1
#        CODE:
#        fprintf(stderr, "FONT DESTROY CALLED\n");
#        destroySWFFont(block);


int
SWFFont_getStringWidth(font, string)
	SWF::Font	font
	char *	string
        ALIAS:
        SWF::Font::getWidth = 1

short
SWFFont_getAscent(font)
	SWF::Font	font

short
SWFFont_getDescent(font)
	SWF::Font	font

short
SWFFont_getLeading(font)
	SWF::Font	font

MODULE = SWF		PACKAGE = SWF::Text		PREFIX = SWFText_

SWF::Text
SWFText_new(package="SWF::Text")
	char *package
	CODE:
        RETVAL = newSWFText2();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);


#void
#destroySWFText(block)
#        SWF::Block      block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
#        ALIAS:
#        SWF::Text::DESTROY = 1
#	CODE:
#        fprintf(stderr, "Text DESTROY CALLED\n");
#        destroySWFText(block);


void
SWFText_setFont(text, font)
	SWF::Text	text
	SWF::Font	font

void
SWFText_setHeight(text, height)
	SWF::Text	text
	int		height

void
SWFText_moveTo(text, x, y)
	SWF::Text	text
	int		x
	int		y

void
SWFText_setColor(text, r, g, b, a=0xff)
	SWF::Text	text
	byte		r
	byte		g
	byte		b
	byte		a

void
SWFText_addString(text, string, advance=NULL)
	SWF::Text	text
	char *		string
	int *		advance
	

void
SWFText_setSpacing(text, spacing)
	SWF::Text	text
	float		spacing

int
SWFText_getStringWidth(text, string)
	SWF::Text	text
	char *		string
        ALIAS:
        SWF::Text::getWidth = 1

short
SWFText_getAscent(text)
	SWF::Text	text

short
SWFText_getDescent(text)
	SWF::Text	text

short
SWFText_getLeading(text)
	SWF::Text	text

MODULE = SWF		PACKAGE = SWF::TextField	PREFIX = SWFTextField_

SWF::TextField
SWFTextField_new(package="SWF::TextField", ...)
	char *package
        PREINIT:
        int flags;
	CODE:
        RETVAL = newSWFTextField();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);
        if(items>1){
            flags = (int)SvIV(ST(1));
            SWFTextField_setFlags(RETVAL, flags);
        }

#void
#destroySWFTextField(block)
#        SWF::Block      block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
#        ALIAS:
#        SWF::TextField::DESTROY = 1

void
SWFTextField_setFont(field, font)
	SWF::TextField	field
	SWF::Block	font = (SWF__Block) SvIV((SV*)SvRV(ST(1)));

void
SWFTextField_setBounds(field, width, height)
	SWF::TextField	field
	int	width
	int	height

void
SWFTextField_setFlags(field, flags)
	SWF::TextField	field
	int	flags

void
SWFTextField_setColor(field, r, g, b, a=0xff)
	SWF::TextField	field
	byte	r
	byte	g
	byte	b
	byte	a

void
SWFTextField_setVariableName(field, name)
	SWF::TextField	field
	char *	name
        ALIAS:
        SWF::TextField::setName = 1

void
SWFTextField_addString(field, string)
	SWF::TextField	field
	char *	string

void
SWFTextField_setHeight(field, height)
	SWF::TextField	field
	int	height

void
SWFTextField_setMargin(field, left, right)
	SWF::TextField	field
	int	left
        int     right
        CODE:
        SWFTextField_setLeftMargin(field, left);
        SWFTextField_setRightMargin(field, right);


void
SWFTextField_setLeftMargin(field, leftMargin)
	SWF::TextField	field
	int	leftMargin

void
SWFTextField_setRightMargin(field, rightMargin)
	SWF::TextField	field
	int	rightMargin

void
SWFTextField_setIndentation(field, indentation)
	SWF::TextField	field
	int	indentation

void
SWFTextField_setLineSpacing(field, lineSpacing)
	SWF::TextField	field
	int	lineSpacing

void
SWFTextField_setAlignment(field, alignment)
	SWF::TextField	field
	int	alignment
        ALIAS:
        SWF::TextField::align =  1


MODULE = SWF		PACKAGE = SWF::Action		PREFIX = SWFAction_

SWF::Action
SWFAction_new(package="SWF::Action", script=NULL)
	char *package
        char *script
	CODE:
        RETVAL = script ? compileSWFActionCode(script) : newSWFAction();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);


MODULE = SWF		PACKAGE = SWF::Button		PREFIX = SWFButton_

SWF::Button
SWFButton_new(package="SWF::Button")
	char *package
	CODE:
        RETVAL = newSWFButton();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);



#void
#destroySWFButton(block)
#        SWF::Block      block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
#        ALIAS:
#        SWF::Button::DESTROY = 1
#        CODE:
#        fprintf(stderr, "Button DESTROY CALLED\n");
#        destroySWFButton(block);

void
SWFButton_addShape(button, character, flags=0)
	SWF::Button	button
	SWF::Character	character = NO_INIT
	byte	flags
        ALIAS:
        SWF::Button::setOver   = 1
        SWF::Button::setHit    = 2
        SWF::Button::setUp     = 3
        SWF::Button::setDown   = 4
        CODE:
        character = (SWF__Character) SvIV((SV*)SvRV(ST(1)));
        switch((ix = XSANY.any_i32)) {
            case 0:
               SWFButton_addShape(button, character, flags);
               break;
            case 1:
               SWFButton_addShape(button, character, SWFBUTTONRECORD_OVERSTATE);
               break;
            case 2:
               SWFButton_addShape(button, character, SWFBUTTONRECORD_HITSTATE);
               break;
            case 3:
               SWFButton_addShape(button, character, SWFBUTTONRECORD_UPSTATE);
               break;
            case 4:
               SWFButton_addShape(button, character, SWFBUTTONRECORD_DOWNSTATE);
               break;
       }


void
SWFButton_addAction(button, action, flags=SWFBUTTON_OVERDOWNTOOVERUP)
	SWF::Button	button
	SWF::Action	action
        int             flags
        ALIAS:
        SWF::Button::setAction = 1


MODULE = SWF		PACKAGE = SWF::MovieClip		PREFIX = SWFMovieClip_

SWF::MovieClip
SWFMovieClip_new(package="SWF::MovieClip")
	char *package
	CODE:
        RETVAL = newSWFMovieClip();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);


SWF::DisplayItem
SWFMovieClip_add(clip, block)
        SWF::MovieClip clip
        SWF::Block block = (SWF__Block) SvIV((SV*)SvRV(ST(1)));
	
void
SWFMovieClip_remove(clip, item)
        SWF::MovieClip clip
        SWF::DisplayItem item

void
SWFMovieClip_nextFrame(clip)
        SWF::MovieClip clip


void
SWFMovieClip_labelFrame(clip, label)
        SWF::MovieClip clip
        char *label

void
SWFMovieClip_setNumberOfFrames(clip, frames)
        SWF::MovieClip clip
        int frames

#void
#destroySWFMovieClip(clip)
#	SWF::MovieClip clip
#        ALIAS:
#        SWF::MovieClip::DESTROY = 1








