#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovie();
	SWFText text = newSWFText();
	
	SWFFont font = newSWFFont_fromFile(MEDIADIR "/font-kerntest.fdb");
	if(font == NULL)
	{
		perror(MEDIADIR "/font-kerntest.fdb");
		exit(EXIT_FAILURE);
	}
	
	SWFText_setFont(text, font);
	SWFText_setColor(text, 0, 0, 0, 0xff);
	SWFText_setHeight(text, 10);
	SWFText_moveTo(text, 10, 100);
	SWFText_addString(text, "ABC", NULL);
	
	SWFMovie_add(m, text);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test06.swf");

	return 0;
}
