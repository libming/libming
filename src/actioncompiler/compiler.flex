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

{DIGIT}+		{ count();	yylval.str = strdup(yytext);
					return NUMBER; 		}
{DIGIT}+"."{DIGIT}*	{ count();	yylval.str = strdup(yytext);
					return NUMBER; 		}
true			{ count();	yylval.str = strdup("1");
					return NUMBER;		}
false			{ count();	yylval.str = strdup("0");
					return NUMBER;		}
break			{ count();	return BREAK;		}
continue		{ count();	return CONTINUE;	}
else			{ count();	return ELSE;		}
for			{ count();	return FOR;		}
if			{ count();	return IF;		}
while			{ count();	return WHILE;		}
do			{ count();	return DO;		}
valueOf			{ count(); 	return EVAL;		}

  /* functions */
random		{ count();	return RANDOM;	}
time		{ count();	return TIME;	}
length		{ count();	return LENGTH;	}
int		{ count();	return INT;	}
concat		{ count();	return CONCAT;	}
duplicateClip	{ count();	return DUPLICATECLIP;	}
removeClip	{ count();	return REMOVECLIP;	}
trace		{ count();	return TRACE;	}
startDrag	{ count();	return STARTDRAG;	}
stopDrag	{ count();	return STOPDRAG;	}
ord		{ count();	return ORD;	}
chr		{ count();	return CHR;	}
callFrame	{ count();	return CALLFRAME;	}
getURL		{ count();	return GETURL;	}
getURL1		{ count();	return GETURL1; }
loadMovie	{ count();	return LOADMOVIE;	}
loadVariables	{ count();	return LOADVARIABLES;	}
substr		{ count();	return SUBSTR;	}

  /* getURL2 methods */
post		{ count();	yylval.getURLMethod = GETURL_METHOD_POST;
				return GETURL_METHOD;	}
get		{ count();	yylval.getURLMethod = GETURL_METHOD_GET;
				return GETURL_METHOD;	}
nosend		{ count();	yylval.getURLMethod = GETURL_METHOD_NOSEND;
				return GETURL_METHOD;	}


  /* v3 functions */
nextFrame	{ count();	return NEXTFRAME;	}
prevFrame	{ count();	return PREVFRAME;	}
play		{ count();	return PLAY;		}
stop		{ count();	return STOP;		}
toggleQuality	{ count();	return TOGGLEQUALITY;	}
stopSounds	{ count();	return STOPSOUNDS;	}
gotoFrame	{ count();	return GOTOFRAME;	}
frameLoaded	{ count();	return FRAMELOADED;	}
setTarget	{ count();	return SETTARGET;	}


this			{ count();      return THIS;	}

{ID}			{ count();	yylval.str = strdup(yytext);
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
"<"			{ count();	return '<'; }
">"			{ count();	return '>'; }
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
"==="			{ count();	return STREQ; }
"!=="			{ count();	return STRNE; }
"<=>"			{ count();	return STRCMP; }
".."			{ count();	return PARENT; }

";"			{ count();	return ';'; }
"="			{ count();	return '='; }
"+"			{ count();	return '+'; }
"-"			{ count();	return '-'; }
"&"			{ count();	return '&'; }
"*"			{ count();	return '*'; }
"/"			{ count();	return '/'; }
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

\n			{ count();	column = 0;
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
