/* output.h */

#if !defined(OUTPUTRET)
#define OUTPUTRET void
#endif

typedef OUTPUTRET (*SWFOutputFunc)(SWF_Parserstruct *);

struct SWFBlockOutput {
	SWFBlocktype	type;
	SWFOutputFunc	output;
};

#include "outputdecl.h"
