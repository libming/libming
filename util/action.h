#ifndef SWF_ACTION_H_INCLUDED
#define SWF_ACTION_H_INCLUDED

#include "swftypes.h"
#include "actiontypes.h"

/* action.c */
extern const char *actionName(Action header);
extern void outputSWF_ACTION(int n,SWF_ACTION *act);
#endif /* SWF_ACTION_H_INCLUDED */
