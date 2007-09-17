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

	SWFFont font2 = newSWFFont_fromFile(MEDIADIR "/test.ttf");
	if(font2 == NULL)
	{
		perror(MEDIADIR "/test.ttf");
		exit(EXIT_FAILURE);
	}
	
	SWFText_setFont(text, font);
	SWFText_setColor(text, 0, 0, 0, 0xff);
	SWFText_setHeight(text, 20);
	SWFText_addString(text, "abc", NULL);
	SWFText_setFont(text, font2);
	SWFText_setColor(text, 0xff, 0, 0, 0xff);
	SWFText_setHeight(text, 40);
	SWFText_addString(text, "def", NULL);
	SWFMovie_add(m, text);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test02.swf");

	return 0;
}
	
