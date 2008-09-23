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
#include "filter.h"
#include "../position.h"
#include "libming.h"

#define RECORD_HASFILTER	(1<<4)
#define RECORD_HASBLEND		(1<<5)

struct SWFButtonRecord_s
{
	byte flags;
	unsigned short layer;
	SWFCharacter character;
	SWFPosition position;
	SWFCXform cxform;
	int blendMode;
	SWFFilterList filterList;
};

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
	SWFButtonRecord *records;

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


static SWFButtonRecord newSWFButtonRecord(byte flags, SWFCharacter character,
									 unsigned short layer, SWFMatrix matrix)
{
	SWFButtonRecord record = (SWFButtonRecord) malloc(sizeof(struct SWFButtonRecord_s));

	record->flags = flags;
	record->character = character;
	record->layer = layer;
	record->position = newSWFPosition(matrix);
	record->filterList = NULL;
	record->blendMode = 0;
	return record;
}


#define BUTTONRECORD_INCREMENT 8

static void SWFButton_addRecord(SWFButton button, SWFButtonRecord record)
{
	if ( button->nRecords % BUTTONRECORD_INCREMENT == 0 )
	{
		button->records = (SWFButtonRecord*) realloc(button->records,
						(button->nRecords + BUTTONRECORD_INCREMENT) *
						sizeof(SWFButtonRecord));
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

/* adds a shape character 
 * Add a shape character to a button for given states
 * possible states:
 * SWFBUTTON_HIT  
 * SWFBUTTON_DOWN  
 * SWFBUTTON_OVER
 * SWFBUTTON_UP
 * states can be combined using the binary or operator    
 * deprecated! use SWFButton_addCharacter instead
 */
void SWFButton_addShape(SWFButton button, SWFCharacter character, byte flags)
{
	SWFMatrix m;
	SWF_warnOnce("SWFButton_addShape is deprecated\nUse SWFButton_addCharacter instead\n");
	if ( SWFCharacter_isFinished((SWFCharacter)button) )
		SWF_error("Can't alter a button after it's been added to another character");

	m = newSWFMatrix(1.0, 0, 0, 1.0, 0, 0);

	SWFCharacter_getDependencies((SWFCharacter)character,
									 &CHARACTER(button)->dependencies,
									 &CHARACTER(button)->nDependencies);

	SWFCharacter_addDependency((SWFCharacter)button, (SWFCharacter)character);

	SWFCharacter_setFinished(character);
	SWFButton_addRecord(button, newSWFButtonRecord(flags, character, 0, m));
}

/* adds a character 
 * Add a character to a button for given states
 * possible states:
 * SWFBUTTON_HIT
 * SWFBUTTON_DOWN
 * SWFBUTTON_OVER
 * SWFBUTTON_UP
 * states can be combined using the binary or operator
 * returns a SWFButtonRecord object which can be further modified.
 */
SWFButtonRecord
SWFButton_addCharacter(SWFButton button /* button object */, 
                       SWFCharacter character /* character to be added */, 
                       byte state /* state description */)
{
	SWFMatrix m;
	SWFButtonRecord record;
	SWFCharacter **depsPtr = &CHARACTER(button)->dependencies;
	int *depCount = &CHARACTER(button)->nDependencies;
	
	if ( SWFCharacter_isFinished((SWFCharacter)button) )
	{
		SWF_warn("Can't alter a button after it's been added to another character");
		return NULL;
	}

	SWFCharacter_getDependencies(character, depsPtr, depCount);
	SWFCharacter_addDependency((SWFCharacter)button, character);
	SWFCharacter_setFinished(character);
	
	m = newSWFMatrix(1.0, 0, 0, 1.0, 0, 0);
	record = newSWFButtonRecord(state, character, 0, m);
	SWFButton_addRecord(button, record);
	return record;
}

void
SWFButtonRecord_setDepth(SWFButtonRecord b, int depth)
{
	b->layer = depth;
}

/*
 * sets blend mode
 * Sets an alternative blend mode instead of default alpha blend. 
 * possible modes are: 
 * SWFBLEND_MODE_NORMAL 
 * SWFBLEND_MODE_LAYER 
 * SWFBLEND_MODE_MULT 
 * SWFBLEND_MODE_SCREEN 
 * SWFBLEND_MODE_DARKEN 
 * SWFBLEND_MODE_ADD 
 * SWFBLEND_MODE_SUB 
 * SWFBLEND_MODE_DIFF 
 * SWFBLEND_MODE_DIFF 
 * SWFBLEND_MODE_INV 
 * SWFBLDEN_MODE_ALPHA 
 * SWFBLEND_MDOE_ERASE 
 * SWFBLEND_MDOE_OVERLAY 
 * SWFBLEND_MODE_HARDLIGHT 
 */
void
SWFButtonRecord_setBlendMode(SWFButtonRecord record, int mode)
{
	record->blendMode = mode;
	record->flags |= RECORD_HASBLEND;
}

/* 
 * adds a surface filter
 * Adds a bitmap filter to the display object.  
 * See SWFFilter for possible filter objects. 
 */
void 
SWFButtonRecord_addFilter(SWFButtonRecord record, SWFFilter filter)
{
	if(record->filterList == NULL)
        {
                record->filterList = newSWFFilterList();
                record->flags |= RECORD_HASFILTER;
        }
        SWFFilterList_add(record->filterList, filter);
}

void 
SWFButtonRecord_move(SWFButtonRecord record, double x, double y)
{
	SWFPosition_move(record->position, x, y);
}

void 
SWFButtonRecord_moveTo(SWFButtonRecord record, double x, double y)
{
	SWFPosition_moveTo(record->position, x, y);
}

void 
SWFButtonRecord_rotate(SWFButtonRecord record, double deg)
{
	SWFPosition_rotate(record->position, deg);
}

void 
SWFButtonRecord_rotateTo(SWFButtonRecord record, double deg)
{
	SWFPosition_rotateTo(record->position, deg);
}

void 
SWFButtonRecord_scale(SWFButtonRecord record, double scaleX, double scaleY)
{
	SWFPosition_scaleXY(record->position, scaleX, scaleY);
}

void 
SWFButtonRecord_scaleTo(SWFButtonRecord record, double scaleX, double scaleY)
{
	SWFPosition_scaleXYTo(record->position, scaleX, scaleY);
}

void 
SWFButtonRecord_skewX(SWFButtonRecord record, double skewX)
{
	SWFPosition_skewX(record->position, skewX);
}

void 
SWFButtonRecord_skewXTo(SWFButtonRecord record, double skewX)
{
	SWFPosition_skewXTo(record->position, skewX);
}

void 
SWFButtonRecord_skewY(SWFButtonRecord record, double skewY)
{
	SWFPosition_skewY(record->position, skewY);
}

void 
SWFButtonRecord_skewYTo(SWFButtonRecord record, double skewY)
{
	SWFPosition_skewYTo(record->position, skewY);
}

void
SWFButtonRecord_setMatrix(SWFButtonRecord record,
                          double a, double b, double c, double d, double x, double y)
{
	SWFPosition_setMatrix(record->position, a, b, c, d, x, y);
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
	SWFButtonRecord record;
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
		SWFOutput_writeMatrix(out, SWFPosition_getMatrix(record->position));
		SWFOutput_writeUInt8(out, 0); /* blank CXForm */
		if(record->flags & RECORD_HASFILTER)
			SWFOutput_writeFilterList(out, record->filterList);
		if(record->flags & RECORD_HASBLEND)
			SWFOutput_writeUInt8(out, record->blendMode);
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
		SWFAction_compile(button->actions[i].action, block->swfVersion, &length);

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
		if ( button->records[i]->position != NULL )
			destroySWFPosition(button->records[i]->position);

		if( button->records[i]->filterList != NULL)
			destroySWFFilterList(button->records[i]->filterList);

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
