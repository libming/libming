
%start program

%{

#include <time.h>
#include <string.h>
#include "compile.h"
#include "assembler.h"

#define YYPARSE_PARAM buffer

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
  SETTARGETEXPRESSION,  DUPLICATECLIP, REMOVECLIP, STARTDRAGMOVIE,
  STOPDRAGMOVIE, STRINGLESSTHAN, MBLENGTH, MBSUBSTRING, MBORD, MBCHR,
  BRANCHALWAYS, BRANCHIFTRUE, GETURL2, POST, GET

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
%token INC "++"
%token DEC "--"
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
%left '*' '/'
%right "++" "--"
%right '!' '~' UMINUS
%right POSTFIX
%left '.' '[' ']'


%type <action> program, code
%type <action> stmt, stmts
%type <action> if_stmt, iter_stmt, cont_stmt, break_stmt, return_stmt
%type <action> switch_stmt, switch_cases, switch_case
%type <action> anon_function_decl, function_decl, function_decls
%type <action> void_function_call, function_call, method_call
%type <action> assign_stmt, assign_stmts, assign_stmts_opt
%type <action> expr, objexpr, expr_opt, pf_expr
%type <action> emptybraces

%type <exprlist> expr_list, objexpr_list, formals_list

%type <op> assignop, incdecop

%type <str> identifier

%type <len> opcode, opcode_list, push_item, with, push_list

/*
%type <intVal> integer
%type <doubleVal> double
*/
%%

/* rules */

program
	: code
		{ Buffer b = newBuffer();
		  bufferWriteConstants(b);
		  bufferConcat(b, $1);
		  *((Buffer *)buffer) = b; }

code
        : stmts

	| function_decls stmts
		{ $$ = $1;
		  bufferConcat($$, $2); }

	| function_decls
	;

function_decls
	: function_decl
	| function_decls function_decl
		{ bufferConcat($1, $2); }
	;

stmts
	: stmt
                { $$ = $1; }

	| stmts stmt
                { $$ = $1;
		  bufferConcat($$, $2); }

	| stmts function_decl
		{ $$ = $2;
		  bufferConcat($$, $1); }

	| function_decl
		{ $$ = $1; }
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
		  bufferResolveJumps($$); }
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
	| DUPLICATECLIP	{ $$ = strdup("duplicateclip"); }
	| REMOVECLIP	{ $$ = strdup("removeclip"); }
	| STARTDRAGMOVIE	{ $$ = strdup("startdragmovie"); }
	| STOPDRAGMOVIE	{ $$ = strdup("stopdragmovie"); }
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
	;

formals_list
	: identifier
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
                { bufferWriteU8($5, SWFACTION_LOGICALNOT);
		  bufferWriteU8($5, SWFACTION_BRANCHIFTRUE);
		  bufferWriteS16($5, 2);
		  bufferWriteS16($5, bufferLength($9)+bufferLength($7)+5);
		  bufferConcat($5, $9);
		  bufferConcat($5, $7);
		  bufferWriteU8($5, SWFACTION_BRANCHALWAYS);
		  bufferWriteS16($5, 2);
		  bufferWriteS16($5, -(bufferLength($5)+2));
		  bufferResolveJumps($5);
		  $$ = $3;
		  if(!$$) $$ = newBuffer();
		  bufferConcat($$, $5);
		}

	| FOR '(' identifier IN expr ')' stmt
		{ }
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

void_function_call
	: IDENTIFIER '(' expr_list ')'
		{ $$ = $3.buffer;
		  bufferWriteInt($$, $3.count);
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferWriteU8($$, SWFACTION_CALLFUNCTION);
		  bufferWriteU8($$, SWFACTION_POP);
		  free($1); }

	| TRACE '(' expr ')'
		{ $$ = $3;
		  bufferWriteU8($$, SWFACTION_TRACE); }

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

	| LENGTH '(' expr ')'
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

	| SUBSTR '(' expr ',' expr ',' expr ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferConcat($$, $7);
		  bufferWriteU8($$, SWFACTION_SUBSTRING); }
	;

expr_list
	: /* empty */
		{ $$.buffer = newBuffer();
		  $$.count = 0; }

	| expr
		{ $$.buffer = $1;
		  $$.count = 1; }

	/* goes backwards. rrgh. */
	| expr_list ',' expr
		{ Buffer tmp = newBuffer();
		  bufferConcat(tmp, $3);
		  bufferConcat(tmp, $$.buffer);
		  $$.buffer = tmp;
		  ++$$.count;  }
	;

pf_expr
	: identifier incdecop %prec POSTFIX
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferWriteU8($$, SWFACTION_GETVARIABLE);
		  bufferWriteU8($$, SWFACTION_DUP);
		  bufferWriteU8($$, $2);
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferWriteU8($$, SWFACTION_SWAP);
		  bufferWriteU8($$, SWFACTION_SETVARIABLE); }
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
	: identifier ':' expr
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
	: "++"	{ $$ = SWFACTION_INCREMENT; }
	| "--"	{ $$ = SWFACTION_DECREMENT; }
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

/* these leave a value on the stack */
expr
	: function_call

	| anon_function_decl

	| method_call

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

	| expr "&&" expr
		{ $$ = $1;
		  bufferWriteU8($$, SWFACTION_DUP);
		  bufferWriteU8($$, SWFACTION_BRANCHIFTRUE);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($3)+1);
		  bufferWriteU8($$, SWFACTION_POP);
		  bufferConcat($$, $3); }

	| expr "||" expr
		{ $$ = $1;
		  bufferWriteU8($$, SWFACTION_DUP);
		  bufferWriteU8($$, SWFACTION_LOGICALNOT);
		  bufferWriteU8($$, SWFACTION_BRANCHIFTRUE);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($3)+1);
		  bufferWriteU8($$, SWFACTION_POP);
		  bufferConcat($$, $3); }

	| '(' expr ')'
		{ $$ = $2; }

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

	| identifier
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferWriteU8($$, SWFACTION_GETVARIABLE);
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

	| incdecop expr '.' identifier
		{ $$ = $2;				    /* a */
		  bufferWriteU8($$, SWFACTION_DUP);	    /* a, a */
		  bufferWriteString($$, $4, strlen($4)+1);  /* a, a, i */
		  bufferWriteU8($$, SWFACTION_SWAP);	    /* a, i, a */
		  bufferWriteString($$, $4, strlen($4)+1);  /* a, i, a, i */
		  bufferWriteU8($$, SWFACTION_GETMEMBER);
		  bufferWriteU8($$, $1);
		  bufferWriteSetRegister($$, 0);
		  bufferWriteU8($$, SWFACTION_SETMEMBER);   /* a.i = a.i+1 */
		  bufferWriteRegister($$, 0);	    /* a.i+1 */
		  free($4); }

	| incdecop expr '[' expr ']'
		{ $$ = $4;				/* i */
		  bufferConcat($$, $2);			/* i, a */
		  bufferWriteSetRegister($$, 0);	/* ($4 can use reg0) */
		  bufferWriteU8($$, SWFACTION_SWAP);	/* a, i */
		  bufferWriteU8($$, SWFACTION_DUP);	/* a, i, i */
		  bufferWriteRegister($$, 0);		/* a, i, i, a */
		  bufferWriteU8($$, SWFACTION_SWAP);	/* a, i, a, i */
		  bufferWriteU8($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		  bufferWriteU8($$, $1);		/* a, i, a[i]+1 */
		  bufferWriteSetRegister($$, 0);
		  bufferWriteU8($$, SWFACTION_SETMEMBER); /* a[i] = a[i]+1 */
		  bufferWriteRegister($$, 0);		/* a[i]+1 */ }

	| incdecop identifier
		{ $$ = newBuffer();
		  bufferWriteString($$, $2, strlen($2)+1);
		  bufferWriteU8($$, SWFACTION_GETVARIABLE);
		  bufferWriteU8($$, $1);
		  bufferWriteU8($$, SWFACTION_DUP);
		  bufferWriteString($$, $2, strlen($2)+1);
		  bufferWriteU8($$, SWFACTION_SWAP);
		  bufferWriteU8($$, SWFACTION_SETVARIABLE);
		  free($2); }

	| '-' expr
		{ $$ = $2;
		  bufferWriteString($2, "-1", 3);
		  bufferWriteU8($2, SWFACTION_MULTIPLY); }

	| '!' expr
		{ $$ = $2;
		  bufferWriteU8($2, SWFACTION_LOGICALNOT); }

	| identifier '=' expr
		{ $$ = newBuffer();
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_DUP);
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferWriteU8($$, SWFACTION_SWAP);
		  bufferWriteU8($$, SWFACTION_SETVARIABLE);
		  free($1); }

	| expr '*' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_MULTIPLY); }

	| expr '/' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_DIVIDE); }

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
	;


assign_stmt
	: pf_expr

	| ASM '{'
		{ asmBuffer = newBuffer(); }
	  opcode_list '}'
		{ $$ = asmBuffer; }

	| VAR identifier '=' expr
		{ $$ = $4;
		  bufferWriteString($$, $2, strlen($2)+1);
		  bufferWriteU8($$, SWFACTION_VAREQUALS); }

	| VAR identifier
		{ $$ = newBuffer();
		  bufferWriteString($$, $2, strlen($2)+1);
		  bufferWriteU8($$, SWFACTION_VAR); }

	| void_function_call

	| function_call
		{ $$ = $1;
		  bufferWriteU8($$, SWFACTION_POP); }

	| method_call
		{ $$ = $1;
		  bufferWriteU8($$, SWFACTION_POP); }

	| incdecop identifier
		{ $$ = newBuffer();
		  bufferWriteString($$, $2, strlen($2)+1);
		  bufferWriteString($$, $2, strlen($2)+1);
		  bufferWriteU8($$, SWFACTION_GETVARIABLE);
		  bufferWriteU8($$, $1);
		  bufferWriteU8($$, SWFACTION_SETVARIABLE);
		  free($2); }

	| incdecop expr '.' identifier
		{ $$ = newBuffer();
		  bufferWriteBuffer($$, $2);		    /* a */
		  bufferWriteU8($$, SWFACTION_DUP);	    /* a, a */
		  bufferWriteString($$, $4, strlen($4)+1);  /* a, a, i */
		  bufferWriteU8($$, SWFACTION_GETMEMBER);   /* a, a.i */
		  bufferWriteU8($$, $1);		    /* a, a.i+1 */
		  bufferWriteString($$, $4, strlen($4)+1);  /* a, a.i+1, i */
		  bufferWriteU8($$, SWFACTION_SWAP);	    /* a, i, a.i+1 */
		  bufferWriteU8($$, SWFACTION_SETMEMBER);   /* a.i = a.i+1 */
		  free($4); }

	| incdecop expr '[' expr ']'
		  /* weird contortions so that $4 can use reg 0.. */
		{ $$ = $4;				  /* i */
		  bufferConcat($$, $2);			  /* i, a */
		  bufferWriteSetRegister($$, 0);
		  bufferWriteU8($$, SWFACTION_SWAP);	  /* a, i */
		  bufferWriteU8($$, SWFACTION_DUP);	  /* a, i, i */
		  bufferWriteRegister($$, 0);		  /* a, i, i, a */
		  bufferWriteU8($$, SWFACTION_SWAP);	  /* a, i, a, i */
		  bufferWriteU8($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		  bufferWriteU8($$, $1);		  /* a, i, a[i]+1 */
		  bufferWriteU8($$, SWFACTION_SETMEMBER); /* a[i] = a[i]+1 */ }

	| identifier '=' expr
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferConcat($$, $3);
		  bufferWriteU8($$, SWFACTION_SETVARIABLE);
		  free($1); }

	| identifier assignop expr
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferWriteU8($$, SWFACTION_GETVARIABLE);
		  bufferConcat($$, $3);
		  bufferWriteU8($$, $2);
		  bufferWriteU8($$, SWFACTION_SETVARIABLE);
		  free($1); }

	| expr '.' identifier '=' expr
                { $$ = $1;
		  bufferWriteString($$, $3, strlen($3)+1);
		  bufferConcat($$, $5);
		  bufferWriteU8($$, SWFACTION_SETMEMBER);
		  free($3); }

	| expr '.' identifier assignop expr
		{ $$ = newBuffer();
		  bufferWriteBuffer($$, $1);		    /* a */
		  bufferWriteU8($$, SWFACTION_DUP);	    /* a, a */
		  bufferWriteString($$, $3, strlen($3)+1);  /* a, a, i */
		  bufferWriteU8($$, SWFACTION_GETMEMBER);   /* a, a.i */
		  bufferConcat($$, $5);			    /* a, a.i, v */
		  bufferWriteU8($$, $4);		    /* a, a.i+v */
		  bufferWriteString($$, $3, strlen($3)+1);  /* a, a.i+v, i */
		  bufferWriteU8($$, SWFACTION_SWAP);	    /* a, i, a.i+v */
		  bufferWriteU8($$, SWFACTION_SETMEMBER);   /* a.i = a.i+v */
		  free($3); }

	| expr '[' expr ']' '=' expr
                { $$ = $1;
		  bufferConcat($$, $3);
		  bufferConcat($$, $6);
		  bufferWriteU8($$, SWFACTION_SETMEMBER); }

	| expr '[' expr ']' assignop expr
		{ $$ = $3;				  /* i */
		  bufferConcat($$, $1);			  /* i, a */
		  bufferWriteSetRegister($$, 0);
		  bufferWriteU8($$, SWFACTION_SWAP);	  /* a, i */
		  bufferWriteU8($$, SWFACTION_DUP);	  /* a, i, i */
		  bufferWriteRegister($$, 0);		  /* a, i, i, a */
		  bufferWriteU8($$, SWFACTION_SWAP);	  /* a, i, a, i */
		  bufferWriteU8($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		  bufferConcat($$, $6);			  /* a, i, a[i], v */
		  bufferWriteU8($$, $5);		  /* a, i, a[i]+v */
		  bufferWriteU8($$, SWFACTION_SETMEMBER); /* a[i] = a[i]+v */ }
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
	| DUPLICATECLIP		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_DUPLICATECLIP); }
	| REMOVECLIP		{ $$ = bufferWriteU8(asmBuffer, SWFACTION_REMOVECLIP); }
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
