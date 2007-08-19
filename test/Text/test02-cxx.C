#include <mingpp.h>

int main()
{
	FILE *fontfile;
	
	SWFMovie *m = new SWFMovie();
	SWFText *text = new SWFText();
	
	fontfile = fopen("../Media/font01.fdb", "rb");
	if(fontfile == NULL)
		return -1;
	
	SWFFont *font = new SWFFont(fontfile);
	if(font == NULL)
		return -1;

	SWFFont *font2 = new SWFFont("../Media/test.ttf");
	if(font2 == NULL)
		return -1;
	
	text->setFont( font);
	text->setColor( 0, 0, 0, 0xff);
	text->setHeight( 20);
	text->addString( "abc", NULL);
	text->setFont( font2);
	text->setColor( 0xff, 0, 0, 0xff);
	text->setHeight( 40);
	text->addString( "def", NULL);
	m->add(text);
	m->nextFrame();
	m->save("test02.swf");

	return 0;
}
	
