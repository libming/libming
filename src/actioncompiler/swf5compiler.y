/* $Id$ */

%start program

%{

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "compile.h"
#include "actiontypes.h"
#include "assembler.h"

#define YYERROR_VERBOSE 1
//#define DEBUG 1

#ifdef _MSC_VER 
#define strcasecmp stricmp 
#endif


Buffer bf, bc;
static int classContext = 0;

%}

%parse-param {void *buffer}

%union
{
  Buffer action;
  char *str;
  SWFGetUrl2Method getURLMethod;
  int op;
  int intVal;
  int len;
  double doubleVal;
  ASFunction 		function;
  ASClass		clazz;
  ASClassMember		classMember;
  struct exprlist_s 	exprlist;
  struct switchcase	switchcase;
  struct switchcases	switchcases;
  struct
  {
	Buffer obj, ident, memexpr;
  } lval;
}

/* 
 * we expect 53 shift/reduce conflicts
 * see also: http://www.gnu.org/software/bison/manual/html_mono/bison.html#Shift_002fReduce
 */
%expect 53

/* tokens etc. */

%token BREAK CONTINUE FUNCTION ELSE SWITCH CASE DEFAULT FOR IN IF WHILE
%token DO VAR NEW DELETE DELETE2 TARGETPATH RETURN END WITH ASM EVAL
%token SWFACTION

%token RANDOM GETTIMER LENGTH CONCAT SUBSTR TRACE INT ORD CHR GETURL
%token GETURL1 NEXTFRAME PREVFRAME PLAY STOP TOGGLEQUALITY STOPSOUNDS

%token DUP SWAP POP PUSH SETREGISTER CALLFUNCTION CALLMETHOD NEWOBJECT NEWMETHOD
%token AND OR XOR MODULO ADD LESSTHAN EQUALS
%token INC DEC TYPEOF INSTANCEOF ENUMERATE ENUMERATE2 INITOBJECT INITARRAY
%token GETMEMBER SETMEMBER SHIFTLEFT SHIFTRIGHT SHIFTRIGHT2 VAREQUALS OLDADD
%token SUBTRACT MULTIPLY DIVIDE OLDEQUALS OLDLESSTHAN LOGICALAND LOGICALOR NOT
%token STRINGEQ STRINGLENGTH SUBSTRING GETVARIABLE SETVARIABLE
%token SETTARGETEXPRESSION  DUPLICATEMOVIECLIP REMOVEMOVIECLIP
%token STRINGLESSTHAN MBLENGTH MBSUBSTRING MBORD MBCHR
%token BRANCHALWAYS BRANCHIFTRUE GETURL2 POST GET CAST 
%token LOADVARIABLES LOADMOVIE LOADVARIABLESNUM LOADMOVIENUM
%token CALLFRAME STARTDRAG STOPDRAG GOTOANDSTOP GOTOANDPLAY SETTARGET 
%token GETPROPERTY SETPROPERTY TONUMBER TOSTRING 

%token TRY THROW CATCH FINALLY THIS

%token EXTENDS IMPLEMENTS FSCOMMAND2 CLASS PUBLIC PRIVATE

%token NULLVAL
%token UNDEFINED
%token <intVal> INTEGER
%token <doubleVal> DOUBLE
%token <intVal> BOOLEAN
%token <str> REGISTER

%token BROKENSTRING
/* these two are strdup'ed in compiler.flex, so free them up here */
%token <str> STRING
%token <str> IDENTIFIER

%token EQ "=="
%token EEQ "==="
%token NEE "!=="
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
%token REQ "%="
%token AEQ "&="
%token OEQ "|="

%token SHL "<<"
%token SHR ">>"
%token SHR2 ">>>"
%token SHLEQ "<<="
%token SHREQ ">>="
%token SHR2EQ ">>>="

%token _P_X _P_Y _P_XSCALE _P_YSCALE _P_CURRENTFRAME _P_TOTALFRAMES _P_ALPHA
%token _P_VISIBLE _P_WIDTH _P_HEIGHT _P_ROTATION _P_TARGET _P_FRAMESLOADED 
%token _P_NAME _P_DROPTARGET _P_URL _P_HIGHQUALITY _P_FOCUSRECT _P_SOUNDBUFTIME
%token _P_QUALITY _P_XMOUSE _P_YMOUSE

/* ascending order of ops ..? */

%nonassoc NOELSE
%nonassoc ELSE
%left ','
%right '=' "*=" "/=" "%=" "+=" "-=" "&=" "|=" "^=" ">>=" ">>>=" "<<="
%right '?' ':'
%left "&&" "||"
%left "==" "!=" "===" "!=="
%left '<' '>' "<=" ">="
%left '&' '|' '^'
%left "<<" ">>" ">>>"
%left '+' '-'
%left '*' '/' '%'
%nonassoc "++" "--"
%right '!' '~' UMINUS
%right POSTFIX
%right TYPEOF
%nonassoc INSTANCEOF
%left '.' '[' ']'


%type <action> program code
%type <action> stmt stmts
%type <action> if_stmt iter_stmt cont_stmt break_stmt return_stmt
%type <action> try_catch_stmt throw_stmt
%type <action> with_stmt
%type <action> switch_stmt
%type <action> anycode
%type <action> void_function_call function_call method_call
%type <action> assign_stmt assign_stmts assign_stmts_opt
%type <action> expr expr_or_obj objexpr expr_opt inpart obj_ref obj_ref_for_delete_only
%type <action> emptybraces level init_vars init_var primary lvalue_expr
%type <action> delete_call primary_constant
%type <classMember> class_stmts class_stmt class_vars class_var class_body 
%type <lval> lvalue 
%type <intVal> property
%type <exprlist> expr_list objexpr_list formals_list

%type <switchcase> switch_case
%type <switchcases> switch_cases

%type <op> assignop incdecop
%type <getURLMethod> urlmethod

%type <str> identifier function_identifier

%type <function> function_decl
%type <len> opcode opcode_list push_item with push_list

%type <clazz> class_decl

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
	| /* nothing */ { Buffer b = newBuffer(); *((Buffer *)buffer) = b; }
	;

code
	: anycode
	| code anycode
	;

anycode
	: stmt
		{ bufferConcat(bc, $1); }
	| function_decl
		{ 
		  if(swfVersion > 6)
			bufferWriteFunction(bf, $1, 2); 
		  else
			bufferWriteFunction(bf, $1, 1);
		}

	| class_decl
		{ bufferWriteClass(bf, $1); }
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
	| try_catch_stmt ';'
	| throw_stmt
	;


class_stmts
	: class_stmt
	| class_stmts class_stmt
	{ 	
		$$ = $1;
		ASClassMember_append($1, $2);			
	}
	;

class_stmt
	: access_attr function_decl 		{ $$ = newASClassMember_function($2); }
	| access_attr VAR class_vars ';' 	{ $$ = $3; }
	;

class_init
	: CLASS 
	{
		if(classContext)
		{
			swf5error(NULL, "Nested classes are not allowed\n");
			YYABORT;
		}
		classContext = 1;
	}
	; 

class_body
	: emptybraces {$$ = NULL; }
	| '{' class_stmts '}' { $$ = $2; }
	;	

class_decl 
	: class_init identifier class_body
	{ 
		$$ = newASClass($2, NULL, $3);
		classContext = 0;
	}
	| class_init identifier EXTENDS identifier class_body
	{ 
		$$ = newASClass($2, $4, $5);
		classContext = 0;
	}
	;

access_attr
	:	// empty
	| PUBLIC
	| PRIVATE
	;

type_attr
	:
	| ':' identifier

class_vars
	: class_var 

	| class_vars ',' class_var 
		{ $$ = $1;
		  ASClassMember_append($1, $3); 
		}
	;

class_var
	: identifier type_attr '=' primary_constant
		{ 
		  ASVariable v = newASVariable($1, $4); 
		  $$ = newASClassMember_variable(v);
		}	  
	| identifier type_attr
		{ 
			ASVariable v = newASVariable($1, NULL);
			$$ = newASClassMember_variable(v);
		}
	;

throw_stmt
	: THROW expr_or_obj ';'		{ $$ = $2; bufferWriteOp($$, SWFACTION_THROW); }
	;

try_catch_stmt
	: TRY stmt					{ 	$$ = newBuffer();
									bufferWriteOp($$, SWFACTION_TRY);
									bufferWriteS16($$, 8);                /* TRY tag length */
									bufferWriteU8($$, 0);                 /* flags */
									bufferWriteS16($$, bufferLength($2)); /* try block length */
									bufferWriteS16($$, 0);                /* catch block length */
									bufferWriteS16($$, 0);                /* finally block length */
									bufferWriteU8($$, 0);                 /* catch name - empty string */
									bufferConcat($$, $2);                 /* append TRY body */
								 }
	| TRY stmt CATCH '(' identifier ')' stmt		{ $$ = newBuffer();
									bufferWriteOp($$, SWFACTION_TRY);
									bufferWriteS16($$, 8+strlen($5));       /* TRY tag length */
									bufferWriteU8($$, 1);                   /* flags */
									bufferWriteS16($$, bufferLength($2)+5); /* try block length + JUMP length */
									bufferWriteS16($$, bufferLength($7));   /* catch block length */
									bufferWriteS16($$, 0);                  /* finally block length */
									bufferWriteHardString($$, $5, strlen($5)+1); /* catch name */
									bufferConcat($$, $2);                   /* append TRY body */
									bufferWriteOp($$, SWFACTION_JUMP);      /* jump after catch */
									bufferWriteS16($$, 2);                  /* ... */
									bufferWriteS16($$, bufferLength($7));   /* ... */
									bufferConcat($$, $7);                   /* append CATCH body */
								}
	| TRY stmt FINALLY stmt		{	$$ = newBuffer();
									bufferWriteOp($$, SWFACTION_TRY);
									bufferWriteS16($$, 8);                /* TRY tag length */
									bufferWriteU8($$, 2);                 /* flags */
									bufferWriteS16($$, bufferLength($2)); /* try block length */
									bufferWriteS16($$, 0);                /* catch block length */
									bufferWriteS16($$, bufferLength($4)); /* finally block length */
									bufferWriteU8($$, 0);                 /* catch name - empty string */
									bufferConcat($$, $2);                 /* append TRY body */
									bufferConcat($$, $4);                 /* append FINALLY body */
								 }
	| TRY stmt CATCH '(' identifier ')' stmt FINALLY stmt	{ $$ = newBuffer();
									bufferWriteOp($$, SWFACTION_TRY);
									bufferWriteS16($$, 8+strlen($5));        /* TRY tag length */
									bufferWriteU8($$, 3);                    /* flags */
									bufferWriteS16($$, bufferLength($2)+5);  /* try block length + JUMP length */
									bufferWriteS16($$, bufferLength($7));    /* catch block length */
									bufferWriteS16($$, bufferLength($9));    /* finally block length */
									bufferWriteHardString($$, $5, strlen($5)+1); /* catch name */
									bufferConcat($$, $2);                    /* append TRY body */
									bufferWriteOp($$, SWFACTION_JUMP);       /* jump after catch */
									bufferWriteS16($$, 2); 
									bufferWriteS16($$, bufferLength($7));
									bufferConcat($$, $7);                    /* append CATCH body */
									bufferConcat($$, $9);                    /* append FINALLY body */
								}
	;

with_stmt
	: WITH '(' expr ')' '{' stmts '}'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_WITH);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($6));
		  bufferConcat($$, $6); }
	;

// only possible if there is an active CTX_FUNCTION
// in some contexts, may have to pop a few values ...
return_stmt
	: RETURN ';'
		{ int tmp = chkctx(CTX_FUNCTION);
		  if(tmp < 0) 
		  {
			swf5error(NULL, "return outside function");
			YYABORT;
		  }
		  $$ = newBuffer();
		  while(--tmp >= 0)
			bufferWriteOp($$, SWFACTION_POP);
		  bufferWriteUndef($$);
		  bufferWriteOp($$, SWFACTION_RETURN); }

	| RETURN expr_or_obj ';'
		{ int tmp = chkctx(CTX_FUNCTION);
		  if(tmp < 0)
		  {
			swf5error(NULL, "return outside function");
			YYABORT;
		  }
		  $$ = newBuffer();
		  while(--tmp >= 0)
			bufferWriteOp($$, SWFACTION_POP);
		  bufferConcat($$, $2);
		  bufferWriteOp($$, SWFACTION_RETURN); }
	;

assign_stmts
	: assign_stmt
	| assign_stmts ',' assign_stmt		{ bufferConcat($1, $3); }
	;

if_stmt
	: IF '(' expr ')' stmt ELSE stmt
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_IF);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($7)+5);
		  bufferConcatSimple($$, $7);
		  bufferWriteOp($$, SWFACTION_JUMP);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($5));
		  bufferConcatSimple($$, $5); }

	| IF '(' expr ')' stmt		%prec NOELSE
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_LOGICALNOT);
		  bufferWriteOp($$, SWFACTION_IF);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($5));
		  bufferConcatSimple($$, $5); }
	;

expr_opt
	: /* empty */	{ $$ = NULL; }
	| expr		{ $$ = $1; }
	;

switch_init
	: SWITCH
		{ addctx(CTX_SWITCH); }
	;

switch_stmt
	: switch_init '(' expr ')' '{'
	  switch_cases '}'
		{ $$ = $3;
		  bufferResolveSwitch($$, &$6);
		  bufferResolveJumps($$);
		  bufferWriteOp($$, SWFACTION_POP);
		  delctx(CTX_SWITCH);
 /* FIXME: continue in switch continues surrounding loop, if any */
	}
	;

/* XXX */
switch_cases
	: /* empty */
		{ $$.count = 0;
		  $$.list = 0; }

	| switch_cases switch_case
		{ $$ = $1;
		  $$.list = (struct switchcase*) realloc($$.list, ($$.count+1) * sizeof(struct switchcase));
		  $$.list[$$.count] = $2;
		  $$.count++; }
	;

switch_case
	: CASE expr ':' stmts
		{ $$.cond = $2;
		  $$.action = $4;
		  if(chkctx(CTX_BREAK) == CTX_BREAK)
		  {
			delctx(CTX_BREAK);
		  	$$.isbreak = 1;
		  }
		  else
			$$.isbreak = 0; 
		}

	| DEFAULT ':' stmts
		{ $$.cond = NULL;
		  $$.action = $3;
		  if(chkctx(CTX_BREAK) == CTX_BREAK)
	          {
			delctx(CTX_BREAK);
		  	$$.isbreak = 1;
		  }
		  else
			$$.isbreak = 0;
		}
	;


/* there's GOT to be a better way than this.. */
/* What is this for exactly ? */

identifier
	: IDENTIFIER
	| TARGETPATH	{ $$ = strdup("targetPath"); }
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
	| ENUMERATE2	{ $$ = strdup("enumerate2"); }
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
	| DUPLICATEMOVIECLIP	{ $$ = strdup("duplicateMovieClip"); }
	| REMOVEMOVIECLIP	{ $$ = strdup("removeMovieClip"); }
	| STARTDRAG	{ $$ = strdup("startDrag"); }
	| STOPDRAG	{ $$ = strdup("stopDrag"); }
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
	| LOADVARIABLES	{ $$ = strdup("loadVariables"); }
	| LOADVARIABLESNUM { $$ = strdup("loadVariablesNum"); }
	| LOADMOVIE	{ $$ = strdup("loadMovie"); }
	| LOADMOVIENUM 	{ $$ = strdup("loadMovieNum"); }
	| GOTOANDSTOP	{ $$ = strdup("gotoAndStop"); }
	| GOTOANDPLAY	{ $$ = strdup("gotoAndPlay"); }
	| SETTARGET 	{ $$ = strdup("setTarget"); }
	| CALLFRAME	{ $$ = strdup("call"); }
	| GETPROPERTY	{ $$ = strdup("getProperty"); }
	| SETPROPERTY	{ $$ = strdup("setProperty"); }
	| CAST		{ $$ = strdup("cast"); }
	| SWFACTION	{ $$ = strdup("swfAction"); }
	/* temp hack to keep things working */
	| THIS		{ $$ = strdup("this"); }
	
	/* property names */ 
	| _P_X			{ $$ = strdup("_x"); }
	| _P_Y			{ $$ = strdup("_y"); }
	| _P_XSCALE		{ $$ = strdup("_xscale"); }
	| _P_YSCALE 		{ $$ = strdup("_yscale"); }
	| _P_CURRENTFRAME	{ $$ = strdup("_currentframe"); }
	| _P_TOTALFRAMES	{ $$ = strdup("_totalframes"); }
	| _P_ALPHA		{ $$ = strdup("_alpha"); }
	| _P_VISIBLE		{ $$ = strdup("_visible"); }
	| _P_WIDTH 		{ $$ = strdup("_width"); }
	| _P_HEIGHT		{ $$ = strdup("_height"); }
	| _P_ROTATION		{ $$ = strdup("_rotation"); }
	| _P_TARGET		{ $$ = strdup("_target"); }
	| _P_FRAMESLOADED	{ $$ = strdup("_framesloaded"); }
	| _P_NAME		{ $$ = strdup("_name"); }
	| _P_DROPTARGET		{ $$ = strdup("_droptarget"); }
	| _P_URL 		{ $$ = strdup("_url"); }
	| _P_HIGHQUALITY	{ $$ = strdup("_highquality"); }
	| _P_FOCUSRECT		{ $$ = strdup("_focusrect"); }
	| _P_SOUNDBUFTIME	{ $$ = strdup("_soundbuftime"); }
	| _P_QUALITY		{ $$ = strdup("_quality"); }
	| _P_XMOUSE		{ $$ = strdup("_xmouse"); }
	| _P_YMOUSE		{ $$ = strdup("_ymouse"); }
	;

formals_list
	: /* empty */
		{ $$.buffer = newBuffer();
		  $$.count = 0; }

	| identifier type_attr
		{ $$.buffer = newBuffer();
		  bufferWriteHardString($$.buffer, $1, strlen($1)+1);
		  $$.count = 1;
		  free($1); }

	| formals_list ',' identifier type_attr
		{ $$ = $1;
		  bufferWriteHardString($$.buffer, $3, strlen($3)+1);
		  ++$$.count;
		  free($3); }
	;

function_identifier
	: 		{ addctx(CTX_FUNCTION); $$ = NULL; }
	| identifier	{ addctx(CTX_FUNCTION); $$ = $1; }
	;

function_decl
	: FUNCTION function_identifier '(' formals_list ')' type_attr stmt
	{
		$$ = newASFunction();
		$$->name = $2;
		$$->params = $4;
		$$->code = $7;	
		delctx(CTX_FUNCTION);	
	}
	;

inpart
	: IN obj_ref
		{ $$ = $2; }
	;

obj_ref
	: lvalue
		{ if($1.obj)
		  {
		    $$ = $1.obj;
		    $$->hasObject = 1;
		    if($1.ident)
		      bufferConcat($$, $1.ident);
		    else
		      bufferConcat($$, $1.memexpr);

		    bufferWriteOp($$, SWFACTION_GETMEMBER);
		  }
		  else
		  {
		    $$ = $1.ident;
		  }
		}
	| function_call
	| method_call
	;

// this is a workaround for DELETE / DELETE2 OPs. (AS is broken!!!)
obj_ref_for_delete_only
	: lvalue
		{ if($1.obj)
		  {
		    $$ = $1.obj;
		    $$->hasObject = 1; 
		    if($1.ident)
		      bufferConcat($$, $1.ident);
		    else
		      bufferConcat($$, $1.memexpr);
		  }
		  else
		  {
		    $$ = $1.ident;
		  }
		}
	| function_call
	| void_function_call
	| method_call
	| '(' obj_ref_for_delete_only ')' { $$ = $2; }
	;

while_init
	: WHILE
		{ addctx(CTX_LOOP); }
	;

do_init
	: DO
		{ addctx(CTX_LOOP); }
	;

for_init
	: /* empty */
		{ addctx(CTX_LOOP); }
	;

for_in_init
	: /* empty */
		{ addctx(CTX_FOR_IN); }
	;

iter_stmt
	: while_init '(' expr ')' stmt
                { $$ = $3;
		  bufferWriteOp($$, SWFACTION_LOGICALNOT);
		  bufferWriteOp($$, SWFACTION_IF);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($5)+5);
		  bufferConcat($$, $5);
		  bufferWriteOp($$, SWFACTION_JUMP);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, -(bufferLength($$)+2));
		  bufferResolveJumps($$);
		  delctx(CTX_LOOP); }

	| do_init stmt WHILE '(' expr ')'
		{ if($2)
			{	$$ = $2;
		  		bufferConcat($$, $5);
			}
			else
				$$ = $5;
		  bufferWriteOp($$, SWFACTION_IF);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, -(bufferLength($$)+2));
		  bufferResolveJumps($$);
		  delctx(CTX_LOOP); }

	| FOR '(' assign_stmts_opt ';' expr_opt ';' assign_stmts_opt ')' for_init stmt
		{
		  int continue_len;
		  if($3)
		    $$ = $3;
		  else
		    $$ = newBuffer();

		  continue_len = bufferLength ($7);
		  if($10)
		    bufferConcatSimple($10, $7);
		  else if ($7)
		    $10 = $7;
		  else 
		    $10 = newBuffer();
		  if($5)
		  {
                    bufferWriteOp($5, SWFACTION_LOGICALNOT);
                    bufferWriteOp($5, SWFACTION_IF);
                    bufferWriteS16($5, 2);
                    bufferWriteS16($5, bufferLength($10)+5);
		    bufferConcat($5, $10);
                  }
		  else
		    $5 = $10;

                  bufferWriteOp($5, SWFACTION_JUMP);
                  bufferWriteS16($5, 2);
                  bufferWriteS16($5, -(bufferLength($5)+2));
		  /* need to jump to last part of for stmt in continue case */
		  if (continue_len)
		    bufferResolveJumpsFull($5, $5->pos, $5->pos - continue_len - 5);
		  else
		    bufferResolveJumps($5);

                  bufferConcat($$, $5);
		  delctx(CTX_LOOP);
                }

	| FOR '(' identifier inpart ')' for_in_init stmt
		{ Buffer b2, b3;
		  int tmp;

		  $$ = $4;
		  if($4->hasObject)
			bufferWriteOp($$, SWFACTION_ENUMERATE2);
		  else
			bufferWriteOp($$, SWFACTION_ENUMERATE);

		  b2 = newBuffer();
		  bufferWriteSetRegister(b2, 0);
		  bufferWriteOp(b2, SWFACTION_PUSH);
		  bufferWriteS16(b2, 1);
		  bufferWriteU8(b2, 2);
		  bufferWriteOp(b2, SWFACTION_EQUALS2);
		  bufferWriteOp(b2, SWFACTION_IF);
		  bufferWriteS16(b2, 2);

		  b3 = newBuffer();
/* basically a lvalue could be used here rather than an ident !!! */
/* probably by using reg1 for the test rather than reg0 */
		  bufferWriteString(b3, $3, strlen($3)+1);
		  bufferWriteRegister(b3, 0);
		  bufferWriteOp(b3, SWFACTION_SETVARIABLE);
		  bufferConcat(b3, $7);
		  bufferWriteS16(b2, bufferLength(b3) + 5);
		  tmp = bufferLength(b2) + bufferLength(b3) + 5;
		  bufferWriteOp(b3, SWFACTION_JUMP);
		  bufferWriteS16(b3, 2);
		  bufferWriteS16(b3, -tmp);
		  bufferConcat(b2, b3);
		  bufferResolveJumps(b2);
		  bufferConcat($$, b2);
		  delctx(CTX_FOR_IN);
		  free($3); }

	| FOR '(' VAR identifier inpart ')' for_in_init stmt
		{ Buffer b2, b3;
		  int tmp;

		  $$ = $5;
		  if($5->hasObject)
			bufferWriteOp($$, SWFACTION_ENUMERATE2);        
		  else
			bufferWriteOp($$, SWFACTION_ENUMERATE); 

		  b2 = newBuffer();
		  bufferWriteSetRegister(b2, 0);
		  bufferWriteOp(b2, SWFACTION_PUSH);
		  bufferWriteS16(b2, 1);
		  bufferWriteU8(b2, 2);
		  bufferWriteOp(b2, SWFACTION_EQUALS2);
		  bufferWriteOp(b2, SWFACTION_IF);
		  bufferWriteS16(b2, 2);
		  // add size later

		  b3 = newBuffer();
		  bufferWriteString(b3, $4, strlen($4)+1);
		  bufferWriteRegister(b3, 0);
		  bufferWriteOp(b3, SWFACTION_DEFINELOCAL);
		  bufferConcat(b3, $8);
		  bufferWriteS16(b2, bufferLength(b3) + 5);
		  tmp = bufferLength(b2) + bufferLength(b3) + 5;
		  bufferWriteOp(b3, SWFACTION_JUMP);
		  bufferWriteS16(b3, 2);
		  bufferWriteS16(b3, -tmp);
		  bufferConcat(b2, b3);
		  bufferResolveJumps(b2);
		  bufferConcat($$, b2);
		  delctx(CTX_FOR_IN);
		  free($4); }
	;

assign_stmts_opt
	: /* empty */				{ $$ = NULL; }
	| assign_stmts
	;

// continue only makes sense if there is a CTX_LOOP or CTX_FOR_IN
// on the stack
cont_stmt
	: CONTINUE ';'
		{ 
		  if(chkctx(CTX_CONTINUE) < 0)
		  {
			swf5error(NULL, "continue outside loop");
			YYABORT;
		  }
		  $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_JUMP);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, MAGIC_CONTINUE_NUMBER); }
	;

// break is possible if there is a CTX_LOOP, CTX_FOR_IN or CTX_SWITCH
break_stmt
	: BREAK ';'
		{ int context = chkctx(CTX_BREAK);
		  $$ = newBuffer();
		  if(context == CTX_FOR_IN || context == CTX_LOOP)
		  {
		  	if(context == CTX_FOR_IN)	/* break out of a for .. in */
				bufferWriteOp($$, SWFACTION_POP);
		  	bufferWriteOp($$, SWFACTION_JUMP);
		  	bufferWriteS16($$, 2);
		  	bufferWriteS16($$, MAGIC_BREAK_NUMBER); 
		  }
		  else if(context == CTX_SWITCH)
		  {
			addctx(CTX_BREAK);	
		  }
		  else
		  {
			swf5error(NULL, "break outside switch / loop");
			YYABORT;
		  }
		}

	;

urlmethod
	: /* empty */		{ $$ = GETURL_METHOD_NOSEND; }

	| ',' GET		{ $$ = GETURL_METHOD_GET; }

	| ',' POST		{ $$ = GETURL_METHOD_POST; }

	| ',' STRING		{ if(strcasecmp($2, "GET") == 0)
				    $$ = GETURL_METHOD_GET;
				  else if(strcasecmp($2, "POST") == 0)
				    $$ = GETURL_METHOD_POST;
				  else $$ = GETURL_METHOD_NOSEND;
				}
	;

level
	: expr
		{ $$ = newBuffer();
		  bufferWriteString($$, "_level", 7);
		  bufferConcat($$, $1);
		  bufferWriteOp($$, SWFACTION_STRINGCONCAT); }
	;

void_function_call

	: TRACE '(' expr_or_obj ')'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_TRACE); }

	| GETURL '(' expr ')'
		{
#ifdef DEBUG
		  printf("void_function_call: GETURL '(' expr ')'\n");
#endif
		  $$ = $3;
		  bufferWriteOp($$, SWFACTION_PUSH);
		  bufferWriteS16($$, 2); bufferWriteS16($$, 0); // two 0 bytes	
		  bufferWriteOp($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, 0); }
	
	| GETURL '(' expr ',' expr urlmethod ')'
		{
#ifdef DEBUG
		  printf("void_function_call: GETURL '(' expr ',' expr urlmethod ')'\n");
#endif
		  $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteOp($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, $6); }

	| LOADVARIABLES '(' expr ',' expr urlmethod ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteOp($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, 0xc0+$6); }

	| LOADVARIABLESNUM '(' expr ',' level urlmethod ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteOp($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, 0x80+$6); }

	| LOADMOVIE '(' expr ',' expr urlmethod ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteOp($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, 0x40+$6); }

	| LOADMOVIENUM '(' expr ',' level urlmethod ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteOp($$, SWFACTION_GETURL2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, $6); }

	| CALLFRAME '(' expr ')'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_CALLFRAME);
		  bufferWriteS16($$, 0); }

	/* startDrag(target, lock, [left, right, top, bottom]) */
	| STARTDRAG '(' expr ',' expr ')'
		{ $$ = newBuffer();
		  bufferWriteString($$, "0", 2); /* no constraint */
		  bufferConcat($$, $5);
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_STARTDRAG); }

	| STARTDRAG '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
		{ $$ = newBuffer();
		  bufferConcat($$, $7);
		  bufferConcat($$, $11);
		  bufferConcat($$, $9);
		  bufferConcat($$, $13);
		  bufferWriteString($$, "1", 2); /* has constraint */
		  bufferConcat($$, $5);
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_STARTDRAG); }

	| STOPDRAG '(' ')' /* no args */
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_ENDDRAG); }

	/* duplicateMovieClip(target, new, depth) */
	| DUPLICATEMOVIECLIP '(' expr ',' expr ',' expr ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferConcat($$, $7);
		  bufferWriteInt($$, 16384); /* magic number */
		  bufferWriteOp($$, SWFACTION_ADD);
		  bufferWriteOp($$, SWFACTION_DUPLICATECLIP); }

	| REMOVEMOVIECLIP '(' expr ')'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_REMOVECLIP); }

	| GETURL1 '(' STRING ',' STRING ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_GETURL);
		  bufferWriteS16($$, strlen($3) + strlen($5) + 2);
		  bufferWriteHardString($$, $3, strlen($3));
		  bufferWriteU8($$, 0);
		  bufferWriteHardString($$, $5, strlen($5));
		  bufferWriteU8($$, 0); }

	/* v3 actions */
	| NEXTFRAME '(' ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_NEXTFRAME); }
		
	| PREVFRAME '(' ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_PREVFRAME); }

	| PLAY '(' ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_PLAY); }

	| STOP '(' ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_STOP); }

	| STOPSOUNDS '(' ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_STOPSOUNDS); }

	| TOGGLEQUALITY '(' ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_TOGGLEQUALITY); }

	| GOTOANDPLAY '(' INTEGER ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_GOTOFRAME);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, $3-1); /* GOTOFRAME target is 0-based */
		  bufferWriteOp($$, SWFACTION_PLAY); }

	| GOTOANDSTOP '(' INTEGER ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_GOTOFRAME);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, $3-1);  /* GOTOFRAME target is 0-based */
		  bufferWriteOp($$, SWFACTION_STOP); }

	| GOTOANDPLAY '(' STRING ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_GOTOLABEL);
		  bufferWriteS16($$, strlen($3)+1);
		  bufferWriteHardString($$, $3, strlen($3)+1);
		  free($3); 
		  bufferWriteOp($$, SWFACTION_PLAY); }

	| GOTOANDSTOP '(' STRING ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_GOTOLABEL);
		  bufferWriteS16($$, strlen($3)+1);
		  bufferWriteHardString($$, $3, strlen($3)+1);
		  free($3); 
		  bufferWriteOp($$, SWFACTION_STOP); }

	| GOTOANDPLAY '(' expr ')'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_GOTOFRAME2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, 1); } /* XXX - and play */

	| GOTOANDSTOP '(' expr ')'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_GOTOFRAME2);
		  bufferWriteS16($$, 1);
		  bufferWriteU8($$, 0); } /* XXX - and stop */

	| SETTARGET '(' STRING ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_SETTARGET);
		  bufferWriteS16($$, strlen($3)+1);
		  bufferWriteHardString($$, $3, strlen($3)+1);
		  free($3); }

	| SETTARGET '(' expr ')'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_SETTARGET2); }

	| SETPROPERTY '(' expr ',' property ',' expr ')'
		{
			$$ = $3;
			bufferWriteFloat($$, $5);
			bufferConcat($$, $7);
			bufferWriteOp($$, SWFACTION_SETPROPERTY);	
		}
	;


function_call
	: IDENTIFIER '(' expr_list ')'
		{
#ifdef DEBUG
		  printf("function_call: %s '(' expr_list ')'\n", $1);
#endif
		  $$ = $3.buffer;
		  bufferWriteInt($$, $3.count);
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferWriteOp($$, SWFACTION_CALLFUNCTION);
		  free($1); }

	| TARGETPATH '(' IDENTIFIER ')'
		{ $$ = newBuffer();
		  bufferWriteString($$, $3, strlen($3)+1);
		  free($3);
		  bufferWriteOp($$, SWFACTION_GETVARIABLE); 
		  bufferWriteOp($$, SWFACTION_TARGETPATH); }

	| EVAL '(' expr ')'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_GETVARIABLE); }

	| GETTIMER '(' ')'
		{ $$ = newBuffer();
		  bufferWriteOp($$, SWFACTION_GETTIME); }

	| RANDOM '(' expr ')'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_RANDOMNUMBER); }

	| LENGTH '(' expr_or_obj ')'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_STRINGLENGTH); }

	| INT '(' expr ')'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_INT); }

	| ORD '(' expr ')'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_ORD); }

	| CHR '(' expr ')'
		{ $$ = $3;
		  bufferWriteOp($$, SWFACTION_CHR); }

	| CONCAT '(' expr ',' expr ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferWriteOp($$, SWFACTION_STRINGCONCAT); }

	| SUBSTR '(' expr ',' expr ',' expr ')'
		{ $$ = $3;
		  bufferConcat($$, $5);
		  bufferConcat($$, $7);
		  bufferWriteOp($$, SWFACTION_SUBSTRING); }

	| TYPEOF '(' expr_or_obj ')'
		{
#ifdef DEBUG
		  printf("function_call: TYPEOF '(' expr_or_obj ')'\n");
#endif
		  $$ = $3;
		  bufferWriteOp($$, SWFACTION_TYPEOF); }
	
	| GETPROPERTY '(' expr ',' property ')'
		{ $$ = newBuffer();
		  bufferConcat($$, $3);
		  bufferWriteFloat($$, $5);
		  bufferWriteOp($$, SWFACTION_GETPROPERTY);
		}
	;

/* legacy and built-in functions */
delete_call
	: DELETE obj_ref_for_delete_only 
		{ $$ = $2;
		  if($2->hasObject)
		    bufferWriteOp($$, SWFACTION_DELETE);
		  else 
		    bufferWriteOp($$, SWFACTION_DELETE2);
		}
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

method_call
	: lvalue_expr '.' identifier '(' expr_list ')'
		{ $$ = $5.buffer;
		  bufferWriteInt($$, $5.count);
		  bufferConcat($$, $1);
		  bufferWriteString($$, $3, strlen($3)+1);
		  bufferWriteOp($$, SWFACTION_CALLMETHOD);
		  free($3); }

	| lvalue_expr '[' expr ']' '(' expr_list ')'
		{ $$ = $6.buffer;
		  bufferWriteInt($$, $6.count);
		  bufferConcat($$, $1);
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_CALLMETHOD); }
	;

objexpr
	: identifier ':' expr_or_obj
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  bufferConcat($$, $3);
		  free($1); }
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
	: "+="		{ $$ = SWFACTION_ADD2; }
	| "-="		{ $$ = SWFACTION_SUBTRACT; }
	| "*="		{ $$ = SWFACTION_MULTIPLY; }
	| "/="		{ $$ = SWFACTION_DIVIDE; }
	| "%="		{ $$ = SWFACTION_MODULO; }
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


/* resolves an lvalue into a buffer */
lvalue_expr
	: lvalue
		{ if($1.obj)
		  {
		    $$ = $1.obj;
		    if($1.ident)
		      bufferConcat($$, $1.ident);
		    else
		      bufferConcat($$, $1.memexpr);

		    bufferWriteOp($$, SWFACTION_GETMEMBER);
		  }
		  else
		  {
		    $$ = $1.ident;
		    bufferWriteOp($$, SWFACTION_GETVARIABLE);
		  }
		}
	| function_call
	| method_call
	;

/* lvalue - things you can assign to */
lvalue
	: identifier
		{ $$.ident = newBuffer();
		  bufferWriteString($$.ident, $1, strlen($1)+1);
		  free($1);
		  $$.obj = 0;
		  $$.memexpr = 0; }

	| lvalue_expr '.' identifier %prec '.'
		{ $$.obj = $1;
		  $$.ident = newBuffer();
		  bufferWriteString($$.ident, $3, strlen($3)+1);
		  free($3);
		  $$.memexpr = 0; }

	| lvalue_expr '[' expr ']' %prec '.'
		{ $$.obj = $1;
		  $$.memexpr = $3;
		  $$.ident = 0; }
	;

/* these leave a value on the stack */

expr
	: primary
	
	| primary_constant

	| '-' expr %prec UMINUS
		{ $$ = $2;
		  bufferWriteInt($2, -1);
		  bufferWriteOp($2, SWFACTION_MULTIPLY); }

	| '~' expr %prec UMINUS
		{ $$ = $2;
		  bufferWriteInt($2, 0xffffffff);
		  bufferWriteOp($2, SWFACTION_BITWISEXOR); }

	| '!' expr
		{ $$ = $2;
		  bufferWriteOp($2, SWFACTION_LOGICALNOT); }

	| expr LAN expr
		{ $$ = $1;
		  bufferWriteOp($$, SWFACTION_PUSHDUP);
		  bufferWriteOp($$, SWFACTION_LOGICALNOT);
		  bufferWriteOp($$, SWFACTION_IF);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($3)+1);
		  bufferWriteOp($$, SWFACTION_POP);
		  bufferConcatSimple($$, $3); }

	| expr LOR expr
		{ $$ = $1;
		  bufferWriteOp($$, SWFACTION_PUSHDUP);
		  bufferWriteOp($$, SWFACTION_IF);
		  bufferWriteS16($$, 2);
		  bufferWriteS16($$, bufferLength($3)+1);
		  bufferWriteOp($$, SWFACTION_POP);
		  bufferConcatSimple($$, $3); }

	| expr '*' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_MULTIPLY); }

	| expr '/' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_DIVIDE); }

	| expr '%' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_MODULO); }

	| expr '+' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_ADD2); }

	| expr '-' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_SUBTRACT); }

	| expr '&' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_BITWISEAND); }

	| expr '|' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_BITWISEOR); }

	| expr '^' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_BITWISEXOR); }

	| expr '<' expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_LESS2); }

	| expr '>' expr
		{ $$ = $3;
		  bufferConcat($$, $1);
		  bufferWriteOp($$, SWFACTION_LESS2); }

	| expr "<=" expr
		{ $$ = $3;
		  bufferConcat($$, $1);
		  bufferWriteOp($$, SWFACTION_LESS2);
		  bufferWriteOp($$, SWFACTION_LOGICALNOT); }

	| expr ">=" expr
		{ bufferConcat($1, $3);
		  bufferWriteOp($1, SWFACTION_LESS2);
		  bufferWriteOp($1, SWFACTION_LOGICALNOT); }

	| expr "==" expr
		{ bufferConcat($1, $3);
		  bufferWriteOp($1, SWFACTION_EQUALS2); }

	| expr "===" expr
		{ bufferConcat($1, $3);
		  bufferWriteOp($1, SWFACTION_STRICTEQUALS); }

	| expr "!=" expr
		{ bufferConcat($1, $3);
		  bufferWriteOp($1, SWFACTION_EQUALS2);
		  bufferWriteOp($1, SWFACTION_LOGICALNOT); }

	| expr "!==" expr
		{ bufferConcat($1, $3);
		  bufferWriteOp($1, SWFACTION_STRICTEQUALS); 
		  bufferWriteOp($1, SWFACTION_LOGICALNOT); }

	| expr "<<" expr
		{ bufferConcat($1, $3);
		  bufferWriteOp($1, SWFACTION_SHIFTLEFT); }

	| expr ">>" expr
		{ bufferConcat($1, $3);
		  bufferWriteOp($1, SWFACTION_SHIFTRIGHT); }

	| expr ">>>" expr
		{ bufferConcat($1, $3);
		  bufferWriteOp($1, SWFACTION_SHIFTRIGHT2); }

	| expr '?' expr ':' expr
		{ bufferWriteOp($1, SWFACTION_IF);
		  bufferWriteS16($1, 2);
		  bufferWriteS16($1, bufferLength($5)+5);
		  bufferConcatSimple($1, $5);
		  bufferWriteOp($1, SWFACTION_JUMP);
		  bufferWriteS16($1, 2);
		  bufferWriteS16($1, bufferLength($3));
		  bufferConcatSimple($1, $3); }

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
		    bufferWriteOp($$, SWFACTION_SETMEMBER);
		    bufferWriteRegister($$, 0);
		  }
		  else /* just ident */
		  {
		    $$ = $3;
		    bufferWriteOp($$, SWFACTION_PUSHDUP);
		    bufferConcat($$, $1.ident);
		    bufferWriteOp($$, SWFACTION_STACKSWAP);
		    bufferWriteOp($$, SWFACTION_SETVARIABLE);
		  }
/* tricky case missing here: lvalue ASSIGN expr */
/* like in x = y += z; */
		}

	| expr INSTANCEOF lvalue_expr
		{ $$ = $1;
		  bufferConcat($$, $3);
		  bufferWriteOp($$, SWFACTION_INSTANCEOF); }

	;

expr_or_obj
	: expr

	| NEW identifier
		{
#ifdef DEBUG
		  printf("NEW %s\n", $2);
#endif
		  $$ = newBuffer();
		  bufferWriteInt($$, 0);
		  bufferWriteString($$, $2, strlen($2)+1);
		  free($2);
		  bufferWriteOp($$, SWFACTION_NEWOBJECT); }

	| NEW IDENTIFIER '(' expr_list ')'
		{
#ifdef DEBUG
		  printf("NEW %s '(' expr_list ')'\n", $2);
#endif
		  $$ = $4.buffer;
		  bufferWriteInt($$, $4.count);
		  bufferWriteString($$, $2, strlen($2)+1);
		  free($2);
		  bufferWriteOp($$, SWFACTION_NEWOBJECT); }

	| NEW lvalue_expr '.' IDENTIFIER
		{
#ifdef DEBUG
		  printf("NEW lvalue_expr '.' %s\n", $4);
#endif
		  $$ = newBuffer();
		  bufferWriteInt($$, 0);
		  bufferConcat($$, $2);
		  bufferWriteString($$, $4, strlen($4)+1);
		  free($4);
		  bufferWriteOp($$, SWFACTION_NEWMETHOD); }

	| NEW lvalue_expr '[' expr ']'
		{
#ifdef DEBUG
		  printf("NEW lvalue_expr '[' expr ']'\n");
#endif
		  $$ = newBuffer();
		  bufferWriteInt($$, 0);
		  bufferConcat($$, $2);
		  bufferConcat($$, $4);
		  bufferWriteOp($$, SWFACTION_NEWMETHOD); }


	| NEW lvalue_expr '.' IDENTIFIER '(' expr_list ')'
		{
#ifdef DEBUG
		  printf("NEW lvalue_expr '.' %s '(' expr_list ')'\n", $4);
#endif
		  $$ = $6.buffer;
		  bufferWriteInt($$, $6.count);
		  bufferConcat($$, $2);
		  bufferWriteString($$, $4, strlen($4)+1);
		  free($4);
		  bufferWriteOp($$, SWFACTION_NEWMETHOD); }

	| NEW lvalue_expr '[' expr ']' '(' expr_list ')'
		{
#ifdef DEBUG
		  printf("NEW lvalue_expr '[' expr ']' '(' expr_list ')'\n");
#endif
		  $$ = $7.buffer;
		  bufferWriteInt($$, $7.count);
		  bufferConcat($$, $2);
		  bufferConcat($$, $4);
		  bufferWriteOp($$, SWFACTION_NEWMETHOD); }

	| '[' expr_list ']'
		{ $$ = $2.buffer;
		  bufferWriteInt($$, $2.count);
		  bufferWriteOp($$, SWFACTION_INITARRAY); }

	| emptybraces
		{ $$ = newBuffer();
		  bufferWriteInt($$, 0);
		  bufferWriteOp($$, SWFACTION_INITOBJECT); }

	| '{' objexpr_list '}'
		{ $$ = $2.buffer;
		  bufferWriteInt($$, $2.count);
		  bufferWriteOp($$, SWFACTION_INITOBJECT); }

	;

primary
	: function_decl
		{
			if($1->name != NULL)
			{
				swf5error(NULL, "anonymous decl only. identifier not allowed");
				YYABORT;
			}
			$$ = newBuffer();
			if(swfVersion > 6)
				bufferWriteFunction($$, $1, 2);
			else
				bufferWriteFunction($$, $1, 1);
		}
	| lvalue_expr

	| delete_call
			
	| incdecop lvalue %prec "++"
		{ if($2.obj)
		  {
		    if($2.ident)	// expr . identifier
		    {
		      $$ = $2.obj;
		      bufferWriteOp($$, SWFACTION_PUSHDUP);	      /* a, a */
		      bufferWriteBuffer($$, $2.ident);        /* a, a, i */
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i, a */
		      bufferConcat($$, $2.ident);             /* a, i, a, i */
		      bufferWriteOp($$, SWFACTION_GETMEMBER);
		      bufferWriteOp($$, $1);
		      bufferWriteSetRegister($$, 0);
		      bufferWriteOp($$, SWFACTION_SETMEMBER); /* a.i = a.i+1 */
		      bufferWriteRegister($$, 0);	      /* a.i+1 */
		    }
		    else	// expr [ expr ]
		    {
		      $$ = $2.memexpr;			      /* i */
		      bufferConcat($$, $2.obj);		      /* i, a */
		      bufferWriteSetRegister($$, 0);	/* ($2.memexpr can use reg0) */
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i */
		      bufferWriteOp($$, SWFACTION_PUSHDUP);	      /* a, i, i */
		      bufferWriteRegister($$, 0);	      /* a, i, i, a */
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i, a, i */
		      bufferWriteOp($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		      bufferWriteOp($$, $1);		      /* a, i, a[i]+1 */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteOp($$, SWFACTION_SETMEMBER); /* a[i] = a[i]+1 */
		      bufferWriteRegister($$, 0);	      /* a[i]+1 */
		    }
		  }
		  else	// identifier
		  {
		    $$ = newBuffer();
		    bufferWriteBuffer($$, $2.ident);
		    bufferWriteOp($$, SWFACTION_GETVARIABLE);
		    bufferWriteOp($$, $1);
		    bufferWriteOp($$, SWFACTION_PUSHDUP);
		    bufferConcat($$, $2.ident);
		    bufferWriteOp($$, SWFACTION_STACKSWAP);
		    bufferWriteOp($$, SWFACTION_SETVARIABLE);
		  }
		}

	| lvalue incdecop %prec POSTFIX
		{ if($1.obj)
		  {
		    if($1.ident)
		    {
		      $$ = $1.obj;	                      /* a */
		      bufferWriteOp($$, SWFACTION_PUSHDUP);	      /* a, a */
		      bufferWriteBuffer($$, $1.ident);        /* a, a, i */
		      bufferWriteOp($$, SWFACTION_GETMEMBER); /* a, a.i */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a.i, a */
		      bufferConcat($$, $1.ident);             /* a.i, a, i */
		      bufferWriteRegister($$, 0);             /* a.i, a, i, a.i */
		      bufferWriteOp($$, $2);		      /* a.i, a, i, a.i+1 */
		      bufferWriteOp($$, SWFACTION_SETMEMBER);
		    }
		    else
		    {
		      $$ = $1.memexpr;
		      bufferConcat($$, $1.obj);               /* i, a */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i */
		      bufferWriteOp($$, SWFACTION_PUSHDUP);	      /* a, i, i */
		      bufferWriteRegister($$, 0);             /* a, i, i, a */
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i, a, i */
		      bufferWriteOp($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteOp($$, $2);		      /* a, i, a[i]+1 */
		      bufferWriteOp($$, SWFACTION_SETMEMBER);
		      bufferWriteRegister($$, 0);             /* a[i] */
		    }
		  }
		  else
		  {
		    $$ = newBuffer();
		    bufferWriteBuffer($$, $1.ident);
		    bufferWriteOp($$, SWFACTION_GETVARIABLE);
		    bufferWriteOp($$, SWFACTION_PUSHDUP);
		    bufferWriteOp($$, $2);
		    bufferConcat($$, $1.ident);
		    bufferWriteOp($$, SWFACTION_STACKSWAP);
		    bufferWriteOp($$, SWFACTION_SETVARIABLE);
		  }
		}

	| '(' expr ')'
		{ $$ = $2; }

	
	| lvalue assignop expr
		{ if($1.obj)
		  {
		    if($1.ident)
		    {
		      $$ = $1.obj;			      /* a */
		      bufferWriteOp($$, SWFACTION_PUSHDUP);	      /* a, a */
		      bufferWriteBuffer($$, $1.ident);	      /* a, a, i */
		      bufferWriteOp($$, SWFACTION_GETMEMBER); /* a, a.i */
		      bufferConcat($$, $3);		      /* a, a.i, v */
		      bufferWriteOp($$, $2);		      /* a, a.i+v */
		      bufferConcat($$, $1.ident);	      /* a, a.i+v, i */
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i, a.i+v */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteOp($$, SWFACTION_SETMEMBER); /* a.i = a.i+v */
		      bufferWriteRegister($$, 0);
		    }
		    else
		    {
		      $$ = $1.memexpr;			      /* i */
		      bufferConcat($$, $1.obj);		      /* i, a */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i */
		      bufferWriteOp($$, SWFACTION_PUSHDUP);	      /* a, i, i */
		      bufferWriteRegister($$, 0);	      /* a, i, i, a */
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i, a, i */
		      bufferWriteOp($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		      bufferConcat($$, $3);		      /* a, i, a[i], v */
		      bufferWriteOp($$, $2);		      /* a, i, a[i]+v */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteOp($$, SWFACTION_SETMEMBER); /* a[i] = a[i]+v */
		      bufferWriteRegister($$, 0);
		    }
		  }
		  else
		  {
		    $$ = $1.ident;
		    bufferWriteOp($$, SWFACTION_PUSHDUP);
		    bufferWriteOp($$, SWFACTION_GETVARIABLE);
		    bufferConcat($$, $3);
		    bufferWriteOp($$, $2);
		    bufferWriteSetRegister($$, 0);
		    bufferWriteOp($$, SWFACTION_SETVARIABLE);
		    bufferWriteRegister($$, 0);
		  }
		}
	;

primary_constant
	: BOOLEAN
		{ $$ = newBuffer();
		  bufferWriteBoolean($$, $1); }
	| NULLVAL
		{ $$ = newBuffer();
		  bufferWriteNull($$); }

	| UNDEFINED
		{ $$ = newBuffer();
		  bufferWriteUndef($$); }

	| STRING
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  free($1); }
	| INTEGER
		{ $$ = newBuffer();
		  bufferWriteInt($$, $1); }
	| DOUBLE
		{ $$ = newBuffer();
		  bufferWriteDouble($$, $1); }
	;

init_vars
	: init_var

	| init_vars ',' init_var
		{ $$ = $1;
		  bufferConcat($$, $3); }
	;

init_var
	: identifier type_attr '=' expr_or_obj
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  free($1);
		  bufferConcat($$, $4);
		  bufferWriteOp($$, SWFACTION_DEFINELOCAL); }

	| identifier type_attr
		{ $$ = newBuffer();
		  bufferWriteString($$, $1, strlen($1)+1);
		  free($1);
		  bufferWriteOp($$, SWFACTION_DEFINELOCAL2); }
	;

assign_stmt
	: ASM '{'
		{ asmBuffer = newBuffer(); }
	  opcode_list '}'
		{ $$ = asmBuffer; }

	| VAR init_vars
		{ $$ = $2; }

	| void_function_call

	| delete_call
		{ $$ = $1;
		  bufferWriteOp($$, SWFACTION_POP); }

	| function_call
		{ $$ = $1;
		  bufferWriteOp($$, SWFACTION_POP); }

	| method_call
		{ $$ = $1;
		  bufferWriteOp($$, SWFACTION_POP); }

	| incdecop lvalue %prec INCR
		{ if($2.obj)
		  {
		    if($2.ident)
		    {
		      $$ = $2.obj;		              /* a */
		      bufferWriteOp($$, SWFACTION_PUSHDUP);	      /* a, a */
		      bufferWriteBuffer($$, $2.ident);	      /* a, a, i */
		      bufferWriteOp($$, SWFACTION_GETMEMBER); /* a, a.i */
		      bufferWriteOp($$, $1);		      /* a, a.i+1 */
		      bufferConcat($$, $2.ident);	      /* a, a.i+1, i */
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i, a.i+1 */
		      bufferWriteOp($$, SWFACTION_SETMEMBER); /* a.i = a.i+1 */
		    }
		    else
		    {
		      /* weird contortions so that $2.memexpr can use reg 0 */
		      $$ = $2.memexpr;			      /* i */
		      bufferConcat($$, $2.obj);		      /* i, a */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i */
		      bufferWriteOp($$, SWFACTION_PUSHDUP);	      /* a, i, i */
		      bufferWriteRegister($$, 0);	      /* a, i, i, a */
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i, a, i */
		      bufferWriteOp($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		      bufferWriteOp($$, $1);		      /* a, i, a[i]+1 */
		      bufferWriteOp($$, SWFACTION_SETMEMBER); /* a[i] = a[i]+1 */
		    }
		  }
		  else
		  {
		    $$ = $2.ident;
		    bufferWriteOp($$, SWFACTION_PUSHDUP);
		    bufferWriteOp($$, SWFACTION_GETVARIABLE);
		    bufferWriteOp($$, $1);
		    bufferWriteOp($$, SWFACTION_SETVARIABLE);
		  }
		}

	| lvalue incdecop %prec POSTFIX
		{ if($1.obj)
		  {
		    if($1.ident)
		    {
		      $$ = $1.obj;			      /* a */
		      bufferWriteOp($$, SWFACTION_PUSHDUP);       /* a, a */
		      bufferWriteBuffer($$, $1.ident);	      /* a, a, i */
		      bufferWriteOp($$, SWFACTION_GETMEMBER); /* a, a.i */
		      bufferWriteOp($$, $2);                  /* a, a.i+1 */
		      bufferConcat($$, $1.ident);             /* a, a.i+1, i */
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i, a.i+1 */
		      bufferWriteOp($$, SWFACTION_SETMEMBER); /* a.i = a.i+1 */
		    }
		    else
		    {
		      /* weird contortions so that $1.memexpr can use reg 0 */
		      $$ = $1.memexpr;	/* i */
		      bufferConcat($$, $1.obj);		      /* i, a */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i */
		      bufferWriteOp($$, SWFACTION_PUSHDUP);       /* a, i, i */
		      bufferWriteRegister($$, 0);             /* a, i, i, a */
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i, a, i */
		      bufferWriteOp($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		      bufferWriteOp($$, $2);                  /* a, i, a[i]+1 */
		      bufferWriteOp($$, SWFACTION_SETMEMBER); /* a[i] = a[i]+1 */
		    }
		  }
		  else
		  {
		    $$ = $1.ident;
		    bufferWriteOp($$, SWFACTION_PUSHDUP);	
		    bufferWriteOp($$, SWFACTION_GETVARIABLE);
		    bufferWriteOp($$, $2);
		    bufferWriteOp($$, SWFACTION_SETVARIABLE);
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
		    bufferWriteOp($$, SWFACTION_SETMEMBER);
		  }
		  else
		  {
		    $$ = $1.ident;
		    bufferConcat($$, $3);
		    bufferWriteOp($$, SWFACTION_SETVARIABLE);
		  }
		}

	| lvalue assignop expr
		{ if($1.obj)
		  {
		    if($1.ident)
		    {
		      $$ = $1.obj;			      /* a */
		      bufferWriteOp($$, SWFACTION_PUSHDUP);	      /* a, a */
		      bufferWriteBuffer($$, $1.ident);	      /* a, a, i */
		      bufferWriteOp($$, SWFACTION_GETMEMBER); /* a, a.i */
		      bufferConcat($$, $3);		      /* a, a.i, v */
		      bufferWriteOp($$, $2);		      /* a, a.i+v */
		      bufferConcat($$, $1.ident);	      /* a, a.i+v, i */
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i, a.i+v */
		      bufferWriteOp($$, SWFACTION_SETMEMBER); /* a.i = a.i+v */
		    }
		    else
		    {
		      $$ = $1.memexpr;			      /* i */
		      bufferConcat($$, $1.obj);		      /* i, a */
		      bufferWriteSetRegister($$, 0);
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i */
		      bufferWriteOp($$, SWFACTION_PUSHDUP);	      /* a, i, i */
		      bufferWriteRegister($$, 0);	      /* a, i, i, a */
		      bufferWriteOp($$, SWFACTION_STACKSWAP);      /* a, i, a, i */
		      bufferWriteOp($$, SWFACTION_GETMEMBER); /* a, i, a[i] */
		      bufferConcat($$, $3);		      /* a, i, a[i], v */
		      bufferWriteOp($$, $2);		      /* a, i, a[i]+v */
		      bufferWriteOp($$, SWFACTION_SETMEMBER); /* a[i] = a[i]+v */
		    }
		  }
		  else
		  {
		    $$ = $1.ident;
		    bufferWriteOp($$, SWFACTION_PUSHDUP);
		    bufferWriteOp($$, SWFACTION_GETVARIABLE);
		    bufferConcat($$, $3);
		    bufferWriteOp($$, $2);
		    bufferWriteOp($$, SWFACTION_SETVARIABLE);
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
				{ $<len>$ = bufferWriteOp(asmBuffer,
						     SWFACTION_WITH); }
	  opcode_list END	{ $$ = $<len>2 + $3;
				  bufferPatchLength(asmBuffer, $3); }
	;

push_item
	: STRING		{ $$ = bufferWriteConstantString(asmBuffer, $1,
								 strlen($1)+1); }

	| INTEGER		{ $$ = bufferWriteInt(asmBuffer, $1); }

	| DOUBLE		{ $$ = bufferWriteDouble(asmBuffer, $1); }

	| BOOLEAN		{ bufferWriteU8(asmBuffer, PUSH_BOOLEAN);
				  $$ = bufferWriteU8(asmBuffer, $1)+1;
				  bufferPatchPushLength(asmBuffer, 2); }

	| NULLVAL		{ $$ = bufferWriteU8(asmBuffer, PUSH_NULL);
				  bufferPatchPushLength(asmBuffer, 1); }

	| UNDEFINED		{ $$ = bufferWriteU8(asmBuffer, PUSH_UNDEF);
				  bufferPatchPushLength(asmBuffer, 1); }

	| REGISTER		{ bufferWriteU8(asmBuffer, PUSH_REGISTER);
				  $$ = bufferWriteU8(asmBuffer,
						     (char)atoi($1))+1;
				  bufferPatchPushLength(asmBuffer, 2); }
	;


push_list
	: push_item			{ $$ = $1; }
	| push_list ',' push_item	{ $$ = $1 + $3; }
	;

opcode
	: PUSH 			{ $<len>$ = bufferWritePushOp(asmBuffer);
				  $<len>$ += bufferWriteS16(asmBuffer, 0); }
	  push_list		{ $$ = $<len>2 + $3;
				  bufferPatchLength(asmBuffer, $3); }

	| with

	| SETREGISTER REGISTER
				{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_STOREREGISTER);
				  $$ += bufferWriteS16(asmBuffer, 1);
				  $$ += bufferWriteU8(asmBuffer,
						      (char)atoi($2)); }
	/* no args */
	| CALLFUNCTION		{ $$ = bufferWriteOp(asmBuffer,
						     SWFACTION_CALLFUNCTION); }
	| RETURN		{ $$ = bufferWriteOp(asmBuffer,
						     SWFACTION_RETURN); }
	| CALLMETHOD	{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_CALLMETHOD); }
	| NEWOBJECT		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_NEWOBJECT); }
	| NEWMETHOD		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_NEWMETHOD); }
	| AND			{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_BITWISEAND); }
	| OR			{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_BITWISEOR); }
	| XOR			{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_BITWISEXOR); }
	| MODULO		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_MODULO); }
	| ADD			{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_ADD2); }
	| LESSTHAN		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_LESS2); }
	| EQUALS		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_EQUALS2); }
	| INC			{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_INCREMENT); }
	| DEC			{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_DECREMENT); }
	| TYPEOF		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_TYPEOF); }
	| INSTANCEOF	{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_INSTANCEOF); }
	| ENUMERATE		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_ENUMERATE); }
	| ENUMERATE2		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_ENUMERATE2); }
	| DELETE		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_DELETE); }
	| DELETE2		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_DELETE2); }
	| NEW			{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_NEWOBJECT); }
	| INITARRAY		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_INITARRAY); }
	| INITOBJECT		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_INITOBJECT); }
	| GETMEMBER		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_GETMEMBER); }
	| SETMEMBER		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_SETMEMBER); }
	| SHIFTLEFT		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_SHIFTLEFT); }
	| SHIFTRIGHT		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_SHIFTRIGHT); }
	| SHIFTRIGHT2		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_SHIFTRIGHT2); }
	| VAR			{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_DEFINELOCAL2); }
	| EXTENDS		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_EXTENDS); }
	| TARGETPATH		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_TARGETPATH); }

	/* TODO: improve this to accept arguments */
	| SWFACTION INTEGER	{ $$ = bufferWriteOp(asmBuffer,
						     (char)$2); }

	| IMPLEMENTS		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_IMPLEMENTSOP); }
	| FSCOMMAND2		{ $$ = bufferWriteOp(asmBuffer, 
						     SWFACTION_FSCOMMAND2); }
	| CAST			{ $$ = bufferWriteOp(asmBuffer,
						     SWFACTION_CASTOP);	}

	/* f4 ops */
	| OLDADD		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_ADD); }
	| SUBTRACT		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_SUBTRACT); }
	| MULTIPLY		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_MULTIPLY); }
	| DIVIDE		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_DIVIDE); }
	| OLDEQUALS		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_EQUAL); }
	| OLDLESSTHAN		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_LESSTHAN); }
	| LOGICALAND		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_LOGICALAND); }
	| LOGICALOR		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_LOGICALOR); }
	| NOT			{ $$ = bufferWriteOp(asmBuffer, SWFACTION_LOGICALNOT); }
	| STRINGEQ		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_STRINGEQ); }
	| STRINGLENGTH		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_STRINGLENGTH); }
	| SUBSTRING		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_SUBSTRING); }
	| INT			{ $$ = bufferWriteOp(asmBuffer, SWFACTION_INT); }
	| DUP			{ $$ = bufferWriteOp(asmBuffer, SWFACTION_PUSHDUP); }
	| SWAP			{ $$ = bufferWriteOp(asmBuffer, SWFACTION_STACKSWAP); }
	| POP			{ $$ = bufferWriteOp(asmBuffer, SWFACTION_POP); }
	| GETVARIABLE		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_GETVARIABLE); }
	| SETVARIABLE		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_SETVARIABLE); }
	| GETPROPERTY		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_GETPROPERTY); }
	| SETPROPERTY		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_SETPROPERTY); }
	| TONUMBER		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_TONUMBER); }
	| TOSTRING		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_TOSTRING); }
	| SETTARGETEXPRESSION	{ $$ = bufferWriteOp(asmBuffer, SWFACTION_SETTARGET2); }
	| CONCAT		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_STRINGCONCAT); }
	| DUPLICATEMOVIECLIP	{ $$ = bufferWriteOp(asmBuffer, SWFACTION_DUPLICATECLIP); }
	| REMOVEMOVIECLIP	{ $$ = bufferWriteOp(asmBuffer, SWFACTION_REMOVECLIP); }
	| TRACE			{ $$ = bufferWriteOp(asmBuffer, SWFACTION_TRACE); }
	| STRINGLESSTHAN	{ $$ = bufferWriteOp(asmBuffer, SWFACTION_STRINGCOMPARE); }
	| RANDOM		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_RANDOMNUMBER); }
	| MBLENGTH		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_MBLENGTH); }
	| ORD			{ $$ = bufferWriteOp(asmBuffer, SWFACTION_ORD); }
	| CHR			{ $$ = bufferWriteOp(asmBuffer, SWFACTION_CHR); }
	| GETTIMER		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_GETTIME); }
	| MBSUBSTRING		{ $$ = bufferWriteOp(asmBuffer, SWFACTION_MBSUBSTRING); }
	| MBORD			{ $$ = bufferWriteOp(asmBuffer, SWFACTION_MBORD); }
	| MBCHR			{ $$ = bufferWriteOp(asmBuffer, SWFACTION_MBCHR); }

	/* with args */
	| BRANCHALWAYS STRING	{ $$ = bufferWriteOp(asmBuffer, SWFACTION_JUMP);
				  $$ += bufferWriteS16(asmBuffer, 2);
				  $$ += bufferBranchTarget(asmBuffer, $2); }

	| BRANCHIFTRUE STRING	{ $$ = bufferWriteOp(asmBuffer, SWFACTION_IF);
				  $$ += bufferWriteS16(asmBuffer, 2);
				  $$ += bufferBranchTarget(asmBuffer, $2); }

	;

property
	: STRING	 	{ $$ = lookupProperty($1); } // Ming extension !
	| _P_X 			{ $$ = PROPERTY_X; }
	| _P_Y 			{ $$ = PROPERTY_Y; }
	| _P_XSCALE		{ $$ = PROPERTY_XSCALE; }
	| _P_YSCALE		{ $$ = PROPERTY_YSCALE; }
	| _P_CURRENTFRAME	{ $$ = PROPERTY_CURRENTFRAME; }
	| _P_TOTALFRAMES	{ $$ = PROPERTY_TOTALFRAMES; }
	| _P_ALPHA		{ $$ = PROPERTY_ALPHA; }
	| _P_VISIBLE		{ $$ = PROPERTY_VISIBLE; }
	| _P_WIDTH		{ $$ = PROPERTY_WIDTH; }
	| _P_HEIGHT		{ $$ = PROPERTY_HEIGHT; }
	| _P_ROTATION		{ $$ = PROPERTY_ROTATION; }
	| _P_TARGET		{ $$ = PROPERTY_TARGET; }
	| _P_FRAMESLOADED	{ $$ = PROPERTY_FRAMESLOADED; }
	| _P_NAME		{ $$ = PROPERTY_NAME; }
	| _P_DROPTARGET		{ $$ = PROPERTY_DROPTARGET; }
	| _P_URL		{ $$ = PROPERTY_URL; }
	| _P_HIGHQUALITY	{ $$ = PROPERTY_HIGHQUALITY; }
	| _P_FOCUSRECT		{ $$ = PROPERTY_FOCUSRECT; }
	| _P_SOUNDBUFTIME	{ $$ = PROPERTY_SOUNDBUFTIME; }	
	| _P_QUALITY		{ $$ = PROPERTY_QUALITY; }
	| _P_XMOUSE		{ $$ = PROPERTY_XMOUSE; }
	| _P_YMOUSE		{ $$ = PROPERTY_YMOUSE; }
	;
%%

