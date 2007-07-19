#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovie();
	SWFTextField text = newSWFTextField();
	SWFFont font = newSWFFont_fromFile("../Media/test.ttf");
	
	if(font == NULL)
		exit(1);
	
	SWFTextField_setFont(text, font);
	SWFTextField_setColor(text, 0, 0, 0, 0xff);
	SWFTextField_setHeight(text, 20);
	
	SWFMovie_add(m, text);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test02.swf");

	return 0;
}
	
