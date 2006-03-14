#define _GNU_SOURCE

#include <assert.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "read.h"
#include "action.h"
#include "swftypes.h"

static char **pool;
struct SWF_ACTIONPUSHPARAM *regs[256];

static char *getName(struct SWF_ACTIONPUSHPARAM *act);

static void
dumpRegs()
{
int i;
for(i=0;i<6;i++)
	if( regs[i] )
		printf("reg[%d] %s\n", i, getName(regs[i]));
}

/*
 * Start Package 
 *
 * A package to build up a string that can be returned to the caller
 * ak/2006: Extended for temporary swichting to a 2nd buffer
 */
#define USE_LIB 1

static int strsize=0;
static int strmaxsize=0;
static char *dcstr=NULL;
static char *dcptr=NULL;

#define DCSTRSIZE 10240
#define PARAM_STRSIZE 512
void
dcinit()
{
	strsize=0;
	strmaxsize=DCSTRSIZE;
	dcstr=calloc(DCSTRSIZE,1);
	dcptr=dcstr;
}

void
dcchkstr(int size)
{
	if( (strsize+size) > strmaxsize ) {
		dcstr=realloc(dcstr,strmaxsize+DCSTRSIZE);
		strmaxsize+=DCSTRSIZE;
		dcptr=dcstr+strsize;
	}

}

void
dcputs(const char *s)
{
	int len=strlen(s);
	dcchkstr(len);
	strcat(dcptr,s);
	dcptr+=len;
	strsize+=len;
}

void
dcputchar(char c)
{
dcchkstr(1);

*dcptr++=c;
*dcptr='\000';
strsize++;
}

int
dcprintf(char *format, ...)
{
	char *s;
	size_t size;

	va_list args;
	va_start(args,format);

	vasprintf(&s,format,args);
	dcputs(s);
	size=strlen(s);
	free(s);
	return size;
}

char *
dcgetstr()
{
	char *ret;
	ret = dcstr;
	dcstr=NULL;
	strmaxsize=0;
	return ret;
}

struct strbufinfo
{
  int size;
  char *str;
  char *ptr;
};


struct strbufinfo setTempString(void)
{
  struct strbufinfo current;
  current.size=strsize;
  current.str=dcstr;
  current.ptr=dcptr;
  dcinit();
  return current;
}
void setOrigString(struct strbufinfo old)
{
   free(dcstr);				/* not needed anymore */
   strsize=old.size;
   dcstr=old.str;
   dcptr=old.ptr;
}

#if USE_LIB
#define puts(s) dcputs(s)
#define putchar(c) dcputchar(c)
#define printf dcprintf
#endif

#define INDENT { int ii=gIndent; while(--ii>=0) { putchar(' '); putchar(' '); } }

/* End Package */

/*
 * Start Package 
 *
 * A package to maintain a representation of the Flash VM stack
 */

struct _stack {
	char type;
	struct SWF_ACTIONPUSHPARAM *val;
	struct _stack *next;
};

struct _stack *Stack;

static char *
getString(struct SWF_ACTIONPUSHPARAM *act)
{
	char *t;

  switch( act->Type ) 
  {
	  case 0: /* STRING */
                t=malloc(strlen(act->p.String)+3); /* 2 '"'s and a NULL */
		strcpy(t,"\"");
		strcat(t,act->p.String);
		strcat(t,"\"");
  		return t;
	  case 2: /* NULL */
  		return "null";
	  case 3: /* Undefined */
  		return "undefined";
	  case 4: /* REGISTER */
		if( regs[act->p.RegisterNumber] &&
		    regs[act->p.RegisterNumber]->Type != 4 &&
		    regs[act->p.RegisterNumber]->Type != 7 )
  		    return getName(regs[act->p.RegisterNumber]);
		else
                    t=malloc(4); /* Rdd */
  		    sprintf(t,"R%d", act->p.RegisterNumber );
  		    return t;
	  case 5: /* BOOLEAN */
		if( act->p.Boolean )
			return "true";
		else
			return "false";
	  case 6: /* DOUBLE */
                t=malloc(10); /* big enough? */
  		sprintf(t,"%g", act->p.Double );
  		return t;
	  case 7: /* INTEGER */
                t=malloc(10); /* 32-bit decimal */
  		sprintf(t,"%ld", act->p.Integer );
  		return t;
	  case 8: /* CONSTANT8 */
                t=malloc(strlen(pool[act->p.Constant8])+3); /* 2 '"'s and a NULL */
		strcpy(t,"\"");
		strcat(t,pool[act->p.Constant8]);
		strcat(t,"\"");
  		return t;
	  case 9: /* CONSTANT16 */
                t=malloc(strlen(pool[act->p.Constant16])+3); /* 2 '"'s and a NULL */
		strcpy(t,"\"");
		strcat(t,pool[act->p.Constant16]);
		strcat(t,"\"");
  		return t;
	  case 10: /* VARIABLE */
  		return act->p.String;
	  default: 
  		printf ("  Can't get string for type: %d\n", act->Type);
		break;
  }
  return "";
}

static char *
getName(struct SWF_ACTIONPUSHPARAM *act)
{
	char *t;

  switch( act->Type ) 
  {
	  case 0: /* STRING */
                t=malloc(strlen(act->p.String)+3); 
		/*
		strcpy(t,"\"");
		strcat(t,act->p.String);
		strcat(t,"\"");
		*/
		strcpy(t,act->p.String);
		if(strlen(t)) /* Not a zero length string */
  			return t;
		else
  			return "_this";
#if 0
	  case 4: /* REGISTER */
                t=malloc(4); /* Rdd */
  		sprintf(t,"R%d", act->p.RegisterNumber );
  		return t;
#endif
	  case 8: /* CONSTANT8 */
                t=malloc(strlen(pool[act->p.Constant8])+1);
		strcpy(t,pool[act->p.Constant8]);
		if(strlen(t)) /* Not a zero length string */
  			return t;
		else
  			return "_this";
	  case 9: /* CONSTANT16 */
                t=malloc(strlen(pool[act->p.Constant16])+1);
		strcpy(t,pool[act->p.Constant16]);
		if(strlen(t)) /* Not a zero length string */
  			return t;
		else
  			return "_this";
	  default: 
  		return getString(act);
  }
}

static int
getInt(struct SWF_ACTIONPUSHPARAM *act)
{

  switch( act->Type ) 
  {
	  case 2: /* NULL */
  		return 0;
	  case 4: /* REGISTER */
  		return getInt(regs[act->p.RegisterNumber]);
	  case 6: /* DOUBLE */
  		return (int)act->p.Double;
	  case 7: /* INTEGER */
  		return act->p.Integer;
	  default: 
  		printf ("  Can't get int for type: %d\n", act->Type);
		break;
  }
  return 0;
}

static char *
getProperty(Property prop)
{
  switch(prop)
  {
    case PROPERTY_X:		   return("_x"); break;
    case PROPERTY_Y:		   return("_y"); break;
    case PROPERTY_XMOUSE:	   return("_xMouse"); break;
    case PROPERTY_YMOUSE:	   return("_yMouse"); break;
    case PROPERTY_XSCALE:	   return("_xScale"); break;
    case PROPERTY_YSCALE:	   return("_yScale"); break;
    case PROPERTY_CURRENTFRAME:	   return("_currentFrame"); break;
    case PROPERTY_TOTALFRAMES:	   return("_totalFrames"); break;
    case PROPERTY_ALPHA:	   return("_alpha"); break;
    case PROPERTY_VISIBLE:	   return("_visible"); break;
    case PROPERTY_WIDTH:	   return("_width"); break;
    case PROPERTY_HEIGHT:	   return("_height"); break;
    case PROPERTY_ROTATION:	   return("_rotation"); break;
    case PROPERTY_TARGET:	   return("_target"); break;
    case PROPERTY_FRAMESLOADED:	   return("_framesLoaded"); break;
    case PROPERTY_NAME:		   return("_name"); break;
    case PROPERTY_DROPTARGET:	   return("_dropTarget"); break;
    case PROPERTY_URL:  	   return("_url"); break;
    case PROPERTY_HIGHQUALITY:	   return("_quality"); break;
    case PROPERTY_FOCUSRECT:       return("_focusRect"); break;
    case PROPERTY_SOUNDBUFTIME:    return("_soundBufTime"); break;
    case PROPERTY_WTHIT:	   return("_WTHIT!?"); break;
    default:			   return("unknown property!"); break;
  }
}

struct SWF_ACTIONPUSHPARAM *
newVar(char *var)
{
	struct SWF_ACTIONPUSHPARAM *v;

	v=malloc(sizeof(struct SWF_ACTIONPUSHPARAM));
	v->Type=10; /* VARIABLE */
	v->p.String = var;
	return v;
}

struct SWF_ACTIONPUSHPARAM *
newVar2(char *var,char *var2)
{
	struct SWF_ACTIONPUSHPARAM *v;

	v=malloc(sizeof(struct SWF_ACTIONPUSHPARAM));
	v->Type=10; /* VARIABLE */
	v->p.String = malloc(strlen(var)+strlen(var2)+1);
	strcpy(v->p.String,var);
	strcat(v->p.String,var2);
	return v;
}

struct SWF_ACTIONPUSHPARAM *
newVar3(char *var,char *var2, char *var3)
{
	struct SWF_ACTIONPUSHPARAM *v;

	v=malloc(sizeof(struct SWF_ACTIONPUSHPARAM));
	v->Type=10; /* VARIABLE */
	v->p.String = malloc(strlen(var)+strlen(var2)+strlen(var3)+1);
	strcpy(v->p.String,var);
	strcat(v->p.String,var2);
	strcat(v->p.String,var3);
	return v;
}

void
push(struct SWF_ACTIONPUSHPARAM *val)
{
	struct _stack *t;
#ifdef DEBUG
	printf("*push*\n");
#endif
	t = calloc(1,sizeof(Stack));
	t->type = val->Type;
	t->val = val;
	t->next = Stack;
	Stack = t;
}


void
pushdup()
{
	struct _stack *t;
#ifdef DEBUG
	printf("*pushdup*\n");
#endif
	t = calloc(1,sizeof(Stack));
	t->type = Stack->type;
	t->val =  Stack->val;
	t->next = Stack;
	Stack = t;
}


void
pushvar(struct SWF_ACTIONPUSHPARAM *val)
{
	struct _stack *t;
#ifdef DEBUG
	printf("*pushvar*\n");
#endif
	t = calloc(1,sizeof(Stack));
	t->type = 'v';
	t->val = val;
	t->next = Stack;
	Stack = t;
}

void
pushvar2( struct SWF_ACTIONPUSHPARAM *var, struct SWF_ACTIONPUSHPARAM *mem)
{
	struct _stack *t;
	char *vname, *varname,*memname;
	int len;
	varname=getName(var);
	memname=getString(mem);
	len = strlen(varname);
	len+= strlen(memname);
#ifdef DEBUG
	printf("*pushvar2*\n");
#endif
	t = calloc(1,sizeof(Stack));
	vname = malloc(len+2);
	strcpy(vname,varname);
	strcat(vname,".");
	strcat(vname,memname);
	var->p.String = vname;
	var->Type = 10; /* VARIABLE */
	t->type = 'v';
	t->val = var;
	t->next = Stack;
	Stack = t;
}

struct SWF_ACTIONPUSHPARAM * pop()
{
	struct _stack *t;
	struct SWF_ACTIONPUSHPARAM * ret;

#ifdef DEBUG
	printf("*pop*\n");
#endif
	if( Stack == NULL ) error("Stack blown!!");
	t=Stack;
	Stack=t->next;
	ret=t->val;
	return ret;
}

struct SWF_ACTIONPUSHPARAM * peek()
{
#ifdef DEBUG
	printf("*peek*\n");
#endif
	if( Stack == NULL ) error("Stack blown!!");
	return Stack->val;
}

void
stackswap()
{
#ifdef DEBUG
	printf("*stackswap*\n");
#endif
	struct SWF_ACTIONPUSHPARAM *p = peek();		/* peek() includes error handling */
	char type = Stack->type;
	Stack->type = Stack->next->type;
	Stack->val  = Stack->next->val;
	Stack->next->type = type;
	Stack->next->val  = p;
}


struct SWF_ACTIONPUSHPARAM *
newVar_N(char *var,char *var2, char *var3,char *var4,int pop_counter,char *final)
{
	struct SWF_ACTIONPUSHPARAM *v;
	int i;
	int slen=strlen(var)+strlen(var2)+strlen(var3)+strlen(var4)+strlen(final);
	
	v=malloc(sizeof(struct SWF_ACTIONPUSHPARAM));
	v->p.String = malloc(PARAM_STRSIZE+slen);
	v->Type=10; /* VARIABLE */
	strcpy(v->p.String,var);
	strcat(v->p.String,var2);
	strcat(v->p.String,var3);
	strcat(v->p.String,var4);
	for(i=0;i<pop_counter;i++) 
	{
	 char *pops=getString(pop());
	 if ( strlen(v->p.String)+ 2 + strlen(pops) < PARAM_STRSIZE+slen)
	 {
	  strcat(v->p.String,pops);
	  if( i < pop_counter-1 ) 
	   strcat(v->p.String,",");
	 }
	}
	strcat(v->p.String,final);
	return v;
}

/* End Package */

static int gIndent;
void decompileActions(int n, SWF_ACTION *actions,int indent);
char * decompile5Action(int n, SWF_ACTION *actions,int indent);

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


#define SanityCheck(curact,test,msg ) \
    if(!(test) ) error( "SanityCheck failed in %s\n %s\n", #curact, msg );

#define OUT_BEGIN(block) \
	                struct block *sact = (struct block *)act;
#define OUT_BEGIN2(block) \
	                struct block *sact = (struct block *)&(actions[n]);

void
decompileCONSTANTPOOL (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONCONSTANTPOOL);

  pool=sact->ConstantPool;
}

void
decompileGOTOLABEL (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONGOTOLABEL);

  INDENT
  printf("gotoLabel(%s);\n", sact->FrameLabel);
}

void
decompileGOTOFRAME (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONGOTOFRAME);

  INDENT
  printf("gotoFrame(%d);\n", sact->Frame);
}

void
decompileWAITFORFRAME (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONWAITFORFRAME);

  INDENT
  printf("WaitForFrame(%d,%d);\n", sact->Frame,sact->SkipCount);
}

void
decompilePLAY (SWF_ACTION *act)
{

  INDENT
  printf("play();\n");
}

void
decompileSTOP (SWF_ACTION *act)
{

  INDENT
  printf("stop();\n");
}

void
decompileNEXTFRAME (SWF_ACTION *act)
{
  INDENT
  printf("nextFrame();\n");
}

void
decompilePREVFRAME (SWF_ACTION *act)
{
  INDENT
  printf("prevFrame();\n");
}

void
decompileGETURL (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONGETURL);

  INDENT
  printf("getUrl(\"%s\",%s);\n", sact->UrlString, sact->TargetString);
}
int
decompileGETURL2 (SWF_ACTION *act)
{
    struct SWF_ACTIONPUSHPARAM *a;

    OUT_BEGIN(SWF_ACTIONGETURL2);
    INDENT
    a = pop();
    if (sact->f.FlagBits.SendVarsMethod)
     puts("loadVars(");
    else
     puts("getUrl(");
    puts(getString(pop()));
    puts(",");
    puts(getString(a));
    puts(");\n");
    return 0;
}

static void
decompilePUSHPARAM (struct SWF_ACTIONPUSHPARAM *act, int wantstring)
{

  switch( act->Type ) 
  {
	  case 0: /* STRING */
		if( wantstring )
  		  printf ("\"%s\"", act->p.String);
		else
  		  printf ("%s", act->p.String);
		break;
	  case 1: /* FLOAT */
  		printf ("%f", act->p.Float);
		break;
	  case 2: /* NULL */
  		printf ("NULL" );
		break;
	  case 3: /* Undefined */
  		printf ("undefined" );
		break;
	  case 4: /* Register */
		if( regs[act->p.RegisterNumber] ) {
  		  printf ("%s", getName(act));
		} else {
  		  printf ("R%d", (int)act->p.RegisterNumber);
		}
		break;
	  case 5: /* BOOLEAN */
  		printf ("  %d\n", act->p.Boolean);
		break;
	  case 6: /* DOUBLE */
  		printf ("%g", act->p.Double);
		break;
	  case 7: /* INTEGER */
  		printf ("%ld", act->p.Integer);
		break;
	  case 8: /* CONSTANT8 */
		if( wantstring )
  		  printf ("\"%s\"", pool[act->p.Constant8]);
		else
  		  printf ("%s", pool[act->p.Constant8]);
		break;
	  case 9: /* CONSTANT16 */
		if( wantstring )
  		  printf ("\"%s\"", pool[act->p.Constant16]);
		else
  		  printf ("%s", pool[act->p.Constant16]);
		break;
	  case 10: /* VARIABLE */
  		printf ("%s", act->p.String);
		break;
	  default: 
  		printf ("  Unknown type: %d\n", act->Type);
		break;
  }
}

int
isStoreOp(int n, SWF_ACTION *actions,int maxn)
{
    switch(actions[n].SWF_ACTIONRECORD.ActionCode)
    {
      case SWFACTION_STOREREGISTER:
      case SWFACTION_SETVARIABLE:
      case SWFACTION_SETMEMBER:
        return 1;
      default:
        return 0;
    }
}

/*
int
isArithmeticOp(int n, SWF_ACTION *actions,int maxn)
{
    switch(actions[n].SWF_ACTIONRECORD.ActionCode)
    {
      case SWFACTION_ADD2:
      case SWFACTION_GETMEMBER:
        return 1;
      default:
        return 0;
    }
}
*/

int
decompileArithmeticOp(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *left, *right;

    switch(actions[n].SWF_ACTIONRECORD.ActionCode)
    {
	    /*
      case SWFACTION_GETMEMBER:
              decompilePUSHPARAM(peek(),0);
	      break;
	      */
      case SWFACTION_ADD:
      case SWFACTION_ADD2:
	      right=pop();
	      left=pop();
	      /* see examples like 3*(6+1) vs. 3*6+1 
	         but if no other arithmetics follows on stack,
	         e.g. because storing the result we don't need neither'(' nor ')' . 
	         To do: store operator type on stack for better decision
	       */ 
	      if (isStoreOp(n+1, actions,maxn) )
	       push(newVar3(getString(left),"+",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"+",getString(right),0,")"));
	      break;
      case SWFACTION_SUBTRACT:
	      right=pop();
	      left=pop();
	      if (isStoreOp(n+1	, actions,maxn) )
		push(newVar3(getString(left),"-",getString(right)));	      
	      else
		push(newVar_N("(",getString(left),"-",getString(right),0,")"));
	      break;
      case SWFACTION_MULTIPLY:
	      right=pop();
	      left=pop();
	      push(newVar3(getString(left),"*",getString(right)));
	      break;
      case SWFACTION_DIVIDE:
	      right=pop();
	      left=pop();
	      push(newVar3(getString(left),"/",getString(right)));
	      break;
      case SWFACTION_MODULO:
	      right=pop();
	      left=pop();
	      push(newVar3(getString(left),"%",getString(right)));
	      break;

      default:
	printf("Unhandled Arithmetic OP %x\n",actions[n].SWF_ACTIONRECORD.ActionCode);
    }
   return 0;
}

int
isLogicalOp(int n, SWF_ACTION *actions,int maxn)
{
    switch(actions[n].SWF_ACTIONRECORD.ActionCode)
    {
      case SWFACTION_LESSTHAN:
      case SWFACTION_LOGICALAND:
      case SWFACTION_LOGICALOR:
      case SWFACTION_LOGICALNOT:
      case SWFACTION_STRINGEQ:
      case SWFACTION_STRINGCOMPARE:
      case SWFACTION_LESS2:
      case SWFACTION_EQUALS2:
      case SWFACTION_BITWISEAND:
      case SWFACTION_BITWISEOR:
      case SWFACTION_BITWISEXOR:
      case SWFACTION_STRICTEQUALS:
      case SWFACTION_GREATER:
      /*
      case SWFACTION_GETMEMBER:
      */
        return 1;
      default:
        return 0;
    }
}
int 
isLogicalOp2(int n, SWF_ACTION *actions,int maxn)
{
    switch(actions[n].SWF_ACTIONRECORD.ActionCode)
    {
      case SWFACTION_LOGICALNOT:
      case SWFACTION_PUSHDUP:
      case SWFACTION_IF:
       return 1;

      default:
        return 0;
    }
}

int
decompileLogicalOp(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *left, *right;

    switch(actions[n].SWF_ACTIONRECORD.ActionCode)
    {
      case SWFACTION_LESSTHAN:
	      puts("LESSTHAN");
	      break;
      case SWFACTION_LOGICALAND:
	      puts("LAND");
	      break;
      case SWFACTION_LOGICALOR:
	      puts("LOR");
	      break;
      case SWFACTION_STRINGEQ:
	      puts("STRINGEQ");
	      break;
      case SWFACTION_STRINGCOMPARE:
	      puts("STRINGCOMPARE");
	      break;
      case SWFACTION_LESS2:
	      if( actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT &&
	          actions[n+2].SWF_ACTIONRECORD.ActionCode != SWFACTION_IF ) {
	      	right=pop();
	      	left=pop();
		push(newVar3(getName(left),"!<",getName(right)));
	        return 1;
	      } else {
	        right=pop();
	        left=pop();
		push(newVar3(getName(left),"<",getName(right)));
	        return 0;
	      }
      case SWFACTION_EQUALS2:
	      if( actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT &&
	          actions[n+2].SWF_ACTIONRECORD.ActionCode != SWFACTION_IF ) {
	      	right=pop();
	      	left=pop();
		push(newVar3(getName(left),"!=",getString(right)));
	        return 1;
	      } else {
	        right=pop();
	        left=pop();
		push(newVar3(getString(left),"==",getString(right)));
	        return 0;
	      }
      case SWFACTION_STRICTEQUALS:
	      if( actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT &&
	          actions[n+2].SWF_ACTIONRECORD.ActionCode != SWFACTION_IF ) {
	      	right=pop();
	      	left=pop();
		push(newVar3(getString(left),"!==",getString(right)));
	        return 1;
	      } else {
	        right=pop();
	        left=pop();
		push(newVar3(getString(left),"===",getString(right)));
	        return 0;
	      }
      case SWFACTION_BITWISEAND:			/* To do: move to Arithmetics..? */
	      right=pop();
	      left=pop();
	      push(newVar3(getString(left),"&",getString(right)));
	      break;
      case SWFACTION_BITWISEOR:
	      right=pop();
	      left=pop();
	      push(newVar3(getString(left),"|",getString(right)));
	      break;
      case SWFACTION_BITWISEXOR:
	      right=pop();
	      left=pop();
	      push(newVar3(getString(left),"^",getString(right)));
	      break;
      case SWFACTION_GREATER:
	      puts("GREATER");
	      break;
      case SWFACTION_LOGICALNOT:
	      if( actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_GETVARIABLE &&
	          actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT &&
	          actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_IF ) {
		      /* It's a class statement  -- skip over both NOTs */
		      return 1;
	      }
	      if( actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_IF ) {
		      return 0;
	      }
	      push(newVar2("!",getString(pop())));
	      break;

      default:
	printf("Unhandled Logic OP %x\n",actions[n].SWF_ACTIONRECORD.ActionCode);
        return 0;
    }
        return 1;
}

void
decompilePUSH (SWF_ACTION *act)
{
  int i;
  OUT_BEGIN(SWF_ACTIONPUSH);

  SanityCheck(SWF_PUSH,
		act->SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH,
		"not a PUSH")

  for(i=0;i<sact->NumParam;i++)
  {
	  push(&(sact->Params[i]));
  }
}

void
decompilePUSHDUP (SWF_ACTION *act)
{
  SanityCheck(SWF_PUSHDUP,
		act->SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP,
		"not a PUSHDUP")
  pushdup();
}

void
decompileSTACKSWAP (SWF_ACTION *act)
{
  SanityCheck(SWF_STACKSWAP,
		act->SWF_ACTIONRECORD.ActionCode == SWFACTION_STACKSWAP,
		"not a STACKSWAP")
  stackswap();
}

int
decompileSETPROPERTY(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *val, *idx, *obj;

    INDENT
    val = pop();
    idx = pop();
    obj = newVar(getName(pop()));
    decompilePUSHPARAM(obj,0);
    puts(".");
    puts(getProperty(getInt(idx)));
    printf(" = " );
    decompilePUSHPARAM(val,0);
    puts(";\n");

    return 0;
}

int
decompileTRACE(int n, SWF_ACTION *actions,int maxn)
{
//    SanityCheck(SWF_TRACE,
//		actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH,
//		"TRACE not preceeded by PUSH")
// One never can expect this ^^^^^^^^^^^^^^  Try: trace(callme(123));
    INDENT
    /* Could there be more than one push for this? */
    puts("trace(");
    decompilePUSHPARAM(pop(),1);
    puts(");\n");

    return 0;
}

int
decompileGETTIME(int n, SWF_ACTION *actions,int maxn)
{
    INDENT
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     puts("getTimer();\n");
     return 1;
    }
    else
    {
     push(newVar("getTimer()"));
     return 0;
    }
}

int
decompileRANDOMNUMBER(int n, SWF_ACTION *actions,int maxn)
{
    INDENT
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     puts("random(");
     decompilePUSHPARAM(pop(),0);
     puts(");\n");
     return 1;
    }
    else
    {
     push(newVar_N("","","random","(", 1,")"));
     return 0;
    }
}

int
decompileDECREMENT(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *var;

//    SanityCheck(SWF_DECREMENT,
//		actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH,
//		"DECREMENT not preceeded by PUSH")
 
    INDENT
    var=pop();
    decompilePUSHPARAM(var,0);
    puts("--;\n");
    push(var);
    if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER) &&
        (var->Type == 4 /* Register */) &&
        (actions[n+1].SWF_ACTIONSTOREREGISTER.Register == var->p.RegisterNumber) ) {
	    regs[var->p.RegisterNumber] = var; /* Do the STOREREGISTER here */
	    return 1; /* Eat the StoreRegister that follows */
    }

    return 0;
}

int
decompileINCREMENT(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *var;

//    SanityCheck(SWF_INCREMENT,
//		actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH,
//		"INCREMENT not preceeded by PUSH")
    INDENT

    var=pop();
    decompilePUSHPARAM(var,0);
    puts("++;\n");
    push(var);
    if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER) &&
        (var->Type == 4 /* Register */) &&
        (actions[n+1].SWF_ACTIONSTOREREGISTER.Register == var->p.RegisterNumber) ) {
	    regs[var->p.RegisterNumber] = var; /* Do the STOREREGISTER here */
	    return 1; /* Eat the StoreRegister that follows */
    }
    return 0;
}

int
decompileSTOREREGISTER(int n, SWF_ACTION *actions,int maxn)
{
    OUT_BEGIN2(SWF_ACTIONSTOREREGISTER);

    INDENT
//    printf("R%d = ", sact->Register );
    regs[sact->Register] = peek();
//    puts(getName(regs[sact->Register]));
//    puts(";\n");
    return 0;
}

int
decompileNEWOBJECT(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *obj, *arg;
    int i,numparams,slen;
    char *t,*objname;

    obj = pop();
    objname = getName(obj);
    slen = strlen(objname);
    t = malloc(slen + PARAM_STRSIZE + 7); /* 'new '+'()'+'\000' */
    strcpy(t,"new ");
    strcat(t,objname);
    strcat(t,"(");
    obj->p.String=t;
    obj->Type=0; /* STRING */
    numparams = getInt(pop());
    for(i=0;i<numparams;i++) {
	    char *pops;    
	    arg=pop();
	    pops=getName(arg);
	    if ( strlen(t) + 2 + strlen(pops) < (slen + PARAM_STRSIZE + 7) )
	    {
	     strcat(t,pops);
	     if (i<numparams-1) 
	      strcat(t,",");
	    }
    }
    strcat(t,")");
    pushvar(obj);

    return 0;
}

int
decompileGETMEMBER(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *mem, *var;

    mem = newVar(getName(pop()));
    var = newVar(getName(pop()));
    pushvar2(var,mem);

    return 0;
}

int
decompileSETMEMBER(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *val, *var, *obj;

    INDENT
    val = pop();
    var = pop();
    obj = newVar(getName(pop()));
    decompilePUSHPARAM(obj,0);
    if (var->Type == 7 || var->Type == 10)			/* initial approach only */
     puts("[");
    else
     puts(".");
    decompilePUSHPARAM(var,0);
    if (var->Type == 7 || var->Type == 10)
     puts("]");
    printf(" = " );
    decompilePUSHPARAM(val,0);	/* FIXME: 0 or 1 this needs more attention */
    puts(";\n");

    return 0;
}

int
decompileGETVARIABLE(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *var;

    var = pop();
    pushvar(newVar(getName(var)));

    return 0;
}

int
decompileSETVARIABLE(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *val, *var;

    INDENT
    val = pop();
    var = pop();
    if (strcmp(getName(val),getName(var)))
    {
    puts(getName(var));
    printf(" = " );
    /*puts(getName(val));*/
    decompilePUSHPARAM(val,0);	/* FIXME: 0 or 1 this needs more attention */
    puts(";\n");
    }
    return 0;
}

#if 0
int
decompileADD2(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *var1, *var3;

    INDENT
    var3 = pop();
    var1 = pop();
    push(newVar3(getString(var1),"+",getString(var3)));

    return 0;
}
#endif

int
decompileJUMP(int n, SWF_ACTION *actions,int maxn)
{
    int i=0,j=0;
    struct SWF_ACTIONIF *sact=NULL;
    INDENT
    if( isLogicalOp(n+1, actions, maxn) ||
        ( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH) &&
           isLogicalOp(n+2, actions, maxn) ) ) {
	    /* Probably the start of a do {} while(), so skip it */
    	return 0;
    }

    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP
	|| actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_JUMP ) 
	    /* Probably the end of a switch{}, so skip it */
    	return 1;

    /* ak,2006 */
      for(i=0;(actions[(n+1)+i].SWF_ACTIONRECORD.Offset <
	(actions[n+1].SWF_ACTIONRECORD.Offset+actions[n ].SWF_ACTIONJUMP.BranchOffset))
	 && n+1+i<maxn; i++)
      {
	 #if 0
	 printf("/* for PART3 OP 0x%x */\n",actions[n+1+i].SWF_ACTIONRECORD.ActionCode)
	 #endif
	 ;
      }
      if (i)
      {
       for (j=0; n+j+i<maxn; j++)
       {
	 #if 0
	 printf("/* FOR part2 OP 0x%x */\n",actions[n+i+j].SWF_ACTIONRECORD.ActionCode)
	 // at least one should push on stack
	 #endif
	 ;
	 if (actions[n+i+j].SWF_ACTIONRECORD.ActionCode == SWFACTION_IF)
	 {
	  sact = (struct SWF_ACTIONIF *)&(actions[n+i+j]);
	  /* chk whether last jump does lead us back to start of loop */
	  if (sact->Actions[sact->numActions-1].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP
	   && sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset+
	      sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.Offset==
	      actions[n].SWF_ACTIONRECORD.Offset )
	  {
	   break;
	  }
	  else
	   sact=NULL;
         }
       }
      }
      if (sact)
      {
	puts("while(");
	decompileActions(j-1 , &actions[n+1+i  ] ,gIndent);
	puts(getName(pop()));
	puts("){         /* original FOR loop rewritten to WHILE */\n");
	decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
	decompileActions(i , &actions[n+1], gIndent+1 );
	INDENT
	puts("};\n");
	return i+j; 
      }
  error("Unhandled JUMP");
  return 0;
}

int
decompileRETURN(int n, SWF_ACTION *actions,int maxn)
{
    INDENT
    printf("return ");
    puts(getName(pop()));
    puts(";\n");

    return 0;
}

int
decompileDEFINELOCAL(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *val, *var;

    INDENT
    val = pop();
    var = pop();
    puts("var ");
    puts(getName(var));
    printf(" = " );
    puts(getName(val));
    puts(";\n");

    return 0;
}

int
decompileDEFINELOCAL2(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *var;

    INDENT
    var = pop();
    puts("var ");
    puts(getName(var));
    puts(";\n");

    return 0;
}

int
decompileIF(int n, SWF_ACTION *actions,int maxn)
{
    int i=0;
    OUT_BEGIN2(SWF_ACTIONIF);

    /*
     * IF is used in various way to implement different types
     * of loops. We try to detect these different types of loops
     * here.
     */

    if( (actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT) &&
        (actions[n-2].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT) &&
        (actions[n-3].SWF_ACTIONRECORD.ActionCode == SWFACTION_GETVARIABLE) &&
        (actions[n-4].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH) ) {
	    /* It's really a class definition */
            INDENT
	    puts("class ");
	    decompilePUSHPARAM(newVar(getName(pop())),0);
	    puts(" {\n");
            decompileActions(sact->numActions, sact->Actions,gIndent+1);
            INDENT
	    puts("}\n");
	    return 0;
    }

    /*
     * do {} while() loops have a JUMP at the end of the if clause
     * that points to a JUMP above the IF statement.
     */
    if( isLogicalOp(n-1, actions, maxn) &&
        (sact->Actions[sact->numActions-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_JUMP) &&
        ( (sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.Offset +
           sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset) < actions[n].SWF_ACTIONRECORD.Offset) &&
	isLogicalOp(sact->numActions-2, sact->Actions, maxn) ) {
            INDENT
	    puts("do {\n");
            decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
	    puts("while( ");
	    puts(getName(pop()));
	    puts(");");
	    return 0;
    }

    /*
     * while() loops have a JUMP at the end of the if clause that jumps backwards
     */
    if( isLogicalOp(n-1, actions, maxn) &&
        ( (sact->Actions[sact->numActions-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_JUMP) &&
           sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset < 0) ) {
	    dumpRegs();
            INDENT
	    puts("while( ");
	    puts(getName(pop()));
	    puts(" ) {\n");
            decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
            INDENT
	    puts("}\n");
	    return 0;
    }

    /*
     * Any other use of IF must be a real if()
     */
#if 0
    if( isLogicalOp(n-1, actions, maxn) ) {
            INDENT
	    puts("if( ");
	    /* Eat a LogicNOT that is part of the If */
	    /* decompileLogicalOp(n-2, actions, maxn); */
	    puts(getName(pop()));
	    puts(" ) {\n");
            if ( (sact->Actions[sact->numActions-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_JUMP) &&
                  sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset > 0 ) {
	      /* There is an else clause also! */
              decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
	      puts("} else {\n");

	      /*
	       * Count the number of action records that are part of
	       * the else clause, and then decompile only that many.
	       */
	      for(i=0;
	          (actions[(n+1)+i].SWF_ACTIONRECORD.Offset <
		  (actions[n+1].SWF_ACTIONRECORD.Offset+
		   sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset)) &&
		  (i<(maxn-(n+1)));
		  i++)
		      /*
		      printf("[%3.3d] %d %x < %x + %x (%x)\n", i, (maxn-(n+1)),
			actions[(n+1)+i].SWF_ACTIONRECORD.Offset,
			actions[n+1].SWF_ACTIONRECORD.Offset,
			sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset,
			actions[n+1].SWF_ACTIONRECORD.Offset+
			sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset)
			*/;
              decompileActions(i+1, &actions[n+1],gIndent+1);
	    } else {
	      /* It's a simple if() {} */
              decompileActions(sact->numActions, sact->Actions,gIndent+1);
	    }
            INDENT
	    puts("}\n");
	    return i;
    }

    SanityCheck(SWF_IF, 0, "IF type not recognized")
#else	/* 2006 NEW stuff comes here */
    {
    	    int has_else= ((sact->Actions[sact->numActions-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_JUMP) &&
                  (sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset > 0 )) ? 1:0;
    	    int has_lognot=(actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT) ? 1:0;
	    int has_chain=0;
            /* does it continue any other condition sequence ? 
               Something like: if (x>2 && 7>y || callme(123) || false) { this(); } else { that(); }
            */
	    if ( isLogicalOp2(n-1, actions, maxn) &&
	    	 ( (isLogicalOp2(n-2, actions, maxn) && has_else) || 
	    	    actions[n-2].SWF_ACTIONRECORD.ActionCode == SWFACTION_IF ))
	    {
	     /* continued with || or && and else clause is present */
	    }
	    else
	    {  
	     if (  actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT 
	     	 && (
	             actions[n-2].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP
	           ||actions[n-2].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT 
	           ) &&
	     	   actions[n-3].SWF_ACTIONRECORD.ActionCode == SWFACTION_IF     )
	     {
	      /* continued with && */
	     }
	     else
	     {
	      INDENT
	      puts("if( ");
	     }
	    }
	    
	    puts(getName(pop()));	/* the condition itself */
	     
	    /* is it followed by another condition ? */
	    if ( actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP  )
	    {
	     puts(" || ");
	     has_chain=1;		/* i.e. do NOT write '}' below */
	    }
	    else
	    {
	     if (actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT &&
  	    	 actions[n-2].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP )  
  	      {
	       puts(" && ");
	       has_chain=1;
	      }
	      else
               puts(" ) {\n");
            }
            if ( has_else ) {
	      /* There is an else clause also!
	         In general we expect a LOGICALNOT before,
	         but if missing we exchange if-part with else-part
	       */
	     if  (has_lognot)
	     {
              decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
              INDENT
	      puts("} else {\n");
	     }	      
	      /*
	       * Count the number of action records that are part of
	       * the else clause, and then decompile only that many.
	       */
	      for(i=0;
	          (actions[(n+1)+i].SWF_ACTIONRECORD.Offset <
		  (actions[n+1].SWF_ACTIONRECORD.Offset+
		   sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset)) &&
		  (i<(maxn-(n+1)));
		  i++)
		;
               decompileActions(i  , &actions[n+1],gIndent+1);

	      if  (!has_lognot)		/* the missing if-part just NOW */
	      {
		INDENT
		puts ("} else {\n");
		decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
	      }
	    } else {
	      /* It's a simple if() {} */
              decompileActions(sact->numActions, sact->Actions,gIndent+1);
	    }
	    if ( !has_chain )		/* standalone condition or last in a chain */
	    {
	      INDENT
	      puts("}\n");
	    }
	    return i;
    }
#endif
    return 0;
}

int
decompileWITH(int n, SWF_ACTION *actions,int maxn)
{
    OUT_BEGIN2(SWF_ACTIONWITH);

    INDENT
    puts("with(");
    decompilePUSHPARAM(pop(),0);
    puts(")");
    puts(" {\n");
    decompileActions(sact->numActions, sact->Actions,gIndent+1);
    INDENT
    puts("}\n");

    return 1;
}

int
decompileDEFINEFUNCTION(int n, SWF_ACTION *actions,int maxn)
{
#if 0
    int i;
    struct SWF_ACTIONPUSHPARAM *name = NULL;
    OUT_BEGIN2(SWF_ACTIONDEFINEFUNCTION);

    INDENT
    puts("function ");
    /*
     * If the FunctionName is null, then it is being declared anonymously,
     * so look for a SETMEMBER to follow which pulls the name off of the stack
     */
    if( sact->FunctionName == NULL || *sact->FunctionName == '\000' ) {
      if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER) &&
          (actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETMEMBER) ) {
        name = pop();
        puts("function ");
        decompilePUSHPARAM(name,0);
      } else
      if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER) ) {
        printf("var R%d = function ", actions[n+1].SWF_ACTIONSTOREREGISTER.Register );
      } else
      if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETMEMBER) ) {
        name = pop();
        puts("function ");
        decompilePUSHPARAM(name,0);
      }
    } else {
      puts(sact->FunctionName);
    }
    puts("(");
    for(i=0;i<sact->NumParams;i++) {
	puts(sact->Params[i]);
	if( sact->NumParams > i+1 ) puts(",");
    }
    puts(")");
    puts(" {\n");
    decompileActions(sact->numActions, sact->Actions,gIndent+1);
    INDENT
    puts("}\n");
    if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER) &&
        (actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETMEMBER) ) {
	regs[actions[n+1].SWF_ACTIONSTOREREGISTER.Register]  = name; /* Do the STOREREGISTER here */
    	return 2;
    }

    if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER) &&
        (actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP) ) {
	regs[actions[n+1].SWF_ACTIONSTOREREGISTER.Register]  = name; /* Do the STOREREGISTER here */
    	return 2;
    }
#else	/* 2006 NEW stuff comes here */
    int i;
    OUT_BEGIN2(SWF_ACTIONDEFINEFUNCTION);
    struct strbufinfo origbuf;
    INDENT
    #if 0
    printf("/* function followed by OP %x */\n", actions[n+1].SWF_ACTIONRECORD.ActionCode);
    #endif
    #if USE_LIB
    if (isStoreOp(n+1, actions,maxn))
      origbuf=setTempString();	/* switch to a temporary string buffer */
    #endif
    puts("function ");
    if (sact->FunctionName) puts(sact->FunctionName);
    puts(" (");
    for(i=0;i<sact->NumParams;i++) {
	puts(sact->Params[i]);
	if( sact->NumParams > i+1 ) puts(",");
    }
    puts(") {\n");
    INDENT
    decompileActions(sact->numActions, sact->Actions,gIndent+1);
    INDENT
    if (isStoreOp(n+1, actions,maxn))
    {
     puts("}");
     #if USE_LIB
      push (newVar(dcgetstr()));			/* push func body for later assignment */
      setOrigString(origbuf);				/* switch back to orig buffer */
     #else
      push (newVar("/* see function code above */"));	/* workaround only if LIB is not in use */
     #endif
    }
    else
     puts("}\n");
#endif

    return 0;
}

int
decompileDEFINEFUNCTION2(int n, SWF_ACTION *actions,int maxn)
{
    int i;
    struct SWF_ACTIONPUSHPARAM *name = NULL;
    OUT_BEGIN2(SWF_ACTIONDEFINEFUNCTION2);

    INDENT
    /*
     * If the FunctionName is null, then it is being declared anonymously,
     * so look for a SETMEMBER to follow which pulls the name off of the stack
     */
    if( sact->FunctionName == NULL || *sact->FunctionName == '\000' ) {
      if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER) &&
          (actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETMEMBER) ) {
        name = pop();
        puts("function ");
        decompilePUSHPARAM(name,0);
      } else
      if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER) ) {
        printf("var R%d = function ", actions[n+1].SWF_ACTIONSTOREREGISTER.Register );
      } else
      if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETMEMBER) ) {
        name = pop();
        puts("function ");
        decompilePUSHPARAM(name,0);
      }
    } else {
      puts(sact->FunctionName);
    }
    puts("(");
    for(i=0;i<sact->NumParams;i++) {
        /* decompilePUSHPARAM(pop(),0); */
	    puts(sact->Params[i].ParamName);
	/* Make a var from this name, and stick it in the register */
	    if( sact->Params[i].Register)
		regs[sact->Params[i].Register] = newVar(sact->Params[i].ParamName);
	if( sact->NumParams > i+1 ) puts(",");
    }
    puts(")");
    puts(" {\n");
    decompileActions(sact->numActions, sact->Actions,gIndent+1);
    INDENT
    puts("}\n");

    if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER) &&
        (actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETMEMBER) ) {
	regs[actions[n+1].SWF_ACTIONSTOREREGISTER.Register]  = name; /* Do the STOREREGISTER here */
    	return 2;
    }

    if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER) &&
        (actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP) ) {
	regs[actions[n+1].SWF_ACTIONSTOREREGISTER.Register]  = name; /* Do the STOREREGISTER here */
    	return 2;
    }

    return 1;
}

int
decompileCALLMETHOD(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *meth, *obj, *nparam;
/*    int i;*/

    SanityCheck(SWF_CALLMETHOD,
		actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH,
		"CALLMETHOD not preceeded by PUSH")
    
    meth=pop();
    obj=pop();
    nparam=pop();
#if 0
    printf("/* %ld params */\n",nparam->p.Integer);
#endif
    INDENT
#if 0
    puts(getName(obj));
    puts(".");
    puts(getName(meth));
    puts("(");
    for(i=0;i<nparam->p.Integer;i++) {
        puts(getString(pop()));
	if( nparam->p.Integer > i+1 ) puts(",");
    }
    puts(");\n");
    push(newVar("funcret"));

#else
    push(newVar_N(getName(obj),".",getName(meth),"(", nparam->p.Integer,")"));
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     /* call method and throw away any result */
     puts(getName(pop()));
     puts(";\n");
     return 1;
    }
#endif
    return 0;
}

int
decompileCALLFUNCTION(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *meth, *nparam;
/*    int i;*/

    SanityCheck(SWF_CALLMETHOD,
		actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH,
		"CALLMETHOD not preceeded by PUSH")
    
    meth=pop();
    nparam=pop();

    INDENT
#if 0
    //decompilePUSHPARAM(meth,0);
    puts(getName(meth));
    puts("(");
    for(i=0;i<nparam->p.Integer;i++) {
    	puts(getString(pop()));
	if( nparam->p.Integer > i+1 ) puts(",");
    }
    puts(");\n");

    return 1;
#else
    push(newVar_N("","",getName(meth),"(", nparam->p.Integer,")"));
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     /* call function and throw away any result */
     puts(getName(pop()));
     puts(";\n");
     return 1;
    }
    return 0;
#endif
}

int
decompileINT(int n, SWF_ACTION *actions,int maxn)
{
    push(newVar_N("","","int","(", 1,")"));
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     /* call function and throw away any result */
     INDENT
     puts(getName(pop()));
     puts(";\n");
     return 1;
    }
    return 0;
}

int
decompileTOSTRING(int n, SWF_ACTION *actions,int maxn)
{
    push(newVar_N("","","String","(", 1,")"));
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     /* call function and throw away any result */
     INDENT
     puts(getName(pop()));
     puts(";\n");
     return 1;
    }
    return 0;
}

int
decompileTONUMBER(int n, SWF_ACTION *actions,int maxn)
{
    push(newVar_N("","","Number","(", 1,")"));
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     /* call function and throw away any result */
     INDENT
     puts(getName(pop()));
     puts(";\n");
     return 1;
    }
    return 0;
}

int
decompileSTRINGCONCAT(int n, SWF_ACTION *actions,int maxn)
{
    push(newVar_N("","","concat","(", 2,")"));
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     /* call function and throw away any result */
     INDENT
     puts(getName(pop()));
     puts(";\n");
     return 1;
    }
    return 0;
}

int
decompileREMOVECLIP(int n, SWF_ACTION *actions,int maxn)
{
    INDENT
    puts("removeMovieClip(");
    puts(getName(pop()));
    puts(");\n");
    return 0;
}

int 
decompileDUPLICATECLIP(int n, SWF_ACTION *actions,int maxn)
{
    INDENT
    struct SWF_ACTIONPUSHPARAM *a, *b;

    INDENT
    a = pop();
    b = pop();

    puts("duplicateMovieClip(");
    puts(getString(pop()));
    puts(",");
    puts(getString(b));
    puts(",");
    puts(getString(a));
    puts(");\n");
    return 0;

}

int
decompileINITARRAY(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *nparam;
    nparam=pop();
    push(newVar_N("","","","[", nparam->p.Integer,"]"));
    return 0;
}
 
int
decompileAction(int n, SWF_ACTION *actions,int maxn)
{
    if( n > maxn ) error("Action overflow!!");

#ifdef DEBUG
    printf("ACTION[%3.3d]: %s\n", n, actionName(actions[n].SWF_ACTIONRECORD.ActionCode));
#endif

    switch(actions[n].SWF_ACTIONRECORD.ActionCode)
    {
      case SWFACTION_END:
	return 0;

      case SWFACTION_CONSTANTPOOL:
        decompileCONSTANTPOOL(&actions[n]);
	return 0;

      case SWFACTION_GOTOLABEL:
        decompileGOTOLABEL(&actions[n]);
        return 0;

      case SWFACTION_GOTOFRAME:
        decompileGOTOFRAME(&actions[n]);
	return 0;

      case SWFACTION_WAITFORFRAME:
        decompileWAITFORFRAME(&actions[n]);
	return 0;

      case SWFACTION_PLAY:
        decompilePLAY(&actions[n]);
	return 0;

      case SWFACTION_STOP:
        decompileSTOP(&actions[n]);
	return 0;

      case SWFACTION_GETURL2:
        decompileGETURL2(&actions[n]);
	return 0;

      case SWFACTION_GETURL:
        decompileGETURL(&actions[n]);
	return 0;

      case SWFACTION_PUSH:
        decompilePUSH(&actions[n]);
	return 0;

      case SWFACTION_PUSHDUP:
        decompilePUSHDUP(&actions[n]);
	return 0;

      case SWFACTION_STACKSWAP:
        decompileSTACKSWAP(&actions[n]);	
	return 0;

      case SWFACTION_SETPROPERTY:
        decompileSETPROPERTY(n, actions, maxn);
	return 0;

      case SWFACTION_NEXTFRAME:
        decompileNEXTFRAME(&actions[n]);
        return 0;

      case SWFACTION_PREVFRAME:
        decompilePREVFRAME(&actions[n]);
        return 0;


      case SWFACTION_RANDOMNUMBER:
        return decompileRANDOMNUMBER(n, actions, maxn);

      case SWFACTION_GETTIME:
        return decompileGETTIME(n, actions, maxn);
        
      case SWFACTION_TRACE:
        decompileTRACE(n, actions, maxn);
	return 0;

      case SWFACTION_NEWOBJECT:
        decompileNEWOBJECT(n, actions, maxn);
	return 0;

      case SWFACTION_GETMEMBER:
        decompileGETMEMBER(n, actions, maxn);
	return 0;

      case SWFACTION_SETMEMBER:
        decompileSETMEMBER(n, actions, maxn);
	return 0;

      case SWFACTION_GETVARIABLE:
        decompileGETVARIABLE(n, actions, maxn);
	return 0;

      case SWFACTION_SETVARIABLE:
        decompileSETVARIABLE(n, actions, maxn);
	return 0;

      case SWFACTION_DEFINELOCAL:
        decompileDEFINELOCAL(n, actions, maxn);
	return 0;

      case SWFACTION_DEFINELOCAL2:
        decompileDEFINELOCAL2(n, actions, maxn);
	return 0;

      case SWFACTION_DECREMENT:
        return decompileDECREMENT(n, actions, maxn);

      case SWFACTION_INCREMENT:
        return decompileINCREMENT(n, actions, maxn);

      case SWFACTION_STOREREGISTER:
        decompileSTOREREGISTER(n, actions, maxn);
	return 0;

      case SWFACTION_JUMP:
        return decompileJUMP(n, actions, maxn);

      case SWFACTION_RETURN:
        decompileRETURN(n, actions, maxn);
	return 0;

      case SWFACTION_IF:
        return decompileIF(n, actions, maxn);

      case SWFACTION_WITH:
        decompileWITH(n, actions, maxn);
	return 0;

      case SWFACTION_INITARRAY:
        return decompileINITARRAY(n, actions, maxn);

      case SWFACTION_DEFINEFUNCTION:
	return decompileDEFINEFUNCTION(n, actions, maxn);

      case SWFACTION_DEFINEFUNCTION2:
	return decompileDEFINEFUNCTION2(n, actions, maxn);

      case SWFACTION_CALLFUNCTION:
        return decompileCALLFUNCTION(n, actions, maxn);

      case SWFACTION_CALLMETHOD:
        return decompileCALLMETHOD(n, actions, maxn);
        
      case SWFACTION_ADD:
      case SWFACTION_ADD2:
      case SWFACTION_SUBTRACT:
      case SWFACTION_MULTIPLY:
      case SWFACTION_DIVIDE:
      case SWFACTION_MODULO:
        return decompileArithmeticOp(n, actions, maxn);

      case SWFACTION_POP:
	pop();
        return 0;

      case SWFACTION_INT:
        return decompileINT(n, actions, maxn);

      case SWFACTION_TOSTRING:
        return decompileTOSTRING(n, actions, maxn);

      case SWFACTION_TONUMBER:
	return decompileTONUMBER(n, actions, maxn);

      case SWFACTION_STRINGCONCAT:
	return decompileSTRINGCONCAT(n, actions, maxn);

      case SWFACTION_REMOVECLIP:
	return decompileREMOVECLIP(n, actions, maxn);

      case SWFACTION_DUPLICATECLIP:
	return decompileDUPLICATECLIP(n, actions, maxn);

      case SWFACTION_LESSTHAN:
      case SWFACTION_LOGICALAND:
      case SWFACTION_LOGICALOR:
      case SWFACTION_STRINGEQ:
      case SWFACTION_STRINGCOMPARE:
      case SWFACTION_LESS2:
      case SWFACTION_EQUALS2:
      case SWFACTION_STRICTEQUALS:
      case SWFACTION_BITWISEAND:
      case SWFACTION_BITWISEOR:
      case SWFACTION_BITWISEXOR:
      case SWFACTION_GREATER:
      case SWFACTION_LOGICALNOT:
        return decompileLogicalOp(n, actions, maxn);

      default:
	outputSWF_ACTION(n,&actions[n]);
	return 0;
    }
}

void
decompileActions(int n, SWF_ACTION *actions, int indent)
{
  int i, svindent;

  svindent = gIndent;
  gIndent = indent;

  for(i=0;i<n;i++) {
  	i+=decompileAction(i, actions, n);
    }

  gIndent = svindent;
}

char *
decompile5Action(int n, SWF_ACTION *actions,int indent)
{

  if( n == 1 )
	  return NULL;

  pool = NULL;

  dcinit();

  decompileActions(n, actions, indent);

  return dcgetstr();
}

