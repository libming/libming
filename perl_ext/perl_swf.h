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
typedef SWFSound           SWF__Sound;
typedef SWFSoundInstance   SWF__SoundInstance;
typedef SWFSoundStream     SWF__SoundStream;
typedef SWFVideoStream	   SWF__VideoStream;
typedef SWFPrebuiltClip	   SWF__PrebuiltClip;
typedef SWFFontCharacter   SWF__FontCharacter;
typedef SWFButtonRecord    SWF__ButtonRecord;
typedef SWFBinaryData      SWF__BinaryData;
typedef SWFInitAction      SWF__InitAction;
typedef SWFMatrix     	   SWF__Matrix;
typedef SWFBlur     	   SWF__Blur;
typedef SWFShadow     	   SWF__Shadow;
typedef SWFFilterMatrix	   SWF__FilterMatrix;
typedef SWFFilter	   SWF__Filter;
typedef SWFCXform	   SWF__CXform;
typedef SWFBrowserFont	   SWF__BrowserFont;
typedef SWFFontCollection  SWF__FontCollection;

#ifndef sv_undef
#define sv_undef PL_sv_undef
#endif

#ifndef na
#define na PL_na
#endif

#ifndef pTHX_
#define aTHXo_
#endif

#ifndef GvCV_set
# define GvCV_set(G, C) (GvCV(G) = (C))
#endif

#ifndef S_DEBUG
#define swf_debug 0   /* Should we get this from, say, $SWF::debug? */
#define S_DEBUG(level,code)  if (swf_debug >= level) { code; }
#endif

void swf_stash_refcnt_inc(SV *sv_key, SV *sv_value);
void swf_stash_refcnt_dec(SV *sv_key);

#endif /* PERL_SWF_H_INCLUDED */

