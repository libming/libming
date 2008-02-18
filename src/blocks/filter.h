#ifndef __FILTER_H
#define __FILTER_H

#include "output.h"

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
		default: return 0;
	}
}

void
SWFOutput_writeSWFFilter(SWFOutput out, SWFFilter filter);

struct SWFFilterList_s
{
        int numFilter;
        SWFFilter *filter;
};

typedef struct SWFFilterList_s * SWFFilterList;

SWFFilterList newSWFFilterList();
void SWFFilterList_add(SWFFilterList list, SWFFilter filter);
void SWFOutput_writeFilterList(SWFOutput out, SWFFilterList list);
void destroySWFFilterList(SWFFilterList list);
#endif
