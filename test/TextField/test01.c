#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovie();
	SWFTextField text = newSWFTextField();
	
	SWFFont font = newSWFFont_fromFile(MEDIADIR "/test.ttf");
	if(font == NULL)
	{
		perror(MEDIADIR "/test.tff");
		exit(EXIT_FAILURE);
	}
	
	SWFTextField_setFont(text, font);
	SWFTextField_setColor(text, 0, 0, 0, 0xff);
	SWFTextField_setHeight(text, 20);
	SWFTextField_setFlags(text, SWFTEXTFIELD_NOEDIT);
	SWFTextField_addString(text, "The quick brown fox jumps over the lazy dog. 1234567890");
	
	SWFMovie_add(m, text);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test01.swf");

	return 0;
}
	
