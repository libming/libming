#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <libming.h>

/*
 * Testcase for bug http://bugs.libming.org/show_bug.cgi?id=89
 *
 */


int main()
{
	SWFMovie m = newSWFMovie();
	SWFText text = newSWFText();
	SWFTextField textfield = newSWFTextField();
	SWFDisplayItem it;
	
	SWFFont font = newSWFFont_fromFile(MEDIADIR "/font01.fdb");
	if(font == NULL)
	{
		perror(MEDIADIR "/font01.fdb");
		exit(EXIT_FAILURE);
	}
	
	SWFText_setFont(text, font);
	SWFText_setColor(text, 0, 0, 0xff, 0xff);
	SWFText_setHeight(text, 20);
	SWFText_moveTo(text, 100, 100);
	SWFText_addString(text, "1234567890", NULL);

	SWFTextField_setFont(textfield, font);
	SWFTextField_addString(textfield, "1234567890");
	SWFTextField_setColor(textfield, 0xff, 0, 0, 0xff);
	SWFTextField_setHeight(textfield, 20);
	
	SWFMovie_add(m, text);
	it = SWFMovie_add(m, textfield);
	SWFDisplayItem_moveTo(it, 100, 120);
	SWFMovie_nextFrame(m);
	SWFMovie_save(m, "test04.swf");

	return 0;
}
	
