#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovie();
	SWFText text = newSWFText();
	
	SWFFont font = newSWFFont_fromFile(MEDIADIR "/test.ttf");
	if(font == NULL)
	{
		perror(MEDIADIR "/test.ttf");
		exit(EXIT_FAILURE);
	}
	
	SWFText_setFont(text, font);
	SWFText_setColor(text, 0, 0, 0, 0xff);
	SWFText_setHeight(text, 20);
	SWFText_moveTo(text, 100, 100);
	SWFText_addString(text, "The quick brown fox jumps over the lazy dog. 1234567890", NULL);
	
	SWFMovie_add(m, text);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test02.swf");

	return 0;
}
	
