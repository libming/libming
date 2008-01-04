/*
    Ming, an SWF output library
    Copyright (C) 2001  Opaque Industries - http://www.opaque.net/
	
    3.3.2007 Klaus Rechert

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
#include "block.h"
#include "output.h"
#include "metadata.h"

static void
writeSWFMetadataToMethod(SWFBlock block, SWFByteOutputMethod method, void* data)
{
        SWFOutput out = ((SWFMetadata)block)->out;
        SWFOutput_writeToMethod(out, method, data);
}

static int
completeSWFMetadata(SWFBlock block)
{
	SWFMetadata metadata = (SWFMetadata)block;
	return SWFOutput_getLength(metadata->out);
}

void
destroySWFMetadata(SWFMetadata metadata)
{
	destroySWFOutput(metadata->out);
	free(metadata);
}

SWFMetadata
newSWFMetadata(const char *data)
{
	SWFMetadata metadata = (SWFMetadata)malloc(sizeof(struct SWFMetadata_s));

	SWFBlockInit(BLOCK(metadata));
	BLOCK(metadata)->type = SWF_METADATA;
	BLOCK(metadata)->writeBlock = writeSWFMetadataToMethod;
	BLOCK(metadata)->complete = completeSWFMetadata;
	BLOCK(metadata)->dtor = (destroySWFBlockMethod) destroySWFMetadata;

	metadata->out = newSWFOutput();
	SWFOutput_writeString(metadata->out, (byte*)data);
	return metadata;
}

