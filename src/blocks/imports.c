#include "block.h"
#include "imports.h"

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
	return length;
}

void
destroySWFImportBlock(SWFBlock block)
{	SWFImportBlock imports = (SWFImportBlock) block;
	struct importitem *ip, *nip;
	if(imports->filename)
		free(imports->filename);
	for(ip = imports->importlist ; ip ; ip = nip)
	{	nip = ip->next;
		if(ip->name)
			free(ip->name);
		free(ip);
	}
	free(block);
}

static char *cpy(const char *text)
{	char *res, *p;
	p = res = (char *)malloc(strlen(text)+1);
	while(*p++ = *text++)
		;
	return res;
}

SWFImportBlock
newSWFImportBlock(const char *filename)
{	SWFImportBlock iblock = (SWFImportBlock) malloc(sizeof(struct SWFImportBlock_s));
	BLOCK(iblock)->type = SWF_IMPORTASSETS;
	BLOCK(iblock)->writeBlock = writeSWFImportBlockToMethod;
	BLOCK(iblock)->complete = completeSWFImportBlock;
	BLOCK(iblock)->dtor = destroySWFImportBlock;
	BLOCK(iblock)->isDefined = 0;
	BLOCK(iblock)->completed = 0;
	iblock->filename = cpy(filename);
	iblock->importlist = NULL;
	return iblock;
}
