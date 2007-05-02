//#include <math.h>
#include <stdlib.h>
#include "blocks/blocktypes.h"
#include "decompile.h"
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
outputBlock (int type, SWF_Parserstruct * blockp, FILE* f,
		int offset, int length)
{
	static int fontnum=0;
	FILE *out;
	char buf[length];
	char name[256];
	int skipBytes;

	if ( type != SWF_DEFINEFONT2 ) return;

	sprintf(name, "font%i.fdb", fontnum++);

	printf("Writing font '%s' to file '%s' (%i bytes)... ",
		blockp->SWF_DEFINEFONT2.FontName, name, length);
	fflush(stdout);

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
	length -= skipBytes;
	fread(buf, length, 1, f);
	fwrite(buf, length, 1, out);

	fclose(out);

	printf("Done.\n");

}
