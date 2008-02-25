#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "SWF.h"
#include "perl_swf.h"


MODULE = SWF::BinaryData       PACKAGE = SWF::BinaryData            PREFIX = SWFBinaryData_
PROTOTYPES: ENABLE

SWF::BinaryData
SWFBinaryData_new(package="SWF::BinaryData", data, length)
	char *package
	char *data
	int length
	CODE:
	RETVAL = newSWFBinaryData(data, length);
	ST(0) = sv_newmortal();
	sv_setref_pv(ST(0), package, (void*)RETVAL);
	
void
destroySWFBinaryData(bd)
	SWF::BinaryData       bd
	ALIAS:
	SWF::BinaryData::DESTROY = 1
	CODE:
	destroySWFBinaryData(bd);


