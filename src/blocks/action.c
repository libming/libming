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

#ifndef __C2MAN__
#include <stdlib.h>
#include <string.h>
#endif

#include "action.h"
#include "output.h"
#include "block.h"
#include "input.h"
#include "method.h"
#include "libming.h"
#include "character.h"
#include "movieclip.h"
#include "actioncompiler/compile.h"
#include "actiontypes.h"

typedef enum {
	INPUT_EMPTY,
	INPUT_FILE,
	INPUT_SCRIPT
} ActionInputType;

struct SWFAction_s
{
	struct SWFBlock_s block;
	ActionInputType inputType;
	SWFOutput out;
	union
	{
		FILE *file;
		char *script;
	} input;
	int debug;	
};

struct SWFInitAction_s
{
	struct SWFBlock_s block;
	int spriteId;
	SWFAction action; 
	SWFMovieClip clip;
};

static char *readActionFile(FILE *file)
{
	int len;
	char *script;
	SWFInput input = newSWFInput_file(file);
	len = SWFInput_length(input);
	script = (char *)malloc(len + 1);
	if(SWFInput_read(input, (unsigned char *)script, len) != len)
	{
		SWF_warn("readActionFile failed\n");
		free(script);
		return NULL;
	}
	destroySWFInput(input);	
	script[len] = '\0';
	return script;
}

void SWFOutput_writeAction(SWFOutput out, SWFAction action)
{
	int len;
	byte *data;

	if(action->out == NULL)
	{
		SWF_warn("SWFAction: compile action first\n");
		return;
	}	
	
	len = SWFOutput_getLength(action->out);
	data = SWFOutput_getBuffer(action->out);
	SWFOutput_writeBuffer(out, data, len);
} 

/*
 * Compiles the current script stored in this SWFAction instance.
 * returns 0 on success, -1 otherwise.
 * the length of the compiled bytecode is storen in the length pointer (if not NULL).
 */
int SWFAction_compile(SWFAction action, 
                      int swfVersion /* target SWF version */, 
                      int *length /* output length */)
{
	char *script = NULL;
	Buffer b;
	int parserError;

	if(action->out != NULL)
	{
		if(length != NULL)
			*length = SWFOutput_getLength(action->out);
		return 0;
	}

	switch(action->inputType)
	{
		case INPUT_SCRIPT:
			script = action->input.script;
			break;
		case INPUT_FILE:
			script = readActionFile(action->input.file);
			break;
		default: break;
	}

	if(script != NULL && swfVersion == 4)
        {
                swf4ParseInit(script, action->debug, swfVersion);
		parserError = swf4parse((void *)&b);
        }
        else if (script != NULL)
        {
                swf5ParseInit(script, action->debug, swfVersion);
		parserError = swf5parse((void *)&b);
        }
	else 
		parserError = 1;
	
	// if INPUT_FILE script was allocated by readActionFile()
	if(action->inputType == INPUT_FILE)
		free(script);
		
	action->out = newSWFOutput();

        if(!parserError)
        {
                SWFOutput_writeBuffer(action->out, b->buffer, bufferLength(b));
                destroyBuffer(b);
        }
	else
		SWF_warn("Parser error: writing empty block\n");

        SWFOutput_writeUInt8(action->out, SWFACTION_END);
	if(length != NULL)
		*length = SWFOutput_getLength(action->out);

	if(parserError)
		return -1;
		
	return 0;
}

/* 
 * Returns the compiled bytecode.
 * If not already compiled the script will compiled for SWF V7.
 *
 * Returns NULL in case of an error. Length pointer stores the length of 
 * the compiled bytecode.
 */
byte *SWFAction_getByteCode(SWFAction action, int *length)
{
	int ret = 0;
	if(action == NULL) 
		return NULL;

	if(action->out == NULL)
	{
		SWF_warn("SWFAction_getByteCode: please use SWFAction_compile first\n");
		SWF_warn("auto-compiling as SWF 7 code now...\n");
		ret = SWFAction_compile(action, 7, (int *)length);
	}

	if(ret < 0)
	{	
		*length = -1;
		return NULL;
	}
	return SWFOutput_getBuffer(action->out);
}

static int
completeSWFAction(SWFBlock block)
{
	int length;
	SWFAction action = (SWFAction)block;
	SWFAction_compile(action, block->swfVersion, &length);
	return length;
}

static int
completeSWFInitAction(SWFBlock block)
{
	SWFInitAction init = (SWFInitAction)block;
	int len;

	SWFAction_compile(init->action, block->swfVersion, &len);
	return len + 2;
}


static void
writeSWFActionToMethod(SWFBlock block, SWFByteOutputMethod method, void* data)
{
	SWFOutput out = ((SWFAction)block)->out;
        SWFOutput_writeToMethod(out, method, data);
}

static void
writeSWFInitActionToMethod(SWFBlock block, SWFByteOutputMethod method, void* data)
{
	SWFInitAction init = (SWFInitAction)block;
	methodWriteUInt16(init->spriteId, method, data);
	SWFOutput_writeToMethod(init->action->out, method, data);
}

/*
 * Destroys a SWFAction instance
 */
void destroySWFAction(SWFAction action)
{
	if(!action)
		return;

	switch(action->inputType)
	{
		case INPUT_FILE:
			fclose(action->input.file);
			break;
		case INPUT_SCRIPT:
			free(action->input.script);
			break;
		default:
			break;
	}
	
	if(action->out)
		destroySWFOutput(action->out);
	
	free(action);
}

void destroySWFInitAction(SWFInitAction init)
{
	if(!init)
		return;

	if(init->clip)
		destroySWFMovieClip(init->clip);
	destroySWFAction(init->action);
	free(init);
}


static SWFAction createEmptyAction()
{
	SWFAction action = (SWFAction)malloc(sizeof(struct SWFAction_s));

        SWFBlockInit(BLOCK(action));
        BLOCK(action)->type = SWF_DOACTION;
        BLOCK(action)->writeBlock = writeSWFActionToMethod;
        BLOCK(action)->complete = completeSWFAction;
        BLOCK(action)->dtor = (destroySWFBlockMethod) destroySWFAction;
	action->inputType = INPUT_EMPTY;
	action->out = NULL;
	action->debug = 0;
	return action;
}

/**
 * enable verbose compiler output 
 *
 * Set debug value to 1 get very! verbose compile messages.
 * @return old value
 */
int SWFAction_setDebug(SWFAction a, int debug /*debug switch*/)
{
	int oldval;
	if(!a)
		return -1;
	oldval = a->debug;
	a->debug = debug;
	return oldval;
}

/*
 * Creates a new SWFAction object.
 * Takes a String containing AS[2] source code.
 *
 * returns a SWFAction instance.
 */
SWFAction newSWFAction(const char *script)
{
	SWFAction action = createEmptyAction();
	action->inputType = INPUT_SCRIPT;
	action->input.script = strdup(script);

	return action;
}

/*
 * Creates a new SWFAction object.
 * Takes a filename pointing to a file containing AS[2] source code.
 *
 * return a SWFAction instance.
 */
SWFAction newSWFAction_fromFile(const char *filename)
{
	SWFAction action = createEmptyAction();
	action->inputType = INPUT_FILE;
	action->input.file = fopen(filename, "r");
	if(action->input.file == NULL)
	{
		destroySWFAction(action);
		return NULL;
	}
	return action;
}

SWFMovieClip SWFInitAction_getMovieClip(SWFInitAction action)
{
	return action->clip;
}

/*
 * create a InitAction block with a given sprite's character id
 *
 * This function creates a InitAction block with a given sprite's character id.
 * Use with care!
 */
SWFInitAction newSWFInitAction_withId(SWFAction action, int id /* mc character id */)
{
	SWFInitAction init = (SWFInitAction)malloc(sizeof(struct SWFInitAction_s));
	SWFBlockInit(BLOCK(init));
	BLOCK(init)->writeBlock = writeSWFInitActionToMethod;
	BLOCK(init)->complete = completeSWFInitAction;
	BLOCK(init)->dtor = (destroySWFBlockMethod) destroySWFInitAction;
	BLOCK(init)->type = SWF_INITACTION;
	init->clip = NULL;	// use external clip
	init->spriteId = id;	
	init->action = action;
	return init;
}

/*
 * create a InitAction block
 *
 * This function creates a InitAction block and defines an empty sprite/mc
 * which is not placed. This functions is usefull for defining classes.
 */
SWFInitAction newSWFInitAction(SWFAction action)
{
	SWFInitAction init = (SWFInitAction)malloc(sizeof(struct SWFInitAction_s));
	SWFBlockInit(BLOCK(init));
	BLOCK(init)->writeBlock = writeSWFInitActionToMethod;
	BLOCK(init)->complete = completeSWFInitAction;
	BLOCK(init)->dtor = (destroySWFBlockMethod) destroySWFInitAction;
	BLOCK(init)->type = SWF_INITACTION;
	init->clip = newSWFMovieClip();
	init->spriteId = CHARACTERID(init->clip);
	init->action = action;
	return init;
}

SWFInitAction newSWFInitAction_MovieClip(SWFMovieClip clip, SWFAction action)
{
	SWFInitAction init = (SWFInitAction)malloc(sizeof(struct SWFInitAction_s));
	SWFBlockInit(BLOCK(init));
	BLOCK(init)->writeBlock = writeSWFInitActionToMethod;
	BLOCK(init)->complete = completeSWFInitAction;
	BLOCK(init)->dtor = (destroySWFBlockMethod) destroySWFInitAction;
	BLOCK(init)->type = SWF_INITACTION;
	init->spriteId = CHARACTERID(clip);
	init->clip = NULL; // use external clip
	init->action = action;
	return init;
}

/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
