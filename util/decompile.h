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

  when all that's left on the stack is a tree, you've got a statement.

  read everything into a list of statements, noting offsets and branches.
  step through list, resolving branches into if/else/while/do:

  if branch backwards, it's a do (or while(true))

  if forwards, check instruction before branch target:
	 if a branch, this is either:
           branch backwards- while 
           branch forwards- if/else

  inside loop, continue jumps to beginning, break jumps to end.
*/

#include "action.h"
#include "swftypes.h"

struct _stack;

struct _tree
{
  struct _stack *left;
  struct _stack *right;
  Action action;
};
typedef struct _tree *Tree;

struct _stack
{
  struct _stack *next;
  char type; /* (s)tring, (t)ree, (p)roperty, (b)ranch, (i)nt, (d)ouble, (r)register */
  char seenit;
  union
  {
    const char *string;
    int inum;
    double dnum;
    int reg;
    Tree tree;
    Property prop;
    int *iptr;
    struct _stack **sptr;
  } data;

  int offset;
  int target; /* statement number which branches to this statement */
};
typedef struct _stack *Stack;

char *decompile5Action(int n, SWF_ACTION *actions,int indent);

void setNewLineString(const char* ch);
