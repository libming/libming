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


#ifndef _METADATA_H
#define _METADATA_H

#include "ming.h"
#include "block.h"
#include "output.h"

struct SWFMetadata_s
{
	struct SWFBlock_s block;
	SWFOutput out;
};

typedef struct SWFMetadata_s *SWFMetadata;

SWFMetadata newSWFMetadata(const char *xml);
void destroySWFMetadata(SWFMetadata data);
#endif
