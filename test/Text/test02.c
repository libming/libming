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

	SWFFont font2 = newSWFFont_fromFile("../Media/test.ttf");
	if(font2 == NULL)
		exit(1);
	
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
	
