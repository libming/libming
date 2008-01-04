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
	
	float w = SWFFont_getStringWidth(font, "The quick brown fox jumps over the lazy dog. 1234567890");
	
	SWFText_setFont(text, font);
	SWFText_setColor(text, 0, 0, 0, 0xff);
	SWFText_setHeight(text, 20);
	SWFText_moveTo(text, w, 0);
	SWFText_addString(text, "|", NULL);
	
	SWFMovie_add(m, text);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test03.swf");

	return 0;
}
	
