/* $Id$ */

#include "placeobject.h"
#include "method.h"

struct SWFPlaceObject2Block_s
{
  struct SWFBlock_s block;

  SWFOutput out;

  SWFCharacter character;
  SWFMatrix matrix;
  SWFCXform cXform;
  int ratio;
  int masklevel;
  char *name;
  int depth;
  int move;

  int nActions;
  int actionORFlags;
  SWFAction *actions;
  int *actionFlags;
};


void writeSWFPlaceObject2BlockToStream(SWFBlock block,
				       SWFByteOutputMethod method, void *data)
{
  int i;

  SWFPlaceObject2Block place = (SWFPlaceObject2Block)block;
  SWFOutput_writeToMethod(place->out, method, data);

  if(place->nActions > 0)
  {
    methodWriteUInt16(0, method, data); /* mystery number */
    methodWriteUInt16(place->actionORFlags, method, data);

    for(i=0; i<place->nActions; ++i)
    {
      SWFOutputBlock block = (SWFOutputBlock)place->actions[i];

      methodWriteUInt16(place->actionFlags[i], method, data);
      methodWriteUInt32(SWFOutputBlock_getLength(block), method, data);
      SWFOutput_writeToMethod(SWFOutputBlock_getOutput(block), method, data);
    }

    methodWriteUInt16(0, method, data); /* trailing 0 for end of actions */
  }
}


int completeSWFPlaceObject2Block(SWFBlock block)
{
  SWFPlaceObject2Block place = (SWFPlaceObject2Block)block;
  SWFOutput out = newSizedSWFOutput(42);

  int actionLen = 0;

  int flags =
    ((place->name != NULL)      ? SWF_PLACE_HAS_NAME : 0) |
    ((place->ratio != -1)       ? SWF_PLACE_HAS_RATIO : 0) |
    ((place->masklevel != -1)   ? SWF_PLACE_HAS_MASK : 0) |
    ((place->cXform != NULL)    ? SWF_PLACE_HAS_CXFORM : 0) |
    ((place->matrix != NULL)    ? SWF_PLACE_HAS_MATRIX : 0) |
    ((place->character != NULL) ? SWF_PLACE_HAS_CHARACTER : 0) |
    ((place->move != 0)         ? SWF_PLACE_MOVE : 0) |
    ((place->nActions != 0)     ? SWF_PLACE_HAS_ACTIONS : 0);

  SWFOutput_writeUInt8(out, flags);
  SWFOutput_writeUInt16(out, place->depth);

  if(place->character != NULL)
    SWFOutput_writeUInt16(out, CHARACTERID(place->character));

  if(place->matrix != NULL)
    SWFOutput_writeMatrix(out, place->matrix);

  if(place->cXform != NULL)
    SWFOutput_writeCXform(out, place->cXform, SWF_PLACEOBJECT2);

  if(place->ratio != -1)
    SWFOutput_writeUInt16(out, place->ratio);

  if(place->masklevel != -1)
    SWFOutput_writeUInt16(out, place->masklevel);

  if(place->name != NULL)
    SWFOutput_writeString(out, place->name);

  if(place->nActions != 0)
  {
    int i;
    actionLen += 4;

    for(i=0; i<place->nActions; ++i)
    {
      SWFOutputBlock block = (SWFOutputBlock)place->actions[i];
      actionLen += 6 + SWFOutputBlock_getLength(block);
    }

    actionLen += 2;
  }

  place->out = out;

  return SWFOutput_getLength(out) + actionLen;
}


void destroySWFPlaceObject2Block(SWFBlock block)
{
  SWFPlaceObject2Block place = (SWFPlaceObject2Block)block;

  if(place->actions)
    sec_free((void**)&place->actions);

  if(place->actionFlags)
    sec_free((void**)&place->actionFlags);

  if(place->name != NULL)
    sec_free((void**)&place->name);

  if(place->out)
    destroySWFOutput(place->out);

  if(place->matrix)
    destroySWFMatrix(place->matrix);

  if(place->cXform)
    destroySWFCXform(place->cXform);

  sec_free((void**)&place);
}


SWFPlaceObject2Block newSWFPlaceObject2Block(int depth)
{
  SWFPlaceObject2Block place = malloc(sizeof(struct SWFPlaceObject2Block_s));

  SWFBlockInit((SWFBlock)place);

  BLOCK(place)->type = SWF_PLACEOBJECT2;
  BLOCK(place)->writeBlock = writeSWFPlaceObject2BlockToStream;
  BLOCK(place)->complete = completeSWFPlaceObject2Block;
  BLOCK(place)->dtor = destroySWFPlaceObject2Block;

  place->out = NULL;
  place->name = NULL;

  place->move = 0;
  place->matrix = NULL;
  place->cXform = NULL;
  place->character = NULL;
  place->masklevel = -1;
  place->ratio = -1;
  place->depth = depth;

  place->nActions = 0;
  place->actionORFlags = 0;
  place->actionFlags = NULL;
  place->actions = NULL;

  return place;
}

void SWFPlaceObject2Block_setName(SWFPlaceObject2Block block, const char *name)
{
  sec_free((void**)&block->name);

  block->name = strdup(name);
}

void SWFPlaceObject2Block_setRatio(SWFPlaceObject2Block block, int ratio)
{
  block->ratio = ratio;
}

void SWFPlaceObject2Block_setDepth(SWFPlaceObject2Block block,
				       int depth)
{
  block->depth = depth;
}

void SWFPlaceObject2Block_setMaskLevel(SWFPlaceObject2Block block,
				       int masklevel)
{
  block->masklevel = masklevel;
}

void SWFPlaceObject2Block_setCXform(SWFPlaceObject2Block block,
				    SWFCXform cXform)
{
  block->cXform = cXform;
}

void SWFPlaceObject2Block_setColorAdd(SWFPlaceObject2Block block,
				      int r, int g, int b, int a)
{
  if(block->cXform == NULL)
    block->cXform = newSWFAddCXform(r, g, b, a);
  else
    SWFCXform_setColorAdd(block->cXform, r, g, b, a);
}

void SWFPlaceObject2Block_setColorMult(SWFPlaceObject2Block block,
				       float r, float g, float b, float a)
{
  if(block->cXform == NULL)
    block->cXform = newSWFMultCXform(r, g, b, a);
  else
    SWFCXform_setColorMult(block->cXform, r, g, b, a);
}

void SWFPlaceObject2Block_setMatrix(SWFPlaceObject2Block block,
				    SWFMatrix matrix)
{
  sec_free((void**)&block->matrix);

  block->matrix = SWFMatrix_dup(matrix);
}

void SWFPlaceObject2Block_setCharacter(SWFPlaceObject2Block block,
				       SWFCharacter character)
{
  block->character = character;
}

void SWFPlaceObject2Block_setMove(SWFPlaceObject2Block block)
{
  block->move = 1;
}

void SWFPlaceObject2Block_addAction(SWFPlaceObject2Block block,
				    SWFAction action, int flags)
{
  block->actions = realloc(block->actions,
			   (block->nActions+1) * sizeof(SWFAction));

  block->actionFlags = realloc(block->actionFlags,
			       (block->nActions+1) * sizeof(int));

  block->actions[block->nActions] = action;
  block->actionFlags[block->nActions] = flags;
  block->actionORFlags |= flags;
  ++block->nActions;
}
