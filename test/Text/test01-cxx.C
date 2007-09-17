#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mingpp.h>

int main()
{
	FILE *fontfile;
	
	SWFMovie *m = new SWFMovie();
	SWFText *text = new SWFText();
	
	fontfile = fopen(MEDIADIR "/font01.fdb", "rb");
	if(fontfile == NULL)
	{
		perror(MEDIADIR "/font01.fdb");
		exit(EXIT_FAILURE);
	}
	
	SWFFont *font = new SWFFont(fontfile);
	if(font == NULL)
	{
		fprintf(stderr, "Could not create SWFFont from file\n");
		exit(EXIT_FAILURE);
	}
	
	text->setFont( font);
	text->setColor( 0, 0, 0, 0xff);
	text->setHeight( 20);
	text->addString( "abc", NULL);
	m->add(text);
	m->nextFrame();
	m->save("test01.swf");

	return 0;
}
	
