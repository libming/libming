#include <mingpp.h>

int main()
{
	FILE *fontfile;
	
	SWFMovie *m = new SWFMovie();
	SWFText *text = new SWFText(1);
	
	fontfile = fopen("../Media/font01.fdb", "rb");
	if(fontfile == NULL)
		return -1;
	
	SWFFont *font = new SWFFont(fontfile);
	if(font == NULL)
		return -1;
	
	text->setFont( font);
	text->addString( "abc", NULL);
	text->setColor( 0, 0, 0, 0xff);
	text->setHeight( 20);
	m->add(text);
	m->nextFrame();
	m->save("test01.swf");

	return 0;
}
	
