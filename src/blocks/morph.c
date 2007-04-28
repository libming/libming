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

#include "morph.h"
#include "character.h"
#include "shape.h"
#include "fillstyle.h"
#include "linestyle.h"

#include "libming.h"

struct SWFMorph_s
{
	struct SWFCharacter_s character;

	SWFOutput out;
	SWFShape shape1;
	SWFShape shape2;
	int flags;
	int useVersion;
};


/* morph stuff */
static int
completeSWFMorphBlock(SWFBlock block)
{
	SWFMorph morph = (SWFMorph)block;

	SWFOutput out = morph->out;
	SWFShape shape1 = morph->shape1;
	SWFShape shape2 = morph->shape2;

	int nFills1, nFills2, nLines1, nLines2, offset, shape_version;
	SWFFillStyle *fills1, *fills2;
	SWFLineStyle *lines1, *lines2;
	byte *patch;

	SWFShape_end(shape1);
	SWFShape_end(shape2);
	
	/* determine shape version
 	 * if SWF_SHAPE4 -> write SWF_DEFINEMORPHSHAPE2
 	 */
	shape_version = SWFShape_getVersion(shape1);
	if(shape_version != SWFShape_getVersion(shape2))
		SWF_error("SWFMorph: shape versions must match\n");

	if(shape_version == SWF_SHAPE4)
		BLOCK(morph)->type = SWF_DEFINEMORPHSHAPE2;

	/* build header */
	SWFOutput_writeUInt16(out, CHARACTERID(morph));
	SWFOutput_writeRect(out, CHARACTER(shape1)->bounds);
	SWFOutput_byteAlign(out);
	SWFOutput_writeRect(out, CHARACTER(shape2)->bounds);
	SWFOutput_byteAlign(out);

	// edge bounds
	if(shape_version == SWF_SHAPE4)
	{
		int flags;

		SWFOutput_writeRect(out, SWFShape_getEdgeBounds(shape1));
		SWFOutput_byteAlign(out);
		SWFOutput_writeRect(out, SWFShape_getEdgeBounds(shape2));
		SWFOutput_byteAlign(out);
		
		flags = SWFShape_getFlags(shape1);
		if(flags != SWFShape_getFlags(shape2))
			SWF_warn("SWFMorph: shape flags do not match. Using flags of first shape\n");

		SWFOutput_writeUInt8(out, flags);
	}

	patch = SWFOutput_getCurPos(out);
	SWFOutput_writeUInt32(out, 0); /* shape2 offset, patched in below.. */

	SWFShape_getFills(shape1, &fills1, &nFills1);
	SWFShape_getFills(shape2, &fills2, &nFills2);

	SWFOutput_writeMorphFillStyles(out, 
		fills1, nFills1, SWFShape_getEdgeBounds(shape1), 
		fills2, nFills2, SWFShape_getEdgeBounds(shape2));
	SWFOutput_byteAlign(out);
	SWFShape_getLines(shape1, &lines1, &nLines1);
	SWFShape_getLines(shape2, &lines2, &nLines2);

	if(shape_version == SWF_SHAPE4)
		SWFOutput_writeMorphLineStyles2(out, lines1, nLines1, lines2, nLines2);
	else 
		SWFOutput_writeMorphLineStyles(out, lines1, nLines1, lines2, nLines2);

	SWFOutput_byteAlign(out);

	offset =
		SWFOutput_getCurPos(out) - patch +
		SWFOutput_getLength(SWFShape_getOutput(shape1)) - 4;
	
	*(patch++) = offset & 0xff;
	offset >>= 8;
	*(patch++) = offset & 0xff;
	offset >>= 8;
	*(patch++) = offset & 0xff;
	offset >>= 8;
	*patch = offset & 0xff;

	return
		SWFOutput_getLength(out) +
		SWFOutput_getLength(SWFShape_getOutput(shape1)) +
		SWFOutput_getLength(SWFShape_getOutput(shape2));
}


void
writeSWFMorphBlockToStream(SWFBlock block,
													 SWFByteOutputMethod method, void *data)
{
	SWFMorph morph = (SWFMorph)block;

	SWFOutput_writeToMethod(morph->out, method, data);
	SWFOutput_writeToMethod(SWFShape_getOutput(morph->shape1), method, data);
	SWFOutput_writeToMethod(SWFShape_getOutput(morph->shape2), method, data);
}


void
destroySWFMorph(SWFMorph morph)
{
	destroySWFOutput(morph->out);

	/* these must be destroyed by hand, since script wrappers can't
		 tell the difference between these and newSWFShape()'s..

	destroySWFShape(morph->shape1);
	destroySWFShape(morph->shape2);
	*/

	free(morph);
}


SWFShape
SWFMorph_getShape1(SWFMorph morph)
{
	return morph->shape1;
}


SWFShape
SWFMorph_getShape2(SWFMorph morph)
{
	return morph->shape2;
}


SWFMorph
newSWFMorphShape()
{
	SWFMorph morph = (SWFMorph) malloc(sizeof(struct SWFMorph_s));

	SWFCharacterInit((SWFCharacter)morph);

	BLOCK(morph)->type = SWF_DEFINEMORPHSHAPE;
	BLOCK(morph)->writeBlock = writeSWFMorphBlockToStream;
	BLOCK(morph)->complete = completeSWFMorphBlock;
	BLOCK(morph)->dtor = (destroySWFBlockMethod) destroySWFMorph;

	CHARACTERID(morph) = ++SWF_gNumCharacters;

	morph->out = newSWFOutput();

	morph->shape1 = newSWFShape();
	BLOCK(morph->shape1)->type = SWF_END;

	morph->shape2 = newSWFShape();
	BLOCK(morph->shape2)->type = SWF_END;
	SWFShape_setMorphFlag(morph->shape2);

	return morph;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
