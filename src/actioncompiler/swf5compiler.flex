%{

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "compile.h"
#include "actiontypes.h"
#include "blocks/error.h"
#include "swf5compiler.tab.h" /* defines token types */

static int swf5debug;

static const char *lexBuffer = NULL;
static int lexBufferLen = 0;

static int  sLineNumber = 0, realLine = 0;
static char msgbufs[2][1024] = { {0}, {0} }, *msgline = {0};
static int  column = 0, realColumn = 0, lastToken = 0;

static void comment();
static void comment1();
static void count();
static void countline();
static void warning(char *msg);
static int yy_first_time = 1;
static int yy_new_state;

#define YY_INPUT(buf,result,max_size) result=lexBufferInput(buf, max_size)
#define YY_NO_UNISTD_H
#define YY_USE_PROTOS

/* thanks to the prolific and brilliant Raff: */
static int lexBufferInput(char *buf, int max_size)
{
  int l = lexBufferLen > max_size ? max_size : lexBufferLen;

  if (lexBufferLen <= 0)
    return YY_NULL;

  memcpy(buf, lexBuffer, l);
  lexBuffer += l;
  lexBufferLen -= l;
  return l;
}

static void unescape(char *buf)
{
  char *r, *w;

  r=buf; // read
  w=buf; // write
  while (*r)
  {
	if ( *r == '\\' )
	{
		r++;
		switch(*r)
		{
			case 'b' : *w = '\b'; break;
			case 'f' : *w = '\f'; break;
			case 'n' : *w = '\n'; break;
			case 'r' : *w = '\r'; break;
			case 't' : *w = '\t'; break;
			case 'x' :
			case 'u' : fprintf(stderr,"unsupported escape sequence\n");
				break;
			default : *w = *r; break;
		}
		w++;
		r++;
	}
	else
	{
		*w++ = *r++;
	}
  }
  *w='\0';
}

void swf5ParseInit(const char *script, int debug, int version)
{
  yyrestart(NULL);

  swf5debug = debug;

  lexBuffer = script;
  lexBufferLen = strlen(script);
  sLineNumber = 0;
  column = 0;
  msgline = msgbufs[0];
  swfVersion = version;
}

int read_int (const char *text, YYSTYPE *num)
{
  unsigned long i;

  i = strtoul (text, NULL, 0);
  if (i > 0x7FFFFFFF) {
    /* note: this catches ERANGE, too */
    num->doubleVal = atof(yytext);
    return DOUBLE;
  }
  num->intVal = i;
  return INTEGER;
}

%}
%option never-interactive
%option stack

%x STATE_ASM
%x STATE_LEGACY
%x STATE_PURE

%{
 // forward declaration needed by the following function
#ifndef YY_PROTO
#ifdef YY_USE_PROTOS
#define YY_PROTO(proto) proto
#else
#define YY_PROTO(proto) ()
#endif
#endif
 static void yyunput YY_PROTO(( int c, char *buf_ptr ));

 void do_unput5(const char c) { unput(c); }
%}

HEXDIGIT [0-9a-fA-F]
OCTDIGIT [0-7]
DIGIT    [0-9]
ID       [$a-zA-Z_][$a-zA-Z0-9_]*
EXPONENT [eE][-+]?{DIGIT}+

%%

%{
if (yy_first_time) {
	/* start in legacy state */
	yy_push_state(STATE_LEGACY);
	yy_first_time = 0;
	
	/* suppress unused warning for static declared function */
	yy_top_state();
}
%}

<*>{
0x{HEXDIGIT}+		{ count(); return read_int (yytext, &swf5lval); }
0{OCTDIGIT}+		{ count(); return read_int (yytext, &swf5lval); }
{DIGIT}+		{ count(); return read_int (yytext, &swf5lval); }
{DIGIT}+"."{DIGIT}*{EXPONENT}?	{ count(); swf5lval.doubleVal = atof(yytext);
				return DOUBLE; }
{DIGIT}+*{EXPONENT}?		{ count(); swf5lval.doubleVal = atof(yytext);
				return DOUBLE;	}
true			{ count();swf5lval.intVal = 1;
				return BOOLEAN;	}
false			{ count(); swf5lval.intVal = 0;
				return BOOLEAN;	}
null			{ count();	return NULLVAL;		}
undefined		{ count();	return UNDEFINED;	}
type[oO]f		{ count();	return TYPEOF; }
}

<STATE_PURE,STATE_LEGACY>{
break			{ count();	return BREAK;		}
continue		{ count();	return CONTINUE;	}
function		{ count();	return FUNCTION;	}
else			{ count();	return ELSE;		}
switch			{ count();	return SWITCH;		}
case			{ count();	return CASE;		}
default			{ count();	return DEFAULT;		}
for			{ count();	return FOR;		}
in			{ count();	return IN;		}
if			{ count();	return IF;		}
while			{ count();	return WHILE;		}
do			{ count();	return DO;		}
var			{ count();	return VAR;		}
new			{ count();	return NEW;		}
delete			{ count();	return DELETE;		}
targetPath		{ count();	return TARGETPATH;	}
return			{ count();	return RETURN;		}
this			{ count(); 	return THIS;		}
with			{ count();
			  /* do not lex legacy functions in with-blocks */
			  yy_push_state(STATE_PURE);
			  yy_new_state = 1;	
			  return WITH;		
			}

  /* switch state to ASM */
asm 			{ count();	
				yy_push_state(STATE_ASM); 
				yy_new_state = 1;	
				return ASM;
			}

eval			{ count();	return EVAL;		}
instance[oO]f		{ count();	return INSTANCEOF; }
trace			{ count();	return TRACE;	}
class			{ count();	return CLASS; }
public 			{ count();	return PUBLIC; }
private			{ count();	return PRIVATE; }

  /* SWF >= 6: lex always as reserved word complain later */
try			{ count(); 	return TRY; }
catch			{ count(); 	return CATCH; }
throw			{ count(); 	return THROW; }
finally			{ count(); 	return FINALLY; }
"===" 			{ count(); 	return EEQ; }
"!==" 			{ count(); 	return NEE; }
}

  /* legacy functions */
  /* in some blocks legacy function names should not be lexed (e.g. with) */
<STATE_LEGACY>{
random			{ count();	return RANDOM;	}
getTimer		{ count();	return GETTIMER;	}
length			{ count();	return LENGTH;	}
concat			{ count();	return CONCAT;	}
substr			{ count();	return SUBSTR;	}
int			{ count();	return INT;	}
ord			{ count();	return ORD;	}
chr			{ count();	return CHR;	}
get[uU][rR][lL]		{ count();	return GETURL;	}
get[uU][rR][lL]1	{ count();	return GETURL1;	}
nextFrame		{ count();	return NEXTFRAME;	}
prevFrame		{ count();	return PREVFRAME;	}
play			{ count();	return PLAY;		}
stop			{ count();	return STOP;		}
toggleQuality		{ count();	return TOGGLEQUALITY;	}
stopSounds		{ count();	return STOPSOUNDS;	}
call			{ count();	return CALLFRAME;	}
gotoAndPlay		{ count();	return GOTOANDPLAY;	}
gotoAndStop		{ count();	return GOTOANDSTOP;	}
setTarget		{ count();	return SETTARGET;	}
loadVariables		{ count();	return LOADVARIABLES;	}
loadMovie		{ count();	return LOADMOVIE;	}
loadVariablesNum	{ count();	return LOADVARIABLESNUM;	}
loadMovieNum		{ count();	return LOADMOVIENUM;	}
duplicateMovieClip	{ count();	return DUPLICATEMOVIECLIP; }
removeMovieClip		{ count();	return REMOVEMOVIECLIP; }
startDrag		{ count();	return STARTDRAG;	}
stopDrag		{ count();	return STOPDRAG;	}
setProperty		{ count();	return SETPROPERTY;	}
getProperty		{ count();	return GETPROPERTY;	}

  /* properties (legacy) */
_x			{ count();	return _P_X; }
_y 			{ count();	return _P_Y; }
_xscale			{ count();	return _P_XSCALE; }
_yscale			{ count();	return _P_YSCALE; }
_currentframe		{ count();	return _P_CURRENTFRAME; }
_totalframes		{ count();	return _P_TOTALFRAMES; }
_alpha			{ count();	return _P_ALPHA; }
_visible		{ count();	return _P_VISIBLE; }
_width			{ count();	return _P_WIDTH; }
_height			{ count();	return _P_HEIGHT; }
_rotation		{ count();	return _P_ROTATION; }
_target			{ count();	return _P_TARGET; }
_framesloaded		{ count();	return _P_FRAMESLOADED; }
_name 			{ count();	return _P_NAME;	}
_droptarget		{ count();	return _P_DROPTARGET; }
_url			{ count();	return _P_URL; }
_highquality		{ count();	return _P_HIGHQUALITY; }
_focusrect		{ count();	return _P_FOCUSRECT; }
_soundbuftime		{ count();	return _P_SOUNDBUFTIME; }
_quality		{ count();	return _P_QUALITY; }
_xmouse			{ count();	return _P_XMOUSE; }
_ymouse			{ count();	return _P_YMOUSE; }
}

  /* assembler ops */
<STATE_ASM>{
new			{ count();	return NEW;		}
dup			{ count();	return DUP; }
swap			{ count();	return SWAP; }
pop			{ count();	return POP; }
push			{ count();	return PUSH; }
setregister		{ count();	return SETREGISTER; }
callfunction		{ count();	return CALLFUNCTION; }
callmethod		{ count();	return CALLMETHOD; }
newobject		{ count();	return NEWOBJECT; }
newmethod		{ count();	return NEWMETHOD; }
and			{ count();	return AND; }
or			{ count();	return OR; }
xor			{ count();	return XOR; }
modulo			{ count();	return MODULO; }
add			{ count();	return ADD; }
newadd			{ count();	return ADD; }
lessthan		{ count();	return LESSTHAN; }
newlessthan		{ count();	return LESSTHAN; }
equals			{ count();	return EQUALS; }
newequals		{ count();	return EQUALS; }
inc			{ count();	return INC; }
dec			{ count();	return DEC; }
enumerate		{ count();	return ENUMERATE; }
initobject		{ count();	return INITOBJECT; }
initarray		{ count();	return INITARRAY; }
getmember		{ count();	return GETMEMBER; }
setmember		{ count();	return SETMEMBER; }
shiftleft		{ count();	return SHIFTLEFT; }
shiftright		{ count();	return SHIFTRIGHT; }
shiftright2		{ count();	return SHIFTRIGHT2; }
varequals		{ count();	return VAREQUALS; }
oldadd			{ count();	return OLDADD; }
subtract		{ count();	return SUBTRACT; }
multiply		{ count();	return MULTIPLY; }
divide			{ count();	return DIVIDE; }
oldequals		{ count();	return OLDEQUALS; }
oldlessthan		{ count();	return OLDLESSTHAN; }
logicaland		{ count();	return LOGICALAND; }
logicalor		{ count();	return LOGICALOR; }
not			{ count();	return NOT; }
stringeq		{ count();	return STRINGEQ; }
stringlength		{ count();	return STRINGLENGTH; }
substring		{ count();	return SUBSTRING; }
getproperty		{ count();	return GETPROPERTY; }
setproperty		{ count();	return SETPROPERTY; }
getvariable		{ count();	return GETVARIABLE; }
setvariable		{ count();	return SETVARIABLE; }
settargetexpr		{ count();	return SETTARGETEXPRESSION; }
startdrag		{ count();	return STARTDRAG; }
stopdrag		{ count();	return STOPDRAG; }
stringlessthan		{ count();	return STRINGLESSTHAN; }
mblength		{ count();	return MBLENGTH; }
mbsubstring		{ count();	return MBSUBSTRING; }
mbord			{ count();	return MBORD; }
mbchr			{ count();	return MBCHR; }
branch			{ count();	return BRANCHALWAYS; }
branchalways		{ count();	return BRANCHALWAYS; }
branchiftrue		{ count();	return BRANCHIFTRUE; }
post			{ count();	return POST; }
get			{ count();	return GET; }
end			{ count();	return END;		}
extends			{ count();	return EXTENDS;		}
tonumber		{ count();	return TONUMBER;	}
tostring		{ count();	return TOSTRING;	}
targetpath		{ count();	return TARGETPATH;	}
duplicatemovieclip	{ count();	return DUPLICATEMOVIECLIP; }
removemovieclip		{ count();	return REMOVEMOVIECLIP; }
implements		{ count(); 	return IMPLEMENTS;	}
fscommand2		{ count(); 	return FSCOMMAND2;	}
cast			{ count();	return CAST;		}
r\:{DIGIT}+		{ count(); swf5lval.str = strdup(yytext+2);
				return REGISTER; }
trace			{ count();	return TRACE;	}
}

  /* rules action in any state */
<*>{
{ID}			{ count();	swf5lval.str = strdup(yytext);
					return IDENTIFIER;	}

\"(\\.|[^\\"])*\"	{ count();	swf5lval.str = strdup(yytext+1);
					swf5lval.str[strlen(swf5lval.str)-1]=0;
                                        unescape(swf5lval.str);
					return STRING;		}

\'(\\.|[^\\'])*\'	{ count();	swf5lval.str = strdup(yytext+1);
					swf5lval.str[strlen(swf5lval.str)-1]=0;
                                        unescape(swf5lval.str);
					return STRING; 		}

\"(\\.|[^\\"])*$	{ count();	swf5lval.str = NULL;
					warning("Unterminated string!");
					return BROKENSTRING;		}

\'(\\.|[^\\'])*$	{ count();	swf5lval.str = NULL;
					warning("Unterminated string!");
					return BROKENSTRING;		}

"/*"			{ count();	comment();		}
"//"			{ count();	comment1();		}
[ \t\v\f]		{ count(); }

"++"			{ count();	return INCR; }
"--"			{ count();	return DECR; }
"<="			{ count();	return LE; }
">="			{ count();	return GE; }
"==" 			{ count();	return EQ; }
"!=" 			{ count();	return NE; }
"&&" 			{ count();	return LAN; }
"||" 			{ count();	return LOR; }
"*="			{ count();	return MEQ; }
"/="			{ count();	return DEQ; }
"+="			{ count();	return IEQ; }
"-="			{ count();	return SEQ; }
"&="			{ count();	return AEQ; }
"|="			{ count();	return OEQ; }
"<<"			{ count();	return SHL; }
">>"			{ count();	return SHR; }
">>>"			{ count();	return SHR2; }
"<<="			{ count();	return SHLEQ; }
">>="			{ count();	return SHREQ; }
">>>="			{ count();	return SHR2EQ; }

"<"			{ count();	return '<'; }
">"			{ count();	return '>'; }
";"			{ count();	return ';'; }
"="			{ count();	return '='; }
"+"			{ count();	return '+'; }
"-"			{ count();	return '-'; }
"&"			{ count();	return '&'; }
"|"			{ count();	return '|'; }
"^"			{ count();	return '^'; }
"*"			{ count();	return '*'; }
"/"			{ count();	return '/'; }
"%"			{ count();	return '%'; }
"!"			{ count();	return '!'; }
"("			{ count();	return '('; }
")"			{ count();	return ')'; }
"["			{ count();	return '['; }
"]"			{ count();	return ']'; }

"{"			{ count();	
			  if(yy_new_state)
			  {
				// if yy_new_state state was already pushed
				// and will be poped by closing '}'
				yy_new_state = 0;
			  }
			  else
			  {
				// push current state on top of stack
				// preserves state when closing '}' pops stack
			  	yy_push_state(YY_START); 
			  }
			  return '{'; 
			}

"}"			{ count();	
			  yy_pop_state(); 
			  return '}'; 
			}

","			{ count();	return ','; }
"."			{ count();	return '.'; }
"?"			{ count();	return '?'; }
":"			{ count();	return ':'; }
"~"			{ count();	return '~'; }

\r?\n			{ countline();	yyless(1);	}

.			SWF_error("Unrecognized character: %s\n", yytext);
}

%%
static int getinput() {
#ifdef __cplusplus
					return yyinput();
#else
					return input();
#endif
}

int swf5wrap()
{
  return 1;
}

static void countline()
{
  ++sLineNumber;
  column = 0;
  msgline = msgbufs[sLineNumber & 1];
}

static int LineNumber(void)
{
   return realLine + 1;
}

static int ColumnNumber(void)
{
   return realColumn;
}

static char *LineText(void)
{
  return msgbufs[realLine & 1];
}

static void comment()
{
   // Handle block comments

   int c, c1;

loop:
   // We have the start of a comment so look skip everything up to the
   // end of the comment character
   while ((c = getinput()) != '*' && c != EOF)
   {
      if(column < 1023)
         msgline[column] = c;

      ++column;

      // keep the line number in synch
      if (c == '\n')
      {
         // start the output (matches the algorithim in the lexx above)
	 countline();
      }

      if (swf5debug) putchar(c);
   }

   // is this the end of comment character
   if ((c1 = getinput()) != '/' && c != EOF)
   {
      // false start as this was no end of comment
      do_unput5(c1);
      goto loop;
   }

   // write out the start of the end of comment
   if (c != EOF)
      if (swf5debug) putchar(c);

   // write out the end of the end of comment
   if (c1 != EOF) 
      if (swf5debug) putchar(c1);
}

static void comment1()
{
   // Handle comment of type 1 (ie '//')

   int c;

   // this is a line comment
   while ((c = getinput()) != '\n' && c != EOF)
   {
      if (swf5debug) putchar(c);

      if(column < 1023)
         msgline[column] = c;

      ++column;
   };

   // keep the line number in synch
   if (c == '\n')
   {
      if (swf5debug) putchar(c);

      countline();
   }
}

static void count(void)
{
   int n;

   if(swf5debug) printf("%s", yytext);
   if(realLine != sLineNumber)
   {
     if(column > 0 || realLine + 1 < sLineNumber)
     {
       realColumn = 0;
       realLine = sLineNumber;
     }
   }
   realColumn += lastToken;
   lastToken = 0;
   for(n=0; n<yyleng; ++n, ++column, ++lastToken)
   {
     if(column < 1023)
       msgline[column] = yytext[n];
   }
   msgline[column] = 0;
}

static void printprog()
{
  if(sLineNumber)
    SWF_warn("\n%s", msgbufs[(sLineNumber-1)&1]);

  if(column < 1023)
    msgline[column] = 0;

  SWF_warn("\n%s", msgline);
}

static void warning(char *msg)
{
   // print a warning message
   printprog();
   SWF_warn("\n%*s", ColumnNumber(), "^");
   SWF_warn("\nLine %4.4d:  Reason: '%s' \n", LineNumber(), msg);
}

void swf5error(char *msg)
{
  // report a error
  if(strlen(yytext))
  {
    SWF_error("\n%s\n%*s\nLine %i:  Reason: '%s'\n",
	      LineText(), ColumnNumber(), "^", LineNumber(), msg);
  }
  else
  {
    SWF_error("\nLine %d: Reason: 'Unexpected EOF found while looking for input.'\n", LineNumber());
  }
}
