#ifndef __FILTER_H
#define __FILTER_H

static inline int SWFFilter_testBlockType(int type)
{
	switch(type)
	{
		case SWF_DEFINEBUTTON:
		case SWF_DEFINEBUTTON2:
		case SWF_DEFINESPRITE:
		case SWF_DEFINETEXT:
		case SWF_DEFINETEXT2:
			return 1;
			break;
		default: return 0;
	}
}

void
SWFOutput_writeSWFFilter(SWFOutput out, SWFFilter filter);
#endif
