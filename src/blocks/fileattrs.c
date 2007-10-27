/*
    Ming, an SWF output library
    Copyright (C) 2001  Opaque Industries - http://www.opaque.net/
	
    27.2.2007 Klaus Rechert

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
#include "fileattrs.h"

#define SWFFILEATTRIBUTES_HASMETADATA 0x10
#define SWFFILEATTRIBUTES_HASABC      0x08
#define SWFFILEATTRIBUTES_USENETWORK  0x01


static void
writeSWFFileAttributesToMethod(SWFBlock block, SWFByteOutputMethod method, void* data)
{

	int flags = 0;
	SWFFileAttributes fattrs = (SWFFileAttributes)block;
	
	if(fattrs->hasMetadata)
		flags |= SWFFILEATTRIBUTES_HASMETADATA;
	if(fattrs->useNetwork)
		flags |= SWFFILEATTRIBUTES_USENETWORK;
	if(fattrs->hasABC)
		flags |= SWFFILEATTRIBUTES_HASABC; 
	
	method(flags, data);
	method(0, data);
	method(0, data);
	method(0, data);

}


static int
completeSWFFileAttributes(SWFBlock block)
{
	return 4;
}


void
destroySWFFileAttributes(SWFFileAttributes fattrs)
{
	free(fattrs);	
}

void 
SWFFileAttributes_hasMetadata(SWFFileAttributes fattrs, int flag)
{
	fattrs->hasMetadata = flag;
}

void 
SWFFileAttributes_useNetwork(SWFFileAttributes fattrs, int flag)
{
	fattrs->useNetwork = flag;
}

SWFFileAttributes
newSWFFileAttributes()
{
        SWFFileAttributes fattrs= (SWFFileAttributes)malloc(sizeof(struct SWFFileAttributes_s));

        SWFBlockInit(BLOCK(fattrs));
        BLOCK(fattrs)->type = SWF_FILEATTRIBUTES;
        BLOCK(fattrs)->writeBlock = writeSWFFileAttributesToMethod;
        BLOCK(fattrs)->complete = completeSWFFileAttributes;
        BLOCK(fattrs)->dtor = (destroySWFBlockMethod) destroySWFFileAttributes;
	
	fattrs->hasMetadata = 0;
	fattrs->useNetwork = 0;
	fattrs->hasABC = 0;
        return fattrs;
}

