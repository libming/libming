/* ====================================================================
 * Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
 *           (c) 2009 Albrecht Kleine
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

MODULE = SWF::Button	PACKAGE = SWF::Button		PREFIX = SWFButton_
PROTOTYPES: ENABLE


SWF::Button
SWFButton_new(package="SWF::Button")
	char *package
	CODE:
        RETVAL = newSWFButton();
        ST(0) = sv_newmortal();
        sv_setref_pv(ST(0), package, (void*)RETVAL);



void
destroySWFButton(button)
        SWF::Button      button
        ALIAS:
        SWF::Button::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "Button DESTROY CALLED\n"));
	swf_stash_refcnt_dec((SV*)SvRV(ST(0)));
        destroySWFButton(button);

void
SWFButton_addShape(button, character, flags=0)
	SWF::Button	button
	SWF::Character	character = NO_INIT
	byte	flags
	# /* ALIAS:                               */
	# /* SWF::Button::setOver .....           */
	# /* 4 ALIASes removed by akleine (2009)  */
	# /* because all of them were used twice, */
	# /* only these in addCharacter() remain. */
	# /* See next function declaration below. */
        CODE:
        character = (SWF__Character) SvIV((SV*)SvRV(ST(1)));
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
        SWFButton_addCharacter(button, character, flags);


SWF::ButtonRecord
SWFButton_addCharacter(button, character, flags=0)
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
               RETVAL = SWFButton_addCharacter(button, character, flags);
               break;
            case 1:
               RETVAL = SWFButton_addCharacter(button, character, SWFBUTTON_OVER);
               break;
            case 2:
               RETVAL = SWFButton_addCharacter(button, character, SWFBUTTON_HIT);
               break;
            case 3:
               RETVAL = SWFButton_addCharacter(button, character, SWFBUTTON_UP);
               break;
            case 4:
               RETVAL = SWFButton_addCharacter(button, character, SWFBUTTON_DOWN);
               break;
            default:                              /* this should never be ... */
               RETVAL = (void*)&PL_sv_undef;      /* ... but avoids a gcc 4.x warning:  ... */
               croak("internal error Button.xs"); /* ... "RETVAL may be used uninitialized" */
               break;
       }
       ST(0) = sv_newmortal();
       sv_setref_pv(ST(0), "SWF::ButtonRecord", (void*)RETVAL);



void
SWFButton_addAction(button, action, flags=SWFBUTTON_MOUSEUP)
	SWF::Button	button
	SWF::Action	action
        int             flags
        ALIAS:
        SWF::Button::setAction = 1


SWF::SoundInstance
SWFButton_addSound(button, sound, flags)
	SWF::Button	button
	SWF::Sound	sound
    int             flags
	CODE:
	swf_stash_refcnt_inc((SV*)SvRV(ST(0)), (SV*)SvRV(ST(1)));
	RETVAL = SWFButton_addSound(button, sound, flags);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), "SWF::SoundInstance", (void*)RETVAL);


void
SWFButton_setMenu(button, flag=1)
	SWF::Button	button
	int flag;

void 
SWFButton_setScalingGrid(button, x, y, w, h)
	SWF::Button button
	int x
	int y
	int w
	int h

void 
SWFButton_removeScalingGrid(button)
	SWF::Button button
