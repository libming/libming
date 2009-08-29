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
SWFSound_new(package="SWF::Sound", arg, flags=0)
	char *package
	int flags
	PREINIT:
	FILE    *f;
	char *filename;
	SWFSoundStream stream;
	CODE:
	if (items < 1)
		fprintf(stderr, "SWF::Sound called with one argument\n\n");

	if(flags == 0 && sv_derived_from(ST(1), "SWF::SoundStream"))
	{
		stream = (SWF__SoundStream) SvIV((SV*)SvRV(ST(1)));
		RETVAL = newSWFSound_fromSoundStream(stream);
		ST(0) = sv_newmortal();
		sv_setref_pv(ST(0), package, (void*)RETVAL);
	}
	else
	{
		filename = (char *)SvPVX(ST(1));
		if (!(f = fopen(filename, "rb"))) {
			fprintf(stderr, "Unable to open %s\n", filename);
			ST(0) = &sv_undef;
		} else {
			RETVAL = newSWFSound(f, flags);
			ST(0) = sv_newmortal();	
			sv_setref_pv(ST(0), package, (void*)RETVAL);
		}
	}

void
SWFSound_setInitialMp3Delay(sound, delay)
	SWF::Sound	sound
	int delay
	

void
destroySWFSound(sound)
	SWF::Sound	sound
        ALIAS:
        SWF::Sound::DESTROY = 1
        CODE:
        S_DEBUG(2, fprintf(stderr, "Sound DESTROY CALLED\n"));
        destroySWFSound(sound);



