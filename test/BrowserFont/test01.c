#include <libming.h>
#include <stdlib.h>

int main()
{
	SWFMovie m = newSWFMovie();
	SWFTextField text = newSWFTextField();
	
	SWFBrowserFont font = newSWFBrowserFont("_sans");
	if(font == NULL)
		return EXIT_FAILURE;
	
	SWFTextField_setFont(text, (SWFBlock)font);
	SWFTextField_setColor(text, 0, 0, 0, 0xff);
	SWFTextField_setHeight(text, 20);
	SWFTextField_addString(text, "abc");
	
	SWFMovie_add(m, text);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test01.swf");

	return 0;
}
	
