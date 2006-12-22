#define _GNU_SOURCE

//#define STATEMENT_CLASS  
//  I have uncommented some buggy class recognition stuff in decompileIF()
//  to make work simple code lines like:  "if(!a) trace(a);"   - ak, November 2006


#include <assert.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "read.h"
#include "action.h"
#include "swftypes.h"

#define NL "\\\n"

#ifndef HAVE_VASPRINTF
/* Workaround for the lack of vasprintf()
 * As found on: http://unixpapa.com/incnote/stdio.html
 * Seems to be Public Domain
 */
int
vasprintf(char **ret, const char *format, va_list ap)
{
	va_list ap2;
	int len = 100;        /* First guess at the size */
	if ((*ret = (char *) malloc(len)) == NULL)
	{
		return -1;
	}
	while (1)
	{
		int nchar;
		va_copy(ap2, ap);
		nchar= vsnprintf(*ret, len, format, ap2);
		if (nchar > -1 && nchar < len)
		{
			return nchar;
		}
		if (nchar > len)
		{
			len= nchar+1;
		} else
		{
			len*= 2;
		}
		if ((*ret = (char *) realloc(*ret, len)) == NULL)
		{
			free(*ret);
			return -1;
		}
	}
}
#endif

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

#define DCSTRSIZE 40960
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


/*
 TODO: add some constants (#define BOOLEAN 5  etc.)
*/
static char *
getString(struct SWF_ACTIONPUSHPARAM *act)
{
	char *t;
#ifdef DEBUG
	printf("*getString* type=%d\n",act->Type);
#endif
  switch( act->Type ) 
  {
	  case 0: /* STRING */
                t=malloc(strlen(act->p.String)+3); /* 2 "'"s and a NULL */
		strcpy(t,"'");
		strcat(t,act->p.String);
		strcat(t,"'");
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
                t=malloc(strlen(pool[act->p.Constant8])+3); /* 2 "'"s and a NULL */
		strcpy(t,"'");
		strcat(t,pool[act->p.Constant8]);
		strcat(t,"'");
  		return t;
	  case 9: /* CONSTANT16 */
                t=malloc(strlen(pool[act->p.Constant16])+3); /* 2 '\"'s and a NULL */
		strcpy(t,"'");
		strcat(t,pool[act->p.Constant16]);
		strcat(t,"'");
  		return t;

	  case 12:
	  case 11: /* INCREMENTED or DECREMENTED VARIABLE */
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
	  case 1: /* FLOAT -- also used for PROPERTY storing */
		return ((int)act->p.Float)>>16;
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
    case SWF_SETPROPERTY_X:
    /* case PROPERTY_X:*/	   return("_x"); break;
    case SWF_SETPROPERTY_Y:
    case PROPERTY_Y:		   return("_y"); break;
    case PROPERTY_XMOUSE:	   return("_xMouse"); break;
    case PROPERTY_YMOUSE:	   return("_yMouse"); break;
    case SWF_SETPROPERTY_XSCALE:
    case PROPERTY_XSCALE:	   return("_xScale"); break;
    case SWF_SETPROPERTY_YSCALE:
    case PROPERTY_YSCALE:	   return("_yScale"); break;
    case PROPERTY_CURRENTFRAME:	   return("_currentFrame"); break;
    case PROPERTY_TOTALFRAMES:	   return("_totalFrames"); break;
    case SWF_SETPROPERTY_ALPHA:
    case PROPERTY_ALPHA:	   return("_alpha"); break;
    case SWF_SETPROPERTY_VISIBILITY:
    case PROPERTY_VISIBLE:	   return("_visible"); break;
    case PROPERTY_WIDTH:	   return("_width"); break;
    case PROPERTY_HEIGHT:	   return("_height"); break;
    case SWF_SETPROPERTY_ROTATION:
    case PROPERTY_ROTATION:	   return("_rotation"); break;
    case PROPERTY_TARGET:	   return("_target"); break;
    case PROPERTY_FRAMESLOADED:	   return("_framesLoaded"); break;
    case SWF_SETPROPERTY_NAME:
    case PROPERTY_NAME:		   return("_name"); break;
    case PROPERTY_DROPTARGET:	   return("_dropTarget"); break;
    case PROPERTY_URL:  	   return("_url"); break;
    case SWF_SETPROPERTY_HIGHQUALITY:
    case PROPERTY_HIGHQUALITY:	   return("_quality"); break;
    case SWF_SETPROPERTY_SHOWFOCUSRECT:
    case PROPERTY_FOCUSRECT:       return("_focusRect"); break;
    case SWF_SETPROPERTY_SOUNDBUFFERTIME:
    case PROPERTY_SOUNDBUFTIME:    return("_soundBufTime"); break;
    case SWF_SETPROPERTY_WTHIT:
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
	printf("*push* type=%d\n",val->Type);
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

struct SWF_ACTIONPUSHPARAM * pop()
{
	struct _stack *t;
	struct SWF_ACTIONPUSHPARAM * ret;

#ifdef DEBUG
	printf("*pop*\n");
#endif
	if( Stack == NULL ) error("Stack blown!! - pop");
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
	if( Stack == NULL ) error("Stack blown!! - peek");
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
	 else {
		fprintf(stderr,"Some string overflowed something in newVar_N()??????\n");
		while (++i<pop_counter)
		  getString(pop());
		printf("/* *** truncated string here: *** */\n");
		break;
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
  printf("gotoLabel(%s);" NL, sact->FrameLabel);
}


void
decompileWAITFORFRAME (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONWAITFORFRAME);

  INDENT
  printf("WaitForFrame(%d,%d);" NL, sact->Frame,sact->SkipCount);
}

void
decompilePLAY (SWF_ACTION *act)
{

  INDENT
  printf("play();" NL);
}

void
decompileSTOP (SWF_ACTION *act)
{

  INDENT
  printf("stop();" NL);
}

void
decompileNEXTFRAME (SWF_ACTION *act)
{
  INDENT
  printf("nextFrame();" NL);
}

void
decompilePREVFRAME (SWF_ACTION *act)
{
  INDENT
  printf("prevFrame();" NL);
}

void
decompileGETURL (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONGETURL);

  INDENT
  printf("getUrl(\"%s\",%s);" NL, sact->UrlString, sact->TargetString);
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
    puts(");" NL);
    return 0;
}

static void
decompilePUSHPARAM (struct SWF_ACTIONPUSHPARAM *act, int wantstring)
{

  switch( act->Type ) 
  {
	  case 0: /* STRING */
		if( wantstring )
  		  printf ("'%s'", act->p.String);
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
  		printf ("%s", act->p.Boolean?"true":"false");
		break;
	  case 6: /* DOUBLE */
  		printf ("%g", act->p.Double);
		break;
	  case 7: /* INTEGER */
  		printf ("%ld", act->p.Integer);
		break;
	  case 8: /* CONSTANT8 */
		if( wantstring )
  		  printf ("'%s'", pool[act->p.Constant8]);
		else
  		  printf ("%s", pool[act->p.Constant8]);
		break;
	  case 9: /* CONSTANT16 */
		if( wantstring )
  		  printf ("'%s'", pool[act->p.Constant16]);
		else
  		  printf ("%s", pool[act->p.Constant16]);
		break;
	  case 12:
	  case 11: /* INCREMENTED or DECREMENTED VARIABLE */
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

void
decompileGOTOFRAME (SWF_ACTION *act,int is_type2)
{
  OUT_BEGIN(SWF_ACTIONGOTOFRAME);

  INDENT
  if (is_type2)
  {
   puts("gotoFrame(");
   decompilePUSHPARAM(pop(),0);
   puts(");" NL);
  }
  else
  printf("gotoFrame(%d);" NL, sact->Frame);
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

int precedence(int op1,int op2)
{
 unsigned char ops[]= { 		// array of opcodes w rising precedence
	SWFACTION_SETVARIABLE,		// TAKE CARE: array is incomplete

//	SWFACTION_LOGICALOR,
//	SWFACTION_LOGICALAND,
	SWFACTION_BITWISEOR,
	SWFACTION_BITWISEXOR,
	SWFACTION_BITWISEAND,

	SWFACTION_EQUALS2,
	SWFACTION_LESS2,
	
	SWFACTION_SHIFTRIGHT,
	SWFACTION_SHIFTRIGHT2,
	SWFACTION_SHIFTLEFT,

	SWFACTION_ADD,
	SWFACTION_ADD2,
	SWFACTION_SUBTRACT,
	SWFACTION_MODULO,
	SWFACTION_DIVIDE,
	SWFACTION_MULTIPLY,
//	SWFACTION_LOGICALNOT,
	SWFACTION_PUSH
 };
 unsigned char* f=memchr(ops,op1,sizeof(ops));
 unsigned char* s=memchr(ops,op2,sizeof(ops));
#ifdef DEBUG
 printf("1op=%d 2op=%d  result=%d\n",op1,op2,f>s);
 if (!f) printf("opcode=%d NOT in precedence list\n",op1);
 if (!s) printf("opcode=%d NOT in precedence list\n",op2);
#endif
 return f>s;
}

int
decompileArithmeticOp(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *left, *right;
    right=pop();
    left=pop();

    switch(actions[n].SWF_ACTIONRECORD.ActionCode)
    {
	    /*
      case SWFACTION_GETMEMBER:
              decompilePUSHPARAM(peek(),0);
	      break;
	      */
      case SWFACTION_ADD:
      case SWFACTION_ADD2:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"+",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"+",getString(right),0,")"));
	      break;
      case SWFACTION_SUBTRACT:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
		push(newVar3(getString(left),"-",getString(right)));	      
	      else
	       push(newVar_N("(",getString(left),"-",getString(right),0,")"));
	      break;
      case SWFACTION_MULTIPLY:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"*",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"*",getString(right),0,")"));
	      break;
      case SWFACTION_DIVIDE:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"/",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"/",getString(right),0,")"));
	      break;
      case SWFACTION_MODULO:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"%",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"%",getString(right),0,")"));
	      break;
      case SWFACTION_SHIFTLEFT:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"<<",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"<<",getString(right),0,")"));
	      break;
      case SWFACTION_SHIFTRIGHT:
      	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),">>",getString(right)));
	      else
	       push(newVar_N("(",getString(left),">>",getString(right),0,")"));
	      break;
      case SWFACTION_SHIFTRIGHT2:        
      	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),">>>",getString(right)));
	      else
	       push(newVar_N("(",getString(left),">>>",getString(right),0,")"));
	      break;
      case SWFACTION_LOGICALAND:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"&&",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"&&",getString(right),0,")"));
	      break;
      case SWFACTION_LOGICALOR:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"||",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"||",getString(right),0,")"));
	      break;
      case SWFACTION_BITWISEAND:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"&",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"&",getString(right),0,")"));
	      break;
      case SWFACTION_BITWISEOR:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"|",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"|",getString(right),0,")"));
	      break;
      case SWFACTION_BITWISEXOR:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"^",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"^",getString(right),0,")"));
	      break;
      case SWFACTION_EQUALS2:							/* including negation */
	      if( actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT &&
	          actions[n+2].SWF_ACTIONRECORD.ActionCode != SWFACTION_IF )
	      {
      	       if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+2].SWF_ACTIONRECORD.ActionCode))
	        push(newVar3(getString(left),"!=",getString(right)));
	       else
	        push(newVar_N("(",getString(left),"!=",getString(right),0,")"));
	       return 1; 							/* due negation op */
	      }
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"==",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"==",getString(right),0,")"));
	      break;

      case SWFACTION_LESS2:
	      if( actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT &&
	          actions[n+2].SWF_ACTIONRECORD.ActionCode != SWFACTION_IF ) 
	       {
      	       if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+2].SWF_ACTIONRECORD.ActionCode))
	        push(newVar3(getString(left),">=",getString(right)));
	       else
	        push(newVar_N("(",getString(left),">=",getString(right),0,")"));
	       return 1; 							/* due negation op */
	      } 
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"<",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"<",getString(right),0,")"));
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


#define RESETCOUNTER_TO_1 99999
int
stackVal(int n, SWF_ACTION *actions)
{
    switch((actions[n]).SWF_ACTIONRECORD.ActionCode)
    {
      case SWFACTION_END:
      case SWFACTION_NEXTFRAME:
      case SWFACTION_PREVFRAME:
      case SWFACTION_PLAY:
      case SWFACTION_STOP:
      case SWFACTION_LOGICALNOT:
      case SWFACTION_WITH:
      case SWFACTION_JUMP:
      case SWFACTION_CONSTANTPOOL:
      case SWFACTION_IF:
      case SWFACTION_STOREREGISTER:
      case SWFACTION_STACKSWAP:
      case SWFACTION_DECREMENT:
      case SWFACTION_INCREMENT:
      case SWFACTION_RANDOMNUMBER:
      case SWFACTION_DUPLICATECLIP:
      case SWFACTION_TOSTRING:
      case SWFACTION_TONUMBER:
      case SWFACTION_ORD:
      case SWFACTION_CHR:
      case SWFACTION_MBORD:
      case SWFACTION_MBCHR:
      case SWFACTION_INT:
      case SWFACTION_GETVARIABLE:
      case SWFACTION_GOTOLABEL:
      case SWFACTION_GOTOFRAME:
      case SWFACTION_TOGGLEQUALITY:
      case SWFACTION_TRY:
      case SWFACTION_THROW:
      case SWFACTION_ENDDRAG:
      case SWFACTION_STOPSOUNDS:
      case SWFACTION_STRINGLENGTH:
      case SWFACTION_MBLENGTH:
      case SWFACTION_CASTOP:
      case SWFACTION_TYPEOF:
      case SWFACTION_DELETE2:
      case SWFACTION_GETURL :
      case SWFACTION_WAITFORFRAME :
      case SWFACTION_WAITFORFRAME2 :
      case SWFACTION_CALLFRAME :
      case SWFACTION_TARGETPATH :
        return 0;

      case SWFACTION_ADD:
      case SWFACTION_ADD2:
      case SWFACTION_SUBTRACT:
      case SWFACTION_MULTIPLY:
      case SWFACTION_DIVIDE:
      case SWFACTION_MODULO:
      case SWFACTION_BITWISEAND:
      case SWFACTION_BITWISEOR:
      case SWFACTION_BITWISEXOR:
      case SWFACTION_LESSTHAN:
      case SWFACTION_LOGICALAND:
      case SWFACTION_LOGICALOR:
      case SWFACTION_STRINGEQ:
      case SWFACTION_STRINGCOMPARE:
      case SWFACTION_LESS2:
      case SWFACTION_EQUALS2:
      case SWFACTION_STRICTEQUALS:
      case SWFACTION_GREATER:
      case SWFACTION_STRINGGREATER:
      case SWFACTION_STRINGCONCAT:
      case SWFACTION_SETPROPERTY:
      case SWFACTION_REMOVECLIP:
      case SWFACTION_RETURN:
      case SWFACTION_POP:
      case SWFACTION_TRACE:
      case SWFACTION_SHIFTLEFT:
      case SWFACTION_SHIFTRIGHT:
      case SWFACTION_SHIFTRIGHT2:
      case SWFACTION_EQUAL:
      case SWFACTION_INSTANCEOF:
      case SWFACTION_ENUMERATE :
      case SWFACTION_ENUMERATE2 :
      case SWFACTION_EXTENDS :
      case SWFACTION_IMPLEMENTSOP :
      case SWFACTION_GETPROPERTY:
      case SWFACTION_SETTARGET:
      case SWFACTION_SETTARGET2:
      case SWFACTION_DEFINELOCAL2:
      case SWFACTION_INITOBJECT:
      case SWFACTION_GOTOFRAME2:
      case SWFACTION_DELETE:
        return -1;

      case SWFACTION_SETVARIABLE:
      case SWFACTION_SUBSTRING:
      case SWFACTION_MBSUBSTRING:
      case SWFACTION_GETMEMBER:
      case SWFACTION_DEFINELOCAL:
      case SWFACTION_GETURL2:
        return -2;

      case SWFACTION_STARTDRAG:
      case SWFACTION_SETMEMBER:
      case SWFACTION_NEWMETHOD:
        return -3;
              
      case SWFACTION_PUSH:
	return (actions[n]).SWF_ACTIONPUSH.NumParam;

      case SWFACTION_INITARRAY:
      case SWFACTION_NEWOBJECT:
      case SWFACTION_CALLMETHOD:
      case SWFACTION_CALLFUNCTION:
	return RESETCOUNTER_TO_1;

      case SWFACTION_PUSHDUP:
      case SWFACTION_GETTIME:
      case SWFACTION_DEFINEFUNCTION:
      case SWFACTION_DEFINEFUNCTION2:
	return 1;

      default:
       printf("/* found undef stack op %x */",(actions[n]).SWF_ACTIONRECORD.ActionCode);
	return 0;
    }
}

int stackBalance (int n, SWF_ACTION *actions)
{
  int i, s=0;
  for(i=0;i<n;i++) 
  {
   int z=stackVal(i, actions);
   s= z==RESETCOUNTER_TO_1 ? 1 : s+z;
  }
  return s;  
}


/* changed stuff moved to decompileArithmeticOp() / ak november 2006 */
int
decompileLogicalOp(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *left, *right;

    switch(actions[n].SWF_ACTIONRECORD.ActionCode)
    {
      case SWFACTION_LESSTHAN:
	      puts("LESSTHAN");
	      break;
      case SWFACTION_STRINGEQ:
	      puts("STRINGEQ");
	      break;
      case SWFACTION_STRINGCOMPARE:
	      puts("STRINGCOMPARE");
	      break;
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
      case SWFACTION_GREATER:
	      right=pop();
	      left=pop();
	      push(newVar3(getString(left),">",getString(right)));
	      break;
      case SWFACTION_LOGICALNOT:
#ifdef STATEMENT_CLASS
	      if( actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_GETVARIABLE &&
	          actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT &&
	          actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_IF ) {
		      /* It's a class statement  -- skip over both NOTs */
		      return 1;
	      }
#endif
	      if( actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_IF ) {
		      return 0;
	      }
	      push(newVar2("!",getString(pop())));
	      return 0;

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
    obj = pop();
#ifdef DEBUG
 printf("*setProp* objName %s (type=%d) Prop (type=%d) =%x\n",getName(obj),obj->Type, idx->Type,getInt(idx));
#endif
    decompilePUSHPARAM(obj,0);
    puts(".");
    puts(getProperty(getInt(idx)));
    printf(" = " );
    decompilePUSHPARAM(val,0);
    puts(";" NL);

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
decompileINCR_DECR(int n, SWF_ACTION *actions,int maxn,int is_incr)
{
    struct SWF_ACTIONPUSHPARAM *var;
    char *dblop=is_incr ? "++":"--";

    if ( actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP
      || actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP 
      || actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETVARIABLE)
    {			// NEW inc/dec code (akleine, Dec 2006)
    	var=pop();
	int is_postop=(actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP)?1:0;
	if (is_postop)
	 var = newVar2(getString(var),dblop);
	else
	 var = newVar2(dblop,getString(var));
	if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETVARIABLE)
	{
 	 var->Type=11;	/* later trigger printing variable inc/dec */
 	}
 	else
 	{
 	 var->Type=12;	/* later be quiet, see decompileSETVARIABLE() */
 	 if (is_postop)
 	 {
 	  pop();
 	  push(var);	/* will duplicate stacktop */
 	 }
 	}
	push(var);
    }
    else		// OLD inc dec code (still used for SETMEMBER etc)
    {
    INDENT
    var=pop();
    decompilePUSHPARAM(var,0);
    puts(dblop);
    puts(";" NL);
    push(var);
    if( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER) &&
        (var->Type == 4 /* Register */) &&
        (actions[n+1].SWF_ACTIONSTOREREGISTER.Register == var->p.RegisterNumber) ) {
	    regs[var->p.RegisterNumber] = var; /* Do the STOREREGISTER here */
	    return 1; /* Eat the StoreRegister that follows */
    }
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
//    puts(";" NL);
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
    struct SWF_ACTIONPUSHPARAM *obj, *mem, *var;
    char *vname, *varname,*memname;
    int len;
    
    mem=pop();
    var=pop();
    varname=getName(var);
    memname=getName(mem);
#ifdef DEBUG
 printf("*getMember* varName %s (type=%d)  memName=%s (type=%d)\n",varname,var->Type, memname,mem->Type);
#endif
    len = strlen(varname)+strlen(memname);
    if (mem->Type == 7 || mem->Type == 10)		/* INTEGER or VARIABLE */ 
    {
     vname = malloc(len+3);
     strcpy(vname,varname);
     strcat(vname,"[");
     strcat(vname,memname);
     strcat(vname,"]");
    }
    else
    {
     vname = malloc(len+2);
     strcpy(vname,varname);
     strcat(vname,".");
     strcat(vname,memname);
    } 
    obj = newVar(vname)	;
    pushvar(obj);

    return 0;
}


int
decompileSETMEMBER(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *val, *var, *obj;

    INDENT
    val = pop();
    var = pop();
    obj = pop();
#ifdef DEBUG
 printf("*SETMember* varName %s (type=%d)  objName=%s (type=%d)\n",getName(var),var->Type, getName(obj),obj->Type);
#endif

    decompilePUSHPARAM(obj,0);
    if (var->Type == 7 || var->Type == 10)		/* INTEGER or VARIABLE */
     puts("[");
    else
     puts(".");
    decompilePUSHPARAM(var,0);
    if (var->Type == 7 || var->Type == 10)
     puts("]");
    printf(" = " );
    if (val->Type != 10)			// later it will be a switch{}
     decompilePUSHPARAM(val,1);
    else
     decompilePUSHPARAM(val,0);
    puts(";" NL);

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
decompileSETVARIABLE(int n, SWF_ACTION *actions,int maxn,int islocalvar)
{
    struct SWF_ACTIONPUSHPARAM *val, *var;

    INDENT
    val = pop();
    var = pop();

    if (val->Type!=12 && islocalvar)
     puts("var ");
    switch (val->Type)
    {
     default:	puts(getName(var));
		printf(" = " );
		decompilePUSHPARAM(val,1);	// for certain types parameter 1 does not care
		puts(";" NL);
		break;
     case 10:	puts(getName(var));		// Variable (NEVER as string)
		printf(" = " );
		decompilePUSHPARAM(val,0);
		puts(";" NL);
		break;		
     case 11:	/* simply output variable and inc/dec op */
		puts(getName(val));
		puts(";" NL);
		break;
     case 12:	/* do nothing: inline increment/decrement (using side effect only) */
     		break;
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
    OUT_BEGIN2(SWF_ACTIONJUMP);
    struct SWF_ACTIONIF *sactif=NULL;
    INDENT
    if( isLogicalOp(n+1, actions, maxn) ||
        ( (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH) &&
           isLogicalOp(n+2, actions, maxn) ) ) {
	    /* Probably the start of a do {} while(), so skip it */
    	return 0;
    }

    /* Probably the end of a switch{}, so skip it */
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
	return 1;
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_JUMP) 
	if (actions[n+1].SWF_ACTIONJUMP.BranchOffset==0)
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
	  sactif = (struct SWF_ACTIONIF *)&(actions[n+i+j]);
	  /* chk whether last jump does lead us back to start of loop */
	  if (sactif->Actions[sactif->numActions-1].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP
	   && sactif->Actions[sactif->numActions-1].SWF_ACTIONJUMP.BranchOffset+
	      sactif->Actions[sactif->numActions-1].SWF_ACTIONJUMP.Offset==
	      actions[n].SWF_ACTIONRECORD.Offset )
	  {
	   break;
	  }
	  else
	   sactif=NULL;
         }
       }
      }
      if (sactif)
      {
	puts("while(");
	decompileActions(j-1, &actions[n+1+i], gIndent);
	puts(getName(pop()));
	puts("){         /* original FOR loop rewritten to WHILE */\n");
	decompileActions(sactif->numActions-1, sactif->Actions,gIndent+1);
	decompileActions(i, &actions[n+1], gIndent+1);
	INDENT
	puts("};\n");
	return i+j; 
      }
  if (sact->BranchOffset>0)
  {
   puts("break;        /*------*/\n");
  }
  else
  {
   if (sact->BranchOffset<0)
   {
    puts("continue;     /*------*/\n");
   }
  }
  /* error("Unhandled JUMP"); */
  return 0;
}

int
decompileRETURN(int n, SWF_ACTION *actions,int maxn)
{
    INDENT
    printf("return ");
    puts(getName(pop()));
    puts(";" NL);

    return 0;
}

/* 
decompileDEFINELOCAL() already MOVED TO decompileSETVARIABLE() 
*/
int
decompileDEFINELOCAL2(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *var;

    INDENT
    var = pop();
    puts("var ");
    puts(getName(var));
    puts(";" NL);

    return 0;
}

int
decompileIF(int n, SWF_ACTION *actions,int maxn)
{
    int i=0;
    OUT_BEGIN2(SWF_ACTIONIF);
struct strbufinfo origbuf;
    /*
     * IF is used in various way to implement different types
     * of loops. We try to detect these different types of loops
     * here.
     */

#ifdef STATEMENT_CLASS
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

    if( (actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT) &&
        (actions[n-2].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT) &&
        (actions[n-3].SWF_ACTIONRECORD.ActionCode == SWFACTION_GETMEMBER) &&
        (actions[n-4].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH) ) {
	    /* It's really a class definition */
            INDENT
	    puts(" {\n");
            decompileActions(sact->numActions, sact->Actions,gIndent+1);
            INDENT
	    puts("}\n");
	    return 0;
    }
#endif

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
    /* ak,2006
     * lots of "do {} while()" have simply a CONDITIONED JUMP back at the end of the loop
     */
    if( actions[n].SWF_ACTIONJUMP.BranchOffset < 0 ) 
    {
	INDENT
	puts("do {                  /* 2nd type */\n");
	decompileActions(sact->numActions, sact->Actions,gIndent+1);
	puts("} while( ");
	puts(getName(pop()));
	puts(");\n");
	return 0;
    }

    /*
     * while() loops have a JUMP at the end of the if clause that jumps backwards
       But also "continue" statements could jump backwards.
     */
    if( isLogicalOp(n-1, actions, maxn) &&
        ( (sact->Actions[sact->numActions-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_JUMP) &&
           sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset < 0) ) {
	    dumpRegs();
            INDENT
	/* if on a level >0 we can check for any outer loop 
	   To do: get the level on a better way than using gIndent */
	if (gIndent	
	  && actions[maxn].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP
	  && actions[maxn].SWF_ACTIONJUMP.Offset+actions[maxn].SWF_ACTIONJUMP.BranchOffset==
	      sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.Offset+sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset)
	{      
	    /* this jump leads from a block to start of a loop on outer block:
	       it is an 'if' later followed by last action 'continue' */
	    puts("if ( ");
	    puts(getName(pop()));
	    puts(" ) {\n");
	    decompileActions(sact->numActions, sact->Actions,gIndent+1);
	}
	else
	{
	    puts("while( ");
	    puts(getName(pop()));
	    puts(" ) {\n");
            decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
	}
	INDENT
	puts("}\n");
	return 0;
    }
#if 0
    /*
     * Any other use of IF must be a real if()
     */
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

#else	/* a.k. 2006 NEW stuff comes here */
    {
	   #define SOME_IF_DEBUG 0	/* coders only */
    	   int has_else_or_break= ((sact->Actions[sact->numActions-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_JUMP) &&
                  (sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset > 0 )) ? 1:0;
    	   int has_lognot=(actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT) ? 1:0;
	   int else_action_cnt=0,is_logor=0,is_logand=0,sbi,sbe;

	   /* before emitting any "if"/"else" characters let's check 
	      for a ternary operation  cond?a:b 
	   */
	   if (has_else_or_break)
	   {
	      int limit=actions[n+1].SWF_ACTIONRECORD.Offset+
	               sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset;
	      /* Count the number of action records that are part of
	       * the else clause, and then decompile only that many.
	       */
	      for(else_action_cnt=0;
	          actions[n+1+else_action_cnt].SWF_ACTIONRECORD.Offset < limit && else_action_cnt+n+1<maxn;
		  else_action_cnt++)
		{
		 #if SOME_IF_DEBUG
		 printf("/* ELSE OP 0x%x at %d*/\n",actions[n+1+else_action_cnt].SWF_ACTIONRECORD.ActionCode,
		 actions[n+1+else_action_cnt].SWF_ACTIONRECORD.Offset)
		 #endif
	 	 ;
		}  
	   }
	   i=else_action_cnt;						// =return value
	   sbi=stackBalance (sact->numActions-1,sact->Actions);
	   sbe=stackBalance (else_action_cnt,&actions[n+1]);
	   if (sbi==1 && sbe==1)
	   {
	     #if SOME_IF_DEBUG
	       puts("/* ****Found ternary ternary operation  \"cond ? a : b\"    **** */\n");
	       printf("If   Actions=%d\n",sact->numActions-1);
	       printf("Else Actions=%d\n",else_action_cnt);
	     #endif
	       struct strbufinfo origbuf;
	       #if USE_LIB
		origbuf=setTempString();	/* switch to a temporary string buffer */
	       #endif
	       puts(getName(pop()));
	       puts(" ? ");
	       decompileActions(else_action_cnt  , &actions[n+1],0);
	       puts(getName(pop()));
	       puts(" : ");
	       decompileActions(sact->numActions-1, sact->Actions,0);
	       puts(getName(pop()));
	       #if USE_LIB
		push (newVar(dcgetstr()));	/* push for later assignment */
		setOrigString(origbuf);		/* switch back to orig buffer */
	       #else
		push (newVar("/* ternary op: see code above */"));
	       #endif
	   } 
	   else
	   {
	   /* at this point let's check for conditioned jumps that are NOT 'if':
	      currently that is code for the locical operations  && and ||
	   */
	   if (actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP)
	     is_logor=1;
	   if (actions[n-2].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP
	    && actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT)
	     is_logand=1;
	   if (is_logor || is_logand)    
	   {
	    #if SOME_IF_DEBUG
	      printf("\n/* detected LOGICAL %s: %d actions*/\n",is_logor ? "OR":"AND",sact->numActions);
	    #endif
	    #if USE_LIB
     	      origbuf=setTempString();	/* switch to a temporary string buffer */
     	    #endif
	      puts(getName(pop()));	/* get left side of logical or */
	      puts(is_logor ? " || ":" && ");
              decompileActions(sact->numActions, sact->Actions,gIndent+1);
	      puts(getName(pop()));	/* get right side of logical or */
	    #if USE_LIB
	      push(newVar(dcgetstr()));
	      setOrigString(origbuf);	/* switch back to orig buffer */
	    #else
	      push (newVar("/* see logical term lines above */")); 
	    #endif
	      return 0;
	   }
	   /* it seems we have a found the REAL 'if' statement,
	      so it's right time to print the "if" just NOW!
	   */
	   INDENT
	   puts("if( ");
	   puts(getName(pop()));	/* the condition itself */
	   puts(" ) {\n");
           if ( has_else_or_break )
           {
	      int limit=actions[n+1].SWF_ACTIONRECORD.Offset+
	               sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset;
 	      if (! (has_lognot
 	       && actions[n-2].SWF_ACTIONRECORD.ActionCode == SWFACTION_EQUALS2 
 	       && actions[n-3].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH
 	       && actions[n-4].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP)
	       && limit > actions[maxn-1].SWF_ACTIONRECORD.Offset+actions[maxn-1].SWF_ACTIONRECORD.Length
	       )
	      {
	      /* the jump leads outside this limit, so it is a simple 'if'
	         with a 'break' at the end, there is NO else clause.
		 But as long switch(){} statements are written as sequence of 
		 if-else-if-else we have to suppress this kind of 'break'.
	       */  
		decompileActions(sact->numActions, sact->Actions,gIndent+1);
	      }
	      else
	      {
	      /* There is an else clause also! 
	         (action counter is set above)
	       */
	      if  (has_lognot)
	      {
               decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
               INDENT
	       puts("} else {\n");
	      }	      
              decompileActions(else_action_cnt  , &actions[n+1],gIndent+1);
	      if  (!has_lognot)		/* the missing if-part just NOW */
	      {
		INDENT
		puts ("} else {\n");
		decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
	      }
	     }
	    } 
	    else 
	    {
	      /* It's a simple if() {} */
              decompileActions(sact->numActions, sact->Actions,gIndent+1);
	    }
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
decompileDEFINEFUNCTION(int n, SWF_ACTION *actions,int maxn,int is_type2)
{
#if 0
// old stuff removed (find it here up to CVS release 1.57)
#else	/* 2006 NEW stuff by ak comes here:  */
    int i;
    OUT_BEGIN2(SWF_ACTIONDEFINEFUNCTION);
    struct SWF_ACTIONDEFINEFUNCTION2 *sactv2 = (struct SWF_ACTIONDEFINEFUNCTION2*)sact;
    struct strbufinfo origbuf;

    INDENT
    #ifdef DEBUG
    printf("/* function followed by OP %x */\n", actions[n+1].SWF_ACTIONRECORD.ActionCode);
    #endif
    #if USE_LIB
    if (isStoreOp(n+1, actions,maxn))
      origbuf=setTempString();	/* switch to a temporary string buffer */
    #endif
    puts("function ");
    if (is_type2)
    {
     if (sactv2->FunctionName) puts(sactv2->FunctionName);
     puts(" (");
     for(i=0;i<sactv2->NumParams;i++) {
	puts(sactv2->Params[i].ParamName);
	if ( sactv2->Params[i].Register)
	{
	 printf(" /*=R%d*/ ",sactv2->Params[i].Register);
	 regs[sactv2->Params[i].Register] = newVar(sactv2->Params[i].ParamName);
	}
	if( sactv2->NumParams > i+1 ) puts(",");
     }
     puts(") {\n");
     INDENT
     decompileActions(sactv2->numActions, sactv2->Actions,gIndent+1);
    }
    else
    {
     if (sact->FunctionName) puts(sact->FunctionName);
     puts(" (");
     for(i=0;i<sact->NumParams;i++) {
	puts(sact->Params[i]);
	if( sact->NumParams > i+1 ) puts(",");
     }
     puts(") {\n");
     INDENT
     decompileActions(sact->numActions, sact->Actions,gIndent+1);
    }
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
    return 0;
#endif
}

int
decompileCALLMETHOD(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *meth, *obj, *nparam;
/*    int i;*/

/*
Maybe check that the top of the stack holds a variable?
    SanityCheck(SWF_CALLMETHOD,
		actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH,
		"CALLMETHOD not preceeded by PUSH")
*/
    
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
     puts(";" NL);
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
     puts(";" NL);
     return 1;
    }
    return 0;
#endif
}

int
decompileSingleArgBuiltInFunctionCall(int n, SWF_ACTION *actions,int maxn,char *functionname)
{
    push(newVar_N("","",functionname,"(", 1,")"));
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     /* call function and throw away any result */
     INDENT
     puts(getName(pop()));
     puts(";" NL);
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
     puts(";" NL);
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
decompileEXTENDS(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *baseclass;

    baseclass=pop();
    printf("class ");
    puts(getName(pop()));
    printf(" extends ");
    puts(getName(baseclass));
    printf(" {\n");

    return 0;
}

int
decompileDELETE(int n, SWF_ACTION *actions,int maxn,int is_type2)
{
    if (is_type2)
     push(newVar3("delete(",getName(pop()),")"));
    else
     push(newVar_N("delete(",getName(pop()),".",getName(pop()), 0,")"));
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     /* call delete() with its args and throw away any result */
     INDENT
     puts(getName(pop()));
     puts(";" NL);
     return 1;
    }
    return 0;
}

 
int
decompileAction(int n, SWF_ACTION *actions,int maxn)
{
    if( n > maxn ) error("Action overflow!!");

#ifdef DEBUG
    fprintf(stderr,"%d:\tACTION[%3.3d]: %s\n",actions[n].SWF_ACTIONRECORD.Offset, n, actionName(actions[n].SWF_ACTIONRECORD.ActionCode));
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
        decompileGOTOFRAME(&actions[n],0);
	return 0;

      case SWFACTION_GOTOFRAME2:
        decompileGOTOFRAME(&actions[n],1);
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


      case SWFACTION_GETTIME:
        return decompileGETTIME(n, actions, maxn);
        
      case SWFACTION_TRACE:
        decompileTRACE(n, actions, maxn);
	return 0;

      case SWFACTION_EXTENDS:
        decompileEXTENDS(n, actions, maxn);
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
        decompileSETVARIABLE(n, actions, maxn,0);
	return 0;

      case SWFACTION_DEFINELOCAL:
        decompileSETVARIABLE(n, actions, maxn,1);
	return 0;

      case SWFACTION_DEFINELOCAL2:
        decompileDEFINELOCAL2(n, actions, maxn); // FIXME: perhaps move to decompileSETVARIABLE() ??
	return 0;

      case SWFACTION_DECREMENT:
        return decompileINCR_DECR(n, actions, maxn,0);

      case SWFACTION_INCREMENT:
        return decompileINCR_DECR(n, actions, maxn,1);

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
	return decompileDEFINEFUNCTION(n, actions, maxn,0);

      case SWFACTION_DEFINEFUNCTION2:
	return decompileDEFINEFUNCTION(n, actions, maxn,1);

      case SWFACTION_CALLFUNCTION:
        return decompileCALLFUNCTION(n, actions, maxn);

      case SWFACTION_CALLMETHOD:
        return decompileCALLMETHOD(n, actions, maxn);

      case SWFACTION_SHIFTLEFT:
      case SWFACTION_SHIFTRIGHT:
      case SWFACTION_SHIFTRIGHT2:        
      case SWFACTION_ADD:
      case SWFACTION_ADD2:
      case SWFACTION_SUBTRACT:
      case SWFACTION_MULTIPLY:
      case SWFACTION_DIVIDE:
      case SWFACTION_MODULO:
      case SWFACTION_BITWISEAND:
      case SWFACTION_BITWISEOR:
      case SWFACTION_BITWISEXOR:
      case SWFACTION_EQUALS2:
      case SWFACTION_LESS2:
      case SWFACTION_LOGICALAND:
      case SWFACTION_LOGICALOR:
        return decompileArithmeticOp(n, actions, maxn);

      case SWFACTION_POP:
	pop();
        return 0;

      case SWFACTION_DELETE:
        return decompileDELETE(n, actions, maxn,0);

      case SWFACTION_DELETE2:
        return decompileDELETE(n, actions, maxn,1);

      case SWFACTION_ORD:
        return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"ord");

      case SWFACTION_CHR:
        return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"chr");

      case SWFACTION_INT:
        return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"int");

      case SWFACTION_TOSTRING:
        return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"String");     

      case SWFACTION_TONUMBER:
	return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"Number");

      case SWFACTION_RANDOMNUMBER:
	return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"random");

      case SWFACTION_STRINGCONCAT:
	return decompileSTRINGCONCAT(n, actions, maxn);

      case SWFACTION_REMOVECLIP:
	return decompileREMOVECLIP(n, actions, maxn);

      case SWFACTION_DUPLICATECLIP:
	return decompileDUPLICATECLIP(n, actions, maxn);

      case SWFACTION_LESSTHAN:
      case SWFACTION_STRINGEQ:
      case SWFACTION_STRINGCOMPARE:
      case SWFACTION_STRICTEQUALS:
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

  if( Stack != NULL ) fprintf(stderr,"Stuff left on the stack at the end of a block of actions!?!?!?\n");

  return dcgetstr();
}

