#include <stdlib.h>
#include "blocks/blocktypes.h"
#include "parser.h"
#include "swfoutput.h"

void
outputHeader (struct Movie *m)
{
	printf("File version: %i\n", m->version);
}

void
outputTrailer (struct Movie *m)
{
}

void
outputBlock (int type, SWF_Parserstruct * blockp, FILE* f)
{
	FILE *out;
	char name[256];
	int skipBytes, ret;
	int offset = blockp->offset;
	int length = blockp->length;
	char *buf;

	switch ( type ) {
		case SWF_DEFINEFONT:
		case SWF_DEFINEFONT2:
		case SWF_DEFINEFONT3:
			break;
		default:
			return;
	}

	sprintf(name, "font%i.fdb", blockp->block.SWF_DEFINEFONT2.FontID);


  	out=fopen(name,"wb");
	if ( out == NULL )
	{
		perror("Failed");
		return;
	}

	fputc('f', out);
	fputc('d', out);
	fputc('b', out);
	fputc('0', out);

	skipBytes = 4; // skip blockType (2 bytes) + fontId (2 bytes) 
	if(length >= 63) // long Block
		skipBytes += 4;

	fseek(f, offset + skipBytes, SEEK_SET); /* skip FontId (UI16) */
	length -= 2;				/* sub FontId (UI16) only */
	
	printf("Writing font '%s' to file '%s' (%i bytes)... ",
		blockp->block.SWF_DEFINEFONT2.FontName, name, length+4); /* +4 for 'fdb0' header */
	fflush(stdout);
	
	buf = malloc(length);
	if (!buf) 
	{
		fclose(out);
		SWF_error("memory allocation error");
		return;
	}
	ret = fread(buf, length, 1, f);
	if (ret != 1) 
	{
		fclose(out);
		free(buf);
		return;
	}
		
	ret = fwrite(buf, length, 1, out);
	free(buf);
	if(ret != 1)
	{
		fclose(out);
		return;
	}
	fclose(out);
	printf("Done.\n");
}
