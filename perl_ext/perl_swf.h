/* ====================================================================
 * Copyright (c) 2000 by Soheil Seyfaie. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the same terms as Perl itself.
 * ====================================================================
 */




#ifndef PERL_SWF_H_INCLUDED
#define PERL_SWF_H_INCLUDED


typedef SWFMovieClip       SWF__MovieClip;
typedef SWFMovie           SWF__Movie;
typedef SWFShape           SWF__Shape;
typedef SWFBlock           SWF__Block;
typedef SWFFont            SWF__Font;
typedef SWFFill            SWF__Fill;
typedef SWFText            SWF__Text;
typedef SWFDisplayItem     SWF__DisplayItem;
typedef SWFFillStyle       SWF__FillStyle;
typedef SWFGradient        SWF__Gradient;
typedef SWFBitmap          SWF__Bitmap;
typedef SWFButton          SWF__Button;
typedef SWFCharacter       SWF__Character;
typedef SWFAction          SWF__Action;
typedef SWFMorph           SWF__Morph;
typedef SWFTextField       SWF__TextField;

#ifndef sv_undef
#define sv_undef PL_sv_undef
#endif

#ifndef na
#define na PL_na
#endif

#ifndef pTHX_
#define aTHXo_
#endif

#endif /* PERL_SWF_H_INCLUDED */

