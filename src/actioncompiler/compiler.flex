%{

#include <math.h>
#include <string.h>

#include "compile.h"
#include "action.h"
#include "compiler.tab.h" /* defines token types */

int yydebug;
char *lexBuffer = NULL;
int lexBufferLen = 0;

static int  sLineNumber = 0;
static char szLine[1024];
static int  column = 0;

/* Function prototypes */
int  yywrap();
void count(void);
void comment(void);
void comment1(void);
void yyerror(char *msg);
void warning(char *msg);

/* thanks to the prolific and brilliant Raff: */
int lexBufferInput(char *buf, int max_size)
{
  int l = lexBufferLen > max_size ? max_size : lexBufferLen;
  
  if (lexBufferLen <= 0)
    return YY_NULL;

  memcpy(buf, lexBuffer, l);
  lexBuffer += l;
  lexBufferLen -= l;
  return l;
}

#define YY_INPUT(buf,result,max_size) result=lexBufferInput(buf, max_size)

%}

DIGIT    [0-9]
ID       [a-zA-Z_][a-zA-Z0-9_]*

%%

{DIGIT}+		{ count();	yylval.intVal = atoi(yytext);
					return INTEGER;		}
{DIGIT}+"."{DIGIT}*	{ count();	yylval.doubleVal = atof(yytext);
					return DOUBLE; 		}
true			{ count();	yylval.intVal = 1;
					return BOOLEAN;		}
false			{ count();	yylval.intVal = 0;
					return BOOLEAN;		}
null			{ count();	return NULLVAL;		}
break			{ count();	return BREAK;		}
continue		{ count();	return CONTINUE;	}
function		{ count();	return FUNCTION;	}
else			{ count();	return ELSE;		}
switch			{ count();	return SWITCH;		}
case			{ count();	return CASE;		}
for			{ count();	return FOR;		}
in			{ count();	return IN;		}
if			{ count();	return IF;		}
while			{ count();	return WHILE;		}
do			{ count();	return DO;		}
var			{ count();	return VAR;		}
new			{ count();	return NEW;		}
delete			{ count();	return DELETE;		}
return			{ count();	return RETURN;		}
end			{ count();	return END;		}
with			{ count();	return WITH;		}
asm			{ count();	return ASM;		}
eval			{ count();	return EVAL;		}

  /* legacy functions */
random		{ count();	return RANDOM;	}
getTimer	{ count();	return GETTIMER;	}
length		{ count();	return LENGTH;	}
concat		{ count();	return CONCAT;	}
substr		{ count();	return SUBSTR;	}
trace		{ count();	return TRACE;	}
int		{ count();	return INT;	}
ord		{ count();	return ORD;	}
chr		{ count();	return CHR;	}
getURL		{ count();	return GETURL;	}
getURL1		{ count();	return GETURL1; }
nextFrame	{ count();	return NEXTFRAME;	}
prevFrame	{ count();	return PREVFRAME;	}
play		{ count();	return PLAY;		}
stop		{ count();	return STOP;		}
toggleQuality	{ count();	return TOGGLEQUALITY;	}
stopSounds	{ count();	return STOPSOUNDS;	}

  /* assembler ops */
dup			{ count();	return DUP; }
swap			{ count();	return SWAP; }
pop			{ count();	return POP; }
push			{ count();	return PUSH; }
setregister		{ count();	return SETREGISTER; }
callfunction		{ count();	return CALLFUNCTION; }
callmethod		{ count();	return CALLMETHOD; }
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
typeof			{ count();	return TYPEOF; }
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
getvariable		{ count();	return GETVARIABLE; }
setvariable		{ count();	return SETVARIABLE; }
settargetexpr		{ count();	return SETTARGETEXPRESSION; }
duplicateclip		{ count();	return DUPLICATECLIP; }
removeclip		{ count();	return REMOVECLIP; }
startdrag		{ count();	return STARTDRAGMOVIE; }
stopdrag		{ count();	return STOPDRAGMOVIE; }
stringlessthan		{ count();	return STRINGLESSTHAN; }
mblength		{ count();	return MBLENGTH; }
mbsubstring		{ count();	return MBSUBSTRING; }
mbord			{ count();	return MBORD; }
mbchr			{ count();	return MBCHR; }
branch			{ count();	return BRANCHALWAYS; }
branchalways		{ count();	return BRANCHALWAYS; }
branchiftrue		{ count();	return BRANCHIFTRUE; }
geturl2			{ count();	return GETURL2; }
post			{ count();	return POST; }
get			{ count();	return GET; }


r\:{DIGIT}+		{ count();	yylval.str = strdup(yytext+2);
					return REGISTER;	}


{ID}			{ count();	yylval.str = strdup(yytext);
					return IDENTIFIER;	}

\/{ID}			{ count();	yylval.str = strdup(yytext);
					return IDENTIFIER;	}

\"(\\.|[^\\"])*\"	{ count();	yylval.str = strdup(yytext+1);
					yylval.str[strlen(yylval.str)-1]=0;
					return STRING;		}

\'(\\.|[^\\'])*\'	{ count();	yylval.str = strdup(yytext+1);
					yylval.str[strlen(yylval.str)-1]=0;
					return STRING; 		}

\"(\\.|[^\\"])*$	{ count();	yylval.str = strdup("");
					warning("Unterminated string!");
					return STRING;		}

\'(\\.|[^\\'])*$	{ count();	yylval.str = strdup("");
					warning("Unterminated string!");
					return STRING;		}

"/*"			{ count();	comment();		}
"//"			{ count();	comment1();		}
[ \t\v\f]		{ count(); }

"++"			{ count();	return INC; }
"--"			{ count();	return DEC; }
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
"<<"			{ count();	return SHIFTLEFT; }
">>"			{ count();	return SHIFTRIGHT; }
">>>"			{ count();	return SHIFTRIGHT2; }
"<<="			{ count();	return SHIFTLEFTEQ; }
">>="			{ count();	return SHIFTRIGHTEQ; }
">>>="			{ count();	return SHIFTRIGHT2EQ; }

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
"{"			{ count();	return '{'; }
"}"			{ count();	return '}'; }
","			{ count();	return ','; }
"."			{ count();	return '.'; }
"?"			{ count();	return '?'; }
":"			{ count();	return ':'; }

"\n"			{ count();	column = 0;
					strcpy(szLine, yytext + 1);
					++sLineNumber;	yyless(1);	}

.			printf( "Unrecognized character: %s\n", yytext );

%%

int yywrap()
{
   return(1);
}

int LineNumber(void)
{
   return (sLineNumber + 1);
}

int ColumnNumber(void)
{
   return column;
}

char *LineText(void)
{
   return (szLine);
}

void comment(void)
{
   // Handle block comments

   char c, c1;

loop:
   // We have the start of a comment so look skip everything up to the
   // end of the comment character
   while ((c = input()) != '*' && c != 0)
   {
      ++column;

      // keep the line number in synch
      if (c == '\n')
      {
         // start the output (matches the algorithim in the lexx above)
         column = 0;
         ++sLineNumber;
      }

      if (yydebug) putchar(c);
   }

   // is this the end of comment character
   if ((c1 = input()) != '/' && c != 0)
   {
      // false start as this was no end of comment
      unput(c1);
      goto loop;
   }

   // write out the start of the end of comment
   if (c != 0)
      if (yydebug) putchar(c);

   // write out the end of the end of comment
   if (c1 != 0) 
      if (yydebug) putchar(c1);
}

void comment1(void)
{
   // Handle comment of type 1 (ie '//')

   char c;

   // this is a line comment
   while ((c = input()) != '\n' && c != 0)
   {
      if (yydebug) putchar(c);
      ++column;
   };

   // keep the line number in synch
   if (c == '\n')
   {
      if (yydebug) putchar(c);
      column = 0;
      ++sLineNumber;
   }
}

void count(void)
{
   // Count the characters to maintain the current column position
   if (yytext[0] == '\n')
   {
      column = 0;
      if (yydebug) printf("\n");
   }
   else
   {
      if (yydebug) printf("%s", yytext);

      //-- keep writing the stuff to standard output
      column += yyleng;
   }
}

void warning(char *msg)
{
   // print a warning message
   printf("\n%s", LineText());
   printf("\n%*s", ColumnNumber(), "^");
   printf("\nLine %4.4d:  Reason: '%s' \n", LineNumber(), msg);
}

void yyerror(char *msg)
{
   // report a error
   if (strlen(yytext))
   {
      printf("\n%s", LineText());
      printf("\n%*s", ColumnNumber(), "^");
      printf("\nLine %4.4d:  Reason: '%s' \n", LineNumber(), msg);
   }
   else
   {
      printf("\n%*s", 0, "^");
      printf("\nLine %d: Reason: 'Unexpected EOF found while looking for input.'\n", LineNumber());
   }
}

/*
main( argc, argv )
int argc;
char **argv;
    {
    ++argv, --argc;  *//* skip over program name *//*
    if ( argc > 0 )
            yyin = fopen( argv[0], "r" );
    else
            yyin = stdin;

    yylex();
    }
*/
