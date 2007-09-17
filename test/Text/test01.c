#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <libming.h>

int main()
{
	FILE *fontfile;
	
	SWFMovie m = newSWFMovie();
	SWFText text = newSWFText2();
	fontfile = fopen(MEDIADIR "/font01.fdb", "rb");
	if(fontfile == NULL)
	{
		perror(MEDIADIR "/font01.fdb");
		exit(EXIT_FAILURE);
	}
	
	SWFFont font = loadSWFFontFromFile(fontfile);
	if(font == NULL)
	{
		fprintf(stderr, "Could not load SWFFont from file\n");
		exit(EXIT_FAILURE);
	}
	
	SWFText_setFont(text, font);
	SWFText_setColor(text, 0, 0, 0, 0xff);
	SWFText_setHeight(text, 20);
	SWFText_addString(text, "abc", NULL);
	SWFMovie_add(m, text);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test01.swf");

	return 0;
}
	
