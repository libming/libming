#include <mingpp.h>

int main()
{
	SWFMovie *m = new SWFMovie(9);
	SWFText *text = new SWFText(1);
	
	SWFFont *font = new SWFFont("../../Media/font01.fdb");
	if(font == NULL)
		return -1;
	m->assignSymbol(text, "mytext");	
	text->setFont( font);
	text->setColor( 0, 0, 0, 0xff);
	text->setHeight( 20);
	text->moveTo(100,100);
	text->addString( "The quick brown fox jumps over the lazy dog. 1234567890", NULL);
	m->add(text);
	m->nextFrame();
	m->save("test01.swf");

	return 0;
}
	
