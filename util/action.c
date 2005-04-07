#include "actiontypes.h"
#include "parser.h"

struct SWFAction
{
  SWFActiontype type;
  char *name;
};

#define ActionType( action ) \
{ action, #action }

static struct SWFAction actions[] = {
  ActionType (SWFACTION_ENDFLAG),
  /* Simple Actions */
  ActionType (SWFACTION_NEXTFRAME),
  ActionType (SWFACTION_PLAY),
  ActionType (SWFACTION_STOP),
  ActionType (SWFACTION_TOGGLEQUALITY),
  ActionType (SWFACTION_STOPSOUNDS),
  /* Actions with additional Data */
  ActionType (SWFACTION_GOTOFRAME),
  ActionType (SWFACTION_GETURL),
  ActionType (SWFACTION_WAITFORFRAME),
  ActionType (SWFACTION_SETTARGET),
  ActionType (SWFACTION_GOTOLABEL),
};

static int numActions = sizeof (actions) / sizeof (struct SWFAction);

const char *
actionName (SWFActiontype header)
{
  int i;

  if (header < 0 || header > numActions)
    return "Invalid Action Type";

  for (i = 0; i < numActions; i++)
    {
      if (actions[i].type == header)
	{
	  return actions[i].name;
	}
    }
  return "Confused Action Type";	/* Should never get here */

}
