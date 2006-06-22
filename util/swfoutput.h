/* output.h */

typedef void (*SWFOutputFunc)(SWF_Parserstruct *);

struct SWFBlockOutput {
	SWFBlocktype	type;
	SWFOutputFunc	output;
};

#include "outputdecl.h"
