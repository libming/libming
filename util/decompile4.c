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

#include <assert.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "decompile.h"
#include "read.h"
#include "action.h"

typedef enum
{
  BRANCH_NONE = 0,
  BRANCH_WHILE,
  BRANCH_DO,
  BRANCH_IF,
  BRANCH_ELSE
} Branchtype;

static void listItem(Stack s, Action parent);
static void resolveOffsets(Stack *statements, int nStatements);
static void untangleBranches(Stack *statements, int start, int stop,
			     Branchtype type, int indent);

static int isNum(char *s)
{
  float f = atof(s);

  if(f != 0 || s[0] == '0')
    return true;
  else
    return false;
}

static Stack stack = NULL;

static Stack newStack()
{
  Stack s = (Stack)malloc(sizeof(struct _stack));
  memset(s, 0, sizeof(struct _stack));
  s->target = -1;
  return s;
}

static void destroy(Stack s)
{
  if(!s)
    return;

  if(s->type == 't')
  {
    destroy(s->data.tree->left);
    destroy(s->data.tree->right);
    free(s->data.tree);
  }
  //else if(s->type == 's') free(s->data.string);

  free(s);
}

static Stack newTree(Stack left, Action action, Stack right)
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

static Stack newString(char *string)
{
  Stack s = newStack();

  s->type = 's';
  s->data.string = string;

  return s;
}

static Stack newProperty(Property prop)
{
  Stack s = newStack();

  s->type = 'p';
  s->data.prop = prop;

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

static char *negateString(char *s)
{
  int i, l = strlen(s)+1;
  char *New = realloc(s, l+1);

  for(i=l; i>0; --i)
    New[i] = New[i-1];

  New[0] = '-';

  return New;
}

static Property getSetProperty(int prop)
{
  switch(prop)
  {
    case SWF_SETPROPERTY_X:		  return PROPERTY_X;
    case SWF_SETPROPERTY_Y:		  return PROPERTY_Y;
    case SWF_SETPROPERTY_XSCALE:	  return PROPERTY_XSCALE;
    case SWF_SETPROPERTY_YSCALE:	  return PROPERTY_YSCALE;
    case SWF_SETPROPERTY_ALPHA:		  return PROPERTY_ALPHA;
    case SWF_SETPROPERTY_VISIBILITY:	  return PROPERTY_VISIBLE;
    case SWF_SETPROPERTY_ROTATION:	  return PROPERTY_ROTATION;
    case SWF_SETPROPERTY_NAME:		  return PROPERTY_NAME;
    case SWF_SETPROPERTY_HIGHQUALITY:	  return PROPERTY_HIGHQUALITY;
    case SWF_SETPROPERTY_SHOWFOCUSRECT:	  return PROPERTY_FOCUSRECT;
    case SWF_SETPROPERTY_SOUNDBUFFERTIME: return PROPERTY_SOUNDBUFTIME;
    case SWF_SETPROPERTY_WTHIT:		  return PROPERTY_WTHIT;
    default:
      error("unknown property: 0x%04x!", prop);
      return SWF_SETPROPERTY_WTHIT;
  }
}

static Stack readActionRecord(FILE *f)
{
  int length = 0, type = readUInt8(f);

  if((type&0x80) == 0x80)
    length = readUInt16(f);

  switch(type)
  {
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

    case SWFACTION_POP:
	  /* pop(); */
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
    case SWFACTION_SETVARIABLE:
    case SWFACTION_STRINGCONCAT:
    case SWFACTION_STRINGCOMPARE:
	{
      Stack right = pop();
      Stack left = pop();
      return newTree(left, type, right);
	}

    case SWFACTION_GETPROPERTY:
    {
      Stack right = pop();
      Stack left = pop();

      if(right->type == 's')
      {
	Stack New = newProperty(atoi(right->data.string));
	destroy(right);
	right = New;
      }
      return newTree(left, type, right);
    }

    case SWFACTION_SUBTRACT:
    {
      Stack right = pop();
      Stack left = pop();

      if(left->type == 's' &&
	 strcmp(left->data.string, "0") == 0)
      {
	destroy(left);
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
	destroy(property);
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

      if(level->type != 't' ||
	 level->data.tree->action != SWFACTION_ADD)
	error("WTHIT property not found in duplicateClip target level!");

      if(level->data.tree->left->type == 'p' &&
	 level->data.tree->left->data.prop == PROPERTY_WTHIT)
      {
        arg = level->data.tree->right;
	level->data.tree->right = NULL;
      }
      else if(level->data.tree->right->type == 'p' &&
	      level->data.tree->right->data.prop == PROPERTY_WTHIT)
      {
        arg = level->data.tree->left;
        level->data.tree->left = NULL;
      }
      else
	error("WTHIT property not found in duplicateClip target level!");

      destroy(level);

      return newTree(source, type, newTree(target, type, arg));
    }


    /* weird ops */
    case SWFACTION_STARTDRAGMOVIE:
    {
      Stack target = pop();
      Stack lockmouse = pop();
      Stack constraint = pop();

      if(constraint->type != 's')
	error("Sorry, decompiler can't deal with conditional constraint!");

      if(strcmp(constraint->data.string, "0") == 0)
	return newTree(constraint, type, newTree(lockmouse, type, target));
      else
	  {
		Stack s4 = pop();
		Stack s3 = pop();
		Stack s2 = pop();
		Stack s1 = pop();

	return newTree(newTree(newTree(s1, type, s2), type,
			       newTree(s3, type, s4)), type,
		       newTree(lockmouse, type, target));
	  }
    }

    case SWFACTION_PUSHDATA:
    {
      int local_type = readUInt8(f);
      if(local_type==0)
	return newString(readString(f));
      else
      {
	readUInt16(f); /* 0x0000 */
        return newProperty(getSetProperty(readUInt16(f)));
      }
    }

    case SWFACTION_GOTOFRAME:
      return newTree((Stack)readUInt16(f), type, NULL);

    case SWFACTION_GETURL:
    {
      char *url = readString(f);
      char *target = readString(f);
      return newTree((Stack)url, type, (Stack)target);
    }

    case SWFACTION_GETURL2:
	{
	  Stack target = pop();
	  Stack url = pop();
      return newTree((Stack)readUInt8(f), type, newTree(url, type, target));
	}

    case SWFACTION_WAITFORFRAMEEXPRESSION:
      return newTree((Stack)readUInt8(f), type, NULL);

    case SWFACTION_GOTOEXPRESSION:
      return newTree(pop(), type, (Stack)readUInt8(f));

    case SWFACTION_SETTARGET:
    case SWFACTION_GOTOLABEL:
      return newTree((Stack)readString(f), type, NULL);


    /* branches */
    case SWFACTION_BRANCHIFTRUE:
      return newTree(pop(), type, (Stack)readSInt16(f));

    case SWFACTION_BRANCHALWAYS:
      return newTree(NULL, type, (Stack)readSInt16(f));

    case SWFACTION_WAITFORFRAME:
	{
	  Stack left = (Stack)readUInt16(f);
	  Stack right = (Stack)readUInt8(f);
      return newTree(left, type, right);
	}

    case SWFACTION_END:
      return NULL;

    default:
      printf("Unknown Action: %02X\n", type);
      dumpBytes(f, length);
      return NULL;
  }
}

static void listProperty(Property prop)
{
  switch(prop)
  {
    case PROPERTY_X:		   printf("x"); break;
    case PROPERTY_Y:		   printf("y"); break;
    case PROPERTY_XSCALE:	   printf("xScale"); break;
    case PROPERTY_YSCALE:	   printf("yScale"); break;
    case PROPERTY_CURRENTFRAME:	   printf("currentFrame"); break;
    case PROPERTY_TOTALFRAMES:	   printf("totalFrames"); break;
    case PROPERTY_ALPHA:	   printf("alpha"); break;
    case PROPERTY_VISIBLE:	   printf("visible"); break;
    case PROPERTY_WIDTH:	   printf("width"); break;
    case PROPERTY_HEIGHT:	   printf("height"); break;
    case PROPERTY_ROTATION:	   printf("rotation"); break;
    case PROPERTY_TARGET:	   printf("target"); break;
    case PROPERTY_FRAMESLOADED:	   printf("framesLoaded"); break;
    case PROPERTY_NAME:		   printf("name"); break;
    case PROPERTY_DROPTARGET:	   printf("dropTarget"); break;
    case PROPERTY_URL:  	   printf("url"); break;
    case PROPERTY_HIGHQUALITY:	   printf("quality"); break;
    case PROPERTY_FOCUSRECT:       printf("focusRect"); break;
    case PROPERTY_SOUNDBUFTIME:    printf("soundBufTime"); break;
    case PROPERTY_WTHIT:	   printf("WTHIT!?"); break;
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

    case SWFACTION_ADD:            return 4;
    case SWFACTION_SUBTRACT:       return 4;

    case SWFACTION_MULTIPLY:       return 5;
    case SWFACTION_DIVIDE:         return 5;

    case SWFACTION_GETVARIABLE:    return 6;
    case SWFACTION_GETPROPERTY:    return 6;

    case SWFACTION_STRINGEQ:       return 7;
    case SWFACTION_STRINGCONCAT:   return 7;
    case SWFACTION_PUSHDATA:       return 7;

    case SWFACTION_SETPROPERTY:    return 8;

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
      listItem(s->data.tree->left, parent);
      return;
    }
    else if(t->action == SWFACTION_EQUAL)
    {
      listItem(s->data.tree->left, SWFACTION_EQUAL);
      printf(" != ");
      listItem(s->data.tree->right, SWFACTION_EQUAL);
      return;
    }
  }

  printf("!");
  listItem(s, SWFACTION_LOGICALNOT);
}

static void listAssign(Stack s)
{
  /* check for ++a, a+=b */

  Stack left = s->data.tree->left;
  Stack right = s->data.tree->right;

  if(right->type == 't' &&
     right->data.tree->action >= SWFACTION_ADD &&
     right->data.tree->action <= SWFACTION_DIVIDE)
  {
    Stack rleft = right->data.tree->left;
    Stack rright = right->data.tree->right;
    const char *op;

    if(right->data.tree->action == SWFACTION_ADD)
      op = " += ";
    else if(right->data.tree->action == SWFACTION_SUBTRACT)
      op = " -= ";
    else if(right->data.tree->action == SWFACTION_MULTIPLY)
      op = " -= ";
    else if(right->data.tree->action == SWFACTION_DIVIDE)
      op = " /= ";
    else
      error("Unexpected operation in listAssign!");

    if(rleft->type == 't' &&
       rleft->data.tree->action == SWFACTION_GETVARIABLE &&
       rleft->data.tree->left->type == 's' &&
       strcmp(rleft->data.tree->left->data.string, left->data.string) == 0)
    {
      if(rright->type == 's' &&
	 strcmp(rright->data.string, "1") == 0)
      {
	if(right->data.tree->action == SWFACTION_ADD)
	{
	  printf("++");
	  listItem(left, SWFACTION_SETVARIABLE);
	  return;
	}
	else if(right->data.tree->action == SWFACTION_SUBTRACT)
	{
	  printf("--");
	  listItem(left, SWFACTION_SETVARIABLE);
	  return;
	}
      }

      listItem(left, SWFACTION_SETVARIABLE);
      printf(op);
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
	if(right->data.tree->action == SWFACTION_ADD)
	{
	  printf("++");
	  listItem(left, SWFACTION_SETVARIABLE);
	  return;
	}
	else if(right->data.tree->action == SWFACTION_SUBTRACT)
	{
	  printf("--");
	  listItem(left, SWFACTION_SETVARIABLE);
	  return;
	}
      }

      listItem(left, SWFACTION_SETVARIABLE);
      printf(op);
      listItem(rleft, right->data.tree->action);
      return;
    }
  }

  listItem(left, SWFACTION_SETVARIABLE);
  printf(" = ");
  listItem(right, 0);
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
	      right->type == 't' &&
	      right->data.tree->action == SWFACTION_GETVARIABLE &&
	      right->data.tree->left->data.string[0] == '/') &&
            (left->type == 's' ||
	     (left->type == 't' &&
	      left->data.tree->action == SWFACTION_GETVARIABLE) ||
	     right->type == 's' ||
	     (right->type == 't' &&
	      right->data.tree->action == SWFACTION_GETVARIABLE));

  switch(t->action)
  {
    case SWFACTION_ADD:            op = (isShort?"+":" + "); break;
    case SWFACTION_SUBTRACT:       op = (isShort?"-":" - "); break;
    case SWFACTION_MULTIPLY:       op = (isShort?"*":" * "); break;
    case SWFACTION_DIVIDE:         op = (isShort?"/":" / "); break;

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

  /* parentesization rule */
  parens = (precedence(parent) > precedence(t->action) ||
	    (t->action == SWFACTION_MULTIPLY && parent == SWFACTION_DIVIDE) ||
	    (t->action == SWFACTION_ADD && parent == SWFACTION_SUBTRACT));

  if(parens)
    putchar('(');

  listItem(left, t->action);
  printf(op);
  listItem(right, t->action);

  if(parens)
    putchar(')');
}

static void listItem(Stack s, Action parent)
{
  Tree t;

  if(s->type == 's')
  {
    if(parent == SWFACTION_GETVARIABLE ||
       parent == SWFACTION_SETVARIABLE ||
       parent == SWFACTION_GETPROPERTY ||
       parent == SWFACTION_SETPROPERTY ||
       parent == SWFACTION_DUPLICATECLIP ||
       isNum(s->data.string))
      printf(s->data.string);
    else
    {
      putchar('\'');
      printf(s->data.string);
      putchar('\'');
    }
  }
  else if(s->type == 'p')
  {
    listProperty(s->data.prop);
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
	  printf("valueOf(");
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
	  printf("this");
	else
	  listItem(t->left, SWFACTION_GETPROPERTY);

	putchar('.');
	listItem(t->right, SWFACTION_GETPROPERTY);
	break;

      case SWFACTION_SETPROPERTY:
	if(t->left->type == 's' &&
	   t->left->data.string[0] == '\0')
	  printf("this");
	else
	  listItem(t->left, SWFACTION_SETPROPERTY);

	putchar('.');
	listItem(t->right, SWFACTION_SETPROPERTY);
	break;

      case SWFACTION_MBSUBSTRING:
      case SWFACTION_SUBSTRING:
	printf("substr(");
	listItem(t->left, t->action);
	printf(", ");
	listItem(t->right->data.tree->left, t->action);
	printf(", ");
	listItem(t->right->data.tree->right, t->action);
	putchar(')');
	break;
      
	/* one-arg */
      case SWFACTION_LOGICALNOT:
	listNot(t->left, SWFACTION_LOGICALNOT);
	break;

      case SWFACTION_STRINGLENGTH:
	printf("strlen(");
	listItem(t->left, SWFACTION_STRINGLENGTH);
	putchar(')');
	break;

      case SWFACTION_INT:
	printf("int(");
	listItem(t->left, SWFACTION_INT);
	putchar(')');
	break;

      case SWFACTION_RANDOM:
	printf("random(");
	listItem(t->left, SWFACTION_RANDOM);
	putchar(')');
	break;

      case SWFACTION_MBLENGTH:
	printf("mbstrlen(");
	listItem(t->left, SWFACTION_MBLENGTH);
	putchar(')');
	break;

      case SWFACTION_ORD:
	printf("ord(");
	listItem(t->left, SWFACTION_ORD);
	putchar(')');
	break;

      case SWFACTION_CHR:
	printf("chr(");
	listItem(t->left, SWFACTION_CHR);
	putchar(')');
	break;

      case SWFACTION_MBORD:
	printf("mbord(");
	listItem(t->left, SWFACTION_MBORD);
	putchar(')');
	break;

      case SWFACTION_MBCHR:
	printf("mbchr(");
	listItem(t->left, SWFACTION_MBCHR);
	putchar(')');
	break;

      case SWFACTION_GETVARIABLE:
	listItem(t->left, SWFACTION_GETVARIABLE);
	break;

      case SWFACTION_GETTIMER:
	printf("getTimer()");
	break;

	/* statements */
      case SWFACTION_DUPLICATECLIP:
	printf("duplicateClip(");
	listItem(t->left, SWFACTION_DUPLICATECLIP);
	printf(", ");
	listItem(t->right->data.tree->left, SWFACTION_DUPLICATECLIP);
	printf(", ");
	listItem(t->right->data.tree->right, SWFACTION_DUPLICATECLIP);
	putchar(')');
	break;

      case SWFACTION_STARTDRAGMOVIE:
	printf("startDrag(");
	listItem(t->right->data.tree->right, SWFACTION_STARTDRAGMOVIE);
	printf(", ");
	listItem(t->right->data.tree->left, SWFACTION_STARTDRAGMOVIE);

	if(t->left->type == 't')
	{
	  Tree root = t->left->data.tree;
	  printf(", ");
	  listItem(root->left->data.tree->left, SWFACTION_STARTDRAGMOVIE);
	  printf(", ");
	  listItem(root->right->data.tree->left, SWFACTION_STARTDRAGMOVIE);
	  printf(", ");
	  listItem(root->left->data.tree->right, SWFACTION_STARTDRAGMOVIE);
	  printf(", ");
	  listItem(root->right->data.tree->right, SWFACTION_STARTDRAGMOVIE);
	}
	putchar(')');
	break;

      case SWFACTION_REMOVECLIP:
	printf("removeClip(");
	listItem(t->left, SWFACTION_REMOVECLIP);
	putchar(')');
	break;
      case SWFACTION_TRACE:
	printf("trace(");
	listItem(t->left, SWFACTION_TRACE);
	putchar(')');
	break;
      case SWFACTION_SETTARGETEXPRESSION:
	printf("setTarget(");
	listItem(t->left, SWFACTION_SETTARGETEXPRESSION);
	putchar(')');
	break;
      case SWFACTION_STOPDRAGMOVIE:
	printf("stopDrag()");
	break;
      case SWFACTION_NEXTFRAME:
	printf("nextFrame()");
	break;
      case SWFACTION_PREVFRAME:
	printf("prevFrame()");
	break;
      case SWFACTION_PLAY:
	printf("play()");
	break;
      case SWFACTION_STOP:
	printf("stop()");
	break;
      case SWFACTION_TOGGLEQUALITY:
	printf("toggleQuality()");
	break;
      case SWFACTION_STOPSOUNDS:
	printf("stopSounds()");
	break;

      case SWFACTION_GOTOFRAME:
	printf("gotoFrame(%i)", (int)t->left);
	break;

      case SWFACTION_GETURL:
      {
	printf("getURL('%s', '%s')", (char *)t->left, (char *)t->right);
	break;
      }

      case SWFACTION_WAITFORFRAMEEXPRESSION:
	printf("Wait For Frame Expression, skip %i", (int)t->left);
	break;

      case SWFACTION_GETURL2:
	printf("getURL(");
	listItem(t->right->data.tree->left, SWFACTION_GETURL2);
	printf(", ");
	listItem(t->right->data.tree->right, SWFACTION_GETURL2);

	switch((int)t->left)
	{
	  case 0: printf(")"); break;
	  case 1: printf(", GET)"); break;
	  case 2: printf(", POST)"); break;
	  default: printf(", 0x%x /* ??? */)", (int)t->left);
	}
	break;

      case SWFACTION_CALLFRAME:
	printf("callFrame(");
	listItem(t->left, SWFACTION_CALLFRAME);
        putchar(')');
	break;

      case SWFACTION_GOTOEXPRESSION:
	printf("gotoFrame(");
	listItem(t->left, SWFACTION_GOTOEXPRESSION);
        putchar(')');

	if((int)t->right == 1)
	  printf(";\nplay()");
	break;

      case SWFACTION_SETTARGET:
	if(((char *)t->left)[0] == '\0')
	  printf("setTarget(this)");
	else
	  printf("setTarget('%s')", (char *)t->left);
	break;

      case SWFACTION_GOTOLABEL:
	printf("gotoFrame('%s')", (char *)t->left);
	break;

	/* branches - shouldn't see these */
      case SWFACTION_BRANCHIFTRUE:
	printf("if(");
	listItem(t->left, SWFACTION_BRANCHIFTRUE);
	printf(") branch %i", (int)t->right);
	break;

      case SWFACTION_BRANCHALWAYS:
	printf("branch %i", (int)t->right);
	break;

      case SWFACTION_WAITFORFRAME:
	printf("Wait for frame %i ", (int)t->left);
	printf(" else skip %i", (int)t->right);
	break;

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
	case SWFACTION_POP:
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

    if(stack == NULL && isStatement(s))
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

    destroy(stack);
    stack = NULL;
  }
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
      int offset = (int)t->right + statements[i+1]->offset;

      if((int)t->right > 0)
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

      t->right = (Stack)j;
      statements[j]->target = i;
    }
  }
}

#define INDENT { int ii=indent; while(ii-->0) { putchar(' '); putchar(' '); } }

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
        printf("do\n");
      INDENT
	printf("{\n");

      s->target = -1;

      untangleBranches(statements, i, target, BRANCH_DO, indent+1);

      INDENT
	printf("}\n");
      INDENT
	printf("while(");
        listItem(statements[target]->data.tree->left, SWFACTION_END);
        printf(");\n");

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

    offset = (int)t->right;

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
	  printf("break;\n");
	continue;
      }
      else if(offset == start)
      {
	INDENT
	  printf("continue;\n");
	continue; /*ha!*/
      }
    }

    if(statements[offset-1]->data.tree->action == SWFACTION_BRANCHALWAYS &&
       (int)statements[offset-1]->data.tree->right == i)
    {
      /* it's a while loop */
      putchar('\n');

      INDENT
	printf("while(");
        listItem(negateExpression(t->left), SWFACTION_END);
        printf(")\n");

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

    /* it's an if */

    if(i>start)
      putchar('\n');

    if(statements[offset-1]->data.tree->action == SWFACTION_BRANCHALWAYS)
    {
      /* got an else */
      hasElse = 1;
      end = (int)statements[offset-1]->data.tree->right;
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
      printf("if(");

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

    if(hasElse)
    {
      INDENT
	printf("else\n");

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
