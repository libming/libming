
%start program

%{

#include <time.h>
#include <string.h>
#include "compile.h"
#include "assembler.h"

#define YYPARSE_PARAM buffer

Buffer bf, bc;

%}

%union
{
  Buffer action;
  char *str;
  SWFGetUrl2Method getURLMethod;
  int op;
  int intVal;
  int len;
  double doubleVal;

  struct
  {
    Buffer buffer;
    int count;
  } exprlist;
  struct
  {
	Buffer obj, ident, memexpr;
  } lval;
}

/* tokens etc. */

%token BREAK, CONTINUE, FUNCTION, ELSE, SWITCH, CASE, FOR, IN, IF, WHILE,
  DO, VAR, NEW, DELETE, RETURN, END, WITH, ASM, EVAL

%token RANDOM, GETTIMER, LENGTH, CONCAT, SUBSTR, TRACE, INT, ORD, CHR, GETURL,
  GETURL1, NEXTFRAME, PREVFRAME, PLAY, STOP, TOGGLEQUALITY, STOPSOUNDS

%token DUP, SWAP, POP, PUSH, SETREGISTER, CALLFUNCTION, CALLMETHOD,
  AND, OR, XOR, MODULO, ADD, LESSTHAN, EQUALS,
  INC, DEC, TYPEOF, ENUMERATE, INITOBJECT, INITARRAY, GETMEMBER,
  SETMEMBER, SHIFTLEFT, SHIFTRIGHT, SHIFTRIGHT2, VAREQUALS, OLDADD, SUBTRACT,
  MULTIPLY, DIVIDE, OLDEQUALS, OLDLESSTHAN, LOGICALAND, LOGICALOR, NOT,
  STRINGEQ, STRINGLENGTH, SUBSTRING, GETVARIABLE, SETVARIABLE,
  SETTARGETEXPRESSION,  DUPLICATEMOVIECLIP, REMOVEMOVIECLIP,
  STRINGLESSTHAN, MBLENGTH, MBSUBSTRING, MBORD, MBCHR,
  BRANCHALWAYS, BRANCHIFTRUE, GETURL2, POST, GET,
  LOADVARIABLES, LOADMOVIE, LOADVARIABLESNUM, LOADMOVIENUM,
  CALLFRAME, STARTDRAG, STOPDRAG, GOTOFRAME, SETTARGET

%token NULLVAL
%token <intVal> INTEGER
%token <doubleVal> DOUBLE
%token <intVal> BOOLEAN
%token <str> REGISTER

/* these two are strdup'ed in compiler.flex, so free them up here */
%token <str> STRING
%token <str> IDENTIFIER

%token <getURLMethod> GETURL_METHOD

%token EQ "=="
%token LE "<="
%token GE ">="
%token NE "!="
%token LAN "&&"
%token LOR "||"
%token INCR "++"
%token DECR "--"
%token IEQ "+="
%token DEQ "/="
%token MEQ "*="
%token SEQ "-="

%token SHIFTLEFT "<<"
%token SHIFTRIGHT ">>"
%token SHIFTRIGHT2 ">>>"
%token SHIFTLEFTEQ "<<="
%token SHIFTRIGHTEQ ">>="
%token SHIFTRIGHT2EQ ">>>="


/* ascending order of ops ..? */

%nonassoc NOELSE
%nonassoc ELSE
%left ','
%right '=' "*=" "/=" "+=" "-=" "&=" "|=" "^=" ">>=" ">>>=" "<<="
%left '?' ':'
%left "&&" "||"
%left "==" "!="
%left '<' '>' "<=" ">="
%left '&' '|' '^'
%left "<<" ">>" ">>>"
%left '+' '-'
%left '*' '/' '%'
%nonassoc "++" "--"
%right '!' '~' UMINUS
%right POSTFIX
%left '.' '[' ']'


%type <action> program, code
%type <action> stmt, stmts
%type <action> if_stmt, iter_stmt, cont_stmt, break_stmt, return_stmt
%type <action> with_stmt
%type <action> switch_stmt, switch_cases, switch_case
%type <action> anon_function_decl, function_decl, anycode
%type <action> void_function_call, function_call, method_call
%type <action> assign_stmt, assign_stmts, assign_stmts_opt
%type <action> expr, expr_or_obj, objexpr, expr_opt, obj_ref
%type <action> emptybraces, level, init_vars, init_var, primary
%type <lval> lvalue

%type <exprlist> expr_list, objexpr_list, formals_list

%type <op> assignop, incdecop
%type <getURLMethod> urlmethod

%type <str> identifier

%type <len> opcode, opcode_list, push_item, with, push_list

/*
%type <intVal> integer
%type <doubleVal> double
*/
%%

/* rules */

program
	: { bf = newBuffer();
		bc = newBuffer();
	} code
		{ Buffer b = newBuffer();
		  bufferWriteConstants(b);
		  bufferConcat(b, bf);
		  bufferConcat(b, bc);
		  *((Buffer *)buffer) = b; }

code
	: anycode
	| code anycode
	;

anycode
	: stmt
		{ bufferConcat(bc, $1); }
	| function_decl
		{ bufferConcat(bf, $1); }
	;

stmts
	: stmt
                { $$ = $1; }

	| stmts stmt
                { $$ = $1;
		  bufferConcat($$, $2); }
	;

emptybraces
	: '{' '}'	{ }
	;

stmt
	: emptybraces				{ $$ = NULL; }
	| '{' stmts '}'				{ $$ = $2; }
	| ';'					{ $$ = NULL; }
	| assign_stmt ';'			{ $$ = $1; }
	| if_stmt
	| iter_stmt
	| cont_stmt
	| break_stmt
	| switch_stmt
	| return_stmt
	| with_stmt
	;

with_stmt
	: WITH '(' expr ')' '{' stmts '}'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_WITH);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($6));
		  bufferConcat($$, $6); }
	;

return_stmt
	: RETURN ';'
		{ $$ = newBuffer();
		  bufferWriteNull($$);
		  bufferWriteU8($$, SWFACTION_RETURN); }

	| RETURN expr ';'
		{ $$ = $2;
		  bufferWriteU8($$, SWFACTION_RETURN); }
	;

assign_stmts
	: assign_stmt
	| assign_stmts ',' assign_stmt		{ bufferConcat($1, $3); }
	;

if_stmt
	: IF '(' expr ')' stmt ELSE stmt
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_BRANCHIFTRUE);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($7)+5);
		  bufferConcat($$, $7);
		  bufferWriteU8($$, SWFACTION_BRANCHALWAYS);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($5));
		  bufferConcat($$, $5); }

	| IF '(' expr ')' stmt		%prec NOELSE
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_LOGICALNOT);
		  bufferWriteU8($$, SWFACTION_BRANCHIFTRUE);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($5));
		  bufferConcat($$, $5); }

expr_opt
	: /* empty */	{ $$ = NULL; }
	| expr		{ $$ = $1; }
	;


switch_stmt
	: SWITCH '(' expr ')' '{'
		{ $$ = $3; }
	  switch_cases '}'
		{ bufferConcat($$, $7);
		  bufferResolveJumps($$);
 /* FIXME: continue in switch continues surrounding loop, if any */
	}
	;

/* XXX */
switch_cases
	: switch_case
		{ $$ = $1; }

	| switch_cases switch_case
		{ $$ = $1;
		  bufferConcat($$, $2); }
	;

switch_case
	: CASE INTEGER ':' stmt
		{ $$ = newBuffer(); }
	;


/* there's GOT to be a better way than this.. */

identifier
	: IDENTIFIER
	| NEW		{ $$ = strdup("new"); }
	| DELETE	{ $$ = strdup("delete"); }
	| RANDOM	{ $$ = strdup("random"); }
	| GETTIMER	{ $$ = strdup("getTimer"); }
	| LENGTH	{ $$ = strdup("length"); }
	| CONCAT	{ $$ = strdup("concat"); }
	| SUBSTR	{ $$ = strdup("substr"); }
	| TRACE	{ $$ = strdup("trace"); }
	| INT	{ $$ = strdup("int"); }
	| ORD	{ $$ = strdup("ord"); }
	| CHR	{ $$ = strdup("chr"); }
	| GETURL	{ $$ = strdup("getURL"); }
	| GETURL1	{ $$ = strdup("getURL1"); }
	| NEXTFRAME	{ $$ = strdup("nextFrame"); }
	| PREVFRAME	{ $$ = strdup("prevFrame"); }
	| PLAY	{ $$ = strdup("play"); }
	| STOP	{ $$ = strdup("stop"); }
	| TOGGLEQUALITY	{ $$ = strdup("toggleQuality"); }
	| STOPSOUNDS	{ $$ = strdup("stopSounds"); }
	| DUP	{ $$ = strdup("dup"); }
	| SWAP	{ $$ = strdup("swap"); }
	| POP	{ $$ = strdup("pop"); }
	| PUSH	{ $$ = strdup("push"); }
	| SETREGISTER	{ $$ = strdup("setRegister"); }
	| CALLFUNCTION	{ $$ = strdup("callFunction"); }
	| CALLMETHOD	{ $$ = strdup("callMethod"); }
	| AND	{ $$ = strdup("and"); }
	| OR	{ $$ = strdup("or"); }
	| XOR	{ $$ = strdup("xor"); }
	| MODULO	{ $$ = strdup("modulo"); }
	| ADD	{ $$ = strdup("add"); }
	| LESSTHAN	{ $$ = strdup("lessThan"); }
	| EQUALS	{ $$ = strdup("equals"); }
	| INC	{ $$ = strdup("inc"); }
	| DEC	{ $$ = strdup("dec"); }
	| TYPEOF	{ $$ = strdup("typeof"); }
	| ENUMERATE	{ $$ = strdup("enumerate"); }
	| INITOBJECT	{ $$ = strdup("initobject"); }
	| INITARRAY	{ $$ = strdup("initarray"); }
	| GETMEMBER	{ $$ = strdup("getmember"); }
	| SETMEMBER	{ $$ = strdup("setmember"); }
	| SHIFTLEFT	{ $$ = strdup("shiftleft"); }
	| SHIFTRIGHT	{ $$ = strdup("shiftright"); }
	| SHIFTRIGHT2	{ $$ = strdup("shiftright2"); }
	| VAREQUALS	{ $$ = strdup("varequals"); }
	| OLDADD	{ $$ = strdup("oldAdd"); }
	| SUBTRACT	{ $$ = strdup("subtract"); }
	| MULTIPLY	{ $$ = strdup("multiply"); }
	| DIVIDE	{ $$ = strdup("divide"); }
	| OLDEQUALS	{ $$ = strdup("oldequals"); }
	| OLDLESSTHAN	{ $$ = strdup("oldlessthan"); }
	| LOGICALAND	{ $$ = strdup("logicaland"); }
	| LOGICALOR	{ $$ = strdup("logicalor"); }
	| NOT	{ $$ = strdup("not"); }
	| STRINGEQ	{ $$ = strdup("stringeq"); }
	| STRINGLENGTH	{ $$ = strdup("stringlength"); }
	| SUBSTRING	{ $$ = strdup("substring"); }
	| GETVARIABLE	{ $$ = strdup("getvariable"); }
	| SETVARIABLE	{ $$ = strdup("setvariable"); }
	| SETTARGETEXPRESSION	{ $$ = strdup("settargetexpression"); }
	| DUPLICATEMOVIECLIP	{ $$ = strdup("duplicatemovieclip"); }
	| REMOVEMOVIECLIP	{ $$ = strdup("removemovieclip"); }
	| STARTDRAG	{ $$ = strdup("startdrag"); }
	| STOPDRAG	{ $$ = strdup("stopdrag"); }
	| STRINGLESSTHAN	{ $$ = strdup("stringlessthan"); }
	| MBLENGTH	{ $$ = strdup("mblength"); }
	| MBSUBSTRING	{ $$ = strdup("mbsubstring"); }
	| MBORD	{ $$ = strdup("mbord"); }
	| MBCHR	{ $$ = strdup("mbchr"); }
	| BRANCHALWAYS	{ $$ = strdup("branchalways"); }
	| BRANCHIFTRUE	{ $$ = strdup("branchiftrue"); }
	| GETURL2	{ $$ = strdup("getURL2"); }
	| POST	{ $$ = strdup("post"); }
	| GET	{ $$ = strdup("get"); }
	| LOADVARIABLES	{ $$ = strdup("loadvariables"); }
	| LOADMOVIE	{ $$ = strdup("loadmovie"); }
	;

formals_list
	: /* empty */
		{ $$.buffer = newBuffer();
		  $$.count = 0; }

	| identifier
		{ $$.buffer = newBuffer();
		  bufferWriteHardString($$.buffer, $1, strlen($1)+1);
		  $$.count = 1; }

	| formals_list ',' identifier
		{ $$ = $1;
		  bufferWriteHardString($$.buffer, $3, strlen($3)+1);
		  ++$$.count; }
	;

function_decl
	: FUNCTION identifier '(' formals_list ')' stmt
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_DEFINEFUNCTION);
		  bufferWriteS16($$, strlen($2) +
				     bufferLength($4.buffer) + 5);
		  bufferWriteHardString($$, $2, strlen($2)+1);
		  bufferWriteS16($$, $4.count);
		  bufferConcat($$, $4.buffer);
		  bufferWriteS16($$, bufferLength($6));
		  bufferConcat($$, $6); }
	;

obj_ref
	: identifier
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  free($1); }

	| expr '.' identifier
		{ $$ = $1;
		  bufferWriteString($$, $3, strlen($3)+1);
		  bufferWriteU8($$, SWFACTION_GETMEMBER);
		  free($3); }

	| expr '[' expr ']'
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_GETMEMBER); }

	| function_call

	| method_call
	;

iter_stmt
	: WHILE '(' expr ')' stmt
                { $$ = $3;
		  bufferWriteU8($$, SWFACTION_LOGICALNOT);
		  bufferWriteU8($$, SWFACTION_BRANCHIFTRUE);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($5)+5);
		  bufferConcat($$, $5);
		  bufferWriteU8($$, SWFACTION_BRANCHALWAYS);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, -(bufferLength($$)+2));
		  bufferResolveJumps($$); }

	| DO stmt WHILE '(' expr ')'
		{ $$ = $2;
		  bufferConcat($$, $5);
		  bufferWriteU8($$, SWFACTION_BRANCHIFTRUE);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, -(bufferLength($$)+2));
		  bufferResolveJumps($$); }

	| FOR '(' assign_stmts_opt ';' expr_opt ';' assign_stmts_opt ')' stmt
		{
		  if($3)
		    $$ = $3;
		  else
		    $$ = newBuffer();

		  if($7)
		  {
                    bufferWriteU8($$, SWFACTION_BRANCHALWAYS);
                    bufferWriteS16($$, 2);
                    bufferWriteS16($$, bufferLength($7));
		  }
		  else
		    $7 = newBuffer();

		  if($5)
		  {
		    bufferConcat($7, $5);
                    bufferWriteU8($7, SWFACTION_LOGICALNOT);
                    bufferWriteU8($7, SWFACTION_BRANCHIFTRUE);
                    bufferWriteS16($7, 2);
                    bufferWriteS16($7, bufferLength($9)+5);
                  }

                  bufferConcat($7, $9);
                  bufferWriteU8($7, SWFACTION_BRANCHALWAYS);
                  bufferWriteS16($7, 2);
                  bufferWriteS16($7, -(bufferLength($7)+2));
                  bufferResolveJumps($7);

                  bufferConcat($$, $7);
                }

	| FOR '(' identifier IN obj_ref ')' stmt
		{ Buffer b2, b3;
		  int tmp;

		  $$ = $5;
		  bufferWriteU8($$, SWFACTION_ENUMERATE);	

		  b2 = newBuffer();
		  bufferWriteSetRegister(b2, 0);
		  bufferWriteU8(b2, SWFACTION_PUSHDATA);
		  bufferWriteS16(b2, 1);
		  bufferWriteU8(b2, 2);
		  bufferWriteU8(b2, SWFACTION_NEWEQUALS);
		  bufferWriteU8(b2, SWFACTION_BRANCHIFTRUE);
		  bufferWriteS16(b2, 2);

		  b3 = newBuffer();
/* basically a lvalue could be used here rather than an ident !!! */
/* probably by using reg1 for the test rather than reg0 */
		  bufferWriteString(b3, $3, strlen($3)+1);
		  bufferWriteRegister(b3, 0);
		  bufferWriteU8(b3, SWFACTION_SETVARIABLE);
		  bufferConcat(b3, $7);
		  bufferWriteS16(b2, bufferLength(b3) + 5);
		  tmp = bufferLength(b2) + bufferLength(b3) + 5;
		  bufferConcat($$, b2);
		  bufferConcat($$, b3);
		  bufferWriteU8($$, SWFACTION_BRANCHALWAYS);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, -tmp); }

	| FOR '(' VAR identifier IN obj_ref ')' stmt
		{ Buffer b2, b3;
		  int tmp;

		  $$ = $6;
		  bufferWriteU8($$, SWFACTION_ENUMERATE);	

		  b2 = newBuffer();
		  bufferWriteSetRegister(b2, 0);
		  bufferWriteU8(b2, SWFACTION_PUSHDATA);
		  bufferWriteS16(b2, 1);
		  bufferWriteU8(b2, 2);
		  bufferWriteU8(b2, SWFACTION_NEWEQUALS);
		  bufferWriteU8(b2, SWFACTION_BRANCHIFTRUE);
		  bufferWriteS16(b2, 2);
		  // add size later

		  b3 = newBuffer();
		  bufferWriteString(b3, $4, strlen($4)+1);
		  bufferWriteRegister(b3, 0);
		  bufferWriteU8(b3, SWFACTION_VAREQUALS);
		  bufferConcat(b3, $8);
		  bufferWriteS16(b2, bufferLength(b3) + 5);
		  tmp = bufferLength(b2) + bufferLength(b3) + 5;
		  bufferConcat($$, b2);
		  bufferConcat($$, b3);
		  bufferWriteU8($$, SWFACTION_BRANCHALWAYS);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, -tmp); }
	;

assign_stmts_opt
	: /* empty */				{ $$ = NULL; }
	| assign_stmts
	;

cont_stmt
	: CONTINUE ';'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_BRANCHALWAYS);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, MAGIC_CONTINUE_NUMBER); }
	;

break_stmt
	: BREAK ';'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_BRANCHALWAYS);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, MAGIC_BREAK_NUMBER); }
	;

urlmethod
	: /* empty */		{ $$ = GETURL_METHOD_NOSEND; }

	| ',' GET		{ $$ = GETURL_METHOD_GET; }

	| ',' POST		{ $$ = GETURL_METHOD_POST; }

	| ',' STRING		{ if(strcmp($2, "GET") == 0)
				    $$ = GETURL_METHOD_GET;
				  else if(strcmp($2, "POST") == 0)
				    $$ = GETURL_METHOD_POST; }
	;

level
	: INTEGER
		{ char *lvlstring = malloc(12*sizeof(char));
		  sprintf(lvlstring, "_level%d", $1);
		  $$ = newBuffer();
		  bufferWriteString($$, lvlstring, strlen(lvlstring)+1);
		  free(lvlstring); }

	| expr
		{ $$ = newBuffer();
		  bufferWriteString($$, "_level", 7);
		  bufferConcat($$, $1);
		  bufferWriteU8($$, SWFACTION_STRINGCONCAT); }
	;


void_function_call
	: IDENTIFIER '(' expr_list ')'
		{ $$ = $3.buffer;
		  bufferWriteInt($$, $3.count);
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferWriteU8($$, SWFACTION_CALLFUNCTION);
		  bufferWriteU8($$, SWFACTION_POP);
		  free($1); }

	| TRACE '(' expr_or_obj ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_TRACE); }

	| GETURL '(' expr ',' expr urlmethod ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteU8($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, $6); }

	| LOADVARIABLES '(' expr ',' expr urlmethod ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteU8($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, 0xc0+$6); }

	| LOADVARIABLESNUM '(' expr ',' level urlmethod ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteU8($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, 0x80+$6); }

	| LOADMOVIE '(' expr ',' expr urlmethod ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteU8($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, 0x40+$6); }

	| LOADMOVIENUM '(' expr ',' level urlmethod ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteU8($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, $6); }

	| CALLFRAME '(' expr ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_CALLFRAME);
		  bufferWriteS16($$, 0); }

	/* startDrag(target, lock, [left, right, top, bottom]) */
	| STARTDRAG '(' expr ',' expr ')'
		{ $$ = newBuffer();
		  bufferWriteString($$, "0", 2); /* no constraint */
		  bufferConcat($$, $5);
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_STARTDRAGMOVIE); }

	| STARTDRAG '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
		{ $$ = newBuffer();
		  bufferConcat($$, $7);
		  bufferConcat($$, $11);
		  bufferConcat($$, $9);
		  bufferConcat($$, $13);
		  bufferWriteString($$, "1", 2); /* has constraint */
		  bufferConcat($$, $5);
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_STARTDRAGMOVIE); }

	| STOPDRAG '(' ')' /* no args */
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_STOPDRAGMOVIE); }

	/* duplicateMovieClip(target, new, depth) */
	| DUPLICATEMOVIECLIP '(' expr ',' expr ',' expr ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferConcat($$, $7);
		  bufferWriteInt($$, 16384); /* magic number */
		  bufferWriteU8($$, SWFACTION_ADD);
		  bufferWriteU8($$, SWFACTION_DUPLICATECLIP); }

	| REMOVEMOVIECLIP '(' expr ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_REMOVECLIP); }

	/* getURL2(url, window, [method]) */
	| GETURL '(' expr ',' expr ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteU8($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, GETURL_METHOD_NOSEND); }

	| GETURL '(' expr ',' expr ',' GETURL_METHOD ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteU8($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, $7); }

	| GETURL1 '(' STRING ',' STRING ')'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_GETURL);
		  bufferWriteS16($$, strlen($3) + strlen($5) + 2);
		  bufferWriteHardString($$, $3, strlen($3));
		  bufferWriteU8($$, 0);
		  bufferWriteHardString($$, $5, strlen($5));
		  bufferWriteU8($$, 0); }

	/* v3 actions */
	| NEXTFRAME '(' ')'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_NEXTFRAME); }
		
	| PREVFRAME '(' ')'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_PREVFRAME); }

	| PLAY '(' ')'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_PLAY); }

	| STOP '(' ')'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_STOP); }

	| STOPSOUNDS '(' ')'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_STOPSOUNDS); }

	| TOGGLEQUALITY '(' ')'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_TOGGLEQUALITY); }

	| GOTOFRAME '(' INTEGER ')'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_GOTOFRAME);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, $3); }

	| GOTOFRAME '(' STRING ')'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_GOTOLABEL);
		  bufferWriteS16($$, strlen($3)+1);
		  bufferWriteHardString($$, $3, strlen($3)+1);
		  free($3); }

	| GOTOFRAME '(' expr ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_GOTOEXPRESSION);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, 0); } /* XXX - and stop */

	| SETTARGET '(' STRING ')'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_SETTARGET);
		  bufferWriteS16($$, strlen($3)+1);
		  bufferWriteHardString($$, $3, strlen($3)+1);
		  free($3); }

	| SETTARGET '(' expr ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_SETTARGETEXPRESSION); }


	;


function_call
	: IDENTIFIER '(' expr_list ')'
		{ $$ = $3.buffer;
		  bufferWriteInt($$, $3.count);
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferWriteU8($$, SWFACTION_CALLFUNCTION);
		  free($1); }

	| EVAL '(' expr ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_GETVARIABLE); }

	| GETTIMER '(' ')'
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_GETTIMER); }

	| RANDOM '(' expr ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_RANDOM); }

	| LENGTH '(' expr_or_obj ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_STRINGLENGTH); }

	| INT '(' expr ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_INT); }

	| ORD '(' expr ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_ORD); }

	| CHR '(' expr ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_CHR); }

	| CONCAT '(' expr ',' expr ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteU8($$, SWFACTION_STRINGCONCAT); }

	| SUBSTRING '(' expr ',' expr ',' expr ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferConcat($$, $7);
		  bufferWriteU8($$, SWFACTION_SUBSTRING); }

	| TYPEOF '(' expr_or_obj ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_TYPEOF); }
	;


expr_list
	: /* empty */
		{ $$.buffer = newBuffer();
		  $$.count = 0; }

	| expr_or_obj
		{ $$.buffer = $1;
		  $$.count = 1; }

	/* goes backwards. rrgh. */
	| expr_list ',' expr_or_obj
		{ Buffer tmp = newBuffer();
		  bufferConcat(tmp, $3);
		  bufferConcat(tmp, $$.buffer);
		  $$.buffer = tmp;
		  ++$$.count;  }
	;

anon_function_decl
	: FUNCTION '(' formals_list ')' stmt
		{ $$ = newBuffer();
		  bufferWriteU8($$, SWFACTION_DEFINEFUNCTION);
		  bufferWriteS16($$, bufferLength($3.buffer) + 5);
		  bufferWriteU8($$, 0); /* empty function name */
		  bufferWriteS16($$, $3.count);
		  bufferConcat($$, $3.buffer);
		  bufferWriteS16($$, bufferLength($5));
		  bufferConcat($$, $5); }
	;

method_call
	: expr '.' identifier '(' expr_list ')'
		{ $$ = $5.buffer;
		  bufferWriteInt($$, $5.count);
		  bufferConcat($$, $1);
		  bufferWriteString($$, $3, strlen($3)+1);
		  bufferWriteU8($$, SWFACTION_CALLMETHOD);
		  free($3); }

	| expr '[' expr ']' '(' expr_list ')'
		{ $$ = $6.buffer;
		  bufferWriteInt($$, $6.count);
		  bufferConcat($$, $1);
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_CALLMETHOD); }
	;

objexpr
	: identifier ':' expr_or_obj
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferConcat($$, $3); }
	;

objexpr_list
	: objexpr
		{ $$.buffer = $1;
		  $$.count = 1; }

	| objexpr_list ',' objexpr
		{ bufferConcat($$.buffer, $3);
		  ++$$.count;  }
	;

assignop
	: "+="		{ $$ = SWFACTION_NEWADD; }
	| "-="		{ $$ = SWFACTION_SUBTRACT; }
	| "*="		{ $$ = SWFACTION_MULTIPLY; }
	| "/="		{ $$ = SWFACTION_DIVIDE; }
	| "&="		{ $$ = SWFACTION_BITWISEAND; }
	| "|="		{ $$ = SWFACTION_BITWISEOR; }
	| "^="		{ $$ = SWFACTION_BITWISEXOR; }
	| "<<="		{ $$ = SWFACTION_SHIFTLEFT; }
	| ">>="		{ $$ = SWFACTION_SHIFTRIGHT; }
	| ">>>="	{ $$ = SWFACTION_SHIFTRIGHT2; }
	;

incdecop
	: "++"		{ $$ = SWFACTION_INCREMENT; }
	| "--"		{ $$ = SWFACTION_DECREMENT; }
	;


/*
integer
	: '-' INTEGER %prec UMINUS	{ $$ = -$2; }
	| INTEGER			{ $$ = $1; }
	;

double
	: '-' DOUBLE %prec UMINUS	{ $$ = -$2; }
	| DOUBLE			{ $$ = $1; }
	;
*/

/* lvalue - things you can assign to */
lvalue
	: identifier
		{ $$.ident = newBuffer();
		  bufferWriteString($$.ident, $1, strlen($1)+1);
		  free($1);
		  $$.obj = 0;
		  $$.memexpr = 0; }

	| expr '.' identifier %prec '.'
		{ $$.obj = $1;
		  $$.ident = newBuffer();
		  bufferWriteString($$.ident, $3, strlen($3)+1);
		  $$.memexpr = 0; }

	| expr '[' expr ']' %prec '.'
		{ $$.obj = $1;
		  $$.memexpr = $3;
		  $$.ident = 0; }
	;

/* these leave a value on the stack */
expr
	: primary

	| '-' expr %prec UMINUS
		{ $$ = $2;
		  bufferWriteInt($2, -1);
		  bufferWriteU8($2, SWFACTION_MULTIPLY); }

	| '!' expr
		{ $$ = $2;
		  bufferWriteU8($2, SWFACTION_LOGICALNOT); }

	| expr "||" expr
		{ $$ = $1;
		  bufferWriteU8($$, SWFACTION_DUP);
		  bufferWriteU8($$, SWFACTION_BRANCHIFTRUE);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($3)+1);
		  bufferWriteU8($$, SWFACTION_POP);
		  bufferConcat($$, $3); }

	| expr "&&" expr
		{ $$ = $1;
		  bufferWriteU8($$, SWFACTION_DUP);
		  bufferWriteU8($$, SWFACTION_LOGICALNOT);
		  bufferWriteU8($$, SWFACTION_BRANCHIFTRUE);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($3)+1);
		  bufferWriteU8($$, SWFACTION_POP);
		  bufferConcat($$, $3); }

	| expr '*' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_MULTIPLY); }

	| expr '/' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_DIVIDE); }

	| expr '%' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_MODULO); }

	| expr '+' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_NEWADD); }

	| expr '-' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_SUBTRACT); }

	| expr '&' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_BITWISEAND); }

	| expr '|' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_BITWISEOR); }

	| expr '^' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_BITWISEXOR); }

	| expr '<' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_NEWLESSTHAN); }

	| expr '>' expr
		{ $$ = $3;
		  bufferConcat($$, $1);
		  bufferWriteU8($$, SWFACTION_NEWLESSTHAN); }

	| expr "<=" expr
		{ $$ = $3;
		  bufferConcat($$, $1);
		  bufferWriteU8($$, SWFACTION_NEWLESSTHAN);
		  bufferWriteU8($$, SWFACTION_LOGICALNOT); }

	| expr ">=" expr
		{ bufferConcat($1, $3);
		  bufferWriteU8($1, SWFACTION_NEWLESSTHAN);
		  bufferWriteU8($1, SWFACTION_LOGICALNOT); }

	| expr "==" expr
		{ bufferConcat($1, $3);
		  bufferWriteU8($1, SWFACTION_NEWEQUALS); }

	| expr "!=" expr
		{ bufferConcat($1, $3);
		  bufferWriteU8($1, SWFACTION_NEWEQUALS);
		  bufferWriteU8($1, SWFACTION_LOGICALNOT); }

	| expr "<<" expr
		{ bufferConcat($1, $3);
		  bufferWriteU8($1, SWFACTION_SHIFTLEFT); }

	| expr ">>" expr
		{ bufferConcat($1, $3);
		  bufferWriteU8($1, SWFACTION_SHIFTRIGHT); }

	| expr ">>>" expr
		{ bufferConcat($1, $3);
		  bufferWriteU8($1, SWFACTION_SHIFTRIGHT2); }

	| expr '?' expr ':' expr
		{ bufferWriteU8($1, SWFACTION_BRANCHIFTRUE);
		  bufferWriteS16($1, 2);
		  bufferWriteS16($1, bufferLength($5)+5);
		  bufferConcat($1, $5);
		  bufferWriteU8($1, SWFACTION_BRANCHALWAYS);
		  bufferWriteS16($1, 2);
		  bufferWriteS16($1, bufferLength($3));
		  bufferConcat($1, $3); }

	| lvalue '=' expr_or_obj
		{ if($1.obj) /* obj[memexpr] or obj.ident */
		  {
		    $$ = $1.obj;

		    if($1.ident)
		      bufferConcat($$, $1.ident);
		    else
		      bufferConcat($$, $1.memexpr);

		    bufferConcat($$, $3);
		    bufferWriteSetRegister($$, 0);
		    bufferWriteU8($$, SWFACTION_SETMEMBER);
		    bufferWriteRegister($$, 0);
		  }
		  else /* just ident */
		  {
		    $$ = $3;
		    bufferWriteU8($$, SWFACTION_DUP);
		    bufferConcat($$, $1.ident);
		    bufferWriteU8($$, SWFACTION_SWAP);
		    bufferWriteU8($$, SWFACTION_SETVARIABLE);
		  }
/* tricky case missing here: lvalue ASSIGN expr */
/* like in x = y += z; */
		}
	;

expr_or_obj
	: expr

	| NEW identifier
		{ $$ = newBuffer();
		  bufferWriteInt($$, 0);
		  bufferWriteString($$, $2, strlen($2)+1);
		  bufferWriteU8($$, SWFACTION_NEW); }

	| NEW identifier '(' expr_list ')'
		{ $$ = $4.buffer;
		  bufferWriteInt($$, $4.count);
		  bufferWriteString($$, $2, strlen($2)+1);
		  bufferWriteU8($$, SWFACTION_NEW); }

	| '[' expr_list ']'
		{ $$ = $2.buffer;
		  bufferWriteInt($$, $2.count);
		  bufferWriteU8($$, SWFACTION_INITARRAY); }

	| emptybraces
		{ $$ = newBuffer();
		  bufferWriteInt($$, 0);
		  bufferWriteU8($$, SWFACTION_INITOBJECT); }

	| '{' objexpr_list '}'
		{ $$ = $2.buffer;
		  bufferWriteInt($$, $2.count);
		  bufferWriteU8($$, SWFACTION_INITOBJECT); }

	;

primary
	: function_call

	| anon_function_decl

	| method_call

	| lvalue
		{ if($1.obj)
		  {
		    $$ = $1.obj;

		    if($1.ident)
		      bufferConcat($$, $1.ident);
		    else
		      bufferConcat($$, $1.memexpr);

		    bufferWriteU8($$, SWFACTION_GETMEMBER);
		  }
		  else
		  {
		    $$ = $1.ident;
		    bufferWriteU8($$, SWFACTION_GETVARIABLE);
		  }
		}

	| incdecop lvalue %prec "++"
		{ if($2.obj)
		  {
		    if($2.ident)	// expr . identifier
		    {
		      $$ = $2.obj;
		      bufferWriteU8($$, SWFACTION_DUP);	      /* a, a */
		      bufferWriteBuffer($$, $2.ident);        /* a, a, i */
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i, a */
		      bufferConcat($$, $2.ident);             /* a, i, a, i */
		      bufferWriteU8($$, SWFACTION_GETMEMBER);
		      bufferWriteU8($$, $1);
		      bufferWriteSetRegister($$, 0);
		      bufferWriteU8($$, SWFACTION_SETMEMBER); /* a.i = a.i+1 */
		      bufferWriteRegister($$, 0);	      /* a.i+1 */
		    }
		    else	// expr [ expr ]
		    {
		      $$ = $2.memexpr;			      /* i */
		      bufferConcat($$, $2.obj);		      /* i, a */
		      bufferWriteSetRegister($$, 0);	/* ($2.memexpr can use reg0) */
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i */
		      bufferWriteU8($$, SWFACTION_DUP);	      /* a, i, i */
		      bufferWriteRegister($$, 0);	      /* a, i, i, a */
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i, a, i */
		      bufferWriteU8($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		      bufferWriteU8($$, $1);		      /* a, i, a[i]+1 */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteU8($$, SWFACTION_SETMEMBER); /* a[i] = a[i]+1 */
		      bufferWriteRegister($$, 0);	      /* a[i]+1 */
		    }
		  }
		  else	// identifier
		  {
		    $$ = newBuffer();
		    bufferWriteBuffer($$, $2.ident);
		    bufferWriteU8($$, SWFACTION_GETVARIABLE);
		    bufferWriteU8($$, $1);
		    bufferWriteU8($$, SWFACTION_DUP);
		    bufferConcat($$, $2.ident);
		    bufferWriteU8($$, SWFACTION_SWAP);
		    bufferWriteU8($$, SWFACTION_SETVARIABLE);
		  }
		}

	| lvalue incdecop %prec POSTFIX
		{ if($1.obj)
		  {
		    if($1.ident)
		    {
		      $$ = $1.obj;	                      /* a */
		      bufferWriteU8($$, SWFACTION_DUP);	      /* a, a */
		      bufferWriteBuffer($$, $1.ident);        /* a, a, i */
		      bufferWriteU8($$, SWFACTION_GETMEMBER); /* a, a.i */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a.i, a */
		      bufferConcat($$, $1.ident);             /* a.i, a, i */
		      bufferWriteRegister($$, 0);             /* a.i, a, i, a.i */
		      bufferWriteU8($$, $2);		      /* a.i, a, i, a.i+1 */
		      bufferWriteU8($$, SWFACTION_SETMEMBER);
		    }
		    else
		    {
		      $$ = $1.memexpr;
		      bufferConcat($$, $1.obj);               /* i, a */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i */
		      bufferWriteU8($$, SWFACTION_DUP);	      /* a, i, i */
		      bufferWriteRegister($$, 0);             /* a, i, i, a */
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i, a, i */
		      bufferWriteU8($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteU8($$, $2);		      /* a, i, a[i]+1 */
		      bufferWriteU8($$, SWFACTION_SETMEMBER);
		      bufferWriteRegister($$, 0);             /* a[i] */
		    }
		  }
		  else
		  {
		    $$ = newBuffer();
		    bufferWriteBuffer($$, $1.ident);
		    bufferWriteU8($$, SWFACTION_GETVARIABLE);
		    bufferWriteU8($$, SWFACTION_DUP);
		    bufferWriteU8($$, $2);
		    bufferConcat($$, $1.ident);
		    bufferWriteU8($$, SWFACTION_SWAP);
		    bufferWriteU8($$, SWFACTION_SETVARIABLE);
		  }
		}

	| '(' expr ')'
		{ $$ = $2; }

	| '-' INTEGER %prec UMINUS
		{ $$ = newBuffer();
		  bufferWriteInt($$, -$2); }

	| INTEGER
		{ $$ = newBuffer();
		  bufferWriteInt($$, $1); }

	| '-' DOUBLE %prec UMINUS
		{ $$ = newBuffer();
		  bufferWriteDouble($$, -$2); }

	| DOUBLE
		{ $$ = newBuffer();
		  bufferWriteDouble($$, $1); }

	| BOOLEAN
		{ $$ = newBuffer();
		  bufferWriteBoolean($$, $1); }

	| NULLVAL
		{ $$ = newBuffer();
		  bufferWriteNull($$); }

	| STRING
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  free($1); }
	;

init_vars
	: init_var

	| init_vars ',' init_var
		{ $$ = $1;
		  bufferConcat($$, $3); }
	;

init_var
	: identifier '=' expr_or_obj
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_VAREQUALS); }

	| identifier
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferWriteU8($$, SWFACTION_VAR); }
	;

assign_stmt
	: ASM '{'
		{ asmBuffer = newBuffer(); }
	  opcode_list '}'
		{ $$ = asmBuffer; }

	| VAR init_vars
		{ $$ = $2; }

	| void_function_call

	| function_call
		{ $$ = $1;
		  bufferWriteU8($$, SWFACTION_POP); }

	| method_call
		{ $$ = $1;
		  bufferWriteU8($$, SWFACTION_POP); }

	| incdecop lvalue %prec INCR
		{ if($2.obj)
		  {
		    if($2.ident)
		    {
		      $$ = $2.obj;		              /* a */
		      bufferWriteU8($$, SWFACTION_DUP);	      /* a, a */
		      bufferWriteBuffer($$, $2.ident);	      /* a, a, i */
		      bufferWriteU8($$, SWFACTION_GETMEMBER); /* a, a.i */
		      bufferWriteU8($$, $1);		      /* a, a.i+1 */
		      bufferConcat($$, $2.ident);	      /* a, a.i+1, i */
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i, a.i+1 */
		      bufferWriteU8($$, SWFACTION_SETMEMBER); /* a.i = a.i+1 */
		    }
		    else
		    {
		      /* weird contortions so that $2.memexpr can use reg 0 */
		      $$ = $2.memexpr;			      /* i */
		      bufferConcat($$, $2.obj);		      /* i, a */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i */
		      bufferWriteU8($$, SWFACTION_DUP);	      /* a, i, i */
		      bufferWriteRegister($$, 0);	      /* a, i, i, a */
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i, a, i */
		      bufferWriteU8($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		      bufferWriteU8($$, $1);		      /* a, i, a[i]+1 */
		      bufferWriteU8($$, SWFACTION_SETMEMBER); /* a[i] = a[i]+1 */
		    }
		  }
		  else
		  {
		    $$ = $2.ident;
		    bufferWriteU8($$, SWFACTION_DUP);
		    bufferWriteU8($$, SWFACTION_GETVARIABLE);
		    bufferWriteU8($$, $1);
		    bufferWriteU8($$, SWFACTION_SETVARIABLE);
		  }
		}

	| lvalue incdecop %prec POSTFIX
		{ if($1.obj)
		  {
		    if($1.ident)
		    {
		      $$ = $1.obj;			      /* a */
		      bufferWriteU8($$, SWFACTION_DUP);       /* a, a */
		      bufferWriteBuffer($$, $1.ident);	      /* a, a, i */
		      bufferWriteU8($$, SWFACTION_GETMEMBER); /* a, a.i */
		      bufferWriteU8($$, $2);                  /* a, a.i+1 */
		      bufferConcat($$, $1.ident);             /* a, a.i+1, i */
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i, a.i+1 */
		      bufferWriteU8($$, SWFACTION_SETMEMBER); /* a.i = a.i+1 */
		    }
		    else
		    {
		      /* weird contortions so that $1.memexpr can use reg 0 */
		      $$ = $1.memexpr;	/* i */
		      bufferConcat($$, $1.obj);		      /* i, a */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i */
		      bufferWriteU8($$, SWFACTION_DUP);       /* a, i, i */
		      bufferWriteRegister($$, 0);             /* a, i, i, a */
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i, a, i */
		      bufferWriteU8($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		      bufferWriteU8($$, $2);                  /* a, i, a[i]+1 */
		      bufferWriteU8($$, SWFACTION_SETMEMBER); /* a[i] = a[i]+1 */
		    }
		  }
		  else
		  {
		    $$ = $1.ident;
		    bufferWriteU8($$, SWFACTION_DUP);	
		    bufferWriteU8($$, SWFACTION_GETVARIABLE);
		    bufferWriteU8($$, $2);
		    bufferWriteU8($$, SWFACTION_SETVARIABLE);
		  }
		}

	| lvalue '=' expr_or_obj
		{ if($1.obj)
		  {
		    $$ = $1.obj;

		    if($1.ident)
		      bufferConcat($$, $1.ident);
		    else
		      bufferConcat($$, $1.memexpr);

		    bufferConcat($$, $3);
		    bufferWriteU8($$, SWFACTION_SETMEMBER);
		  }
		  else
		  {
		    $$ = $1.ident;
		    bufferConcat($$, $3);
		    bufferWriteU8($$, SWFACTION_SETVARIABLE);
		  }
		}

	| lvalue assignop expr
		{ if($1.obj)
		  {
		    if($1.ident)
		    {
		      $$ = $1.obj;			      /* a */
		      bufferWriteU8($$, SWFACTION_DUP);	      /* a, a */
		      bufferWriteBuffer($$, $1.ident);	      /* a, a, i */
		      bufferWriteU8($$, SWFACTION_GETMEMBER); /* a, a.i */
		      bufferConcat($$, $3);		      /* a, a.i, v */
		      bufferWriteU8($$, $2);		      /* a, a.i+v */
		      bufferConcat($$, $1.ident);	      /* a, a.i+v, i */
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i, a.i+v */
		      bufferWriteU8($$, SWFACTION_SETMEMBER); /* a.i = a.i+v */
		    }
		    else
		    {
		      $$ = $1.memexpr;			      /* i */
		      bufferConcat($$, $1.obj);		      /* i, a */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i */
		      bufferWriteU8($$, SWFACTION_DUP);	      /* a, i, i */
		      bufferWriteRegister($$, 0);	      /* a, i, i, a */
		      bufferWriteU8($$, SWFACTION_SWAP);      /* a, i, a, i */
		      bufferWriteU8($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		      bufferConcat($$, $3);		      /* a, i, a[i], v */
		      bufferWriteU8($$, $2);		      /* a, i, a[i]+v */
		      bufferWriteU8($$, SWFACTION_SETMEMBER); /* a[i] = a[i]+v */
		    }
		  }
		  else
		  {
		    $$ = $1.ident;
		    bufferWriteU8($$, SWFACTION_DUP);
		    bufferWriteU8($$, SWFACTION_GETVARIABLE);
		    bufferConcat($$, $3);
		    bufferWriteU8($$, $2);
		    bufferWriteU8($$, SWFACTION_SETVARIABLE);
		  }
		}
	;

/* assembler stuff */

opcode_list
	: opcode
	| opcode_list opcode	{ $$ = $1 + $2; }
	;

with
	: WITH
				{ $$ = bufferWriteU8(asmBuffer,
						     SWFACTION_WITH); }
	  opcode_list END	{ $$ = $<len>2 + $3;
				  bufferPatchLength(asmBuffer, $3); }
	;

push_item
	: STRING		{ $$ = bufferWriteConstantString(asmBuffer, $1,
								 strlen($1)+1); }

	| INTEGER		{ bufferWriteU8(asmBuffer, PUSH_INT);
				  $$ = bufferWriteInt(asmBuffer, $1)+1; }

	| DOUBLE		{ bufferWriteU8(asmBuffer, PUSH_DOUBLE);
				  $$ = bufferWriteDouble(asmBuffer, $1)+1; }

	| BOOLEAN		{ bufferWriteU8(asmBuffer, PUSH_BOOLEAN);
				  $$ = bufferWriteU8(asmBuffer, $1)+1; }

	| NULLVAL		{ $$ = bufferWriteU8(asmBuffer, PUSH_NULL); }

	| REGISTER		{ bufferWriteU8(asmBuffer, PUSH_REGISTER);
				  $$ = bufferWriteU8(asmBuffer,
						     (char)atoi($1))+1; }
	;


push_list
	: push_item			{ $$ = $1; }
	| push_list ',' push_item	{ $$ += $3; }
	;

opcode
	: PUSH 			{ $$ = bufferWriteU8(asmBuffer,
						     SWFACTION_PUSHDATA);
				  $$ += bufferWriteS16(asmBuffer, 0); }
	  push_list		{ $$ = $<len>2 + $3;
				  bufferPatchLength(asmBuffer, $3); }

	| with

	| SETREGISTER REGISTER
				{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_SETREGISTER);
				  $$ += bufferWriteS16(asmBuffer, 1);
				  $$ += bufferWriteU8(asmBuffer,
						      (char)atoi($2)); }
	/* no args */
	| CALLFUNCTION		{ $$ = bufferWriteU8(asmBuffer,
						     SWFACTION_CALLFUNCTION); }
	| RETURN		{ $$ = bufferWriteU8(asmBuffer,
						     SWFACTION_RETURN); }
	| CALLMETHOD		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_CALLMETHOD); }
	| AND			{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_BITWISEAND); }
	| OR			{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_BITWISEOR); }
	| XOR			{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_BITWISEXOR); }
	| MODULO		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_MODULO); }
	| ADD			{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_NEWADD); }
	| LESSTHAN		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_NEWLESSTHAN); }
	| EQUALS		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_NEWEQUALS); }
	| INC			{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_INCREMENT); }
	| DEC			{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_DECREMENT); }
	| TYPEOF		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_TYPEOF); }
	| ENUMERATE		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_ENUMERATE); }
	| DELETE		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_DELETE); }
	| NEW			{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_NEW); }
	| INITARRAY		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_INITARRAY); }
	| INITOBJECT		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_INITOBJECT); }
	| GETMEMBER		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_GETMEMBER); }
	| SETMEMBER		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_SETMEMBER); }
	| SHIFTLEFT		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_SHIFTLEFT); }
	| SHIFTRIGHT		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_SHIFTRIGHT); }
	| SHIFTRIGHT2		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_SHIFTRIGHT2); }
	| VAR			{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_VAR); }
	| VAREQUALS		{ $$ = bufferWriteU8(asmBuffer, 
						     SWFACTION_VAREQUALS); }

	/* f4 ops */
	| OLDADD		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_ADD); }
	| SUBTRACT		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_SUBTRACT); }
	| MULTIPLY		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_MULTIPLY); }
	| DIVIDE		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_DIVIDE); }
	| OLDEQUALS		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_EQUAL); }
	| OLDLESSTHAN		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_LESSTHAN); }
	| LOGICALAND		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_LOGICALAND); }
	| LOGICALOR		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_LOGICALOR); }
	| NOT			{ $$ = bufferWriteU8(asmBuffer, SWFACTION_LOGICALNOT); }
	| STRINGEQ		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_STRINGEQ); }
	| STRINGLENGTH		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_STRINGLENGTH); }
	| SUBSTRING		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_SUBSTRING); }
	| INT			{ $$ = bufferWriteU8(asmBuffer, SWFACTION_INT); }
	| DUP			{ $$ = bufferWriteU8(asmBuffer, SWFACTION_DUP); }
	| SWAP			{ $$ = bufferWriteU8(asmBuffer, SWFACTION_SWAP); }
	| POP			{ $$ = bufferWriteU8(asmBuffer, SWFACTION_POP); }
	| GETVARIABLE		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_GETVARIABLE); }
	| SETVARIABLE		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_SETVARIABLE); }
	| SETTARGETEXPRESSION	{ $$ = bufferWriteU8(asmBuffer, SWFACTION_SETTARGETEXPRESSION); }
	| CONCAT		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_STRINGCONCAT); }
	| DUPLICATEMOVIECLIP	{ $$ = bufferWriteU8(asmBuffer, SWFACTION_DUPLICATECLIP); }
	| REMOVEMOVIECLIP	{ $$ = bufferWriteU8(asmBuffer, SWFACTION_REMOVECLIP); }
	| TRACE			{ $$ = bufferWriteU8(asmBuffer, SWFACTION_TRACE); }
	| STRINGLESSTHAN	{ $$ = bufferWriteU8(asmBuffer, SWFACTION_STRINGCOMPARE); }
	| RANDOM		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_RANDOM); }
	| MBLENGTH		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_MBLENGTH); }
	| ORD			{ $$ = bufferWriteU8(asmBuffer, SWFACTION_ORD); }
	| CHR			{ $$ = bufferWriteU8(asmBuffer, SWFACTION_CHR); }
	| GETTIMER		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_GETTIMER); }
	| MBSUBSTRING		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_MBSUBSTRING); }
	| MBORD			{ $$ = bufferWriteU8(asmBuffer, SWFACTION_MBORD); }
	| MBCHR			{ $$ = bufferWriteU8(asmBuffer, SWFACTION_MBCHR); }

	/* with args */
	| BRANCHALWAYS STRING	{ $$ = bufferWriteU8(asmBuffer, SWFACTION_BRANCHALWAYS);
				  $$ += bufferWriteS16(asmBuffer, 2);
				  $$ += bufferBranchTarget(asmBuffer, $2); }

	| BRANCHIFTRUE STRING	{ $$ = bufferWriteU8(asmBuffer, SWFACTION_BRANCHIFTRUE);
				  $$ += bufferWriteS16(asmBuffer, 2);
				  $$ += bufferBranchTarget(asmBuffer, $2); }
	;

%%
