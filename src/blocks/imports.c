/* $Id$ */

#ifndef __C2MAN__
#include <stdlib.h>
#include <string.h>
#endif

#include "imports.h"
#include "method.h"
#include "libming.h"

int
writeSWFImportBlockToMethod(SWFBlock block, SWFByteOutputMethod method, void *data)
{	SWFImportBlock imports = (SWFImportBlock) block;
	struct importitem *ip;
	int length, count;
	char *p;
	length = 3 + strlen(imports->filename);
	for(ip = imports->importlist, count = 0 ; ip ; ip = ip->next)
	{	length += 3 + strlen(ip->name);
		count++;
	}

	for(p = imports->filename ; *p ; )
		method(*p++, data);
	method(0, data);

	if(block->swfVersion >= 8)
	{
		method(1, data);
		method(0, data);
	}

	methodWriteUInt16(count, method, data);
	for(ip = imports->importlist ; ip ; ip = ip->next)
	{	methodWriteUInt16(ip->id, method, data);
		for(p = ip->name ; *p ; )
			method(*p++, data);
		method(0, data);
	}
	return length;
}

int
completeSWFImportBlock(SWFBlock block)
{	SWFImportBlock imports = (SWFImportBlock) block;
	struct importitem *ip;
	int length = 3 + strlen(imports->filename);
	for(ip = imports->importlist ; ip ; ip = ip->next)
		length += 3 + strlen(ip->name);

	/* SWF_IMPORTASSETS is deprecated with version >= 8. */
	if(block->swfVersion >= 8)
	{
		block->type = SWF_IMPORTASSETS2;
		length += 2;
	}	

	return length;
}

void
destroySWFImportBlock(SWFImportBlock importBlock)
{
	struct importitem *ip, *nip;
	if(importBlock->filename)
		free(importBlock->filename);
	for(ip = importBlock->importlist ; ip ; ip = nip)
	{	nip = ip->next;
		if(ip->name)
			free(ip->name);
		free(ip);
	}
	free(importBlock);
}

static char *cpy(const char *text)
{	char *res, *p;
	p = res = (char *)malloc(strlen(text)+1);
	while((*p++ = *text++))
		;
	return res;
}

SWFImportBlock
newSWFImportBlock(const char *filename)
{	SWFImportBlock iblock = (SWFImportBlock) malloc(sizeof(struct SWFImportBlock_s));
	
	BLOCK(iblock)->type = SWF_IMPORTASSETS;

	BLOCK(iblock)->writeBlock = (writeSWFBlockMethod) writeSWFImportBlockToMethod;
	BLOCK(iblock)->complete = completeSWFImportBlock;
	BLOCK(iblock)->dtor = (destroySWFBlockMethod) destroySWFImportBlock;
	BLOCK(iblock)->isDefined = 0;
	BLOCK(iblock)->completed = 0;
	iblock->filename = cpy(filename);
	iblock->importlist = NULL;
	return iblock;
}
