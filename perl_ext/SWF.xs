/* ====================================================================
 * Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the same terms as Perl itself.
 * ====================================================================
 *
 * $Author$
 * $Id$
 */


#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "SWF.h"
#include "swf_util.c"


MODULE = SWF		PACKAGE = SWF		
PROTOTYPES: ENABLE

void
fileOutputMethod(b, data)
	byte	b
	void *	data


void 
Ming_setScale(scale)
    float scale
    ALIAS:
        SWF::setScale = 1

MODULE = SWF		PACKAGE = SWF::Bitmap		PREFIX = SWFBitmap_

SWF::Bitmap
SWFBitmap_new(package="SWF::Bitmap", filename, alpha=NULL)
	char    *package
	char    *filename = NO_INIT
        char    *alpha
        PREINIT:
        CV      *cv;
        STRLEN  len;
        char   *my_sub;
	CODE:
        filename = (char *) SvPV(ST(1), len);
        if( strncasecmp(filename+len-4, ".jpg", 4) == 0 ||
            strncasecmp(filename+len-5, ".jpeg", 5) == 0)
            my_sub = alpha ? "SWF::Bitmap::newSWFJpegWithAlpha" : "SWF::Bitmap::newSWFJpegBitmap";
        else if(strncasecmp(filename+len-4, ".dbl", 4) == 0)
            my_sub = "SWF::Bitmap::newSWFDBLBitmap";
        else
            croak("argument to SWF::Bitmap::New must be a JPG or dbl filename");

        PUSHMARK(mark);
        cv = GvCV(gv_fetchpv(my_sub, FALSE, SVt_PVCV));
#ifdef PERL_OBJECT
        (void)(*CvXSUB(cv))(cv, pPerl);
#else
        (void)(*CvXSUB(cv))(aTHXo_ cv);
#endif


SWF::Bitmap
newSWFDBLBitmap(package="SWF::Bitmap", filename)
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

void
destroySWFBitmap(bitmap)
        SWF::Bitmap     bitmap 
        ALIAS:
        SWF::Bitmap::DESTROY = 1
	PREINIT:
	SWF__Block 	block;
        CODE:
	S_DEBUG(2, fprintf(stderr, "Bitmap DESTROY CALLED\n"));
	block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
        destroySWFBitmap(block);


MODULE = SWF		PACKAGE = SWF::Fill		PREFIX = SWFFill_

void
SWFFill_import(pclass, ...)
    SV *pclass

    PREINIT:
    I32 i = 0;
    SV *caller = perl_eval_pv("scalar caller", TRUE);

    CODE:
    for(i=1; i<items; i++) {
        my_import(pclass, caller, ST(i));
    }

I32
constant()
    ALIAS:
	SWFFILL_SOLID = SWFFILL_SOLID
	SWFFILL_GRADIENT = SWFFILL_GRADIENT
	SWFFILL_LINEAR_GRADIENT = SWFFILL_LINEAR_GRADIENT
	SWFFILL_RADIAL_GRADIENT = SWFFILL_RADIAL_GRADIENT
	SWFFILL_BITMAP = SWFFILL_BITMAP
	SWFFILL_TILED_BITMAP = SWFFILL_TILED_BITMAP
	SWFFILL_CLIPPED_BITMAP = SWFFILL_CLIPPED_BITMAP
    CODE:
    RETVAL = ix;
    OUTPUT:
    RETVAL



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


void
SWFFill_DESTROY(fill)
	SWF::Fill	fill
	CODE:
        S_DEBUG(2, fprintf(stderr, "Fill DESTROY CALLED\n"));

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

void
destroySWFGradient(gradient)
	SWF::Gradient	gradient
	ALIAS:
        SWF::Gradient::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "Gradient DESTROY CALLED\n"));
        destroySWFGradient(gradient);



MODULE = SWF		PACKAGE = SWF::Movie		PREFIX = SWFMovie_

void
destroySWFMovie(movie)
	SWF::Movie	movie
        ALIAS:
	SWF::Movie::DESTROY = 1
	CODE:
        S_DEBUG(2, fprintf(stderr, "Movie DESTROY CALLED\n"));
	swf_stash_refcnt_dec((SV*)SvRV(ST(0)));
        destroySWFMovie(movie);

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
	ALIAS:
	SWF::Movie::setFrames = 1

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
	RETVAL = SWFMovie_output(movie, fileOutputMethod, stdout);

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
                RETVAL = SWFMovie_output(movie, fileOutputMethod, f);
                fclose(f);
	}
        OUTPUT:
        RETVAL


SWF::DisplayItem
SWFMovie_add(movie, block)
	SWF::Movie movie
	SWF::Block block = (SWF__Block) SvIV((SV*)SvRV(ST(1)));
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	RETVAL = SWFMovie_add(movie, block);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::DisplayItem", (void*)RETVAL);

void
SWFMovie_remove(movie, item)
        SWF::Movie movie
        SWF::DisplayItem item

void
SWFMovie_setSoundStream(movie, sound)
	SWF::Movie movie
	SWF::Sound sound
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	SWFMovie_setSoundStream(movie, sound);

MODULE = SWF           PACKAGE = SWF::Sound            PREFIX = SWFSound_

SWF::Sound
SWFSound_new(package="SWF::Sound", filename)
       char *package
       char *filename
       PREINIT:
       FILE    *f;
       CODE:
       if (items < 1)
           fprintf(stderr, "SWF::Sound called with one argument\n\n");
       if (!(f = fopen(filename, "rb"))) {
               fprintf(stderr, "Unable to open %s\n", filename);
               ST(0) = &sv_undef;
       } else {
               RETVAL = newSWFSound(f);
               ST(0) = sv_newmortal();
               sv_setref_pv(ST(0), package, (void*)RETVAL);
       }

void
destroySWFSound(sound)
	SWF::Sound	sound
        ALIAS:
        SWF::Sound::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "Sound DESTROY CALLED\n"));
        destroySWFSound(sound);


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


void 
SWFDisplayItem_remove(item)
	SWF::DisplayItem item

void
SWFDisplayItem_DESTROY(item)
	SWF::DisplayItem	item
	CODE:
        S_DEBUG(2, fprintf(stderr, "DisplayItem DESTROY CALLED\n"));
	


MODULE = SWF		PACKAGE = SWF::Shape		PREFIX = SWFShape_

SWF::Shape
SWFShape_new(package="SWF::Shape")
     	char *package
	CODE:
        RETVAL = newSWFShape();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);

void
destroySWFShape(block)
	SWF::Block	block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
        ALIAS:
        SWF::Shape::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "Shape DESTROY CALLED\n"));
	swf_stash_refcnt_dec((SV*)SvRV(ST(0)));
        destroySWFShape(block);

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

void
SWFShape_end(shape)
	SWF::Shape	shape

int
SWFShape_setLineStyle(shape, width, r, g, b, a=0xff)
	SWF::Shape	shape
	unsigned short	width
	byte	r
	byte	g
	byte	b
	byte	a

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
        PREINIT:
	SV *sv;
	CODE:
        sv = (SV*)SvRV(ST(0));
	RETVAL = SWFShape_addSolidFill(shape, r, g, b, a);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::Fill", (void*)RETVAL);
	swf_stash_refcnt_inc(sv, (SV*)SvRV(ST(0)));	

SWF::Fill
SWFShape_addGradientFill(shape, gradient, flags=SWFFILL_LINEAR_GRADIENT)
	SWF::Shape      shape
	SWF::Gradient   gradient
	byte		flags
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	RETVAL = SWFShape_addGradientFill(shape, gradient, flags);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::Fill", (void*)RETVAL);

SWF::Fill
SWFShape_addBitmapFill(shape, bitmap, flags=SWFFILL_TILED_BITMAP)
	SWF::Shape	shape
	SWF::Bitmap	bitmap
	byte	        flags
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	RETVAL = SWFShape_addBitmapFill(shape, bitmap, flags);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::Fill", (void*)RETVAL);

void
SWFShape_xs_setLeftFill(shape, fill=NULL)
	SWF::Shape	shape
	SWF::Fill	fill
	CODE:
	SWFShape_setLeftFill(shape, fill);

void
SWFShape_xs_setRightFill(shape, fill=NULL)
	SWF::Shape	shape
	SWF::Fill	fill
	CODE:
	SWFShape_setRightFill(shape, fill);

#void
#SWFShape_drawFontGlyph(shape, font, c)
#        SWF::Shape shape
#        SWF::Font font
#        int c
#        ALIAS:
#        SWF::Shape::drawGlyph = 1

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


void
destroySWFMorph(block)
	SWF::Block	block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
        ALIAS:
        SWF::Morph::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "Morph DESTROY CALLED\n"));
        destroySWFMorph(block);


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


void
destroySWFFont(block)
        SWF::Block      block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
        ALIAS:
        SWF::Font::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "FONT DESTROY CALLED\n"));
        destroySWFFont(block);


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


void
destroySWFText(block)
        SWF::Block      block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
        ALIAS:
        SWF::Text::DESTROY = 1
	CODE:
        S_DEBUG(2, fprintf(stderr, "Text DESTROY CALLED\n"));
	swf_stash_refcnt_dec((SV*)SvRV(ST(0)));
        destroySWFText(block);


void
SWFText_setFont(text, font)
	SWF::Text	text
	SWF::Font	font
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	SWFText_setFont(text, font);

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

void
SWFTextField_import(pclass, ...)
    SV *pclass

    PREINIT:
    I32 i = 0;
    SV *caller = perl_eval_pv("scalar caller", TRUE);

    CODE:
    for(i=1; i<items; i++) {
        my_import(pclass, caller, ST(i));
    }

I32
constant()
    ALIAS:
	SWFTEXTFIELD_NOEDIT = SWFTEXTFIELD_NOEDIT
	SWFTEXTFIELD_PASSWORD = SWFTEXTFIELD_PASSWORD
	SWFTEXTFIELD_DRAWBOX = SWFTEXTFIELD_DRAWBOX
	SWFTEXTFIELD_MULTILINE = SWFTEXTFIELD_MULTILINE
	SWFTEXTFIELD_WORDWRAP = SWFTEXTFIELD_WORDWRAP
	SWFTEXTFIELD_NOSELECT = SWFTEXTFIELD_NOSELECT
	SWFTEXTFIELD_ALIGN_LEFT = SWFTEXTFIELD_ALIGN_LEFT
	SWFTEXTFIELD_ALIGN_RIGHT = SWFTEXTFIELD_ALIGN_RIGHT
	SWFTEXTFIELD_ALIGN_CENTER = SWFTEXTFIELD_ALIGN_CENTER
	SWFTEXTFIELD_ALIGN_JUSTIFY = SWFTEXTFIELD_ALIGN_JUSTIFY
    CODE:
    RETVAL = ix;
    OUTPUT:
    RETVAL




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

void
destroySWFTextField(block)
        SWF::Block      block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
        ALIAS:
        SWF::TextField::DESTROY = 1
	CODE:
        S_DEBUG(2, fprintf(stderr, "TextField DESTROY CALLED\n"));
	swf_stash_refcnt_dec((SV*)SvRV(ST(0)));
        destroySWFTextField(block);

void
SWFTextField_setFont(field, font)
	SWF::TextField	field
	SWF::Block	font = (SWF__Block) SvIV((SV*)SvRV(ST(1)));
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	SWFTextField_setFont(field, font);


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

void
SWFAction_DESTROY(action)
	SWF::Action	action
	CODE:
        S_DEBUG(2, fprintf(stderr, "Action DESTROY CALLED\n"));

MODULE = SWF		PACKAGE = SWF::Button		PREFIX = SWFButton_

void
SWFButton_import(pclass, ...)
    SV *pclass

    PREINIT:
    I32 i = 0;
    SV *caller = perl_eval_pv("scalar caller", TRUE);

    CODE:
    for(i=1; i<items; i++) {
        my_import(pclass, caller, ST(i));
    }

I32
constant()
    ALIAS:
        SWFBUTTON_MOUSEUP = SWFBUTTON_MOUSEUP
	SWFBUTTON_HIT = SWFBUTTON_HIT
	SWFBUTTON_DOWN = SWFBUTTON_DOWN
	SWFBUTTON_OVER = SWFBUTTON_OVER
	SWFBUTTON_UP = SWFBUTTON_UP
	SWFBUTTON_MOUSEOVER = SWFBUTTON_MOUSEOVER
	SWFBUTTON_MOUSEOUT = SWFBUTTON_MOUSEOUT
	SWFBUTTON_MOUSEDOWN = SWFBUTTON_MOUSEDOWN
	SWFBUTTON_DRAGOUT = SWFBUTTON_DRAGOUT
	SWFBUTTON_DRAGOVER = SWFBUTTON_DRAGOVER
	SWFBUTTON_MOUSEUPOUTSIDE = SWFBUTTON_MOUSEUPOUTSIDE
    CODE:
    RETVAL = ix;
    OUTPUT:
    RETVAL

int
SWFBUTTON_KEYPRESS(c)
    char c

int
SWFBUTTON_ONKEYPRESS(c)
    char c

SWF::Button
SWFButton_new(package="SWF::Button")
	char *package
	CODE:
        RETVAL = newSWFButton();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);



void
destroySWFButton(block)
        SWF::Block      block = (SWF__Block) SvIV((SV*)SvRV(ST(0)));
        ALIAS:
        SWF::Button::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "Button DESTROY CALLED\n"));
	swf_stash_refcnt_dec((SV*)SvRV(ST(0)));
        destroySWFButton(block);

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
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
        switch((ix = XSANY.any_i32)) {
            case 0:
               SWFButton_addShape(button, character, flags);
               break;
            case 1:
               SWFButton_addShape(button, character, SWFBUTTON_OVER);
               break;
            case 2:
               SWFButton_addShape(button, character, SWFBUTTON_HIT);
               break;
            case 3:
               SWFButton_addShape(button, character, SWFBUTTON_UP);
               break;
            case 4:
               SWFButton_addShape(button, character, SWFBUTTON_DOWN);
               break;
       }


void
SWFButton_addAction(button, action, flags=SWFBUTTON_MOUSEUP)
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
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	RETVAL = SWFMovieClip_add(clip, block);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::DisplayItem", (void*)RETVAL);
	
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
	ALIAS:
	SWF::MovieClip::setFrames   = 1

void
destroySWFMovieClip(clip)
	SWF::MovieClip clip
        ALIAS:
        SWF::MovieClip::DESTROY = 1
	CODE:
        S_DEBUG(2, fprintf(stderr, "MovieClip DESTROY CALLED\n"));
	swf_stash_refcnt_dec((SV*)SvRV(ST(0)));
        destroySWFMovieClip(clip);







