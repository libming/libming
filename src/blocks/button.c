/*
    Ming, an SWF output library
    Copyright (C) 2001  Opaque Industries - http://www.opaque.net/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "button.h"

static SWFButtonRecord newSWFButtonRecord(byte flags, SWFCharacter character,
					  unsigned short layer,
					  SWFMatrix matrix)
{
  SWFButtonRecord record = calloc(1, BUTTONRECORD_SIZE);

  record->flags = flags;
  record->character = character;
  record->layer = layer;
  record->matrix = matrix;

  return record;
}

#define BUTTONRECORD_INCREMENT 8

static void SWFButton_addRecord(SWFButton button, SWFButtonRecord record)
{
  if(button->nRecords%BUTTONRECORD_INCREMENT == 0)
    button->records = realloc(button->records,
			      (button->nRecords + BUTTONRECORD_INCREMENT) *
			      sizeof(SWFButtonRecord));

  button->records[button->nRecords++] = record;

  SWFCharacter_addDependency((SWFCharacter)button, (SWFBlock)record->character);
}

/* note: replaces action, doesn't append.. */
void SWFButton_addAction(SWFButton button, SWFAction action, int flags)
{
  if(button->nActions%BUTTONRECORD_INCREMENT == 0)
    button->actions = realloc(button->actions,
			      (button->nActions + BUTTONRECORD_INCREMENT) *
			      sizeof(swfActionRecord));

  button->actions[button->nActions].action = action;
  button->actions[button->nActions].flags = flags;
  ++button->nActions;
}

/* XXX - temp hack */
void SWFButton_addShape(SWFButton button, SWFCharacter character, byte flags)
{
  SWFMatrix m = newSWFMatrix(0, 0, 0, 0, 0, 0);

  SWFButton_addRecord(button, newSWFButtonRecord(flags, character, 0, m));
}

void writeSWFButtonToMethod(SWFBlock block,
			    SWFByteOutputMethod method, void *data)
{
  int i;
  SWFButton button = (SWFButton)block;
  SWFOutput_writeToMethod(button->out, method, data);

  for(i=0; i<button->nActions; ++i)
  {
    if(i == button->nActions-1)
      methodWriteUInt16(0, method, data);
    else
      methodWriteUInt16(SWFOutput_length(button->actions[i].action->output)+4,
			method, data);

    methodWriteUInt16(button->actions[i].flags, method, data);
    SWFOutput_writeToMethod(button->actions[i].action->output, method, data);
  }
}
int completeSWFButton(SWFBlock block)
{
  SWFButton button = (SWFButton)block;
  SWFButtonRecord record;
  SWFOutput out = newSWFOutput();
  int i, length = 0;
  char *offset;

  SWFOutput_writeUInt16(out, CHARACTERID(button));
  SWFOutput_writeUInt8(out, 0); /* XXX - track as menu item. ??? */

  /* fill in offset later */
  offset = out->pos;
  SWFOutput_writeUInt16(out, 0);

  for(i=0; i<button->nRecords; ++i)
  {
    record = button->records[i];
    SWFOutput_writeUInt8(out, record->flags);
    SWFOutput_writeUInt16(out, CHARACTERID(record->character));
    SWFOutput_writeUInt16(out, record->layer);
    SWFOutput_writeMatrix(out, record->matrix);
    SWFOutput_writeUInt8(out, 0); /* blank CXForm */
  }

  SWFOutput_writeUInt8(out, 0); /* end buttons */

  length = SWFOutput_length(out) - 3;

  if(button->nActions > 0)
  {
    *offset = length&0xff;
    *(offset+1) = (length>>8)&0xff;
  }

  button->out = out;

  length = 0;

  for(i=0; i<button->nActions; ++i)
    length += SWFOutput_length(button->actions[i].action->output) + 4;

  return SWFOutput_length(out) + length;
}
void destroySWFButton(SWFBlock block)
{
  SWFButton button = (SWFButton)block;
  int i;

  SWFCharacter_clearDependencies(CHARACTER(block));

  for(i=0; i<button->nRecords; ++i)
  {
    if(button->records[i]->matrix != NULL)
      destroySWFMatrix(button->records[i]->matrix);

    free(button->records[i]);
  }

  free(button->records);

  for(i=0; i<button->nActions; ++i)
    destroySWFAction(button->actions[i].action);

  free(button->actions);

  destroySWFOutput(button->out);

  free(button);
}

SWFButton newSWFButton()
{
  SWFButton button = calloc(1, SWFBUTTON_SIZE);

  CHARACTERID(button) = ++SWF_gNumCharacters;
  BLOCK(button)->type = SWF_DEFINEBUTTON2;
  BLOCK(button)->writeBlock = writeSWFButtonToMethod;
  BLOCK(button)->complete = completeSWFButton;
  BLOCK(button)->dtor = destroySWFButton;

  button->records = NULL;
  button->actions = NULL;
  button->out = NULL;

  return button;
}

SWFBlock newDefineSWFButtonCXform(SWFButton button, SWFCXform *cXforms)
{
  return newEmptySWFBlock(SWF_DEFINEBUTTONCXFORM);
  /* 		  &writeSWFShapeBlockToStream, &completeSWFShapeBlock); */
}
SWFBlock newDefineSWFButtonSWFSound(SWFButton button, SWFSound sound)
{
  return newEmptySWFBlock(SWF_DEFINEBUTTONSOUND);
  /* 		  &writeSWFShapeBlockToStream, &completeSWFShapeBlock); */
}

