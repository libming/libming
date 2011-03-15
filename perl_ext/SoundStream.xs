/* ====================================================================
 * Copyright (c) 2000-2001 by Soheil Seyfaie. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the same terms as Perl itself.
 * ====================================================================
 *
 * $Author$
 */


#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "SWF.h"
#include "perl_swf.h"


MODULE = SWF::SoundStream       PACKAGE = SWF::SoundStream            PREFIX = SWFSoundStream_
PROTOTYPES: ENABLE


SWF::SoundStream
SWFSoundStream_new(package="SWF::SoundStream", filename)
       char *package
       char *filename
       PREINIT:
       FILE    *f;
       CODE:
       if (items < 1)
           fprintf(stderr, "SWF::SoundStream called with one argument\n\n");
       if (!(f = fopen(filename, "rb"))) {
               fprintf(stderr, "Unable to open %s\n", filename);
               ST(0) = &sv_undef;
       } else {
               RETVAL = newSWFSoundStream(f);
               ST(0) = sv_newmortal();
               sv_setref_pv(ST(0), package, (void*)RETVAL);
       }

unsigned int SWFSoundStream_getDuration(sound)
	SWF::SoundStream sound

int SWFSoundStream_getFrames(sound)
	SWF::SoundStream	sound
	CODE:
	 warn("Using getFrames() is deprecated. Use getDuration() instead!");
	 RETVAL=SWFSoundStream_getFrames(sound);
	OUTPUT:
	 RETVAL

void
SWFSoundStream_setInitialMp3Delay(sound, delay)
	SWF::SoundStream	sound
	int delay

void
destroySWFSoundStream(sound)
	SWF::SoundStream	sound
        ALIAS:
        SWF::SoundStream::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "SoundStream DESTROY CALLED\n"));
        destroySWFSoundStream(sound);

