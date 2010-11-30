#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <libming.h>

/*
 * Testcase for bug http://bugs.libming.org/show_bug.cgi?id=98
 */

int main()
{
	/*
	 * We must target SWF6 or higher or UTF-8 will not be supported 
	 * by _player_ (no changes in SWF, player fails rendering UTF-8
	 * in TextField)
	 */
	SWFMovie m = newSWFMovieWithVersion(6);

	SWFText text = newSWFText();
	SWFTextField textfield = newSWFTextField();
	const char *string = "Euro \xe2\x82\xac - Trademark \xe2\x84\xa2 - Ligatures \xef\xac\x81 and \xef\xac\x82 ";


	SWFFont font = newSWFFont_fromFile(MEDIADIR "/font-kerntest.fdb");
	if(font == NULL)
	{
		perror(MEDIADIR "/font-kerntest.fdb");
		exit(EXIT_FAILURE);
	}

	SWFText_setFont(text, font);
	SWFText_setColor(text, 0, 0, 0, 0xff);
	SWFText_setHeight(text, 10);
	SWFText_moveTo(text, 0, 30);
	SWFText_addUTF8String(text, string, NULL);
	SWFMovie_add(m, text);

        SWFTextField_setFont(textfield, font);
        SWFTextField_setFlags(textfield, SWFTEXTFIELD_NOEDIT);
        SWFTextField_setColor(textfield, 0xff, 0, 0, 0xff);
        SWFTextField_setHeight(textfield, 10);
        SWFTextField_addUTF8String(textfield, string);
	SWFMovie_add(m, textfield);

	SWFMovie_save(m, "test07.swf");

	return 0;
}

