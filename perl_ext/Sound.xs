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


MODULE = SWF::Sound       PACKAGE = SWF::Sound            PREFIX = SWFSound_
PROTOTYPES: ENABLE


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

