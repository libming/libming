/*
    Ming, an SWF output library
    Copyright (C) 2002  Opaque Industries - http://www.opaque.net/

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

/* $Id$ */

#include <stdlib.h>
#include "button.h"
#include "character.h"
#include "error.h"
#include "matrix.h"
#include "outputblock.h"
#include "method.h"
#include "soundinstance.h"
#include "browserfont.h"
#include "action.h"
#include "libming.h"

struct buttonRecord
{
	byte flags;
	unsigned short layer;
	SWFCharacter character;
	SWFMatrix matrix;
	SWFCXform cxform;
};
typedef struct buttonRecord* ButtonRecord;

struct actionRecord
{
	int flags;
	SWFAction action;
};
/* typedef struct actionRecord* ActionRecord; */

struct SWFButton_s
{
	struct SWFCharacter_s character;

	int menuflag;

	int nRecords;
	ButtonRecord *records;

	int nActions;
	struct actionRecord *actions;

	SWFButtonSound sounds;
	SWFScalingGrid grid;
	SWFOutput out;
};

struct SWFButtonSound_s
{
	struct SWFBlock_s character;

	SWFButton button;
	SWFSoundInstance sounds[4];
};


static ButtonRecord newSWFButtonRecord(byte flags, SWFCharacter character,
									 unsigned short layer, SWFMatrix matrix)
{
	ButtonRecord record = (ButtonRecord) malloc(sizeof(struct buttonRecord));

	record->flags = flags;
	record->character = character;
	record->layer = layer;
	record->matrix = matrix;

	return record;
}


#define BUTTONRECORD_INCREMENT 8

static void SWFButton_addRecord(SWFButton button, ButtonRecord record)
{
	if ( button->nRecords % BUTTONRECORD_INCREMENT == 0 )
	{
		button->records = (ButtonRecord*) realloc(button->records,
						(button->nRecords + BUTTONRECORD_INCREMENT) *
						sizeof(ButtonRecord));
	}

	button->records[button->nRecords++] = record;

	SWFCharacter_addDependency((SWFCharacter)button,
														 (SWFCharacter)record->character);
}


/* note: replaces action, doesn't append.. */
void SWFButton_addAction(SWFButton button, SWFAction action, int flags)
{
	if ( SWFCharacter_isFinished((SWFCharacter)button) )
		SWF_error("Can't alter a button after it's been added to another character");

	if ( button->nActions % BUTTONRECORD_INCREMENT == 0 )
	{
		button->actions = (struct actionRecord*) realloc(button->actions,
						(button->nActions + BUTTONRECORD_INCREMENT) *
						sizeof(struct actionRecord));
	}

	button->actions[button->nActions].action = action;
	button->actions[button->nActions].flags = flags;
	++button->nActions;
}


/* XXX - temp hack */
void SWFButton_addShape(SWFButton button, SWFCharacter character, byte flags)
{
	SWFMatrix m;

	if ( SWFCharacter_isFinished((SWFCharacter)button) )
		SWF_error("Can't alter a button after it's been added to another character");

	m = newSWFMatrix(0, 0, 0, 0, 0, 0);

	SWFCharacter_getDependencies((SWFCharacter)character,
									 &CHARACTER(button)->dependencies,
									 &CHARACTER(button)->nDependencies);

	SWFCharacter_addDependency((SWFCharacter)button, (SWFCharacter)character);

	SWFCharacter_setFinished(character);
	SWFButton_addRecord(button, newSWFButtonRecord(flags, character, 0, m));
}

void SWFButton_setMenu(SWFButton button, int flag)
{
	button->menuflag = flag;
}

void writeSWFButtonToMethod(SWFBlock block, 
					SWFByteOutputMethod method, void *data)
{
	SWFButton button = (SWFButton)block;
	SWFOutput_writeToMethod(button->out, method, data);
}


int completeSWFButton(SWFBlock block)
{
	SWFButton button = (SWFButton)block;
	ButtonRecord record;
	SWFOutput out = newSWFOutput();
	int i, length = 0, layer;
	byte *offset;

	SWFOutput_writeUInt16(out, CHARACTERID(button));
	SWFOutput_writeUInt8(out, button->menuflag);

	/* fill in offset later */
	offset = SWFOutput_getCurPos(out);
	SWFOutput_writeUInt16(out, 0);

	for(i=0; i<button->nRecords; ++i)
	{
		record = button->records[i];
		SWFOutput_writeUInt8(out, record->flags);
		SWFOutput_writeUInt16(out, CHARACTERID(record->character));
		layer = record->layer;
		if(layer == 0 && block->swfVersion >= 5) layer = i+1;
		SWFOutput_writeUInt16(out, layer);
		SWFOutput_writeMatrix(out, record->matrix);
		SWFOutput_writeUInt8(out, 0); /* blank CXForm */
	}

	SWFOutput_writeUInt8(out, 0); /* end buttons */

	length = SWFOutput_getLength(out) - 3;

	if(button->nActions > 0)
	{
		*offset = length&0xff;
		*(offset+1) = (length>>8)&0xff;
	}

	for(i=0; i<button->nActions; ++i)
	{
		length = SWFAction_compile(button->actions[i].action, block->swfVersion);

		if(i == button->nActions-1)
			SWFOutput_writeUInt16(out, 0);
		else
			SWFOutput_writeUInt16(out, length + 4);

		SWFOutput_writeUInt16(out, button->actions[i].flags);
		SWFOutput_writeAction(out, button->actions[i].action);
	}
	
	button->out = out;
	return SWFOutput_getLength(out);
}


void destroySWFButton(SWFButton button)
{
	int i;

	for ( i=0; i<button->nRecords; ++i )
	{
		if ( button->records[i]->matrix != NULL )
			destroySWFMatrix(button->records[i]->matrix);

		free(button->records[i]);
	}

	if ( button->records != NULL )
		free(button->records);

	for ( i=0; i<button->nActions; ++i )
		destroySWFAction(button->actions[i].action);
	
	if ( button->actions != NULL )
		free(button->actions);

	destroySWFOutput(button->out);

	destroySWFCharacter((SWFCharacter) button);
}

static int onSWFButtonPlace(SWFDisplayItem item, SWFBlockList list)
{
	SWFButton button = (SWFButton)SWFDisplayItem_getCharacter(item);
	if(!button->grid)
		return 0;

	SWFBlockList_addBlock(list, (SWFBlock)button->grid);
	return 1;
}

static int onSWFButtonFrame(SWFDisplayItem item, SWFBlockList list)
{
	SWFButton button = (SWFButton)SWFDisplayItem_getCharacter(item);
	if(!button->sounds)
		return 0;
	SWFBlockList_addBlock(list, (SWFBlock)button->sounds);
	return 1;
}


SWFButton
newSWFButton()
{
	SWFButton button = (SWFButton) malloc(sizeof(struct SWFButton_s));

	SWFCharacterInit((SWFCharacter)button);

	CHARACTERID(button) = ++SWF_gNumCharacters;
	BLOCK(button)->type = SWF_DEFINEBUTTON2;
	BLOCK(button)->writeBlock = writeSWFButtonToMethod;
	BLOCK(button)->complete = completeSWFButton;
	BLOCK(button)->dtor = (destroySWFBlockMethod) destroySWFButton;
	
	((SWFCharacter)button)->onPlace = onSWFButtonPlace;
	((SWFCharacter)button)->onFrame = onSWFButtonFrame;

	button->menuflag = 0;
	button->nRecords = 0;
	button->records = NULL;
	button->nActions = 0;
	button->actions = NULL;
	button->out = NULL;
	button->sounds = NULL;
	button->grid = NULL;
	return button;
}

SWFSoundInstance
SWFButton_addSound(SWFButton button, SWFSound sound, byte flag)
{
	if ( SWFCharacter_isFinished((SWFCharacter)button) )
		SWF_error("Can't alter a button after it's been added to another character");

	if ( button->sounds == NULL )
	{
		button->sounds = newSWFButtonSound(button);

//		SWFCharacter_addDependency((SWFCharacter)button->sounds,
//															 (SWFCharacter)button);
	}

	SWFCharacter_addDependency((SWFCharacter)button, (SWFCharacter)sound);
	return SWFButtonSound_setSound(button->sounds, sound, flag);
}


void
destroySWFButtonSound(SWFButtonSound buttonSound)
{
	free(buttonSound);
}


/* NOTE: at least F5 and F6 do not write an extra 0 style byte if there is
   no sound for a particular transition
   the docs say otherwise 
 */
void
writeSWFButtonSoundToMethod(SWFBlock block,
														SWFByteOutputMethod method, void *data)
{
	SWFButtonSound buttonSound = (SWFButtonSound)block;
	int i;

	methodWriteUInt16(CHARACTERID(buttonSound->button), method, data);

	for ( i=0; i<4; ++i )
	{
		SWFSoundInstance sound = buttonSound->sounds[i];
		if(sound)
			writeSWFSoundInstanceToMethod((SWFBlock)sound, method, data);
		else
			methodWriteUInt16(0, method, data);
	}
}


int
completeSWFButtonSound(SWFBlock block)
{
	int size = 2;	 /* button ID */
	int i;
	SWFButtonSound buttonSound = (SWFButtonSound)block;

	for ( i=0; i<4; ++i )
	{
		SWFSoundInstance sound = buttonSound->sounds[i];
		if(sound)
			size += completeSWFSoundInstance((SWFBlock)sound);
		else
			size += 2;
	}

	return size;
}


SWFButtonSound
newSWFButtonSound(SWFButton button)
{
	SWFButtonSound buttonSound = (SWFButtonSound) malloc(sizeof(struct SWFButtonSound_s));
	SWFBlock block = (SWFBlock)buttonSound;

	SWFBlockInit(block);

	block->type = SWF_DEFINEBUTTONSOUND;

	block->writeBlock = writeSWFButtonSoundToMethod;
	block->complete = completeSWFButtonSound;
	block->dtor = (destroySWFBlockMethod) destroySWFButtonSound;

	buttonSound->button = button;

	buttonSound->sounds[0] = NULL;
	buttonSound->sounds[1] = NULL;
	buttonSound->sounds[2] = NULL;
	buttonSound->sounds[3] = NULL;

	return buttonSound;
}


SWFSoundInstance
SWFButtonSound_setSound(SWFButtonSound sounds, SWFSound sound, byte flags)
{
	if ( flags == SWFBUTTON_OVERUPTOIDLE )
		return (sounds->sounds[0] = newSWFSoundInstance(sound));

	else if ( flags == SWFBUTTON_IDLETOOVERUP )
		return (sounds->sounds[1] = newSWFSoundInstance(sound));

	else if ( flags == SWFBUTTON_OVERUPTOOVERDOWN )
		return (sounds->sounds[2] = newSWFSoundInstance(sound));

	else if ( flags == SWFBUTTON_OVERDOWNTOOVERUP )
		return (sounds->sounds[3] = newSWFSoundInstance(sound));

	else
		SWF_error("SWFButtonSound_setSound: Invalid flags argument");

	return NULL;
}

/*
 * set a 9 slice scaling grid to this sprite / movieclip / button
 * This function sets a 9 slice scaling grid to buttons / sprites / movieclips
 *
 *      1 2 3
 *      4 5 6 
 *      7 8 9
 *
 * x, y, w and h define a rectangle, which is the dimension of the center slice (5)
 * all other slices are determined out of the characters bounds and the defined rect.
 * While slice 5 is scalde vertical and horizontal, slice 2 and 8 are only scale horizontal.
 * slice 4 and 6 only vertical. the 4 corner slices are not scaled (1, 3, 7, 9).  
 */
void 
SWFButton_setScalingGrid(SWFButton b, int x, int y, int w, int h)
{
	if(b->grid)
		destroySWFScalingGrid(b->grid);

	b->grid = newSWFScalingGrid((SWFCharacter)b, x, y, w, h);
}

/*
 * removes the scaling grid
 */
void 
SWFButton_removeScalingGrid(SWFButton b)
{
	if(b->grid)
	{
		destroySWFScalingGrid(b->grid);
		b->grid = NULL;
	}
}

/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
