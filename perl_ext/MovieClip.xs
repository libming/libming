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
#include "perl_swf.h"

MODULE = SWF::MovieClip	PACKAGE = SWF::MovieClip	PREFIX = SWFMovieClip_
PROTOTYPES: ENABLE

SWF::MovieClip
SWFMovieClip_new(package="SWF::MovieClip")
	char *package
	CODE:
        RETVAL = newSWFMovieClip();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);
        if (!strcmp(package,"SWF::Sprite"))
         warn("Using SWF::Sprite is deprecated. Use SWF::MovieClip instead!");


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

SWF::SoundInstance
SWFMovieClip_startSound(movieclip, sound) 
	SWF::MovieClip movieclip
	SWF::Sound sound
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	RETVAL = SWFMovieClip_startSound(movieclip, sound);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::SoundInstance", (void*)RETVAL);

void
SWFMovieClip_stopSound(movieclip, sound)
	SWF::MovieClip movieclip
	SWF::Sound sound
	CODE:
	SWFMovieClip_stopSound(movieclip, sound);

void 
SWFMovieClip_setScalingGrid(movieclip, x, y, w, h)
	SWF::MovieClip movieclip
	int x
	int y
	int w
	int h

void
SWFMovieClip_removeScalingGrid(movieclip)
	SWF::MovieClip movieclip

void 
SWFMovieClip_addInitAction(movieclip, action)
	SWF::MovieClip movieclip
	SWF::Action action

void
SWFMovieClip_setSoundStream(movie, sound, rate, skip=0.0)
	SWF::MovieClip movie
	SWF::SoundStream sound
	float rate
	float skip
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	SWFMovieClip_setSoundStreamAt(movie, sound, rate, skip);

