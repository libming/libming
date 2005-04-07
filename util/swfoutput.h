/* output.h */

typedef void (*SWFOutputFunc)(SWF_Parserstruct *);

struct SWFOutput {
	SWFBlocktype	type;
	SWFOutputFunc	output;
};

#include "outputdecl.h"
