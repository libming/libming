#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "SWF.h"
#include "perl_swf.h"

MODULE = SWF::VideoStream       PACKAGE = SWF::VideoStream            PREFIX = SWFVideoStream_
PROTOTYPES: ENABLE

SWF::VideoStream
SWFVideoStream_new(package="SWF::VideoStream", filename=NULL)
	char *package
	char *filename
	PREINIT:
	FILE *f;
	CODE:
	if(filename && (f = fopen(filename, "rb"))) 
		RETVAL = newSWFVideoStream_fromFile(f);
	else 
		RETVAL = newSWFVideoStream();
	
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), package, (void*)RETVAL);


void destroySWFVideoStream(stream)
	SWF::VideoStream stream
	ALIAS:
	SWF::VideoStream::DESTROY = 1
	CODE:
	swf_stash_refcnt_dec((SV*)SvRV(ST(0)));
	destroySWFVideoStream(stream);

int SWFVideoStream_getNumFrames(stream)
	SWF::VideoStream stream
	
void SWFVideoStream_setDimension(stream, width, height)
	SWF::VideoStream stream
	int width
	int height

int SWFVideoStream_hasAudio(stream)
	SWF::VideoStream stream
		
int SWFVideoStream_setFrameMode(stream, mode)
	SWF::VideoStream stream
	int mode

int SWFVideoStream_nextFrame(stream)
	SWF::VideoStream stream

int SWFVideoStream_seek(stream, frame, whence)
	SWF::VideoStream stream
	int frame
	int whence

