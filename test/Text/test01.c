#include <libming.h>

int main()
{
	FILE *fontfile;
	
	SWFMovie m = newSWFMovie();
	SWFText text = newSWFText2();
	fontfile = fopen("../Media/font01.fdb", "rb");
	if(fontfile == NULL)
		exit(1);
	
	SWFFont font = loadSWFFontFromFile(fontfile);
	if(font == NULL)
		exit(1);
	
	SWFText_setFont(text, font);
	SWFText_addString(text, "abc", NULL);
	SWFText_setColor(text, 0, 0, 0, 0xff);
	SWFText_setHeight(text, 20);
	SWFMovie_add(m, text);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test01.swf");

	return 0;
}
	
