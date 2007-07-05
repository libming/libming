#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovie();
	SWFText text = newSWFText();
	
	SWFFont font = newSWFFont_fromFile("../Media/test.ttf");
	if(font == NULL)
		exit(1);
	
	SWFText_setFont(text, font);
	SWFText_setColor(text, 0, 0, 0, 0xff);
	SWFText_setHeight(text, 20);
	SWFText_moveTo(text, 100, 100);
	SWFText_addString(text, "abc", NULL);
	
	SWFMovie_add(m, text);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test02.swf");

	return 0;
}
	
