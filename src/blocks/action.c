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

#include "stdlib.h"
#include "string.h"

#include "stdlib.h"
#include "string.h"

#include "action.h"
#include "output.h"
#include "block.h"
#include "input.h"
#include "libming.h"
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
};

static char *readActionFile(FILE *file)
{
	int len;
	char *script;
	SWFInput input = newSWFInput_file(file);
	len = SWFInput_length(input);
	script = (char *)malloc(len + 1);
	printf("len %i\n", len);
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


int SWFAction_compile(SWFAction action, int swfVersion)
{
	char *script = NULL;
	Buffer b;
	int parserError;

	if(action->out != NULL)
		return SWFOutput_getLength(action->out);

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
                swf4ParseInit(script, 0, swfVersion);
		parserError = swf4parse((void *)&b);
        }
        else if (script != NULL)
        {
                swf5ParseInit(script, 0, swfVersion);
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
	return SWFOutput_getLength(action->out);
}

static int
completeSWFAction(SWFBlock block)
{
	SWFAction action = (SWFAction)block;
	return SWFAction_compile(action, block->swfVersion);
}

static void
writeSWFActionToMethod(SWFBlock block, SWFByteOutputMethod method, void* data)
{
	SWFOutput out = ((SWFAction)block)->out;
        SWFOutput_writeToMethod(out, method, data);
}

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
	return action;
}

SWFAction newSWFAction(const char *script)
{
	SWFAction action = createEmptyAction();
	action->inputType = INPUT_SCRIPT;
	action->input.script = strdup(script);

	return action;
}


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



/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
