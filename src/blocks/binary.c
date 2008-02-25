/*
    Ming, an SWF output library
    Copyright (C) 2007 Klaus Rechert

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
#include <stdlib.h>

#include "ming.h"
#include "character.h"
#include "output.h"

struct SWFBinaryData_s
{
	struct SWFCharacter_s character;	
	SWFOutput out;
};

static void
writeSWFBinaryDataToMethod(SWFBlock block, 
                            SWFByteOutputMethod method, void* data)
{
	SWFOutput out = ((SWFBinaryData)block)->out;
        SWFOutput_writeToMethod(out, method, data);
}

static int
completeSWFBinaryData(SWFBlock block)
{
	SWFBinaryData data = (SWFBinaryData)block;
	return SWFOutput_getLength(data->out);
}

/*
 * destroys a SWFBinaryData instance
 */
void
destroySWFBinaryData(SWFBinaryData data)
{
	destroySWFOutput(data->out);
	free(data);
}

/* 
 * creates an SWFBlock with arbitrary binary data
 * This SWFBlock allows to embed arbitrary binary data to a SWF file.
 * With SWFMovie_assignSymbol() this data can be used as a ByteArray in 
 * AS3 classes. 
 * returns a SWFBinaryData instance
 * see also SWFMovie_assignSymbol, destroySWFBinaryData
 */
SWFBinaryData newSWFBinaryData(unsigned char *blob, int length)
{
        SWFBinaryData data = (SWFBinaryData)malloc(sizeof(struct SWFBinaryData_s));

      	SWFCharacterInit((SWFCharacter)data);
        BLOCK(data)->type = SWF_DEFINEBINARYDATA;
        BLOCK(data)->writeBlock = writeSWFBinaryDataToMethod;
        BLOCK(data)->complete = completeSWFBinaryData;
        BLOCK(data)->dtor = (destroySWFBlockMethod) destroySWFBinaryData;

	data->out = newSizedSWFOutput(length);
	SWFOutput_writeBuffer(data->out, blob, length);
        return data;
}

