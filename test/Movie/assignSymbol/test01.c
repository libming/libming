#include <libming.h>
#include <stdlib.h>

int main()
{
	FILE *fontfile;
	
	SWFMovie m = newSWFMovieWithVersion(9);
	SWFText text = newSWFText();
	
	SWFFont font = newSWFFont_fromFile(MEDIADIR "/font01.fdb");
	if(font == NULL)
	{
		perror(MEDIADIR "/font01.fdb");
		return EXIT_FAILURE;
	}
	
	SWFMovie_assignSymbol(m, (SWFCharacter)text, "mytext");
	SWFText_setFont(text, font);
	SWFText_setColor(text, 0, 0, 0, 0xff);
	SWFText_setHeight(text, 10);
	SWFText_moveTo(text, 10, 100);
	SWFText_addString(text, "The quick brown fox jumps over the lazy dog. 1234567890", NULL);
	
	SWFMovie_add(m, text);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test01.swf");

	return 0;
}
	
