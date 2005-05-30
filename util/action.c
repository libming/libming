#include "action.h"
#include "parser.h"

struct SWFActionName
{
  Action type;
  char *name;
};

#define ActionType( action ) \
{ action, #action }

static struct SWFActionName actions[] = {
  ActionType (SWFACTION_END),
  /* v3 actions */
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
  /* v4 actions */
  ActionType (SWFACTION_PUSH),
  ActionType (SWFACTION_POP),
  ActionType (SWFACTION_ADD),
  ActionType (SWFACTION_SUBTRACT),
  ActionType (SWFACTION_MULTIPLY),
  ActionType (SWFACTION_DIVIDE),
  ActionType (SWFACTION_EQUAL),
  ActionType (SWFACTION_LESSTHAN),
  ActionType (SWFACTION_LOGICALAND),
  ActionType (SWFACTION_LOGICALOR),
  ActionType (SWFACTION_LOGICALNOT),
  ActionType (SWFACTION_STRINGEQ),
  ActionType (SWFACTION_STRINGLENGTH),
  ActionType (SWFACTION_STRINGCONCAT),
  ActionType (SWFACTION_SUBSTRING),
  ActionType (SWFACTION_STRINGCOMPARE),
  ActionType (SWFACTION_MBLENGTH),
  ActionType (SWFACTION_MBSUBSTRING),
  ActionType (SWFACTION_INT),
  ActionType (SWFACTION_ORD),
  ActionType (SWFACTION_CHR),
  ActionType (SWFACTION_MBORD),
  ActionType (SWFACTION_MBCHR),
  ActionType (SWFACTION_JUMP),
  ActionType (SWFACTION_IF),
  ActionType (SWFACTION_CALLFRAME),
  ActionType (SWFACTION_GETVARIABLE),
  ActionType (SWFACTION_SETVARIABLE),
  ActionType (SWFACTION_GETURL2),
  ActionType (SWFACTION_GOTOFRAME2),
  ActionType (SWFACTION_SETTARGET2),
  ActionType (SWFACTION_GETPROPERTY),
  ActionType (SWFACTION_SETPROPERTY),
  ActionType (SWFACTION_DUPLICATECLIP),
  ActionType (SWFACTION_REMOVECLIP),
  ActionType (SWFACTION_STARTDRAG),
  ActionType (SWFACTION_ENDDRAG),
  ActionType (SWFACTION_WAITFORFRAME2),
  ActionType (SWFACTION_TRACE),
  ActionType (SWFACTION_GETTIME),
  ActionType (SWFACTION_RANDOMNUMBER),
  /* v5 actions */
  ActionType (SWFACTION_CALLFUNCTION),
  ActionType (SWFACTION_CALLMETHOD),
  ActionType (SWFACTION_CONSTANTPOOL),
  ActionType (SWFACTION_DEFINEFUNCTION),
  ActionType (SWFACTION_DEFINELOCAL),
  ActionType (SWFACTION_DEFINELOCAL2),
  ActionType (SWFACTION_DELETE),
  ActionType (SWFACTION_DELETE2),
  ActionType (SWFACTION_ENUMERATE),
  ActionType (SWFACTION_EQUALS2),
  ActionType (SWFACTION_GETMEMBER),
  ActionType (SWFACTION_INITARRAY),
  ActionType (SWFACTION_INITOBJECT),
  ActionType (SWFACTION_NEWMETHOD),
  ActionType (SWFACTION_NEWOBJECT),
  ActionType (SWFACTION_SETMEMBER),
  ActionType (SWFACTION_TARGETPATH),
  ActionType (SWFACTION_WITH),
  ActionType (SWFACTION_TONUMBER),
  ActionType (SWFACTION_TOSTRING),
  ActionType (SWFACTION_TYPEOF),
  ActionType (SWFACTION_ADD2),
  ActionType (SWFACTION_LESS2),
  ActionType (SWFACTION_MODULO),
  ActionType (SWFACTION_BITWISEAND),
  ActionType (SWFACTION_SHIFTLEFT),
  ActionType (SWFACTION_BITWISEOR),
  ActionType (SWFACTION_SHIFTRIGHT),
  ActionType (SWFACTION_SHIFTRIGHT2),
  ActionType (SWFACTION_BITWISEXOR),
  ActionType (SWFACTION_DECREMENT),
  ActionType (SWFACTION_INCREMENT),
  ActionType (SWFACTION_PUSHDUP),
  ActionType (SWFACTION_RETURN),
  ActionType (SWFACTION_STACKSWAP),
  ActionType (SWFACTION_STOREREGISTER),
  /* v6 actions */
  ActionType (SWFACTION_INSTANCEOF),
  ActionType (SWFACTION_ENUMERATE2),
  ActionType (SWFACTION_STRICTEQUALS),
  ActionType (SWFACTION_GREATER),
  ActionType (SWFACTION_STRINGGREATER),
  /* v7 actions */
  ActionType (SWFACTION_DEFINEFUNCTION2),
  ActionType (SWFACTION_EXTENDS),
  ActionType (SWFACTION_CASTOP),
  ActionType (SWFACTION_IMPLEMENTSOP),
  ActionType (SWFACTION_TRY),
  ActionType (SWFACTION_THROW),
};

static int numActions = sizeof (actions) / sizeof (struct SWFActionName);

const char *
actionName (Action header)
{
  int i;

  for (i = 0; i < numActions; i++)
    {
      if (actions[i].type == header)
	{
	  return actions[i].name;
	}
    }
  return "Confused Action Type";	/* Should never get here */

}
