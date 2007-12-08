/****************************************************************************
 *
 *  Copyright (C) 2006,2007 A.Kleine
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

#define _GNU_SOURCE

#define DEBUGSTACK
#define DECOMP_SWITCH
// #define DEBUGSWITCH

//#define STATEMENT_CLASS  
//  I have uncommented some buggy class recognition stuff in decompileIF()
//  to make work simple code lines like:  "if(!a) trace(a);"   - ak, November 2006

//  To do: add some free()-calls for allocated blocks

#include <assert.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "read.h"
#include "action.h"
#include "swftypes.h"

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

static int offseoloop;	// offset wherever a break can jump to (loops and switch)

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
  int maxsize;
  char *str;
  char *ptr;
};


struct strbufinfo setTempString(void)
{
  struct strbufinfo current;
  current.size=strsize;
  current.maxsize=strmaxsize;
  current.str=dcstr;
  current.ptr=dcptr;
  dcinit();
  return current;
}

void setOrigString(struct strbufinfo old)
{
   free(dcstr);				/* not needed anymore */
   strsize=old.size;
   strmaxsize=old.maxsize;
   dcstr=old.str;
   dcptr=old.ptr;
}

// a variant of setOrigString()
// but for further usage of 2nd buffer
//
static char *
switchToOrigString(struct strbufinfo old)
{
   char *tmp=dcstr;
   strsize=old.size;
   strmaxsize=old.maxsize;
   dcstr=old.str;
   dcptr=old.ptr;
   return tmp;
}

#if USE_LIB
#define puts(s) dcputs(s)
#define putchar(c) dcputchar(c)
#define printf dcprintf
#endif

#define INDENT { int ii=gIndent; while(--ii>=0) { putchar(' '); putchar(' '); } }

/* String used for terminating lines (see println) */
static const char* newlinestring = "\\\n";

/* Set the newline character. By default it is an escaped NL. */
void
setNewLineString(const char* ch)
{
	newlinestring = ch;
}

/* Print a line with a terminating newline, which can be set by
 * setNewLineString()
 */
static void
println(const char* fmt, ...)
{
	char *tmp;

	va_list ap;
	va_start (ap, fmt);
	vasprintf (&tmp, fmt, ap);

	dcprintf("%s%s", tmp, newlinestring);

	free(tmp);
}


/* End Package */

/*
 * Start Package 
 *
 * A package to maintain escaped characters strings
 * [ BSC == BackSlashCounter ]
 */
#define BSC 2
static int strlenext(char *str)
{
 int i=0;
 while (*str)
 {
   i++;
   if (*str=='\'') i+=BSC;
   str++;
 }
 return i;
}

static char* strcpyext(char *dest,char *src)
{
 char *r=dest;
 while (*src)
 {
  if (*src=='\'')
  {
   *dest++='\\';
#if BSC == 2
   *dest++='\\';
#endif
  }
  *dest++=*src++;
 }
 *dest='\0';
 return r;
}

static char* strcatext(char *dest,char *src)
{
 char *r=dest;
 while (*dest)
  dest++;
 strcpyext(dest,src);
 return r;
}
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
		t=malloc(strlenext(pool[act->p.Constant8])+3); /* 2 "'"s and a NULL */
		strcpy(t,"'");
		strcatext(t,pool[act->p.Constant8]);
		strcat(t,"'");
		return t;
	  case 9: /* CONSTANT16 */
		t=malloc(strlenext(pool[act->p.Constant16])+3); /* 2 '\"'s and a NULL */
		strcpy(t,"'");
		strcatext(t,pool[act->p.Constant16]);
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
		t=malloc(strlenext(pool[act->p.Constant8])+1);
		strcpyext(t,pool[act->p.Constant8]);
		if(strlen(t)) /* Not a zero length string */
		  return t;
		else
		{
		 t=realloc(t,6);
		 return strcpy(t,"_this");
  		}
	  case 9: /* CONSTANT16 */
		t=malloc(strlenext(pool[act->p.Constant16])+1);
		strcpyext(t,pool[act->p.Constant16]);
		if(strlen(t)) /* Not a zero length string */
		  return t;
		else
		{
		 t=realloc(t,6);
		 return strcpy(t,"_this");
		}
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
		return ((int)act->p.Float);
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

struct SWF_ACTIONPUSHPARAM *
newVar5(char *var,char *var2, char *var3,char *var4,char *var5)
{
	struct SWF_ACTIONPUSHPARAM *v;

	v=malloc(sizeof(struct SWF_ACTIONPUSHPARAM));
	v->Type=10; /* VARIABLE */
	v->p.String = malloc(strlen(var)+strlen(var2)+strlen(var3)+strlen(var4)+strlen(var5)+1);
	strcpy(v->p.String,var);
	strcat(v->p.String,var2);
	strcat(v->p.String,var3);
	strcat(v->p.String,var4);
	strcat(v->p.String,var5);
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
#ifdef DEBUGSTACK		/* continue w stack dummy */
	if( Stack == NULL ) push(newVar("// *** pop(): INTERNAL STACK ERROR FOUND ***"));
#else
	if( Stack == NULL ) error("Stack blown!! - pop");
#endif
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
#ifdef DEBUGSTACK		/* continue w stack dummy */
	if( Stack == NULL ) push(newVar("// *** peek(): INTERNAL STACK ERROR FOUND ***"));
#else
	if( Stack == NULL ) error("Stack blown!! - peek");
#endif
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


static struct SWF_ACTIONPUSHPARAM *
newVar_N(char *var,char *var2, char *var3,char *var4,int pop_counter,char *final)
{
	struct SWF_ACTIONPUSHPARAM *v;
	int psize=PARAM_STRSIZE;
	int i;
	int slen=strlen(var)+strlen(var2)+strlen(var3)+strlen(var4)+strlen(final);
	
	v=malloc(sizeof(struct SWF_ACTIONPUSHPARAM));
	v->p.String = malloc(psize + slen);
	v->Type=10; /* VARIABLE */
	strcpy(v->p.String,var);
	strcat(v->p.String,var2);
	strcat(v->p.String,var3);
	strcat(v->p.String,var4);
	for(i=0;i<pop_counter;i++) 
	{
	 char *pops=getString(pop());
	 while ( strlen(v->p.String)+ 2 + strlen(pops) +slen >= psize)
	 {
	  psize += PARAM_STRSIZE;
	  v->p.String = realloc( v->p.String, psize);
	 }
	 strcat(v->p.String,pops);
	 if( i < pop_counter-1 ) 
	   strcat(v->p.String,",");
	}
	strcat(v->p.String,final);
	return v;
}

// similar to newVar_N(), 
// but pops 2 items from stack per counter,
// and second of them we are interested in getName() instead of getString()
static struct SWF_ACTIONPUSHPARAM *
newVar_N2(char *var,char *var2, char *var3,char *var4,int pop_counter,char *final)
{
	struct SWF_ACTIONPUSHPARAM *v;
	int psize=PARAM_STRSIZE;
	int i;
	int slen=strlen(var)+strlen(var2)+strlen(var3)+strlen(var4)+strlen(final);
	
	v=malloc(sizeof(struct SWF_ACTIONPUSHPARAM));
	v->p.String = malloc(psize + slen);
	v->Type=10; /* VARIABLE */
	strcpy(v->p.String,var);
	strcat(v->p.String,var2);
	strcat(v->p.String,var3);
	strcat(v->p.String,var4);
	for(i=0;i<pop_counter;i++) 
	{
	 char *pops1=getString(pop());
	 char *pops2=getName  (pop());

	 while ( strlen(v->p.String)+ 3 + strlen(pops1)+ strlen(pops2) +slen >= psize)
	 {
	  psize += PARAM_STRSIZE;
	  v->p.String = realloc( v->p.String, psize);
	 }
	 strcat(v->p.String,pops2);
	 strcat(v->p.String,":");
	 strcat(v->p.String,pops1);
	 if( i < pop_counter-1 ) 
	   strcat(v->p.String,",");
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
  println("gotoLabel(%s);", sact->FrameLabel);
}


void
decompileWAITFORFRAME (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONWAITFORFRAME);

  INDENT
  println("WaitForFrame(%d,%d);", sact->Frame,sact->SkipCount);
}


void
decompileGETURL (SWF_ACTION *act)
{
  OUT_BEGIN(SWF_ACTIONGETURL);

  INDENT
  println("getUrl('%s',%s);", sact->UrlString, sact->TargetString);
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
    println(");");
    return 0;
}

static void
decompilePUSHPARAM (struct SWF_ACTIONPUSHPARAM *act, int wantstring)
{
  char *t;
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
	  case 9: /* CONSTANT16 */
	  	if( wantstring )
	  	  t=getString(act);
	  	else
	  	  t=getName(act);
	  	puts(t);  
	  	free(t);  
	  	break;

#if 0
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
#endif
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
      case SWFACTION_CASTOP:
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
   println(");");
  }
  else
  println("gotoFrame(%d);", sact->Frame+1); /* GOTOFRAME arg is 0-based */
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
//	SWFACTION_SETVARIABLE,		// TAKE CARE: array is incomplete
//	SWFACTION_TRACE,
	// missing ops are considered with low precedence
	SWFACTION_LOGICALOR,
	SWFACTION_LOGICALAND,
	SWFACTION_BITWISEOR,
	SWFACTION_BITWISEXOR,
	SWFACTION_BITWISEAND,
	SWFACTION_STRICTEQUALS,
	SWFACTION_EQUALS2,
	SWFACTION_EQUAL,

	SWFACTION_GREATER,
	SWFACTION_LESSTHAN,
	SWFACTION_LESS2,
	
	SWFACTION_SHIFTRIGHT,
	SWFACTION_SHIFTRIGHT2,
	SWFACTION_SHIFTLEFT,
	SWFACTION_ADD,
	SWFACTION_ADD2,
	SWFACTION_SUBTRACT,
	SWFACTION_MODULO,
	SWFACTION_MULTIPLY,
	SWFACTION_DIVIDE,
	SWFACTION_LOGICALNOT,
	SWFACTION_PUSH			// FIXME: need more analysis on code after PUSH

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

#ifdef DECOMP_SWITCH
static int
check_switch(int firstcode)
{
 return (firstcode == SWFACTION_PUSH || firstcode == SWFACTION_JUMP);
}
#endif

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
      case SWFACTION_INSTANCEOF:
      	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left)," instanceof ",getString(right)));
	      else
	       push(newVar_N("(",getString(left)," instanceof ",getString(right),0,")"));
	      break;
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
      case SWFACTION_EQUAL:
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

      case SWFACTION_GREATER:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),">",getString(right)));
	      else
	       push(newVar_N("(",getString(left),">",getString(right),0,")"));
	      break;
      case SWFACTION_LESSTHAN:
	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"<",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"<",getString(right),0,")"));
	      break;
      case SWFACTION_STRINGEQ:
      	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"==",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"==",getString(right),0,")"));
	      break;

      case SWFACTION_STRINGCOMPARE:
	      puts("STRINGCOMPARE");
	      break;
      case SWFACTION_STRICTEQUALS:
#ifdef DECOMP_SWITCH
	      if  (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_IF)
	      {
		if (check_switch(actions[n+1].SWF_ACTIONIF.Actions[0].SWF_ACTIONRECORD.ActionCode))
		{
		 push(right);			// keep left and right side separated
		 push(left);			// because it seems we have found a switch(){} and
	         break;				// let decompileIF() once more do all the dirty work
	        }
      	      }
#endif
	      if( actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT &&
	          actions[n+2].SWF_ACTIONRECORD.ActionCode != SWFACTION_IF ) {
      	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"!==",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"!==",getString(right),0,")"));
	        return 1;							/* due negation op */
	      } else {
      	      if (precedence(actions[n].SWF_ACTIONRECORD.ActionCode,actions[n+1].SWF_ACTIONRECORD.ActionCode))
	       push(newVar3(getString(left),"===",getString(right)));
	      else
	       push(newVar_N("(",getString(left),"===",getString(right),0,")"));
	      break;
	      }

      default:
	printf("Unhandled Arithmetic/Logic OP %x\n",actions[n].SWF_ACTIONRECORD.ActionCode);
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
      case SWFACTION_EQUAL:
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
stackVal(int n, SWF_ACTION *actions)
{
    if (!n) return 0;
    switch((actions[n-1]).SWF_ACTIONRECORD.ActionCode)
    {
      case SWFACTION_LOGICALNOT:
      case SWFACTION_DECREMENT:
      case SWFACTION_INCREMENT:
      case SWFACTION_RANDOMNUMBER:
      case SWFACTION_TOSTRING:
      case SWFACTION_TONUMBER:
      case SWFACTION_ORD:
      case SWFACTION_CHR:
      case SWFACTION_MBORD:
      case SWFACTION_MBCHR:
      case SWFACTION_INT:
      case SWFACTION_GETVARIABLE:
      case SWFACTION_SUBSTRING:
      case SWFACTION_MBSUBSTRING:
      case SWFACTION_GETMEMBER:
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
      case SWFACTION_EQUAL:
      case SWFACTION_STRICTEQUALS:
      case SWFACTION_GREATER:
      case SWFACTION_STRINGGREATER:
      case SWFACTION_STRINGCONCAT:
      case SWFACTION_SHIFTLEFT:
      case SWFACTION_SHIFTRIGHT:
      case SWFACTION_SHIFTRIGHT2:
      case SWFACTION_INSTANCEOF:
      case SWFACTION_CALLMETHOD:
      case SWFACTION_CALLFUNCTION:
      case SWFACTION_GETTIME:
      case SWFACTION_GETPROPERTY:
      case SWFACTION_PUSH:
      case SWFACTION_DELETE:
      case SWFACTION_DELETE2:
      case SWFACTION_MBLENGTH:
      case SWFACTION_STRINGLENGTH:
      case SWFACTION_CASTOP:
      case SWFACTION_TYPEOF:
      case SWFACTION_PUSHDUP:
	return 1;
      default:
	return 0;
    }
}

int
decompileLogicalNot(int n, SWF_ACTION *actions,int maxn)
{
#ifdef STATEMENT_CLASS
	      if( actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_GETVARIABLE &&
	          actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT &&
	          actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_IF ) {
		      /* It's a class statement  -- skip over both NOTs */
		      return 1;
	      }
#endif
  if( actions[n+1].SWF_ACTIONRECORD.ActionCode != SWFACTION_IF )
   push(newVar2("!",getString(pop())));
  return 0;
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
    if (obj->Type==10)
      puts("eval(");
    decompilePUSHPARAM(obj,0);
    if (obj->Type==10)
      puts(")");
    puts(".");
    puts(getProperty(getInt(idx)));
    printf(" = " );
    decompilePUSHPARAM(val,0);
    println(";");

    return 0;
}

int
decompileGETPROPERTY(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *idx, *obj;

    INDENT
    idx = pop();
    obj = pop();
#ifdef DEBUG
 printf("*GETProp* objName %s (type=%d) Prop (type=%d) =%x\n",getName(obj),obj->Type, idx->Type,getInt(idx));
#endif
    if (obj->Type==10)
     push( newVar5("eval(",getName(obj),".",getProperty(getInt(idx)),")"));
    else
     push( newVar3( getName(obj),".",getProperty(getInt(idx))));
    return 0;
}

int
decompileTRACE(int n, SWF_ACTION *actions,int maxn)
{
    INDENT
    puts("trace(");
    decompilePUSHPARAM(pop(),1);
    println(");");
    return 0;
}

int
decompileCALLFRAME(int n, SWF_ACTION *actions,int maxn)
{
    INDENT
    puts("callFrame(");
    decompilePUSHPARAM(pop(),1);
    println(");");
    return 0;
}

int
decompileGETTIME(int n, SWF_ACTION *actions,int maxn)
{
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     INDENT
     println("getTimer();");
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
    int is_postop;
    struct SWF_ACTIONPUSHPARAM *var=pop();
    char *dblop=is_incr ? "++":"--";

    if(( actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP
      || actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP 
      || actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETVARIABLE)
      ||
      ( actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_GETVARIABLE
      &&  actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER
      &&   actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETVARIABLE)
      )
    {
	is_postop=(actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP)?1:0;
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
    else
    {
	if((actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_GETMEMBER &&
	    actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER &&
	    actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETMEMBER ) ||
	   (actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER &&
	    actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETMEMBER &&
	    actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH ) ||
	   (actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH &&
	    actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETMEMBER)  ||
	   (actions[n-3].SWF_ACTIONRECORD.ActionCode == SWFACTION_GETMEMBER &&
	    actions[n-2].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH &&
	    actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_GETMEMBER &&
	    actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETMEMBER ) )
	{		// incr/decr object variables with side effects
	 is_postop= (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_SETMEMBER)?1:0;
	 if (is_postop)
	  var = newVar2(getString(var),dblop);
	 else
	  var = newVar2(dblop,getString(var));
	 if (is_postop && actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH ) pop();
	 if (is_postop && actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_GETMEMBER ) pop();
	 pop();
	 pop();
	 var->Type=12;	// to be quiet later in ...SETMEMBER()
	 regs[0]=var;	// FIXME: r0 perhaps a ming special
	 push(var);
	 push(var);
	 push(var);
	 if (is_postop && actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH ) push(var);
	 if (is_postop && actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_GETMEMBER ) push(var);
	}
	else
	{
	 if(actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH &&
	    actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER &&
	    regs[actions[n+1].SWF_ACTIONSTOREREGISTER.Register]->Type==10)
	 {
	  var = newVar2(dblop,getString(var));
	  if ((actions[n+2].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP && actions[n-1].SWF_ACTIONPUSH.NumParam==1) ||
	       actions[n+3].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
	  {
	   var->Type=11;	// later print inc/dec
	  }
	  else
	  {
	   var->Type=12;	// later be quiet in ..STOREREGISTER()
	   if (actions[n-1].SWF_ACTIONPUSH.NumParam>1) 
	   {
	    pop();
	    push(var);
	   }
	  }
	  push(var);
	 }
	 else		// fallback to old incr/decr code
	 {		// FIXME: this is bad designed for handling side effect code
	  INDENT	//        like post-incrementing a function argument etc.
	  decompilePUSHPARAM(var,0);
	  puts(dblop);
	  println(";");
	  push(var);
	 }
	}
    }
    return 0;
}

int
decompileSTOREREGISTER(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *data;
    OUT_BEGIN2(SWF_ACTIONSTOREREGISTER);
    data=peek();

    if (!regs[sact->Register] || sact->Register==0 )	// ===internal===
    {
     regs[sact->Register] = data;
    }
    else						// ===user visible level===
    {
     if ( regs[sact->Register]->Type==10)		// V7: a named function parameter in register
     {							// V7: a local var in register
      if (data->Type==12)
       data->Type=10;					// do nothing, but only once
      else
      {
       char *l=getName(regs[sact->Register]);
       char *r=getName(data);
       if (strcmp(l,r))
       {
	INDENT
	if (data->Type==11)
	 println("%s;", r);
	else
	 println("%s = %s;",l,r); 
       }
      }
     }
    }
    return 0;
}

int
decompileNEWOBJECT(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *obj, *nparam;
    obj = pop();
    nparam=pop();
    push(newVar_N("new ","",getName(obj),"(", nparam->p.Integer,")"));
    return 0;
}

int
decompileNEWMETHOD(int n, SWF_ACTION *actions,int maxn)
{
    char *t;
    struct SWF_ACTIONPUSHPARAM *meth, *nparam, *obj;
    meth = pop();
    obj = pop();
    nparam=pop();
    
    t=malloc(strlen( getName(obj) ) +2);
    strcpy(t,getName(obj));
    strcat(t,".");
    
    push(newVar_N("new ",t,getName(meth),"(", nparam->p.Integer,")"));
    free (t);
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
    if (mem->Type == 7 || mem->Type == 6 || mem->Type == 10 		/* INTEGER, DOUBLE  or VARIABLE */ 
     || mem->Type == 4 || mem->Type == 12 )
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
    val = pop();
    var = pop();
    obj = pop();
#ifdef DEBUG
 printf("*SETMember* varName %s (type=%d)  objName=%s (type=%d)\n",getName(var),var->Type, getName(obj),obj->Type);
#endif
    if (obj->Type == 12)				/* do nothing: inline inc/dec using side effect */
    {
     obj->Type = 10;					/* ...but only once */
     return 0;
    }
    INDENT
    if (obj->Type == 11)				/* simply output variable and inc/dec op */
    {
     decompilePUSHPARAM(obj,0);
     println(";");
     return 0;
    }

    decompilePUSHPARAM(obj,0);
    if (var->Type == 7 || var->Type == 6 || var->Type == 10 		/* INTEGER, DOUBLE or VARIABLE */
     || var->Type == 4 || var->Type == 12 )
     puts("[");
    else
    {
     puts(".");
     if (n && actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER)
     {
      struct SWF_ACTIONSTOREREGISTER *sactv2 = (struct SWF_ACTIONSTOREREGISTER*)&actions[n-1];
      if (sactv2->Register==0)
	regs[0]=newVar3(getName(obj),".",getName(var));		// easter 07: some sugar for mtc et al.
     }
    }
    decompilePUSHPARAM(var,0);
    if (var->Type == 7 || var->Type == 6 || var->Type == 10
     || var->Type == 4 || var->Type == 12 )
     puts("]");
    printf(" = " );
    if (val->Type != 10)			// later it will be a switch{}
     decompilePUSHPARAM(val,1);
    else
     decompilePUSHPARAM(val,0);
    println(";");

    return 0;
}

int
decompileGETVARIABLE(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *var;

    var = pop();
#ifdef DEBUG
    printf("*GETVariable* varName %s (type=%d)\n",getName(var),var->Type);
#endif
    if (var->Type == 10)
     pushvar(newVar3("eval(",getName(var),")"));
    else
     pushvar(newVar(getName(var)));

    return 0;
}

int
decompileSETVARIABLE(int n, SWF_ACTION *actions,int maxn,int islocalvar)
{
    struct SWF_ACTIONPUSHPARAM *val, *var;

    val = pop();
    var = pop();
    if (val->Type!=12)
    {
     INDENT
    }
#ifdef DEBUG
 printf("*SETVariable* varName %s (type=%d)  valName=%s (type=%d)\n",getName(var),var->Type, getName(val),val->Type);
#endif
    if (val->Type!=12 && islocalvar)
    {
     puts("var ");
    }
    if (gIndent<0)	/* the ENUM workaround:  */
    {			/* in "for (xx in yy) { }" we need xx, but nothing else */
     puts(getName(var));
     return 0;
    }


    switch (val->Type)
    {
     default:	puts(getName(var));
		printf(" = " );
		decompilePUSHPARAM(val,1);	// for certain types parameter 1 does not care
		println(";");
		break;
     case 10:	puts(getName(var));		// Variable (NEVER as string)
		printf(" = " );
		decompilePUSHPARAM(val,0);
		println(";");
		break;		
     case 11:	/* simply output variable and inc/dec op */
		puts(getName(val));
		println(";");
		break;
     case 12:	/* do nothing: inline increment/decrement (using side effect only) */
		val->Type=10;     		// but print next time  e.g. in y=++x;
     		break;
    }
    return 0;
}

int
decompileRETURN(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *var=pop();
    INDENT
    printf("return ");
    if (var->Type==4 && var->p.RegisterNumber==0)	/* REGISTER 0 used as helper variable */
     puts(getName(regs[0]));
    else
     decompilePUSHPARAM(var,1);                                                                                             
    println(";");
    return 0;
}

int
decompileJUMP(int n, SWF_ACTION *actions,int maxn)
{
    int i=0,j=0;
    int offSave;
    struct SWF_ACTIONIF *sactif;
    OUT_BEGIN2(SWF_ACTIONJUMP);
    sactif=NULL;

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
	INDENT
	puts("while(");
	decompileActions(j-1, &actions[n+1+i], gIndent);
	puts(getName(pop()));
	println("){         /* original FOR loop rewritten to WHILE */");
	offSave=offseoloop;
	if (n+i+j+1<maxn)						// see part2 above
	  offseoloop=actions[n+i+j+1].SWF_ACTIONRECORD.Offset;
	else
	  offseoloop=actions[n+i+j].SWF_ACTIONRECORD.Offset+5;
	decompileActions(sactif->numActions-1, sactif->Actions,gIndent+1);
	decompileActions(i, &actions[n+1], gIndent+1);
	offseoloop=offSave;
	INDENT
	println("};");
	return i+j; 
      }
  if (sact->BranchOffset>0)
  {
   if ( stackVal(n,actions) == 1 && n+1==maxn)
   {	// leaving block @last op with value on stack: a return x;
     return decompileRETURN(n, actions,maxn);
   }
   if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH && n+2 < maxn
    && actions[n+2].SWF_ACTIONRECORD.Offset == actions[n+1].SWF_ACTIONRECORD.Offset+sact->BranchOffset)
   {
	return 1; 	// jump to short to be a 'break': but an internal jump over a push
   }			// to do: add some control flow analysis
   INDENT
   if (offseoloop==actions[n].SWF_ACTIONRECORD.Offset+sact->BranchOffset+5)
    puts("break;" );
   else
    puts("return;" );
   println("\t\t\t// offs_end_of_loop=%d  offs_jmp_dest=%d",offseoloop, actions[n].SWF_ACTIONRECORD.Offset+sact->BranchOffset+5);
  }
  else
  {
   if (sact->BranchOffset<0)
   {
    INDENT
    println("continue;     /*------*/");
   }
  }
  /* error("Unhandled JUMP"); */
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
    println(";");

    return 0;
}

int 
decompileENUMERATE(int n, SWF_ACTION *actions,int maxn,int is_type2)
{
 int i=0;
 while (actions[n+i].SWF_ACTIONRECORD.ActionCode != SWFACTION_IF && i<maxn && i<5)
   i++;
 INDENT  
 println("/* a for-var-in  loop should follow below: */" );
 return i-1;		// preserve some code for decompileIF()... 
} 			// ... and let decompileIF() do all the dirty work ;-)


#ifdef DECOMP_SWITCH

// [recursive] estimate size of buffer needed for decompiling 'switch' 
// [ only call by decompileIF() ]
//
static int
countAllSwitchActions (union SWF_ACTION *actions,union SWF_ACTION *pre)
{
  int i,j=1;
  if (actions->SWF_ACTIONRECORD.ActionCode==SWFACTION_IF && pre->SWF_ACTIONRECORD.ActionCode==SWFACTION_STRICTEQUALS )
  {
    for(i=0; i < ((struct SWF_ACTIONIF*)actions)->numActions; i++)
    {
     j+=countAllSwitchActions(&((struct SWF_ACTIONIF*)actions)->Actions[i],pre);
     pre=&((struct SWF_ACTIONIF*)actions)->Actions[i];
    }
  }                                
  return j;
}


// [recursive] copy all actions in a 'flat' buffer by 
// unpackung all if-actions that are part of the switch operation
// [ only call by decompileIF() ]
//
static union SWF_ACTION *
getAllSwitchActions(union SWF_ACTION *dest,union SWF_ACTION *actions,union SWF_ACTION *pre)
{
#ifdef DEBUGSWITCH
   println("SWCODE: %p %d %s   %s",dest,actions->SWF_ACTIONRECORD.Offset,actionName(actions->SWF_ACTIONRECORD.ActionCode),actionName(pre->SWF_ACTIONRECORD.ActionCode));
#endif
  *dest++=*actions;
  if (actions->SWF_ACTIONRECORD.ActionCode==SWFACTION_IF 
       && pre->SWF_ACTIONRECORD.ActionCode==SWFACTION_STRICTEQUALS )
  {
    int i;
    struct SWF_ACTIONIF *sactv2 = (struct SWF_ACTIONIF*)actions;
    for(i=0; i< sactv2->numActions ;i++)
    {
     dest=getAllSwitchActions(dest,&sactv2->Actions[i],pre);
     pre=&((struct SWF_ACTIONIF*)actions)->Actions[i];
    }
  }                                
  return dest;
}

// looks similar other decompileXXXX() but 
// can't called by decompileAction()
// [ do only call by decompileIF() ]
//
static int
decompile_SWITCH(int n, SWF_ACTION *actions,int maxn,int off1end)
{
  int i,j;
  int start;						// base action index for case value and code
  int ccsize=0;						// size of code for case value
  int cvsize=0;						// size of case value
  int maxoff=0;						// action offset AFTER switch
  int n_maxoff=0;					// array index of maxoff
  int pend=0;						// control pending output
  int xsize=0;						// ret val
  int jmpsize=0;					// debug helper
  int lastoff=0;					// debug helper
  int n_firstactions=maxn;				// array index of 1st case actions code
  int lastcasestart=0;					// offs where last "case x:" begins
  char *defa="[last]";					// debug helper for early "default:" 
  char *tmp=NULL;					// helper for pending output
  struct strbufinfo origbuf;				// pending output buffer
  struct _stack *StackSave;
  struct SWF_ACTIONPUSHPARAM *swcopy,*sw=pop();
  struct SWF_ACTIONPUSHPARAM *compare=pop();
  int offSave;
  for (i=0;i<n_firstactions;i++)			// seek last op in 1st if
  {
   if (actions[i+1].SWF_ACTIONRECORD.Offset==off1end)
    {
      // println("found #off end first= %d",i+1);
      if (actions[i].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP)
      {
	maxoff=actions[i].SWF_ACTIONJUMP.BranchOffset+actions[i].SWF_ACTIONJUMP.Offset+5;
	j=1;
      }
      else
      {
       // SanityCheck(decompile_SWITCH,0,"no jump found where expected");
      }
      break;
    } 
  }
  if (!maxoff)
   for (i=maxn-1;i>=0;i--)			// seek from end of block last op of switch{}
   {
    if (actions[i].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP && !actions[i].SWF_ACTIONJUMP.BranchOffset)
    {
      maxoff=actions[i].SWF_ACTIONRECORD.Offset+5;
      j=2;
      break;
    } 
   }
  for (i=0;i<maxn;i++)
  {
   if (actions[i].SWF_ACTIONRECORD.Offset>=maxoff)
   {
     n_maxoff=i;					// part of block is switch
     break;
   }
  }
  if (!n_maxoff) n_maxoff=maxn;			// whole block is switch

  INDENT
  println("switch( %s ) {			// end switch at %d (index %d) / found via meth %d)",getString(sw), maxoff,n_maxoff,j);
  push(sw);
  push(compare);
  
  i=1;
  do 							// here we go into main loop
  {
   if((actions[i].SWF_ACTIONRECORD.ActionCode==SWFACTION_IF
    && actions[i-1].SWF_ACTIONRECORD.ActionCode==SWFACTION_STRICTEQUALS )
    ||(actions[i].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP
    && actions[i-1].SWF_ACTIONRECORD.ActionCode==SWFACTION_IF) )
   {
     start=i;
     while (actions[start].SWF_ACTIONRECORD.Offset < actions[i].SWF_ACTIONRECORD.Offset+5+actions[i].SWF_ACTIONJUMP.BranchOffset)
     {
      start++;						// count actions until start of "case x:"
     }
     if (n_firstactions==maxn)				// if not done store earliest "case x: "actions
     {
     	n_firstactions=start;				// same as array index
     }

     for (ccsize=0; ccsize+start<n_maxoff; ccsize++)	// count actions belonging to "case x:"
     {
#ifdef DEBUGSWITCH
       println("in ccsize: ccsize=%d  off=%d %s",ccsize,actions[ccsize+start].SWF_ACTIONRECORD.Offset,actionName(actions[ccsize+start].SWF_ACTIONRECORD.ActionCode));
#endif
       if (actions[ccsize+start].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP)
       {
         if (maxoff == actions[ccsize+start].SWF_ACTIONJUMP.Offset+5 + actions[ccsize+start].SWF_ACTIONJUMP.BranchOffset)
         {
           jmpsize= actions[ccsize+start].SWF_ACTIONJUMP.BranchOffset;
           lastoff= actions[ccsize+start].SWF_ACTIONJUMP.Offset;
           ccsize++; // the jmp itself
           break;
         }
       }
     }

#if USE_LIB
     if (tmp && (start!=pend))				// output pending buffer if neccessary
     {
       puts(tmp);
     }
     if (tmp)
     {
       free(tmp);
       tmp=NULL;
     }
     pend=start;
#endif

     if (actions[i].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP)
     {
      if (ccsize<=1)
       break;						// ready
      else
      {
       INDENT
       if (actions[start].SWF_ACTIONRECORD.Offset>lastcasestart)
	xsize+=ccsize;        
       else
	defa="[early]";
       println("default:			// at %d %s start=%d ccsize=%d",actions[start].SWF_ACTIONRECORD.Offset,defa, start, ccsize);
      }
     }
     else
     {
      INDENT
      xsize=ccsize;
      lastcasestart=actions[start].SWF_ACTIONRECORD.Offset;
      println("case %s:			// at %d  start=%d ccsize=%d jmp=%d+%d+5",getString(pop()), lastcasestart,start,ccsize,lastoff,jmpsize);
      swcopy=pop();
      //   SanityCheck(decompile_SWITCH,!strcmp(getName(swcopy),getName(sw)),"sw0 != sw");
     }

#if USE_LIB
    origbuf=setTempString();						// switch to temp buffer
#endif
    StackSave=Stack;
    offSave=offseoloop;
    offseoloop=maxoff;
    decompileActions( ccsize, &actions[start],gIndent+1);
    offseoloop=offSave;
    Stack=StackSave;
#if USE_LIB
    tmp=switchToOrigString(origbuf);
#endif

    if (actions[i].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP)		// after "default:"
    {
      break;     							// ready
    }
    else
     if (actions[i+1].SWF_ACTIONRECORD.ActionCode!=SWFACTION_JUMP) 	// not before "default:" or end
     {
      i++; // the 'if' itself
      cvsize=0;
      while (i+cvsize < n_firstactions && actions[i+cvsize].SWF_ACTIONRECORD.ActionCode!=SWFACTION_STRICTEQUALS)
      {
#ifdef DEBUGSWITCH
       println("in  cvsize=%d  %d %s",cvsize,actions[i+cvsize].SWF_ACTIONRECORD.Offset,actionName(actions[i+cvsize].SWF_ACTIONRECORD.ActionCode));
#endif
       cvsize++;							// count "case X:" code size
      }
      decompileActions( cvsize, &actions[i],gIndent+1);			// at least one push on stack expected
      i+=cvsize;
     }
   }
  } while (++i < n_firstactions);

#if USE_LIB
  if (tmp)
  {
   puts(tmp);								// print last pending output
   free(tmp);
  }
#endif	
  INDENT
  println("}					// switch ret value =%d",xsize);
  return xsize;
}
#endif

int
decompileIF(int n, SWF_ACTION *actions,int maxn)
{
    int offSave;
    int j,i=0;
    struct strbufinfo origbuf;
    OUT_BEGIN2(SWF_ACTIONIF);
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
	    println(" {" );
            decompileActions(sact->numActions, sact->Actions,gIndent+1);
            INDENT
	    println("}");
	    return 0;
    }

    if( (actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT) &&
        (actions[n-2].SWF_ACTIONRECORD.ActionCode == SWFACTION_LOGICALNOT) &&
        (actions[n-3].SWF_ACTIONRECORD.ActionCode == SWFACTION_GETMEMBER) &&
        (actions[n-4].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH) ) {
	    /* It's really a class definition */
            INDENT
	    println(" {");
            decompileActions(sact->numActions, sact->Actions,gIndent+1);
            INDENT
	    println("}");
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
	    println("do {");
	    offSave=offseoloop;
	    offseoloop=actions[n].SWF_ACTIONRECORD.Offset+5;
            decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
            offseoloop=offSave;
	    INDENT
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
	println("do {                  /* 2nd type */ ");
	offSave=offseoloop;
	offseoloop=actions[n  ].SWF_ACTIONRECORD.Offset+5;
	decompileActions(sact->numActions, sact->Actions,gIndent+1);
	offseoloop=offSave;
	INDENT
	puts("} while( ");
	puts(getName(pop()));
	println(");");
	return 0;
    }

    j=0;
    while (actions[n-j].SWF_ACTIONRECORD.ActionCode != SWFACTION_ENUMERATE  && 
	        actions[n-j].SWF_ACTIONRECORD.ActionCode != SWFACTION_ENUMERATE2 && j<n && j<5) 
     j++;		// check for a pending ENUMERATE
     if ((actions[n-j].SWF_ACTIONRECORD.ActionCode == SWFACTION_ENUMERATE ||
	  actions[n-j].SWF_ACTIONRECORD.ActionCode == SWFACTION_ENUMERATE2 ) && 
	  actions[n-j+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER )
     {
	struct SWF_ACTIONPUSHPARAM *var;
	int x;
	var = pop();
	INDENT
	puts("for ( ");
	// check for an usual special case w register Rx
	if (sact->Actions[1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STOREREGISTER)
	{
	  struct SWF_ACTIONSTOREREGISTER *sactv2 = (struct SWF_ACTIONSTOREREGISTER*)&sact->Actions[1];
	  puts("var ");
	  puts(getName(regs[sactv2->Register]));
	  x=3;
	}
	else
	{
	  decompileActions( 2 , sact->Actions,-1);   /* -1 == the ENUM workaround */
	  x=2;
	}
	puts(" in ");
	puts(getName(var));
	println(" ) {");
	offSave=offseoloop;
	offseoloop=actions[n+1].SWF_ACTIONRECORD.Offset;
        decompileActions(sact->numActions-1-x, &sact->Actions[x],gIndent+1);
        offseoloop=offSave;
	INDENT
	println("}");
	return 0;
     }

    /*
     * while() loops have a JUMP at the end of the if clause that jumps backwards
       But also "continue" statements could jump backwards.
     */
    if( isLogicalOp(n-1, actions, maxn) &&
        ( (sact->Actions[sact->numActions-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_JUMP) &&
           sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset < 0) ) {
if(0)	    dumpRegs();
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
	   println(" ) {");
	   decompileActions(sact->numActions, sact->Actions,gIndent+1);
	}
        else	/* while(){}  as usual */
	{
	   puts("while( ");
	   puts(getName(pop()));
	   println(" ) {");
	   offSave=offseoloop;
	   offseoloop=actions[n+1].SWF_ACTIONRECORD.Offset;
	   decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
	   offseoloop=offSave;
	}
	INDENT
	println("}");
	return 0;
    }
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
		 println("/* ELSE OP 0x%x at %d*/",actions[n+1+else_action_cnt].SWF_ACTIONRECORD.ActionCode,
		 actions[n+1+else_action_cnt].SWF_ACTIONRECORD.Offset)
		 #endif
	 	 ;
		}  
	   }
	   i=else_action_cnt;						// =return value
	   sbi=stackVal (sact->numActions-1,sact->Actions);
	   sbe=stackVal (else_action_cnt,&actions[n+1]);

	   // check against opcodes we do not expect in a ternary operation
	   if (sbi==1 && sbe==1)
	   {
	     for (j=0;j<sact->numActions-1;j++)
	     {
	 	if (sact->Actions[j].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP) // perhaps more ops
		{
		  sbi=i=has_else_or_break=0;
		  break;
		}
	     }
	     for (j=0;j<else_action_cnt;j++)
	     {
		if (actions[n+j].SWF_ACTIONRECORD.ActionCode==SWFACTION_JUMP) // perhaps more ops
		{
		  sbe=i=has_else_or_break=0;
		  break;
		}
	     }
	   }
	   #if SOME_IF_DEBUG
	   printf("sbi=%d   sbe=%d\n", sbi,sbe);
	   #endif
	   if (sbi==1 && sbe==1)
	   {
	     #if SOME_IF_DEBUG
	       println("/* ****Found ternary ternary operation  \"cond ? a : b\"    **** */");
	       printf("If   Actions=%d\n",sact->numActions-1);
	       printf("Else Actions=%d\n",else_action_cnt);
	     #endif
	       struct strbufinfo origbuf;
	       #if USE_LIB
		origbuf=setTempString();	/* switch to a temporary string buffer */
	       #endif
	       puts("(");
	       puts(getName(pop()));
	       puts(" ? ");
	       decompileActions(else_action_cnt  , &actions[n+1],0);
	       puts(getName(pop()));
	       puts(" : ");
	       decompileActions(sact->numActions-1, sact->Actions,0);
	       puts(getName(pop()));
	       puts(")");
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
	      println("");
	      println("/* detected LOGICAL %s: %d actions*/", is_logor ? "OR":"AND",sact->numActions);
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
#ifdef DECOMP_SWITCH
 	   if ( actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_STRICTEQUALS
 	        && check_switch(sact->Actions[0].SWF_ACTIONRECORD.ActionCode) )
 	   {
 	    union SWF_ACTION *xact,*xact0;
 	    for(i=n-1,j=0; i< maxn ;i++)	// n-1 due adding 1st SWFACTION_STRICTEQUALS in buffer	
 	    {
 	    	j+=countAllSwitchActions(&actions[i],&actions[i-1]); 		// FIRST count size of code
	    }
	    xact0=xact = (union SWF_ACTION *) calloc (j,sizeof (SWF_ACTION));
	    INDENT
	    println("// checking %d actions for switch(){}",j);
	    for(i=n-1; i< maxn ;i++)
	    {
		xact=getAllSwitchActions(xact,&actions[i],&actions[i-1]);	// SECOND copy into xtra buffer
	    }
	    j=decompile_SWITCH(0,xact0,j,actions[n+1].SWF_ACTIONRECORD.Offset);	// THIRD decompile xtra buffer
	    free(xact0);
	    return j;
	   }
#endif
	   /* it seems we have a found the REAL 'if' statement,
	      so it's right time to print the "if" just NOW!
	   */
	   INDENT
	   puts("if( ");
	   puts(getName(pop()));	/* the condition itself */
	   println(" ) {");
           if ( has_else_or_break )
           {
	      int limit=actions[n+1].SWF_ACTIONRECORD.Offset+
	               sact->Actions[sact->numActions-1].SWF_ACTIONJUMP.BranchOffset;
	      // limit == dest of jmp == offset next op after 'if' + jumpdist at end of 'if'
 	      int lastopsize=actions[maxn-1].SWF_ACTIONRECORD.Length;
	      if (actions[maxn-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_IF)
		 lastopsize+=actions[maxn-1].SWF_ACTIONIF.BranchOffset  
		 +3; /* +3 see parser.c: "Action + Length bytes not included in the length" */
 	      if (offseoloop 
 	       &&  ! (has_lognot
 	       && actions[n-2].SWF_ACTIONRECORD.ActionCode == SWFACTION_EQUALS2 
 	       && actions[n-3].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH
 	       && actions[n-4].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSHDUP
 		)
	       && limit > actions[maxn-1].SWF_ACTIONRECORD.Offset+lastopsize
	       )
	      {
	       /* the jump leads outside this limit, so it is a simple 'if'
	          with a 'break' or 'return' at the end, and there is NO else clause.
	       */  
		INDENT
		println("// offs_endjump_dest=%d  offs_after_blk %d",limit,actions[maxn-1].SWF_ACTIONRECORD.Offset+lastopsize);
		decompileActions(sact->numActions, sact->Actions,gIndent+1);
		i=0;			/* found break/return but no else and thus return 0 */
	      }
	      else
	      {
	      /* There is an else clause also! 
	         (action counter is set above)
	       */
	      struct _stack *StackSave=Stack;	/* decompile if and else blocks at same stack base */
	      if  (has_lognot)
	      {
               decompileActions(sact->numActions-1, sact->Actions,gIndent+1);
               INDENT
	       println("} else {");
	      }	      
	      Stack=StackSave;
              decompileActions(else_action_cnt  , &actions[n+1],gIndent+1);
	      if  (!has_lognot)		/* the missing if-part just NOW */
	      {
		Stack=StackSave;
		INDENT
		println ("} else {" );
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
	    println("}");
	 }
	return i;
    }
    return 0;
}

int
decompileINITOBJECT(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *nparam;
    nparam=pop();
    push(newVar_N2("","","","{", nparam->p.Integer,"}"));
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
    println(" {" );
    decompileActions(sact->numActions, sact->Actions,gIndent+1);
    INDENT
    println("}" );

    return 1;
}

int
decompileTRY(int n, SWF_ACTION *actions,int maxn)
{
#ifdef DEBUG
    struct _stack *StackSave=Stack;
#endif    
    OUT_BEGIN2(SWF_ACTIONTRY);
    INDENT
    println("try {");
    decompileActions(sact->numTryActs, sact->TryActs,gIndent+1);
    INDENT
    println("}");
#ifdef DEBUG
    if (Stack!=StackSave)
    {
      println("/* Stack problem in try{} code above */");
      Stack=StackSave;
    }
#endif
    if (sact->numCatchActs)    
    {
     struct SWF_ACTIONPUSHPARAM *rsave=NULL;
     INDENT
     if( ! sact->CatchInRegisterFlag)
      println("catch (%s) {",sact->CatchName);
     else
     {
      char *t=malloc(5); /* Rddd */
      sprintf(t,"R%d", sact->CatchRegister );
      rsave=regs[sact->CatchRegister];
      regs[sact->CatchRegister] = newVar(t);
      println("catch (%s) {",t);
     }
     decompileActions(sact->numCatchActs, sact->CatchActs,gIndent+1);
     INDENT
     println("}");
     if (rsave)
      regs[sact->CatchRegister]=rsave;
#ifdef DEBUG
     if (Stack!=StackSave)
     {
      println("/* Stack problem in catch{} code above */");
      Stack=StackSave;
     }
#endif
    } 
    if (sact->numFinallyActs)
    {
     INDENT
     println("finally () {");
     decompileActions(sact->numFinallyActs, sact->FinallyActs,gIndent+1);
     INDENT
     println("}");
#ifdef DEBUG
     if (Stack!=StackSave)
     {
      println("/* Stack problem in finally{} code above */");
      Stack=StackSave;
     }
#endif
    }
    return 0;
}


int
decompileDEFINEFUNCTION(int n, SWF_ACTION *actions,int maxn,int is_type2)
{
    int i,j,k,m,r;
    struct SWF_ACTIONPUSHPARAM *myregs[ 256 ];
    struct _stack *StackSave;    
    struct SWF_ACTIONDEFINEFUNCTION2 *sactv2;
    struct strbufinfo origbuf;
    OUT_BEGIN2(SWF_ACTIONDEFINEFUNCTION);
    sactv2 = (struct SWF_ACTIONDEFINEFUNCTION2*)sact;

    #ifdef DEBUG
    println("/* function followed by OP %x */", actions[n+1].SWF_ACTIONRECORD.ActionCode);
    #endif
    #if USE_LIB
    if (isStoreOp(n+1, actions,maxn) 
       || (  *sact->FunctionName==0 && !is_type2 )
       || (*sactv2->FunctionName==0 && is_type2  ))
      origbuf=setTempString();	/* switch to a temporary string buffer */
    #endif
    puts("function ");
    if (is_type2)
    {
     for(j=1;j<sactv2->RegisterCount;j++) 
     {
      myregs[j]=regs[j];
      regs[j]=NULL;
     }
     r=1;
     if (sactv2->PreloadThisFlag)	regs[r++]=newVar("this");
     if (sactv2->PreloadArgumentsFlag)	regs[r++]=newVar("arguments");
     if (sactv2->PreloadSuperFlag)	regs[r++]=newVar("super");
     if (sactv2->PreloadRootFlag)	regs[r++]=newVar("root");
     if (sactv2->PreloadParentFlag)	regs[r++]=newVar("parent");
     if (sactv2->PreloadGlobalFlag)	regs[r++]=newVar("global");

     puts(sactv2->FunctionName);
     puts("(");
     
     for(i=0,m=0;i<sactv2->NumParams;i++) 
     {
	puts(sactv2->Params[i].ParamName);
	if ( sactv2->Params[i].Register)
	{
	 printf(" /*=R%d*/ ",sactv2->Params[i].Register);
	 regs[sactv2->Params[i].Register] = newVar(sactv2->Params[i].ParamName);
	 m++;					// do not count 'void' etc
	}
	if( sactv2->NumParams > i+1 ) puts(",");
     }
     println(") {" );
     if (r+m < sactv2->RegisterCount)
     {
       INDENT
       puts("  var ");
     }
     for(k=r;r<sactv2->RegisterCount;r++)
     {
       if (!regs[r])
       {
        char *t=malloc(5); /* Rddd */
  	sprintf(t,"R%d", r );
  	puts (t);
  	if (k++ < sactv2->RegisterCount- m -1)
  	 puts(", ");
  	else
  	 println(";" );
	regs[r]=newVar(t);
       }
     }
     StackSave=Stack;
     decompileActions(sactv2->numActions, sactv2->Actions,gIndent+1);
#ifdef DEBUG
     if (Stack!=StackSave)
     {
      println("/* Stack problem in function code above */");
     }
#endif
     Stack=StackSave;
     for(j=1;j<sactv2->RegisterCount;j++) regs[j]=myregs[j];
    }
    else
    {
     puts(sact->FunctionName);
     puts("(");
     for(i=0;i<sact->NumParams;i++) {
	puts(sact->Params[i]);
	if( sact->NumParams > i+1 ) puts(",");
     }
     println(") {" );
     k=0;
     if (sact->Actions[0].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH)
     {
      struct SWF_ACTIONPUSH *sactPush=(struct SWF_ACTIONPUSH *)sact->Actions;
      for(i=0;i<sactPush->NumParam;i++)
      {
       if ((&(sactPush->Params[i]))->Type==4) k++;	/* REGISTER */
      }
      if (k)
      {
       INDENT
       puts("  var ");
       for(i=1;i<=k;i++)
       {
	char *t=malloc(5); /* Rddd */
	sprintf(t,"R%d", i );
	puts (t);
	if (i < k)
	 puts(", ");
	else
	 println(";" );
	regs[i]=newVar(t);
       }
      }
     }
     for(j=1;j<=k;j++) myregs[j]=regs[j];
     StackSave=Stack;
     decompileActions(sact->numActions, sact->Actions,gIndent+1);
#ifdef DEBUG
     if (Stack!=StackSave)
     {
      println("/* Stack problem in function code above */");
     }
#endif
     Stack=StackSave;
     for(j=1;j<=k;j++) regs[j]=myregs[j];
    }
    INDENT
    if (isStoreOp(n+1, actions,maxn) 
       || ( *sact->FunctionName==0 && !is_type2 )
       || (*sactv2->FunctionName==0 && is_type2 ))
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
     println("}" );
    return 0;
}

int
decompileCALLMETHOD(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *meth, *obj, *nparam;
    meth=pop();
    obj=pop();
    nparam=pop();
    if (nparam->p.Integer>25)
    {
     INDENT
     println("// Problem getting method arguments (%d ignored) below:",nparam->p.Integer);
     nparam->p.Integer=0;
    }
    push(newVar_N(getName(obj),".",getName(meth),"(", nparam->p.Integer,")"));
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     /* call method and throw away any result */
     INDENT
     puts(getName(pop()));
     println(";" );
     return 1;
    }
    return 0;
}

int
decompileCALLFUNCTION(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *meth, *nparam;

    SanityCheck(SWF_CALLMETHOD,
		actions[n-1].SWF_ACTIONRECORD.ActionCode == SWFACTION_PUSH,
		"CALLMETHOD not preceeded by PUSH")
    
    meth=pop();
    nparam=pop();
    if (nparam->p.Integer>25)
    {
     INDENT
     println("// Problem getting function arguments (%d ignored) below:",nparam->p.Integer);
     nparam->p.Integer=0;
    }
    push(newVar_N("","",getName(meth),"(", nparam->p.Integer,")"));
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     /* call function and throw away any result */
     INDENT
     puts(getName(pop()));
     println(";" );
     return 1;
    }
    return 0;
}

int
decompile_Null_ArgBuiltInFunctionCall(int n, SWF_ACTION *actions,int maxn,char *functionname)
{
    INDENT
    puts(functionname);		// only used for cases w/o return value
    println("();" );
    return 0;
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
     println(";" );
     return 1;
    }
    return 0;
}

int
decompileSTARTDRAG(int n, SWF_ACTION *actions,int maxn)
{
    INDENT
    puts("startDrag(");
    decompilePUSHPARAM(pop(),1);
    puts(",");
    decompilePUSHPARAM(pop(),0);
    puts(",");
    decompilePUSHPARAM(pop(),0);	//
    println(");" );
    return 0;
}

int
decompileSUBSTRING(int n, SWF_ACTION *actions,int maxn)
{
    push(newVar_N("","","substr","(", 3,")"));
    if (actions[n+1].SWF_ACTIONRECORD.ActionCode == SWFACTION_POP)
    {
     /* call function and throw away any result */
     INDENT
     puts(getName(pop()));
     println(";" );
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
     println(";" );
     return 1;
    }
    return 0;
}

int
decompileTHROW(int n, SWF_ACTION *actions,int maxn)
{
    INDENT
    puts("throw ");
    puts(getName(pop()));
    println(";");
    return 0;
}

int
decompileREMOVECLIP(int n, SWF_ACTION *actions,int maxn)
{
    INDENT
    puts("removeMovieClip(");
    puts(getName(pop()));
    println(");" );
    return 0;
}

int 
decompileDUPLICATECLIP(int n, SWF_ACTION *actions,int maxn)
{
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
    println(");" );
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
    println(" {" );

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
     println(";" );
     return 1;
    }
    return 0;
}

int
decompileSETTARGET(int n, SWF_ACTION *actions,int maxn,int is_type2)
{
    int action_cnt=0;
    char *name;
    OUT_BEGIN2(SWF_ACTIONSETTARGET);
    name = is_type2 ? getString(pop()) : sact->TargetName;
    if (*name)
    {
     INDENT
     println("tellTarget('%s') {" ,name);
     while(action_cnt+n<maxn)
     {
	if (actions[n+1+action_cnt].SWF_ACTIONRECORD.ActionCode==SWFACTION_SETTARGET
	 || actions[n+1+action_cnt].SWF_ACTIONRECORD.ActionCode==SWFACTION_SETTARGET2
	 || actions[n+1+action_cnt].SWF_ACTIONRECORD.ActionCode==SWFACTION_END) 
	  break;
	action_cnt++;
     }
     decompileActions(action_cnt,&actions[n+1],gIndent+1);
     INDENT
     println("}" );
    }
    return action_cnt;
}

int
decompileIMPLEMENTS(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *nparam;
    int i;
    INDENT;
    puts(getName(pop()));
    printf(" implements ");
    nparam=pop();
    for(i=0;i<nparam->p.Integer;i++) 
    {
     puts(getName(pop()));
    }
    println(" ;");
    return 0;
}

int
decompileCAST(int n, SWF_ACTION *actions,int maxn)
{
    struct SWF_ACTIONPUSHPARAM *iparam=pop();
    struct SWF_ACTIONPUSHPARAM *tparam=pop();
    push(newVar_N( getName(tparam),"(",getName(iparam),"", 0,")")); 
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

      case SWFACTION_GETPROPERTY:
        decompileGETPROPERTY(n, actions, maxn);
	return 0;

      case SWFACTION_GETTIME:
        return decompileGETTIME(n, actions, maxn);
        
      case SWFACTION_TRACE:
        decompileTRACE(n, actions, maxn);
	return 0;

      case SWFACTION_CALLFRAME:
        decompileCALLFRAME(n, actions, maxn);
        return 0;

      case SWFACTION_EXTENDS:
        decompileEXTENDS(n, actions, maxn);
	return 0;

      case SWFACTION_INITOBJECT:
        decompileINITOBJECT(n, actions, maxn);
	return 0;	        

      case SWFACTION_NEWOBJECT:
        decompileNEWOBJECT(n, actions, maxn);
	return 0;

      case SWFACTION_NEWMETHOD:
        decompileNEWMETHOD(n, actions, maxn);
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
        decompileDEFINELOCAL2(n, actions, maxn);
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

      case SWFACTION_LOGICALNOT:
        return decompileLogicalNot(n, actions, maxn);

      case SWFACTION_IF:
        return decompileIF(n, actions, maxn);

      case SWFACTION_WITH:
        decompileWITH(n, actions, maxn);
	return 0;

      case SWFACTION_ENUMERATE:
        return decompileENUMERATE(n, actions, maxn,0);

      case SWFACTION_ENUMERATE2 :
        return decompileENUMERATE(n, actions, maxn,1);

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

      case SWFACTION_INSTANCEOF:
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
      case SWFACTION_EQUAL:
      case SWFACTION_EQUALS2:
      case SWFACTION_LESS2:
      case SWFACTION_LOGICALAND:
      case SWFACTION_LOGICALOR:
      case SWFACTION_GREATER:
      case SWFACTION_LESSTHAN:
      case SWFACTION_STRINGEQ:
      case SWFACTION_STRINGCOMPARE:
      case SWFACTION_STRICTEQUALS:
        return decompileArithmeticOp(n, actions, maxn);

      case SWFACTION_POP:
	pop();
        return 0;

      case SWFACTION_STARTDRAG:
	return decompileSTARTDRAG(n, actions, maxn);

      case SWFACTION_DELETE:
        return decompileDELETE(n, actions, maxn,0);

      case SWFACTION_DELETE2:
        return decompileDELETE(n, actions, maxn,1);

      case SWFACTION_TARGETPATH:
        return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"targetPath");

      case SWFACTION_TYPEOF:
        return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"typeof");

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

      case SWFACTION_STRINGLENGTH:
      	return decompileSingleArgBuiltInFunctionCall(n, actions, maxn,"length");

      case SWFACTION_PLAY:
	return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"play");

      case SWFACTION_STOP:
	return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"stop");

      case SWFACTION_NEXTFRAME:
	return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"nextFrame");

      case SWFACTION_PREVFRAME:
	return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"prevFrame");

      case SWFACTION_ENDDRAG:
	return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"stopDrag");

      case SWFACTION_STOPSOUNDS:
	return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"stopAllSounds");      

      case SWFACTION_TOGGLEQUALITY:
	return decompile_Null_ArgBuiltInFunctionCall(n, actions, maxn,"toggleHighQuality");      

      case SWFACTION_MBSUBSTRING:
      case SWFACTION_SUBSTRING:
      	return decompileSUBSTRING(n, actions, maxn);

      case SWFACTION_STRINGCONCAT:
	return decompileSTRINGCONCAT(n, actions, maxn);

      case SWFACTION_REMOVECLIP:
	return decompileREMOVECLIP(n, actions, maxn);

      case SWFACTION_DUPLICATECLIP:
	return decompileDUPLICATECLIP(n, actions, maxn);

      case SWFACTION_SETTARGET:
        return decompileSETTARGET(n, actions, maxn,0);

      case SWFACTION_SETTARGET2:
        return decompileSETTARGET(n, actions, maxn,1);

      case SWFACTION_IMPLEMENTSOP:
	return decompileIMPLEMENTS(n, actions, maxn);

      case SWFACTION_CASTOP:
	return decompileCAST(n, actions, maxn);

      case SWFACTION_THROW:
	return decompileTHROW(n, actions, maxn);

      case SWFACTION_TRY:
	return decompileTRY(n, actions, maxn);

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
int j;
  if( !n  )
	  return NULL;

  pool = NULL;

  dcinit();

     for(j=0;j<256;j++) regs[j]=0;
     regs[1] = newVar("R1");
     regs[2] = newVar("R2");
     regs[3] = newVar("R3");
     regs[4] = newVar("R4");

  decompileActions(n, actions, indent);
#ifdef DEBUGSTACK
  if( Stack != NULL && *dcstr) 
  { 
   int i=0;
   println("/* -----------------------------------------------------------------");
   println("NOTE: some stuff left on the stack at the end of a block of actions:");
   while (Stack)
   {
    i++;
    printf("%d.:\t%s",i, getString(pop()));
    println("");
   }
   println("*/");
  }
#else
  if( Stack != NULL ) fprintf(stderr,"Stuff left on the stack at the end of a block of actions!?!?!?\n");
  while (Stack)
  {
    pop();
  }
#endif
  return dcgetstr();
}

