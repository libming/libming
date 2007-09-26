/*
 * Copyright:
 * I hereby assign copyright of the code listed in the remainder of this post to
 * the libMing project. I remove my rights to ownership and indemnify myself from
 * any consquences caused by the use/misuse of the following code. Use at your own
 * risk. 
 * James Austin <viper.xz@gmail.com>.
 */

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "SWF.h"
#include "perl_swf.h"


MODULE = SWF::PrebuiltClip       PACKAGE = SWF::PrebuiltClip            PREFIX = SWFPrebuiltClip_
PROTOTYPES: ENABLE

SWF::PrebuiltClip
SWFPrebuiltClip_new(package="SWF::PrebuiltClip", filename)
	char *package
	char *filename
	PREINIT:
	FILE    *f;
	SWFInput input;
	CODE:
	if (items < 2)
		fprintf(stderr, "SWF::PrebuiltClip requires a filename\n\n");

	if (!(f = fopen(filename, "rb"))) 
	{
		fprintf(stderr, "Unable to open %s\n", filename);
		ST(0) = &sv_undef;
	} else 
	{
		input = newSWFInput_file(f);
		RETVAL = newSWFPrebuiltClip_fromInput(input);
		destroySWFInput(input);
		fclose(f);
		ST(0) = sv_newmortal();
		sv_setref_pv(ST(0), package, (void*)RETVAL);
	}

void
destroySWFPrebuiltClip(clip)
	SWF::PrebuiltClip       clip
	ALIAS:
	SWF::PrebuiltClip::DESTROY = 1
	CODE:
	destroySWFPrebuiltClip(clip);


