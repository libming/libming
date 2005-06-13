#include "action.h"
#include "parser.h"

typedef void (*outputfunc) (SWF_ACTION *act);

char **pool;

struct SWFActionName
{
  Action type;
  char *name;
  outputfunc func;
};

#define OUT_BEGIN(block) \
	        struct block *sact = (struct block *)act;

void
outputSWFACTION_CONSTANTPOOL (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONCONSTANTPOOL);
  int i;

  printf ("  Length: %d\n", sact->Length);
  pool=sact->ConstantPool;
  for(i=0;i<sact->Count;i++)
  {
	  printf ("    [%3.3d] %s\n", i, sact->ConstantPool[i] );
  }
}

void
outputSWFACTION_WITH (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONWITH);
  int i;

  printf ("  Length: %d\n", sact->Length);
  printf ("  Size: %d\n", sact->Size);
  for(i=0;i<sact->numActions;i++)
  {
	outputSWF_ACTION (&(sact->Actions[i]));
  }
}

void
outputSWFACTION_PUSHPARAM (struct SWF_ACTIONPUSHPARAM *act)
{

  switch( act->Type ) 
  {
	  case 0: /* STRING */
  		printf ("  String: %s\n", act->String);
		break;
	  case 1: /* FLOAT */
  		printf ("  Float: %f\n", act->Float);
		break;
	  case 2: /* NULL */
  		printf ("  NULL: \n" );
		break;
	  case 3: /* Undefined */
  		printf ("  undefiend:\n" );
		break;
	  case 4: /* Register */
  		printf ("  Register: %d\n", (int)act->RegisterNumber);
		break;
	  case 5: /* BOOLEAN */
  		printf ("  Boolean: %d\n", act->Boolean);
		break;
	  case 6: /* DOUBLE */
  		printf ("  Double: %g\n", act->Double);
		break;
	  case 7: /* INTEGER */
  		printf ("  Integer: %d\n", act->Integer);
		break;
	  case 8: /* CONSTANT8 */
  		printf ("  Constant: %d %s\n", act->Constant8, pool[act->Constant8]);
		break;
	  case 9: /* CONSTANT16 */
  		printf ("  Constant: %d %s\n", act->Constant16, pool[act->Constant16]);
		break;
	  default: 
  		printf ("  Unknown type: %d\n", act->Type);
		break;
  }
}

void
outputSWFACTION_PUSH (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONPUSH);
  int i;

  printf ("  Length: %d\n", sact->Length);
  for(i=0;i<sact->NumParam;i++)
  {
	  printf ("    [%3.3d] ", i );
	  outputSWFACTION_PUSHPARAM(&(sact->Params[i]));
  }
}


#define ActionType( action ) \
{ action, #action, NULL }

#define ActionTypeLong( action ) \
{ action, #action, output##action }

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
  ActionTypeLong (SWFACTION_PUSH),
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
  ActionTypeLong (SWFACTION_CONSTANTPOOL),
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
  ActionTypeLong (SWFACTION_WITH),
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

void
outputSWF_ACTION (SWF_ACTION *act)
{
  struct SWF_ACTIONRECORD *action = (struct SWF_ACTIONRECORD *)act;
  int i;

  for (i = 0; i < numActions; i++)
    {
      if (actions[i].type == action->ActionCode)
      {
  	printf ("  Action: %x %s\n", action->ActionCode, actionName (action->ActionCode));
  	if (actions[i].func != NULL )
  	{
	  	actions[i].func(act);
  	} else {
  		if (action->ActionCode >= 0x80)
  		{
        		printf ("  Length: %d\n", action->Length);
  		}
  	}
      }
    }
}

