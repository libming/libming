/*
  take a stack, turn it into a tree.
  e.g.:

  cmd        stack
  push 'a'   'a'
  push 'a'   'a' 'a'
  getvar     'a'  a
  push '1'   'a'  a   1
  add        'a'  a+1
  setvar     a = a+1
*/

#define _GNU_SOURCE

#include <assert.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "decompile.h"
#include "read.h"
#include "action.h"

/*
 * Start Package 
 *
 * A package to build up a string that can be returned to the caller
 */

static int strsize=0;
static int strmaxsize=0;
static char *dcstr=NULL;
static char *dcptr=NULL;

#define DCSTRSIZE 1024

void
dcinit()
{
	strsize=0;
	strmaxsize=DCSTRSIZE;
	dcstr=malloc(DCSTRSIZE);
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

#define puts(s) dcputs(s)
#define putchar(c) dcputchar(c)
#define printf dcprintf

/* End Package */

static int gIndent;

typedef enum
{
  BRANCH_NONE = 0,
  BRANCH_WHILE,
  BRANCH_DO,
  BRANCH_IF,
  BRANCH_ELSE
} Branchtype;

static int readStatements(FILE *f, int length, Stack **slist);
void decompileStatements(Stack *statements, int n);

static void listItem(Stack s, Action parent);
static void resolveOffsets(Stack *statements, int nStatements);
static void untangleBranches(Stack *statements, int start, int stop,
			     Branchtype type, int indent);
static Stack negateExpression(Stack s);
static void destroyStack(Stack s);
void showStack(Stack s);
static void destroyTree(Tree t);

char **dictionary;
Stack reg0;

static int
isNum(const char *s)
{
  float f = atof(s);

  if(f != 0 || s[0] == '0') return true;
  else return false;
}

int intVal(Stack s)
{
  if(s->type == 'i')
    return s->data.inum;

  if(s->type == 'd')
    return (int)floor(s->data.dnum);

  if(s->type == 's')
    return atoi(s->data.string);

  if(s->type == 't')
    error("Can't call intVal on a tree!");

  return 0;
}

static Stack stack = NULL;

static Stack newStack()
{
  Stack s = (Stack)malloc(sizeof(struct _stack));
  memset(s, 0, sizeof(struct _stack));
  s->target = -1;
  return s;
}

static void
destroyTree(Tree t)
{
	if(!t) return;
	destroyStack(t->left);
	destroyStack(t->right);
	free(t);
}

static void
destroyStack(Stack s)
{
  if(!s) return;
  if(s->type == 't') destroyTree(s->data.tree);
  //if(s->type == 's') free(s->data.string);
  free(s);
}

#define min(a,b) (((a)<(b))?(a):(b))

/* XXX - this is lame nomenclature */
static Stack newTreeBase(Stack left, Action action, Stack right)
{
  Tree t = (Tree)malloc(sizeof(struct _tree));
  Stack s = newStack();

  t->left = left;
  t->right = right;
  t->action = action;

  s->type = 't';
  s->data.tree = t;

  return s;
}

static Stack newTree(Stack left, Action action, Stack right)
{
  Stack s = newTreeBase(left, action, right);

  /* propagate offset values- tree head should have offset = min of its
     children */

  if(left == NULL)
  {
    if(right == NULL)
      s->offset = 0;
    else
      s->offset = right->offset;
  }
  else
  {
    if(right == NULL)
      s->offset = left->offset;
    else
      s->offset = min(left->offset, right->offset);
  }

  return s;
}

static Stack newString(const char *string)
{
  Stack s = newStack();

  s->type = 's';
  s->data.string = string;

  return s;
}

static Stack newDouble(double d)
{
  Stack s = newStack();

  s->type = 'd';
  s->data.dnum = d;

  return s;
}

static Stack newInteger(int i)
{
  Stack s = newStack();

  s->type = 'i';
  s->data.inum = i;

  return s;
}

static Stack newProperty(Property prop)
{
  Stack s = newStack();

  s->type = 'p';
  s->data.prop = prop;

  return s;
}

static Stack newStackArray(Stack *stkp)
{
  Stack s = newStack();

  s->type = 'S';
  s->data.sptr = stkp;

  return s;
}

static Stack pop()
{
  Stack s = stack;

  if(!stack)
    error("blown stack!");

  stack = stack->next;

  return s;
}

static void push(Stack s)
{
  s->next = stack;
  stack = s;
}

static char *
negateString(const char *s)
{
	int l = strlen(s)+1;
	char *new = malloc(l+1);

	memcpy(new, s+1, l);
	new[0] = '-';

	return new;
}

/* looks like setProperty has been replaced by setMember in F5
   but it still uses the pushdata property type */

static Property getSetProperty(int prop)
{
  switch(prop)
  {
    case SWF_SETPROPERTY_X:               return PROPERTY_X;
    case SWF_SETPROPERTY_Y:               return PROPERTY_Y;
    case SWF_SETPROPERTY_XSCALE:          return PROPERTY_XSCALE;
    case SWF_SETPROPERTY_YSCALE:          return PROPERTY_YSCALE;
    case SWF_SETPROPERTY_ALPHA:           return PROPERTY_ALPHA;
    case SWF_SETPROPERTY_VISIBILITY:      return PROPERTY_VISIBLE;
    case SWF_SETPROPERTY_ROTATION:        return PROPERTY_ROTATION;
    case SWF_SETPROPERTY_NAME:            return PROPERTY_NAME;
    case SWF_SETPROPERTY_HIGHQUALITY:     return PROPERTY_HIGHQUALITY;
    case SWF_SETPROPERTY_SHOWFOCUSRECT:   return PROPERTY_FOCUSRECT;
    case SWF_SETPROPERTY_SOUNDBUFFERTIME: return PROPERTY_SOUNDBUFTIME;
    default: return -1;
  }
}

static int getInteger(Stack s)
{
	if( s->type != 'i' ) error("getInteger() not an interger");
	return s->data.inum;
}

static const char *getString(Stack s)
{
	if( s->type != 's' ) error("getString() not an string");
	return s->data.string;
}

static Stack *getStackArray(Stack s)
{
	if( s->type != 'S' ) error("getStackArray() not a Stack Array");
	return s->data.sptr;
}

static Stack readActionRecord(FILE *f)
{
  int length = 0, type = readUInt8(f);

  if((type&0x80) == 0x80)
    length = readUInt16(f);

  switch(type)
  {
    case SWFACTION_POP:
      return newTree(NULL, type, NULL);
      /*      return NULL; */

    /* no-arg */
    case SWFACTION_GETTIMER:
    case SWFACTION_STOPDRAGMOVIE:
    case SWFACTION_NEXTFRAME:
    case SWFACTION_PREVFRAME:
    case SWFACTION_PLAY:
    case SWFACTION_STOP:
    case SWFACTION_TOGGLEQUALITY:
    case SWFACTION_STOPSOUNDS:
      return newTree(NULL, type, NULL);

    /* one-arg */
    case SWFACTION_STRINGLENGTH:
    case SWFACTION_INT:
    case SWFACTION_RANDOM:
    case SWFACTION_MBLENGTH:
    case SWFACTION_ORD:
    case SWFACTION_CHR:
    case SWFACTION_MBORD:
    case SWFACTION_MBCHR:
    case SWFACTION_LOGICALNOT:
    case SWFACTION_GETVARIABLE:
    case SWFACTION_REMOVECLIP:
    case SWFACTION_TRACE:
    case SWFACTION_SETTARGETEXPRESSION:
    case SWFACTION_CALLFRAME:
      return newTree(pop(), type, NULL);

    /* two-arg */
    case SWFACTION_ADD:
    case SWFACTION_MULTIPLY:
    case SWFACTION_DIVIDE:
    case SWFACTION_EQUAL:
    case SWFACTION_LESSTHAN:
    case SWFACTION_LOGICALAND:
    case SWFACTION_LOGICALOR:
    case SWFACTION_STRINGEQ:
    case SWFACTION_STRINGCONCAT:
    case SWFACTION_STRINGCOMPARE:
	{
      Stack right = pop();
      Stack left = pop();
      return newTree(left, type, right);
	}

    case SWFACTION_SETVARIABLE:
    {
      Stack right = pop();
      Stack left = pop();

      if(right->type == 't' &&
	 right->data.tree->action == SWFACTION_SETREGISTER)
      {
	/* copy tree to register so we can use it on getregister */
	/* and remove the register bit so we don't loop endlessly */
	reg0 = newTree(left, type, right->data.tree->right);

	/* Unlink from the tree we are about to destroy */
	right->data.tree->right=NULL;
	destroyStack(right);

	return NULL;
      }
      else
	return newTree(left, type, right);
    }

    case SWFACTION_GETPROPERTY:
    {
      Stack right = pop();
      Stack left = pop();

      if(right->type == 's')
      {
	Stack New = newProperty(atoi(right->data.string));
	destroyStack(right);
	right = New;
      }
      return newTree(left, type, right);
    }

    case SWFACTION_SUBTRACT:
    {
      Stack right = pop();
      Stack left = pop();

      /* XXX - shouldn't we move this to listArithmetic? */
      if(left->type == 's' &&
	 strcmp(left->data.string, "0") == 0)
      {
	destroyStack(left);
	right->data.string = negateString(right->data.string);
	return right;
      }

      return newTree(left, type, right);
    }

    /* three-arg */
    case SWFACTION_SETPROPERTY:
	{
	  Stack value = pop();
	  Stack property = pop();
	  Stack target = pop();

      if(property->type == 's')
      {
	Stack New = newProperty(atoi(property->data.string));
	destroyStack(property);
	property = New;
      }
	
      return newTree(newTree(target, type, property),
		     SWFACTION_SETVARIABLE, value);
	}

    case SWFACTION_MBSUBSTRING:
    case SWFACTION_SUBSTRING:
	{
	  Stack s3 = pop();
	  Stack s2 = pop();
	  Stack s1 = pop();
      return newTree(s1, type, newTree(s2, type, s3));
	}

    case SWFACTION_DUPLICATECLIP:
    {
      Stack level = pop();
      Stack target = pop();
      Stack source = pop();
      Stack arg;

      if(level->type == 'i' &&
	 level->data.inum >= DUPCLIP_NUMBER)
      {
	arg = level;
	arg->data.inum -= DUPCLIP_NUMBER;
      }
      else
      {
	if(level->type != 't' ||
	   (level->data.tree->action != SWFACTION_ADD &&
	    level->data.tree->action != SWFACTION_NEWADD))
	  error("magic number 0x4000 not found in duplicateClip target level!");

	if(level->data.tree->left->type == 'i' &&
	   level->data.tree->left->data.inum == DUPCLIP_NUMBER)
	{
	  arg = level->data.tree->right;
	  level->data.tree->right = NULL;
	}
	else if(level->data.tree->right->type == 'i' &&
		level->data.tree->right->data.inum == DUPCLIP_NUMBER)
	{
	  arg = level->data.tree->left;
	  level->data.tree->left = NULL;
	}
	else
	{
	  error("magic number 0x4000 not found in duplicateClip target level!");
          return NULL;
	}

	destroyStack(level);
      }

      return newTree(source, type, newTree(target, type, arg));
    }


    /* weird ops */
    case SWFACTION_STARTDRAGMOVIE:
    {
      Stack target = pop();
      Stack lockmouse = pop();
      Stack constraint = pop();

      if(constraint->type == 't')
	error("Sorry, decompiler can't deal with conditional constraint in dragMovie!");

      if(intVal(constraint) == 0)
	return newTree(constraint, type, newTree(lockmouse, type, target));
      else {
		Stack p4 = pop();
		Stack p3 = pop();
		Stack p2 = pop();
		Stack p1 = pop();

	return newTree(newTree(newTree(p1, type, p2), type,
			       newTree(p3, type, p4)), type,
		       newTree(lockmouse, type, target));
	  }
    }

    case SWFACTION_PUSHDATA:
    {
      Stack s = NULL;
      int end = fileOffset + length;
      int type;

      int off = fileOffset - 3; /* save statement offset */

      while(fileOffset < end)
      {
	if(s != NULL)
	  push(s);

	switch(type = readUInt8(f))
	{
	  case 0: /* string */
	    s = newString(readString(f));
	    break;
	  case 1: /* property - not used? */
	    readUInt16(f); /* 0x0000 */
	    s = newProperty(getSetProperty(readUInt16(f)));
	    break;
	  case 2: /* null? */
	    s = newString("NULL");
	    break;
	  case 3: /* ??? */
	    s = newString("data type 0x03 (?)");
	    break;
	  case 4: /* register? */
	    if(readUInt8(f) != 0)
	      error("Sorry, can't deal with other than reg0!");

	    s = reg0;
	    s->offset = reg0->offset;
	    //	    reg0 = NULL;
	    break;
	  case 5: /* boolean? */
	    s = newString((readUInt8(f) == 0) ? "false" : "true");
	    break;
	  case 6: /* double */
	    s = newDouble(readDouble(f));
	    break;
	  case 7: /* int */
	    s = newInteger(readSInt32(f));
	    break;
	  case 8: /* dictionary ref */
	    s = newString(dictionary[readUInt8(f)]);
	    break;

	  default:
	    error("Unknown data type %i", type);
	}

	if(s->offset == 0)
	  s->offset = off;
      }

      return s;
    }

    case SWFACTION_GOTOFRAME:
      return newTreeBase(newInteger(readUInt16(f)), type, NULL);

    case SWFACTION_GETURL:
    {
      char *url = readString(f);
      char *target = readString(f);
      return newTreeBase(newString(url), type, newString(target));
    }

    case SWFACTION_GETURL2:
    {
	  Stack target = pop();
	  Stack url = pop();

      Stack s = newTree(url, type, target);
      Stack t = newTreeBase(newInteger(readUInt8(f)), type, s);
      t->offset = s->offset;
      return t;
    }

    case SWFACTION_WAITFORFRAMEEXPRESSION:
      return newTreeBase(newInteger(readUInt8(f)), type, NULL);

    case SWFACTION_GOTOEXPRESSION:
    {
      Stack s = pop();
      Stack t = newTreeBase(s, type, newInteger(readUInt8(f)));
      t->offset = s->offset;
      return t;
    }

    case SWFACTION_SETTARGET:
    case SWFACTION_GOTOLABEL:
      return newTreeBase(newString(readString(f)), type, NULL);

    /* branches */
    case SWFACTION_BRANCHIFTRUE:
    {
      Stack s = pop(), t;
      int offset = readSInt16(f);
      Action action;

      /* if there's a dup or !dup in the condition, we've got an || or && */

      /* XXX - silly hackery wrapping things in nots so the untangler
	 recognises it.  Would be nice to clean up. */

      if(s->type == 't' &&
	 ((action = s->data.tree->action) == SWFACTION_DUP || /* it's an or */
	  ((action = s->data.tree->action) == SWFACTION_LOGICALNOT && /* and */
	   s->data.tree->left->type == 't' &&
	   s->data.tree->left->data.tree->action == SWFACTION_DUP)))
      {
	int start = fileOffset; /* we're at the start of the next statement */
	int off;
	int end = start+offset; /* should be a logical not statement */

	if(action == SWFACTION_DUP)
	  push(newTree(s, SWFACTION_LOGICALOR, NULL));
	else
	{
	  push(newTree(s->data.tree->left, SWFACTION_LOGICALAND, NULL));

	  /* Unlink from the tree we are about to destroy */
	  s->data.tree->left=NULL;
	  destroyStack(s);
	}

	/* now grab statements between here and end */

	while(fileOffset < end)
	{
	  if(feof(f))
	    break;

	  off = fileOffset;

	  if((s = readActionRecord(f)) != NULL)
	  {
	    push(s);

	    if(s->offset == 0)
	      s->offset = off;
	  }
	}

	while(stack && stack->next &&
	      stack->next->type == 't')
	{
	  if(stack->next->data.tree->action == SWFACTION_POP &&
	     (stack->next->next->data.tree->action == SWFACTION_LOGICALAND ||
	      stack->next->next->data.tree->action == SWFACTION_LOGICALOR))
	  {
	    t = pop();
	    pop();

	    if(stack->data.tree->right != NULL)
	      error("Was expecting logical op's right side to be empty!");

	    stack->data.tree->right = t;
	  }
	  else
	    break;
	}

	return NULL;
      }

      /* check for conditions left on the stack from code above */

      while(stack && stack->type == 't')
      {
	if(stack->data.tree->action == SWFACTION_LOGICALOR)
	{
	  stack->data.tree->right = negateExpression(s);
	  s = newTree(pop(), SWFACTION_LOGICALNOT, NULL);
	}
	else if(stack->data.tree->action == SWFACTION_LOGICALAND)
	{
	  stack->data.tree->right = negateExpression(s);
	  s = newTree(pop(), SWFACTION_LOGICALNOT, NULL);
	}
	else
	  break;
      }

      t = newTreeBase(s, SWFACTION_BRANCHIFTRUE, newInteger(offset));

      t->offset = s->offset;
      return t;
    }

    case SWFACTION_BRANCHALWAYS:
      return newTreeBase(NULL, type, newInteger(readSInt16(f)));

    case SWFACTION_WAITFORFRAME:
	{
	  Stack left = newInteger(readUInt16(f));
	  Stack right = newInteger(readUInt8(f));
      return newTreeBase(left, type, right);
	}

    case SWFACTION_END:
      return NULL;


    /* v5 ops */
    case SWFACTION_VAR:
      return newTree(pop(), type, NULL);

    case SWFACTION_VAREQUALS:
	{
      Stack right = pop();
      Stack left = pop();

      return newTree(left, type, right);
	}

    case SWFACTION_DELETE:
      return newTree(pop(), type, NULL);

    case SWFACTION_CALLFUNCTION:
    {
      Stack last = NULL, t;
      int i, off;

      Stack name = pop();
      Stack nargs = pop();

      if(nargs->type == 't')
	error("Sorry, decompiler can't deal with conditional nargs!");

      for(i=intVal(nargs); i>0; --i)
	last = newTree(pop(), type, last);

      if(intVal(nargs) > 0)
	off = last->offset;
      else
	off = nargs->offset;

      last = newTreeBase(newInteger(intVal(nargs)), type, last);

      t = newTreeBase(name, type, last);
      t->offset = off;
      return t;
    }

    case SWFACTION_INCREMENT:
    case SWFACTION_DECREMENT:
    case SWFACTION_TYPEOF:
    case SWFACTION_RETURN:
    case SWFACTION_DUP:
    case SWFACTION_TONUMBER:
    case SWFACTION_TOSTRING:
      return newTree(pop(), type, NULL);

    case SWFACTION_MODULO:
    case SWFACTION_NEW:
    case SWFACTION_NEWADD:
    case SWFACTION_NEWLESSTHAN:
    case SWFACTION_NEWEQUAL:
    case SWFACTION_SHIFTLEFT:
    case SWFACTION_SHIFTRIGHT:
    case SWFACTION_SHIFTRIGHT2:
    case SWFACTION_BITWISEAND:
    case SWFACTION_BITWISEOR:
    case SWFACTION_BITWISEXOR:
    case SWFACTION_GETMEMBER:
    {
      Stack right = pop();
      Stack left = pop();

      return newTree(left, type, right);
    }

    case SWFACTION_SETMEMBER:
    {
      Stack p3 = pop();
      Stack p2 = pop();
      Stack p1 = pop();

      return newTree(p1, type, newTree(p2, type, p3));
    }

    case SWFACTION_CALLMETHOD:
    {
      Stack last = NULL, t;
      int i, off;

      Stack name = pop();
      Stack object = pop();
      Stack nargs = pop();

      if(nargs->type == 't')
	error("Sorry, decompiler can't deal with conditional nargs!");

      for(i=intVal(nargs); i>0; --i)
	last = newTree(pop(), type, last);

      if(intVal(nargs) > 0)
	off = last->offset;
      else
	off = nargs->offset;

      last = newTreeBase(newInteger(intVal(nargs)), type, last);

      t = newTree(name, type, newTree(object, type, last));
      t->offset = off;
      return t;
    }

    case SWFACTION_DECLARENAMES:
    {
      int i, items = readUInt8(f);

      readUInt8(f);

      /* should just keep static dictionary[256]? */
      dictionary = realloc(dictionary, items*sizeof(char *));

      for(i=0; i<items; ++i)
	dictionary[i] = readString(f);

      return NULL;
    }

    case SWFACTION_WITH:
    {
      Stack *statements;
      int size = readUInt16(f);

      int n = readStatements(f, size, &statements);

      return newTree(pop(), type, newTreeBase(newInteger(n), type, newStackArray(statements)));
    }
    case SWFACTION_DEFINEFUNCTION:
    {
      Stack tree, p, *statements;
      int nargs, size, n;

      char *name = readString(f);

      tree = newTreeBase(newString(name), type, NULL);
      nargs = readUInt16(f);

      p = tree;

      for(; nargs>0; --nargs)
      {
	p->data.tree->right = newTreeBase(newString(readString(f)), type, NULL);
	p = p->data.tree->right;
      }

      size = readUInt16(f);

      n = readStatements(f, size, &statements);

      return newTree(tree, type, newTreeBase(newInteger(n), type, newStackArray(statements)));
    }

    case SWFACTION_ENUMERATE:
      return newTree(pop(), type, NULL);

    case SWFACTION_SETREGISTER:
    {
      Stack s, t;

      if(length != 1)
	error("Unexpected length (!=1) in setregister");

      s = pop();
      t = newTreeBase(newInteger(readUInt8(f)), type, s);
      t->offset = s->offset;
      return t;
    }

    case SWFACTION_INITOBJECT:
    {
      Stack *names, *values;
      int i, nEntries = intVal(pop());

      names = malloc(sizeof(Stack) * nEntries);
      values = malloc(sizeof(Stack) * nEntries);

      for(i=0; i<nEntries; ++i)
      {
	names[i] = pop();
	values[i] = pop();
      }

      return newTreeBase(newInteger(nEntries), type,
			 newTreeBase(newStackArray(names), type, newStackArray(values)));
    }

    case SWFACTION_INITARRAY:
    {
      Stack *values;
      int i, nEntries = intVal(pop());

      values = malloc(sizeof(Stack) * nEntries);

      for(i=0; i<nEntries; ++i)
	values[i] = pop();

      return newTreeBase(newInteger(nEntries), type, newStackArray(values));
    }

    default:
      printf("Unknown Action: 0x%02X\n", type); fflush(stdout);
      dumpBytes(f, length);
      putchar('\n');
      assert(0);
      return NULL;
  }
}

static void listProperty(Property prop)
{
  switch(prop)
  {
    case PROPERTY_X:		   printf("_x"); break;
    case PROPERTY_Y:		   printf("_y"); break;
    case PROPERTY_XMOUSE:	   printf("_xMouse"); break;
    case PROPERTY_YMOUSE:	   printf("_yMouse"); break;
    case PROPERTY_XSCALE:	   printf("_xScale"); break;
    case PROPERTY_YSCALE:	   printf("_yScale"); break;
    case PROPERTY_CURRENTFRAME:	   printf("_currentFrame"); break;
    case PROPERTY_TOTALFRAMES:	   printf("_totalFrames"); break;
    case PROPERTY_ALPHA:	   printf("_alpha"); break;
    case PROPERTY_VISIBLE:	   printf("_visible"); break;
    case PROPERTY_WIDTH:	   printf("_width"); break;
    case PROPERTY_HEIGHT:	   printf("_height"); break;
    case PROPERTY_ROTATION:	   printf("_rotation"); break;
    case PROPERTY_TARGET:	   printf("_target"); break;
    case PROPERTY_FRAMESLOADED:	   printf("_framesLoaded"); break;
    case PROPERTY_NAME:		   printf("_name"); break;
    case PROPERTY_DROPTARGET:	   printf("_dropTarget"); break;
    case PROPERTY_URL:  	   printf("_url"); break;
    case PROPERTY_HIGHQUALITY:	   printf("_quality"); break;
    case PROPERTY_FOCUSRECT:       printf("_focusRect"); break;
    case PROPERTY_SOUNDBUFTIME:    printf("_soundBufTime"); break;
    case PROPERTY_WTHIT:	   printf("_WTHIT!?"); break;
    default:			   printf("unknown property!"); break;
  }
}

static int precedence(Action type)
{
  switch(type)
  {
    case SWFACTION_SETVARIABLE:    return 0;

    case SWFACTION_LOGICALAND:     return 1;
    case SWFACTION_LOGICALOR:      return 1;

    case SWFACTION_LOGICALNOT:     return 2;

    case SWFACTION_LESSTHAN:       return 3;
    case SWFACTION_EQUAL:          return 3;
    case SWFACTION_NEWEQUAL:       return 3;

    case SWFACTION_SHIFTLEFT:      return 4;
    case SWFACTION_SHIFTRIGHT:     return 4;
    case SWFACTION_SHIFTRIGHT2:    return 4;

    case SWFACTION_NEWADD:         return 5;
    case SWFACTION_ADD:            return 5;
    case SWFACTION_SUBTRACT:       return 5;

    case SWFACTION_MULTIPLY:       return 6;
    case SWFACTION_DIVIDE:         return 6;

    case SWFACTION_BITWISEAND:     return 7;
    case SWFACTION_BITWISEOR:      return 7;
    case SWFACTION_BITWISEXOR:     return 7;

    case SWFACTION_GETVARIABLE:    return 8;
    case SWFACTION_GETPROPERTY:    return 8;

    case SWFACTION_STRINGEQ:       return 9;
    case SWFACTION_STRINGCONCAT:   return 9;
    case SWFACTION_PUSHDATA:       return 9;

    case SWFACTION_SETPROPERTY:    return 10;

    default:                       return 0;
  }
}

typedef enum
{
  NONEGATE = 0,
  NEGATE = 1
} negateFlag;

static void listLessThan(Stack s, negateFlag negate)
{
  Stack left = s->data.tree->left, right = s->data.tree->right;

  /* put variable on left */
  if(left->type == 's' ||
     (right->type == 't' && right->data.tree->action == SWFACTION_GETVARIABLE))
  {
    listItem(right, SWFACTION_LESSTHAN);

    if(negate == NEGATE)
      printf(" <= ");
    else
      printf(" > ");

    listItem(left, SWFACTION_LESSTHAN);
  }
  else
  {
    listItem(left, SWFACTION_LESSTHAN);

    if(negate == NEGATE)
      printf(" >= ");
    else
      printf(" < ");

    listItem(right, SWFACTION_LESSTHAN);
  }
}

static void listNot(Stack s, Action parent)
{
  /* check for !<, !=, !! */
  /* put variable on left */

  if(s->type == 't')
  {
    Tree t = s->data.tree;

    if(t->action == SWFACTION_LESSTHAN)
    {
      listLessThan(s, NEGATE);
      return;
    }
    else if(t->action == SWFACTION_LOGICALNOT)
    {
      listItem(t->left, parent);
      return;
    }
    else if(t->action == SWFACTION_EQUAL)
    {
      listItem(t->left, SWFACTION_EQUAL);
      printf(" != ");
      listItem(t->right, SWFACTION_EQUAL);
      return;
    }
  }

  printf("!");
  listItem(s, SWFACTION_LOGICALNOT);
}

static void listAssign(Stack s)
{
  Stack left = s->data.tree->left;
  Stack right = s->data.tree->right;

  /* if it's a string, quote it
     (can't do it in listItem because we don't know what side of the equals
     sign we are..) */

  if(right->type == 's')
  {
    listItem(left, SWFACTION_SETVARIABLE);
    puts(" = '");
    listItem(right, SWFACTION_SETVARIABLE);
    putchar('\'');
    return;
  }

  /* check for ++a w/ increment op */

  if(right->type == 't' &&
     (right->data.tree->action == SWFACTION_INCREMENT ||
      right->data.tree->action == SWFACTION_DECREMENT))
  {
    Stack rleft = right->data.tree->left;

    if(rleft->type == 't' &&
       rleft->data.tree->action == SWFACTION_GETVARIABLE &&
       rleft->data.tree->left->type == 's' &&
       left->type == 's' &&
       strcmp(rleft->data.tree->left->data.string, left->data.string) == 0)
    {
      if(right->data.tree->action == SWFACTION_INCREMENT)
	puts("++");
      else
	puts("--");

      listItem(left, SWFACTION_SETVARIABLE);
      return;
    }
  }

  /* check for ++a and a+=b w/ traditional ops */

  if(right->type == 't' &&
     (right->data.tree->action == SWFACTION_NEWADD ||
      (right->data.tree->action >= SWFACTION_ADD &&
       right->data.tree->action <= SWFACTION_DIVIDE)))
  {
    Stack rleft = right->data.tree->left;
    Stack rright = right->data.tree->right;
    const char *op;

    if(right->data.tree->action == SWFACTION_ADD ||
       right->data.tree->action == SWFACTION_NEWADD)
      op = " += ";
    else if(right->data.tree->action == SWFACTION_SUBTRACT)
      op = " -= ";
    else if(right->data.tree->action == SWFACTION_MULTIPLY)
      op = " -= ";
    else if(right->data.tree->action == SWFACTION_DIVIDE)
      op = " /= ";
    else
    {
      error("Unexpected operation in listAssign!");
      return;
    }

    if(rleft->type == 't' &&
       rleft->data.tree->action == SWFACTION_GETVARIABLE &&
       rleft->data.tree->left->type == 's' &&
       left->type == 's' &&
       strcmp(rleft->data.tree->left->data.string, left->data.string) == 0)
    {
      if(rright->type == 's' &&
	 strcmp(rright->data.string, "1") == 0)
      {
	if(right->data.tree->action == SWFACTION_ADD ||
	   right->data.tree->action == SWFACTION_NEWADD)
	{
	  puts("++");
	  listItem(left, SWFACTION_SETVARIABLE);
	  return;
	}
	else if(right->data.tree->action == SWFACTION_SUBTRACT)
	{
	  puts("--");
	  listItem(left, SWFACTION_SETVARIABLE);
	  return;
	}
      }

      listItem(left, SWFACTION_SETVARIABLE);
      puts(op);
      listItem(rright, right->data.tree->action);
      return;
    }
    else if(rright->type == 't' &&
	    rright->data.tree->action == SWFACTION_GETVARIABLE &&
	    rright->data.tree->left->type == 's' &&
	    strcmp(rright->data.tree->left->data.string,
		   left->data.string) == 0)
    {
      if(rleft->type == 's' &&
	 strcmp(rleft->data.string, "1") == 0)
      {
	if(right->data.tree->action == SWFACTION_ADD ||
	   right->data.tree->action == SWFACTION_NEWADD)
	{
	  puts("++");
	  listItem(left, SWFACTION_SETVARIABLE);
	  return;
	}
	else if(right->data.tree->action == SWFACTION_SUBTRACT)
	{
	  puts("--");
	  listItem(left, SWFACTION_SETVARIABLE);
	  return;
	}
      }

      listItem(left, SWFACTION_SETVARIABLE);
      puts(op);
      listItem(rleft, right->data.tree->action);
      return;
    }
  }

  listItem(left, SWFACTION_SETVARIABLE);
  puts(" = ");
  listItem(right, SWFACTION_SETVARIABLE);
}

static void listArithmetic(Stack s, Action parent)
{
  int isShort, parens = 0;
  const char *op;
  Tree t = s->data.tree;
  Stack left = t->left, right = t->right;

  /* leave out spaces around op if either side's just a constant or variable */
  /* but not if op is divide and right side starts w/ '/' */
  isShort = !(t->action == SWFACTION_DIVIDE &&
	      right &&
	      right->type == 't' &&
	      right->data.tree->action == SWFACTION_GETVARIABLE &&
	      right->data.tree->left->data.string[0] == '/') &&
            (left->type == 's' ||
	     (left->type == 't' &&
	      left->data.tree->action == SWFACTION_GETVARIABLE) ||
	     (right && right->type == 's') ||
	     (right && (right->type == 't' &&
	      right->data.tree->action == SWFACTION_GETVARIABLE)));

  switch(t->action)
  {
    case SWFACTION_NEWADD:
    case SWFACTION_ADD:            op = (isShort?"+":" + "); break;
    case SWFACTION_SUBTRACT:       op = (isShort?"-":" - "); break;
    case SWFACTION_MULTIPLY:       op = (isShort?"*":" * "); break;
    case SWFACTION_DIVIDE:         op = (isShort?"/":" / "); break;
    case SWFACTION_MODULO:         op = (isShort?"%":" % "); break;
    case SWFACTION_BITWISEOR:      op = (isShort?"|":" | "); break;
    case SWFACTION_BITWISEAND:     op = (isShort?"&":" & "); break;
    case SWFACTION_BITWISEXOR:     op = (isShort?"^":" ^ "); break;
    case SWFACTION_SHIFTLEFT:      op = (isShort?"<<":" << "); break;
    case SWFACTION_SHIFTRIGHT:     op = (isShort?">>":" >> "); break;
    case SWFACTION_SHIFTRIGHT2:    op = (isShort?">>>":" >>> "); break;

    case SWFACTION_NEWEQUAL:
    case SWFACTION_EQUAL:          op = " == ";   break;
    case SWFACTION_LOGICALAND:     op = " && ";   break;
    case SWFACTION_LOGICALOR:      op = " || ";   break;
    case SWFACTION_STRINGEQ:       op = " eq ";   break;
    case SWFACTION_STRINGCONCAT:   op = " & ";    break;
    case SWFACTION_STRINGCOMPARE:  op = " <=> ";  break;
    default: op = " ??? "; break;
  }

  if(t->action == SWFACTION_MULTIPLY)
  {
    if(left->type == 's' &&
       strcmp(left->data.string, "-1") == 0)
    {
      putchar('-');
      listItem(right, SWFACTION_MULTIPLY);
      return;
    }
    else if(right->type == 's')
    {
      if(strcmp(right->data.string, "-1") == 0)
      {
	putchar('-');
	listItem(left, SWFACTION_MULTIPLY);
	return;
      }
      else
      {
	/* put constant first: a*2 ==> 2*a */

	Stack tmp = left;
	left = right;
	right = tmp;
      }
    }
  }

  /* parenthesization rule */
  parens = (precedence(parent) > precedence(t->action) ||
	    (t->action == SWFACTION_MULTIPLY && parent == SWFACTION_DIVIDE) ||
	    (t->action == SWFACTION_ADD && parent == SWFACTION_SUBTRACT) ||
	    (t->action == SWFACTION_LOGICALAND && parent == SWFACTION_LOGICALOR) ||
	    (t->action == SWFACTION_LOGICALOR && parent == SWFACTION_LOGICALAND));

  if(parens)
    putchar('(');

  listItem(left, t->action);
  puts(op);
  listItem(right, t->action);

  if(parens)
    putchar(')');
}

static void listItem(Stack s, Action parent)
{
  Tree t;

  if( !s ) return;

  if(s->type == 's')
  {
    if(parent == SWFACTION_GETVARIABLE ||
       parent == SWFACTION_SETVARIABLE ||
       parent == SWFACTION_GETPROPERTY ||
       parent == SWFACTION_SETPROPERTY ||
       parent == SWFACTION_GETMEMBER ||
       parent == SWFACTION_SETMEMBER ||
       parent == SWFACTION_DUPLICATECLIP ||
       parent == SWFACTION_NEW ||
       parent == SWFACTION_VAR ||
       parent == SWFACTION_VAREQUALS ||
       parent == SWFACTION_CALLMETHOD ||
       parent == SWFACTION_CALLFUNCTION ||
       isNum(s->data.string))
      puts(s->data.string);
    else
    {
      putchar('\'');
      puts(s->data.string);
      putchar('\'');
    }
  }
  else if(s->type == 'p')
  {
    listProperty(s->data.prop);
  }
  else if(s->type == 'd')
  {
    /* XXX - should prolly check for integerness, too */
    if(s->data.dnum == 0)
      putchar('0');
    else
      printf("%f", s->data.dnum);
  }
  else if(s->type == 'i')
  {
    printf("%i", s->data.inum);
  }
  else if(s->type == 't')
  {
    t = s->data.tree;

    switch(t->action)
      {
      case SWFACTION_POP: /* ignore */
	break;

	/* two args */
      case SWFACTION_ADD:
      case SWFACTION_SUBTRACT:
      case SWFACTION_MULTIPLY:
      case SWFACTION_DIVIDE:
      case SWFACTION_EQUAL:
      case SWFACTION_LOGICALAND:
      case SWFACTION_LOGICALOR:
      case SWFACTION_STRINGEQ:
      case SWFACTION_STRINGCOMPARE:
	listArithmetic(s, parent);
	break;

      case SWFACTION_STRINGCONCAT:
	if(parent == SWFACTION_GETVARIABLE ||
	   parent == SWFACTION_GETPROPERTY)
	{
	  puts("valueOf(");
	  listArithmetic(s, parent);
	  putchar(')');
	}
	else
	  listArithmetic(s, parent);
	break;

      case SWFACTION_SETVARIABLE:
	listAssign(s);
	break;

      case SWFACTION_LESSTHAN:
	listLessThan(s, NONEGATE);
	break;

      case SWFACTION_GETPROPERTY:
	if(t->left->type == 's' &&
	   t->left->data.string[0] == '\0')
	  puts("this");
	else
	  listItem(t->left, SWFACTION_GETPROPERTY);

	putchar('.');

	if(t->right->type == 'i')
	  listProperty(t->right->data.inum);
	else
	  listItem(t->right, SWFACTION_GETPROPERTY);

	break;

      case SWFACTION_SETPROPERTY:
	if(t->left->type == 's' &&
	   t->left->data.string[0] == '\0')
	  puts("this");
	else
	  listItem(t->left, SWFACTION_SETPROPERTY);

	putchar('.');
	listItem(t->right, SWFACTION_SETPROPERTY);
	break;

      case SWFACTION_MBSUBSTRING:
      case SWFACTION_SUBSTRING:
	puts("substr(");
	listItem(t->left, t->action);
	puts(", ");
	listItem(t->right->data.tree->left, t->action);
	puts(", ");
	listItem(t->right->data.tree->right, t->action);
	putchar(')');
	break;
      
	/* one-arg */
      case SWFACTION_LOGICALNOT:
	listNot(t->left, SWFACTION_LOGICALNOT);
	break;

      case SWFACTION_STRINGLENGTH:
	puts("strlen(");
	listItem(t->left, SWFACTION_STRINGLENGTH);
	putchar(')');
	break;

      case SWFACTION_INT:
	puts("int(");
	listItem(t->left, SWFACTION_INT);
	putchar(')');
	break;

      case SWFACTION_RANDOM:
	puts("random(");
	listItem(t->left, SWFACTION_RANDOM);
	putchar(')');
	break;

      case SWFACTION_MBLENGTH:
	puts("mbstrlen(");
	listItem(t->left, SWFACTION_MBLENGTH);
	putchar(')');
	break;

      case SWFACTION_ORD:
	puts("ord(");
	listItem(t->left, SWFACTION_ORD);
	putchar(')');
	break;

      case SWFACTION_CHR:
	puts("chr(");
	listItem(t->left, SWFACTION_CHR);
	putchar(')');
	break;

      case SWFACTION_MBORD:
	puts("mbord(");
	listItem(t->left, SWFACTION_MBORD);
	putchar(')');
	break;

      case SWFACTION_MBCHR:
	puts("mbchr(");
	listItem(t->left, SWFACTION_MBCHR);
	putchar(')');
	break;

      case SWFACTION_TONUMBER:
	puts("Number(");
	listItem(t->left, SWFACTION_TONUMBER);
	putchar(')');
	break;

      case SWFACTION_TOSTRING:
	puts("String(");
	listItem(t->left, SWFACTION_TOSTRING);
	putchar(')');
	break;

      case SWFACTION_GETVARIABLE:
	listItem(t->left, SWFACTION_GETVARIABLE);
	break;

      case SWFACTION_GETTIMER:
	puts("getTimer()");
	break;

	/* statements */
      case SWFACTION_DUPLICATECLIP:
	puts("duplicateClip(");
	listItem(t->left, SWFACTION_DUPLICATECLIP);
	puts(", ");
	listItem(t->right->data.tree->left, SWFACTION_DUPLICATECLIP);
	puts(", ");
	listItem(t->right->data.tree->right, SWFACTION_DUPLICATECLIP);
	putchar(')');
	break;

      case SWFACTION_STARTDRAGMOVIE:
	puts("startDrag(");
	listItem(t->right->data.tree->right, SWFACTION_STARTDRAGMOVIE);
	puts(", ");
	listItem(t->right->data.tree->left, SWFACTION_STARTDRAGMOVIE);

	if(t->left->type == 't')
	{
	  Tree root = t->left->data.tree;
	  puts(", ");
	  listItem(root->left->data.tree->left, SWFACTION_STARTDRAGMOVIE);
	  puts(", ");
	  listItem(root->right->data.tree->left, SWFACTION_STARTDRAGMOVIE);
	  puts(", ");
	  listItem(root->left->data.tree->right, SWFACTION_STARTDRAGMOVIE);
	  puts(", ");
	  listItem(root->right->data.tree->right, SWFACTION_STARTDRAGMOVIE);
	}
	putchar(')');
	break;

      case SWFACTION_REMOVECLIP:
	puts("removeClip(");
	listItem(t->left, SWFACTION_REMOVECLIP);
	putchar(')');
	break;
      case SWFACTION_TRACE:
	puts("trace(");
	listItem(t->left, SWFACTION_TRACE);
	putchar(')');
	break;
      case SWFACTION_SETTARGETEXPRESSION:
	puts("setTarget(");
	listItem(t->left, SWFACTION_SETTARGETEXPRESSION);
	putchar(')');
	break;
      case SWFACTION_STOPDRAGMOVIE:
	puts("stopDrag()");
	break;
      case SWFACTION_NEXTFRAME:
	puts("nextFrame()");
	break;
      case SWFACTION_PREVFRAME:
	puts("prevFrame()");
	break;
      case SWFACTION_PLAY:
	puts("play()");
	break;
      case SWFACTION_STOP:
	puts("stop()");
	break;
      case SWFACTION_TOGGLEQUALITY:
	puts("toggleQuality()");
	break;
      case SWFACTION_STOPSOUNDS:
	puts("stopSounds()");
	break;

      case SWFACTION_GOTOFRAME:
	printf("gotoFrame(%i)", getInteger(t->left));
	break;

      case SWFACTION_GETURL:
      {
	printf("getURL(%s, %s)", getString(t->left), getString(t->right));
	break;
      }

      case SWFACTION_WAITFORFRAMEEXPRESSION:
	printf("Wait For Frame Expression, skip %i", getInteger(t->left));
	break;

      case SWFACTION_GETURL2:
      {
	int type = getInteger(t->left);

	puts("getURL2(");
	listItem(t->right->data.tree->left, SWFACTION_GETURL2);
	puts(", ");
	listItem(t->right->data.tree->right, SWFACTION_GETURL2);

	switch(type)
	{
	  case 0: putchar(')'); break;
	  case 1: puts(", GET)"); break;
	  case 2: puts(", POST)"); break;
	  default: printf(", type=%i (?))", type); break;
	}
	break;
      }

      case SWFACTION_CALLFRAME:
	puts("callFrame(");
	listItem(t->left, SWFACTION_CALLFRAME);
        putchar(')');
	break;

      case SWFACTION_GOTOEXPRESSION:
	puts("gotoFrame(");
	listItem(t->left, SWFACTION_GOTOEXPRESSION);
        putchar(')');

	if(getInteger(t->right) == 1)
	  puts(";\nplay()");
	break;

      case SWFACTION_SETTARGET:
	if(((char *)t->left)[0] == '\0')
	  puts("setTarget(this)");
	else
	  printf("setTarget('%s')", (char *)t->left);
	free(t->left);
	t->left=NULL;
	break;

      case SWFACTION_GOTOLABEL:
	printf("gotoFrame('%s')", getString(t->left));
	break;

	/* branches - shouldn't see these (but is good for debugging) */
      case SWFACTION_BRANCHIFTRUE:
	puts("if(");
	listItem(t->left, SWFACTION_BRANCHIFTRUE);
	printf(") branch %i", getInteger(t->right));
	break;

      case SWFACTION_BRANCHALWAYS:
	printf("branch %i", getInteger(t->right));
	break;

      case SWFACTION_WAITFORFRAME:
	printf("Wait for frame %i ", getInteger(t->left));
	printf(" else skip %i", getInteger(t->right));
	break;


    /* v5 ops */
      case SWFACTION_VAR:
	puts("var ");
	listItem(t->left, SWFACTION_VAR);
	break;

      case SWFACTION_VAREQUALS:
	puts("var ");
	listItem(t->left, SWFACTION_VAREQUALS);
	puts(" = ");
	listItem(t->right, SWFACTION_VAREQUALS);
	break;

      case SWFACTION_DELETE:
	puts("delete ");
	listItem(t->left, SWFACTION_DELETE);
	break;

      case SWFACTION_CALLFUNCTION:
      {
	int nargs, i;

	listItem(t->left, SWFACTION_CALLFUNCTION);
	putchar('(');

	t = t->right->data.tree;
	nargs = getInteger(t->left);

	for(i=0; i<nargs; ++i)
	{
	  t = t->right->data.tree;
	  if(i>0)
	    puts(", ");
	  listItem(t->left, SWFACTION_CALLFUNCTION);
	}

	putchar(')');

	break;
      }

      case SWFACTION_RETURN:
	puts("return ");
	listItem(t->left, SWFACTION_RETURN);
	break;

      case SWFACTION_NEWEQUAL:
      case SWFACTION_NEWADD:
      case SWFACTION_SHIFTLEFT:
      case SWFACTION_SHIFTRIGHT:
      case SWFACTION_SHIFTRIGHT2:
      case SWFACTION_BITWISEAND:
      case SWFACTION_BITWISEOR:
      case SWFACTION_BITWISEXOR:
      case SWFACTION_MODULO:
	listArithmetic(s, parent);
	break;

      case SWFACTION_NEW:
	puts("new ");
	listItem(t->right, SWFACTION_NEW);
	putchar('(');
	listItem(t->left, SWFACTION_NEW);
	putchar(')');
	break;

      case SWFACTION_TYPEOF:
	puts("typeof(");
	listItem(t->left, SWFACTION_TYPEOF);
	putchar(')');
	break;

      case SWFACTION_NEWLESSTHAN:
	listLessThan(s, NONEGATE);
	break;

      case SWFACTION_GETMEMBER:
	listItem(t->left, SWFACTION_GETMEMBER);
	if(t->right->type == 'i')
	{	putchar('[');
		listItem(t->right, SWFACTION_GETMEMBER);
		putchar(']');
	}
	else
	{	putchar('.');
		listItem(t->right, SWFACTION_GETMEMBER);
	}
	break;

      case SWFACTION_SETMEMBER:
	listItem(t->left, SWFACTION_SETMEMBER);
	putchar('.');
	listItem(t->right->data.tree->left, SWFACTION_SETMEMBER);
	puts(" = ");
	listItem(t->right->data.tree->right, SWFACTION_SETMEMBER);
	break;

      case SWFACTION_INCREMENT:
	listItem(t->left, SWFACTION_INCREMENT);
	puts("+1");
	break;

      case SWFACTION_DECREMENT:
	listItem(t->left, SWFACTION_DECREMENT);
	puts("-1");
	break;

      case SWFACTION_CALLMETHOD:
      {
	int nargs, i;

	listItem(t->right->data.tree->left, SWFACTION_CALLMETHOD);
	putchar('.');

	listItem(t->left, SWFACTION_CALLMETHOD);
	putchar('(');

	t = t->right->data.tree->right->data.tree;
	nargs = getInteger(t->left);

	for(i=0; i<nargs; ++i)
	{
	  t = t->right->data.tree;
	  if(i>0)
	    puts(", ");
	  listItem(t->left, SWFACTION_CALLMETHOD);
	}

	putchar(')');

	break;
      }

      /* dealt with on parse */
      case SWFACTION_DECLARENAMES:
	error("Shouldn't get declareNames in listItem!");
	break;

      case SWFACTION_WITH:
      {
	int n = getInteger(t->right->data.tree->left);
	Stack *statements = getStackArray(t->right->data.tree->right);

	puts("with(");
	if ( t->left->data.tree->left->type != 's' ) {
		printf("[bogus(%c)]", t->left->data.tree->left->type);
	} else {
		puts(t->left->data.tree->left->data.string);
	}
	puts(")\n{\n");
	decompileStatements(statements, n);
	puts("}");
	break;
      }

      case SWFACTION_DEFINEFUNCTION:
      {
	int n = getInteger(t->right->data.tree->left);
	Stack *statements = getStackArray(t->right->data.tree->right);

	int first = 1;
	Stack args = t->left->data.tree->right;

	puts("function ");
	puts(getString(t->left->data.tree->left));
	putchar('(');

	while(args != NULL)
	{
	  if(!first)
	    puts(", ");

	  first = 0;
	  puts(getString(args->data.tree->left));
	  args = args->data.tree->right;
	}

	puts(")\n{\n");
	decompileStatements(statements, n);
	puts("}");

	break;
      }

      case SWFACTION_ENUMERATE:
	puts("iterate over "); /* XXX */
	listItem(t->left, SWFACTION_ENUMERATE);
	break;

      case SWFACTION_SETREGISTER:
      {
	/* XXX - shouldn't ever see this.. */
	int n = getInteger(t->left);
	t->left=NULL;
	puts("(_tmp");
	putchar('0'+n);
	puts(" = ");
	listItem(t->right, SWFACTION_SETREGISTER);
	putchar(')');

	break;
      }

      case SWFACTION_DUP:
	listItem(t->left, SWFACTION_DUP);
	break;


      case SWFACTION_INITOBJECT:
      {
	int i, nEntries = getInteger(t->left);
	Stack *names = getStackArray(t->right->data.tree->left);
	Stack *values = getStackArray(t->right->data.tree->right);

	puts("{ ");

	for(i=0; i<nEntries; ++i)
	{
	  listItem(names[i], SWFACTION_INITOBJECT);
	  puts(" : ");
	  listItem(values[i], SWFACTION_INITOBJECT);

	  if(i < nEntries-1)
	    puts(", ");
	}

	puts(" }");

	break;
      }

      case SWFACTION_INITARRAY:
      {
	int i, nEntries = getInteger(t->left);
	Stack *values = getStackArray(t->right);

	puts("[ ");

	for(i=0; i<nEntries; ++i)
	{
	  listItem(values[i], SWFACTION_INITOBJECT);

	  if(i < nEntries-1)
	    puts(", ");
	}

	puts(" ]");

	break;
      }

      default:
	break;
      }
  }
}

static int isStatement(Stack s)
{
  Tree t;

  if(s->type != 't')
    return 0;

  t = s->data.tree;

  switch(t->action)
  {
    case SWFACTION_NEXTFRAME:
    case SWFACTION_PREVFRAME:
    case SWFACTION_PLAY:
    case SWFACTION_STOP:
    case SWFACTION_TOGGLEQUALITY:
    case SWFACTION_STOPSOUNDS:
    case SWFACTION_GOTOFRAME:
    case SWFACTION_GETURL:
    case SWFACTION_SETTARGET:
    case SWFACTION_GOTOLABEL:
    case SWFACTION_SETVARIABLE:
    case SWFACTION_SETTARGETEXPRESSION:
    case SWFACTION_SETPROPERTY:
    case SWFACTION_DUPLICATECLIP:
    case SWFACTION_REMOVECLIP:
    case SWFACTION_TRACE:
    case SWFACTION_STARTDRAGMOVIE:
    case SWFACTION_STOPDRAGMOVIE:
    case SWFACTION_WAITFORFRAMEEXPRESSION:
    case SWFACTION_BRANCHALWAYS:
    case SWFACTION_GETURL2:
    case SWFACTION_BRANCHIFTRUE:
    case SWFACTION_CALLFRAME:
    case SWFACTION_GOTOEXPRESSION:

      /*
    case SWFACTION_CALLFUNCTION:
    case SWFACTION_CALLMETHOD:
      */

    case SWFACTION_VAR:
    case SWFACTION_VAREQUALS:
    case SWFACTION_DELETE:
    case SWFACTION_POP:
    case SWFACTION_RETURN:
    case SWFACTION_SETMEMBER:
    case SWFACTION_DECLARENAMES:
    case SWFACTION_WITH:
    case SWFACTION_DEFINEFUNCTION:

    case SWFACTION_ENUMERATE:

      return 1;
    default:
      return 0;
  }
}

static Stack readStatement(FILE *f)
{
  Stack s;

  for(;;)
  {
    if(feof(f))
      return NULL;

    if((s = readActionRecord(f)) == NULL)
      return NULL;

    if((int)s == -1) /* declarenames */
      continue;

    if(stack == NULL && reg0 == NULL && isStatement(s))
      /* supposedly, we've got a complete statement. */
      return s;
    else
      push(s);
  }
}

static Stack negateExpression(Stack s)
{
  Tree t = s->data.tree;
  Stack ret;

  if(s->type != 't')
    return s;

  if(t->action == SWFACTION_LOGICALNOT)
  {
    ret = t->left;
    /* free(t); */
    return ret;
  }

  return newTree(s, SWFACTION_LOGICALNOT, NULL);
}

void showStack(Stack s)
{
  if( s == NULL )
	s = stack;

  while(s != NULL &&
	(s->type != 't' || s->data.tree->action != SWFACTION_DEFINEFUNCTION))
  {
    listItem(s, SWFACTION_END);
    putchar('\n');
    s = s->next;
  }
}

int readStatements(FILE *f, int length, Stack **slist)
{
  Stack s, start = stack, *statements;
  int i, n, off, end = fileOffset + length;
  int wasNull = 0;

  /* XXX - should use our own stack here? */

  while(fileOffset<end)
  {
    if(feof(f))
      break;

    off = fileOffset;

    if((s = readActionRecord(f)) != NULL)
    {
      push(s);

      if(s->offset == 0)
	s->offset = off;

      wasNull = 0;
    }
    else
      /* blah. */
      wasNull = 1;

    /*
    showStack(NULL);
    putchar('\n');
    */
  }

  s = stack;
  n = 0;

  /* count statements */
  while(s && s != start)
  {
    ++n;
    s = s->next;
  }

  /* load statements from stack into array */

  *slist = statements = (Stack *)malloc((n+1)*sizeof(Stack));

  for(i=n-1; i>=0; --i)
    statements[i] = pop();

  /* give branch something to target */
  s = newStack();
  statements[n] = s;

  s->offset = fileOffset;

  if(wasNull)
    --s->offset;

  return n;
}

void listStatements(Stack *statements, int n)
{
  int i;

  for(i=0; i<=n; ++i)
  {
    printf("%03i|%04i:  ", i, statements[i]->offset);
    listItem(statements[i], SWFACTION_END);
    putchar(';');
    putchar('\n');
  }
}

void decompileStatements(Stack *statements, int n)
{
  resolveOffsets(statements, n);
  untangleBranches(statements, 0, n, BRANCH_NONE, gIndent);

  putchar('\n');
}

#define STATEMENTS_INCREMENT 16
void decompile4Action(FILE *f, int length, int indent)
{
  Stack s, *statements = NULL;
  int /*i, j,*/ off, nStatements = 0;

  int end = fileOffset+length;

  /* pass 1: read all statements */
  while(fileOffset<end)
  {
    if(nStatements%STATEMENTS_INCREMENT == 0)
      statements = (Stack *)realloc(statements,
				    (nStatements+STATEMENTS_INCREMENT) * 
				    sizeof(Stack));

    off = fileOffset;
    s = readStatement(f);

    if(s!=NULL)
    {
      s->offset = off;
      statements[nStatements++] = s;
    }
    else
    {
      /* give branch something to target */
      s = newStack();
      statements[nStatements] = s;
      s->offset = off;

      if(fileOffset<end)
	error("unexpected End action!");
    }
  }

  /*
  for(i=0; i<nStatements; ++i)
  {
    printf("%03i|%04i:  ", i, statements[i]->offset);
    listItem(statements[i], SWFACTION_END);
    putchar(';');
    putchar('\n');
  }
  */

  resolveOffsets(statements, nStatements);
  untangleBranches(statements, 0, nStatements, BRANCH_NONE, 0);

  putchar('\n');

  if(stack != NULL)
  {
    printf("Decompiler error: stack not empty!\n");
    printf("Here's what's left over:\n\n");

    /* dump stack remains */
    while(stack)
    {
      listItem(stack, SWFACTION_END);
      putchar(';');
      putchar('\n');
      stack = stack->next;
    }

    destroyStack(stack);
    stack = NULL;
  }
}

char *
decompile5Action(FILE *f, int length, int indent)
{
  Stack *statements = NULL;
  int n;

  if( length == 0 )
	  return NULL;

  dcinit();
  gIndent = indent;

  n = readStatements(f, length, &statements);
  /* listStatements(statements, n); */
  decompileStatements(statements, n);

  if(stack != NULL)
  {
    printf("Decompiler error: stack not empty!\n");
    printf("Here's what's left over:\n\n");

    /* dump stack remains */
    while(stack)
    {
      listItem(stack, SWFACTION_END);
      putchar(';');
      putchar('\n');
      stack = stack->next;
    }

    destroyStack(stack);
    stack = NULL;

    assert(0);
  }
  free(statements);
  return dcgetstr();
}

static void resolveOffsets(Stack *statements, int nStatements)
{
  int i, j;

  /* first change branch byte offsets to statement offsets */
  for(i=0; i<nStatements; ++i)
  {
    Tree t = statements[i]->data.tree;

    if(t->action == SWFACTION_BRANCHIFTRUE ||
       t->action == SWFACTION_BRANCHALWAYS)
    {
      int offset = getInteger(t->right) + statements[i+1]->offset;

      if(getInteger(t->right) == 0)
      {
	/* offset == 0 means jump to the next instruction */
	t->right = newInteger(i+1);
	statements[i]->target = i+1;
	continue;
      }
      else if(getInteger(t->right) > 0)
      {
	for(j=i+2; j<nStatements; ++j)
	{
	  if(statements[j]->offset > offset)
	    error("went too far!");

	  if(statements[j]->offset == offset)
	    break;
	}

	if(j==nStatements+1)
	  error("couldn't find (forward) target offset!");
      }
      else
      {
	for(j=i; j>=0; --j)
	{
	  if(statements[j]->offset < offset)
	    error("went too far!");

	  if(statements[j]->offset == offset)
	    break;
	}

	if(j==-1)
	  error("couldn't find (backward) target offset!");
      }

      t->right = newInteger(j);
      statements[j]->target = i;
    }
  }
}

#define INDENT { int ii=indent; while(--ii>=0) { putchar(' '); putchar(' '); } }

static void untangleBranches(Stack *statements, int start, int stop,
			     Branchtype type, int indent)
{
  Stack s;
  Tree t;
  int i, offset, end, hasElse, wasIf = 0;

  for(i=start; i<stop; ++i)
  {
    s = statements[i];
    t = s->data.tree;

    if(s->target > i && t->action != SWFACTION_BRANCHIFTRUE)
    {
      /* it's a do loop */

      int target = s->target;

      if(s->target < start || s->target > stop)
	error("stmt %i: do target (%i) outside scope (%i,%i)!",
	      i, s->target, start, stop);

      putchar('\n');

      INDENT
	puts("do\n");
      INDENT
	puts("{\n");

      s->target = -1;

      untangleBranches(statements, i, target, BRANCH_DO, indent+1);

      INDENT
	puts("}\n");
      INDENT
	puts("while(");
        listItem(statements[target]->data.tree->left, SWFACTION_END);
        puts(");\n");

      wasIf = 1;

      i = target;
      continue;
    }

    if(t->action == SWFACTION_BRANCHALWAYS)
      error("stmt %i: unexpected unconditional branch!", i);

    if(t->action != SWFACTION_BRANCHIFTRUE)
    {
      /* it's just a statement. */

      if(wasIf)
	putchar('\n');

      INDENT
	listItem(s, SWFACTION_END);
        putchar(';');
        putchar('\n');

      wasIf = 0;

      continue;
    }

    /* it's a conditional branch */
    offset = getInteger(t->right);

    if(offset < start || offset > stop)
      error("stmt %i: branch target (%i) outside scope (%i,%i)!",
	    i, offset, start, stop);

    if(offset < i)
      error("stmt %i: Unexpected backwards branch!", i);

    if(type==BRANCH_WHILE || type==BRANCH_DO)
    {
      if(offset == stop)
      {
	INDENT
	  puts("break;\n");
	continue;
      }
      else if(offset == start)
      {
	INDENT
	  puts("continue;\n");
	continue; /*ha!*/
      }
    }

    if(statements[offset-1]->data.tree->action == SWFACTION_BRANCHALWAYS &&
       getInteger(statements[offset-1]->data.tree->right) == i)
    {
      /* it's a while loop */
      putchar('\n');

      INDENT
	puts("while(");
        listItem(negateExpression(t->left), SWFACTION_END);
        puts(")\n");

      if(i < offset-3)
      {
	INDENT
	  putchar('{');
	  putchar('\n');
      }

      untangleBranches(statements, i+1, offset-1, BRANCH_WHILE, indent+1);

      if(i < offset-3)
      {
	INDENT
	  putchar('}');
	  putchar('\n');
      }

      wasIf = 1;

      i = offset-1;
      continue;
    }

    /* it's just an if */

    if(i>start)
      putchar('\n');

    if(statements[offset-1]->data.tree->action == SWFACTION_BRANCHALWAYS)
    {
      /* got an else */
      hasElse = 1;
      end = getInteger(statements[offset-1]->data.tree->right);
    }
    else
    {
      hasElse = 0;
      end = offset;
    }

    if(end < start || end > stop)
      error("stmt %i: else target (%i) outside scope (%i,%i)!",
	    i, end, start, stop);

    if(end < i)
      error("stmt %i: Unexpected backwards branch!", i);

    INDENT
      puts("if(");

    /* XXX - would like to reverse else and if if expression is already
       negated.. */
    listItem(negateExpression(t->left), SWFACTION_END);
    putchar(')');
    putchar('\n');

    if(i < offset-(hasElse?3:2))
    {
      INDENT
	putchar('{');
        putchar('\n');
    }

    /* if hasElse, i+1 to offset-1 */
    untangleBranches(statements, i+1, offset-(hasElse?1:0), BRANCH_IF, indent+1);

    if(i < offset-(hasElse?3:2))
    {
      INDENT
	putchar('}');
        putchar('\n');
    }

    /* swallow up else-ifs */

    for(;;)
    {
      int newOff;

      if(!hasElse)
	break;

      if(statements[offset]->data.tree->action != SWFACTION_BRANCHIFTRUE)
	break;

      newOff = getInteger(statements[offset]->data.tree->right);

      /* make sure we're still in this if's else bit */

      if(statements[newOff-1]->data.tree->action != SWFACTION_BRANCHALWAYS ||
	 getInteger(statements[newOff-1]->data.tree->right) != end)
	break;

      i = offset;
      offset = newOff;

      s = statements[i];
      t = s->data.tree;

      INDENT
	puts("else if(");

      listItem(negateExpression(t->left), SWFACTION_END);
      putchar(')');
      putchar('\n');

      if(i < offset-(hasElse?3:2))
      {
	INDENT
	  putchar('{');
	  putchar('\n');
      }

      if(offset-(hasElse?1:0) < i+1)
      {
	/* gimpy code */
	INDENT
	  puts("  ;\n");
	break;
      }

      untangleBranches(statements, i+1, offset-(hasElse?1:0), BRANCH_IF, indent+1);

      if(i < offset-(hasElse?3:2))
      {
	INDENT
	  putchar('}');
	  putchar('\n');
      }
    }

    /* now take out ending else */

    if(hasElse)
    {
      INDENT
	puts("else\n");

      if(offset+2 < end)
      {
	INDENT
	  putchar('{');
          putchar('\n');
      }

      untangleBranches(statements, offset, end, BRANCH_ELSE, indent+1);

      if(offset+2 < end)
      {
	INDENT
	  putchar('}');
	  putchar('\n');
      }
    }

    wasIf = 1;

    i = end-1;
  }
}
