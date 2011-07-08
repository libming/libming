/****************************************************************************
 *
 *  Copyright (C) 2005-2006 "Stuart R. Anderson" <anderson@netsweng.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************/

#include "action.h"
#include "parser.h"

extern int verbose;

typedef void (*outputfunc) (SWF_ACTION *act);

static struct SWF_ACTIONCONSTANTPOOL *pool = NULL;

struct SWFActionName
{
  Action type;
  char *name;
  outputfunc func;
};

#define OUT_BEGIN(block) \
	        struct block *sact = (struct block *)act;

static int indent = 1;

#define INDENT {int idt;for(idt=0;idt<indent;idt++)printf("  ");}

void
outputSWFACTION_CONSTANTPOOL (SWF_ACTION *act)
{
  int i;
  OUT_BEGIN(SWF_ACTIONCONSTANTPOOL);

  if( verbose ) {
      INDENT;
      printf ("Length: %d\n", sact->Length);
  }
  pool=sact;
  for(i=0;i<sact->Count;i++)
  {
	  INDENT;
	  printf ("   [%3.3d] %s\n", i, sact->ConstantPool[i] );
  }
}

void
outputSWFACTION_STOREREGISTER (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONSTOREREGISTER);

  if( verbose ) {
      INDENT;
      printf ("  Length: %d\n", sact->Length);
  }
  INDENT;
  printf ("  Register: %d\n", sact->Register);
}

void
outputSWFACTION_IF (SWF_ACTION *act)
{
  int i;
  OUT_BEGIN(SWF_ACTIONIF);

  if( verbose ) {
      INDENT;
      printf ("  Length: %d\n", sact->Length);
  }
  INDENT;
  printf ("  BranchOffset: %d, TargetOffset: %lu\n", sact->BranchOffset, 
		sact->Offset +  sact->Length + sact->BranchOffset + 3);
  INDENT;
  printf ("  %d Action\n", sact->numActions);
  if( verbose ) {
      INDENT;
      printf ("  ****IF-begin\n");
  }
  for(i=0;i<sact->numActions;i++)
  {
	outputSWF_ACTION (i,&(sact->Actions[i]));
  }
  if( verbose ) {
      INDENT;
      printf ("  ****IF-end\n");
  }
}

void
outputSWFACTION_JUMP (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONJUMP);

  if( verbose ) {
      INDENT;
      printf ("  Length: %d\n", sact->Length);
  }
  INDENT;
  printf ("  BranchOffset: %d, TargetOffset: %lu\n", sact->BranchOffset,
		sact->Offset +  sact->Length + sact->BranchOffset + 3);
}

void
outputSWFACTION_WAITFORFRAME (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONWAITFORFRAME);

  if( verbose ) {
      INDENT;
      printf ("   Length: %d\n", sact->Length);
  }
  INDENT;
  printf ("   Frame: %d\n", sact->Frame);
  INDENT;
  printf ("   SkipCount: %d\n", sact->SkipCount);
}

void
outputSWFACTION_SETTARGET (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONSETTARGET);

  if( verbose ) {
      INDENT;
      printf ("   Length: %d\n", sact->Length);
  }
  INDENT;
  printf ("   TargetName: %s\n", sact->TargetName);
}

void
outputSWFACTION_WITH (SWF_ACTION *act)
{
  int i;
  OUT_BEGIN(SWF_ACTIONWITH);

  if( verbose ) {
      INDENT;
      printf ("  Length: %d\n", sact->Length);
  }
  INDENT;
  printf ("  Size: %d\n", sact->Size);
  if( verbose )
      INDENT;
      printf ("  ****WITH-begin\n");
  indent++;
  for(i=0;i<sact->numActions;i++)
  {
	outputSWF_ACTION (i,&(sact->Actions[i]));
  }
  if( verbose )
      INDENT;
      printf ("  ****WITH-end\n");
  indent--;
}

void
outputSWFACTION_DEFINEFUNCTION (SWF_ACTION *act)
{
  int i;
  OUT_BEGIN(SWF_ACTIONDEFINEFUNCTION);

  if( verbose ) {
      INDENT;
      printf ("  Length: %d\n", sact->Length);
  }
  INDENT;
  printf ("  Name: %s\n", sact->FunctionName);
  INDENT;
  printf ("  NumParams: %d\n", sact->NumParams);
  for(i=0;i<sact->NumParams;i++)
  {
        INDENT;
	printf("   [%2.2d] %s\n", i,
			sact->Params[i] );
  }
  INDENT;
  printf ("  CodeSize: %d\n", sact->CodeSize);
  INDENT;
  printf ("  %d Action\n", sact->numActions);
  if( verbose ) {
      INDENT;
      printf ("  ****FUN-begin\n");
  }
  for(i=0;i<sact->numActions;i++)
  {
	outputSWF_ACTION (i,&(sact->Actions[i]));
  }
  if( verbose ) {
      INDENT;
      printf ("  ****FUN-end\n");
  }
}

void
outputSWFACTION_DEFINEFUNCTION2 (SWF_ACTION *act)
{
  int i, registerCount = 1;
  OUT_BEGIN(SWF_ACTIONDEFINEFUNCTION2);

  if( verbose ) {
      INDENT;
      printf ("  Length: %d\n", sact->Length);
  }
  INDENT;
  printf ("  Name: %s\n", sact->FunctionName);
  INDENT;
  printf ("  RegisterCount: %d\n", sact->RegisterCount);
  INDENT;
  printf ("  Preloaded Registers:\n");
  if(sact->PreloadThisFlag)
	printf("     Register %i: this\n", registerCount++);
  if(sact->PreloadArgumentsFlag)
 	printf("     Register %i: arguments\n", registerCount++);
  if(sact->PreloadSuperFlag)
 	printf("     Register %i: super\n", registerCount++);
  if(sact->PreloadRootFlag)
 	printf("     Register %i: _root\n", registerCount++); 
  if(sact->PreloadParentFlag)
 	printf("     Register %i: _parent\n", registerCount++);
  if(sact->PreloadGlobalFlag)
 	printf("     Register %i: _global\n", registerCount++);
  INDENT;
  printf ("  NumParams: %d\n", sact->NumParams);
  for(i=0;i<sact->NumParams;i++)
  {
        INDENT;
	printf("   [%2.2d] %d %s\n", i,
			sact->Params[i].Register,
			sact->Params[i].ParamName );
  }
  INDENT;
  printf ("  Flags: \n");
  INDENT;
  printf ("     Suppress: super %d, arguments %d, this %d\n", 
		sact->SuppressSuperFlag, sact->SuppressArgumentsFlag, 
		sact->SuppressThisFlag);
  INDENT;
  printf ("  CodeSize: %d\n", sact->CodeSize);
  INDENT;
  printf ("  %d Action\n", sact->numActions);
  if( verbose ) {
      INDENT;
      printf ("  ****FUN2-begin\n");
  }
  for(i=0;i<sact->numActions;i++)
  {
	outputSWF_ACTION (i,&(sact->Actions[i]));
  }
  if( verbose ) {
      INDENT;
      printf ("  ****FUN2-end\n");
  }
}

const char* 
getConstant(unsigned int n)
{
	if ( ! pool || n >= pool->Count ) return NULL;
	return pool->ConstantPool[n];
}

void
outputSWFACTION_PUSHPARAM (struct SWF_ACTIONPUSHPARAM *act)
{

  switch( act->Type ) 
  {
	  case 0: /* STRING */
  		printf ("  String: '%s'\n", act->p.String);
		break;
	  case 1: /* FLOAT */
  		printf ("  Float: %f\n", act->p.Float);
		break;
	  case 2: /* NULL */
  		printf ("  NULL: \n" );
		break;
	  case 3: /* Undefined */
  		printf ("  undefined\n" );
		break;
	  case 4: /* Register */
  		printf ("  Register: %d\n", (int)act->p.RegisterNumber);
		break;
	  case 5: /* BOOLEAN */
  		printf ("  Boolean: %d\n", act->p.Boolean);
		break;
	  case 6: /* DOUBLE */
  		printf ("  Double: %g\n", act->p.Double);
		break;
	  case 7: /* INTEGER */
  		printf ("  Integer: %ld\n", act->p.Integer);
		break;
	  case 8: /* CONSTANT8 */
	  {
	        const char* v = getConstant(act->p.Constant8);
  		printf ("  Constant: %d", act->p.Constant8);
  		if ( v ) printf (" \"%s\"", v);
		putchar('\n');
		break;
	  }
	  case 9: /* CONSTANT16 */
	  {
	        const char* v = getConstant(act->p.Constant16);
  		printf ("  Constant: %d", act->p.Constant16);
  		if ( v ) printf (" \"%s\"", v);
		putchar('\n');
		break;
	  }
	  default: 
  		printf ("  Unknown type: %d\n", act->Type);
		break;
  }
}

void
outputSWFACTION_PUSH (SWF_ACTION *act)
{
  int i;
  OUT_BEGIN(SWF_ACTIONPUSH);

  if( verbose )
      printf ("  Length: %d\n", sact->Length);
  for(i=0;i<sact->NumParam;i++)
  {
	  INDENT;
	  printf ("   [%3.3d] ", i );
	  outputSWFACTION_PUSHPARAM(&(sact->Params[i]));
  }
}

void
outputSWFACTION_GETURL (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONGETURL);
  printf("   UrlString: %s\n", sact->UrlString);
  printf("   TargetString: %s\n", sact->TargetString);
}

void
outputSWFACTION_GETURL2 (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONGETURL2);

  if( verbose )
      printf ("  Length: %d\n", sact->Length);
  switch( sact->f.FlagBits.SendVarsMethod )
  {
  	case 0:
      		printf ("  Method: none\n");
  		break;
  	case 1:
      		printf ("  Method: GET\n");
  		break;
  	case 2:
      		printf ("  Method: POST\n");
  		break;
  }
  switch( sact->f.FlagBits.LoadTargetFlag )
  {
  	case 0:
      		printf ("  Target: Window\n");
  		break;
  	case 1:
      		printf ("  Target: Sprite\n");
  		break;
  }
  switch( sact->f.FlagBits.LoadVariableFlag )
  {
  	case 0:
      		printf ("  LoadVars: No\n");
  		break;
  	case 1:
      		printf ("  LoadVars: Yes\n");
  		break;
  }
  printf ("  Reserved bits: %x\n", sact->f.FlagBits.Reserved);
}

void
outputSWFACTION_GOTOFRAME (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONGOTOFRAME);

  if( verbose ) {
      INDENT;
      printf ("  Length: %d\n", sact->Length);
  }

  INDENT;
  printf ("   Frame: %d\n", sact->Frame);
}

void
outputSWFACTION_GOTOFRAME2 (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONGOTOFRAME2);

  if( verbose )
      printf ("  Length: %d\n", sact->Length);

  printf ("  Reserved bits: %x\n", sact->f.FlagBits.Reserved);
  printf ("  Scene bias flag: %x\n", sact->f.FlagBits.SceneBiasFlag);
  printf ("  Play flag : %x\n", sact->f.FlagBits.PlayFlag);
  if(sact->f.FlagBits.SceneBiasFlag)
	  printf ("  Scene bias : %u\n", sact->SceneBias);
}

void 
outputSWFACTION_TRY (SWF_ACTION *act)
{
  int i;
  OUT_BEGIN(SWF_ACTIONTRY);
  
  if( verbose )
      printf ("    Length: %d\n", sact->Length);
  printf("    Offset %i\n",(int) sact->Offset);
  printf("    CatchInRegisterFlag %d, FinallyBlockFlag %d, CatchBlockFlag %d\n", 
    sact->CatchInRegisterFlag, sact->FinallyBlockFlag, sact->CatchBlockFlag);
  printf("    TrySize %d\n", sact->TrySize);
  printf("    CatchSize %d\n", sact->CatchSize);
  printf("    FinallySize %d\n", sact->FinallySize);
  if(sact->CatchInRegisterFlag)
    printf("    CatchRegister %i\n", sact->CatchRegister);
  else
    printf("    CatchName %s\n", sact->CatchName);

  printf("  Try Actions: %i\n", sact->numTryActs);
  for(i = 0; i < sact->numTryActs; i++)
  {
    outputSWF_ACTION(4, sact->TryActs + i);
  }
  printf("\n");
 
  printf("  Catch Actions: %i\n", sact->numCatchActs);
  for(i = 0; i < sact->numCatchActs; i++)
  {
    outputSWF_ACTION(4, sact->CatchActs + i);
  }
  printf("\n");
  
  printf("  Finally Actions: %i\n", sact->numFinallyActs);
  for(i = 0; i < sact->numFinallyActs; i++)
  {
    outputSWF_ACTION(4, sact->FinallyActs + i);
  }
  printf("  ## TRY END ##\n\n");
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
  ActionTypeLong (SWFACTION_GOTOFRAME),
  ActionTypeLong (SWFACTION_GETURL),
  ActionTypeLong (SWFACTION_WAITFORFRAME),
  ActionTypeLong (SWFACTION_SETTARGET),
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
  ActionTypeLong (SWFACTION_JUMP),
  ActionTypeLong (SWFACTION_IF),
  ActionType (SWFACTION_CALLFRAME),
  ActionType (SWFACTION_GETVARIABLE),
  ActionType (SWFACTION_SETVARIABLE),
  ActionTypeLong (SWFACTION_GETURL2),
  ActionTypeLong (SWFACTION_GOTOFRAME2),
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
  ActionTypeLong (SWFACTION_DEFINEFUNCTION),
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
  ActionTypeLong (SWFACTION_STOREREGISTER),
  /* v6 actions */
  ActionType (SWFACTION_INSTANCEOF),
  ActionType (SWFACTION_ENUMERATE2),
  ActionType (SWFACTION_STRICTEQUALS),
  ActionType (SWFACTION_GREATER),
  ActionType (SWFACTION_STRINGGREATER),
  /* v7 actions */
  ActionTypeLong (SWFACTION_DEFINEFUNCTION2),
  ActionType (SWFACTION_EXTENDS),
  ActionType (SWFACTION_CASTOP),
  ActionType (SWFACTION_IMPLEMENTSOP),
  ActionTypeLong (SWFACTION_TRY),
  ActionType (SWFACTION_THROW),
  ActionType (SWFACTION_FSCOMMAND2),
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
outputSWF_ACTION (int n, SWF_ACTION *act)
{
  struct SWF_ACTIONRECORD *action = (struct SWF_ACTIONRECORD *)act;
  int i;

  for (i = 0; i < numActions; i++)
    {
      if (actions[i].type == action->ActionCode)
      {
	INDENT;
	if( verbose )
  	    printf ("  Action: %d ", n );
  	printf ("  %lu:%s\n", action->Offset, actionName (action->ActionCode));
	if( verbose ) {
	    INDENT;
  	    printf ("  Offset: %lx\n", action->Offset );
	}
  	if (actions[i].func != NULL )
  	{
	  	actions[i].func(act);
  	} else {
  		if (action->ActionCode >= 0x80)
  		{
			if( verbose ) {
				INDENT;
				printf ("  Length: %d\n", action->Length);
			}
  		}
  	}
      }
    }
}

