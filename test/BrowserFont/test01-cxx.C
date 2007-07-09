#include <mingpp.h>

int main()
{
	SWFMovie *m = new SWFMovie();
	SWFTextField *text = new SWFTextField();
	
	SWFBrowserFont *font = new SWFBrowserFont("_sans");
	if(font == NULL)
		return -1;
	
	text->setFont(font);
	text->setColor( 0, 0, 0, 0xff);
	text->setHeight( 20);
	text->addString( "abc");
	m->add(text);
	m->nextFrame();
	m->save("test01.swf");

	return 0;
}
	
