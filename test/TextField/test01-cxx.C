#include <mingpp.h>

int main()
{
	SWFMovie *m = new SWFMovie();
	SWFTextField *text = new SWFTextField();
	
	SWFFont *font = new SWFFont("../Media/test.ttf");
	if(font == NULL)
		return -1;
	
	text->setFont(font);
	text->setColor( 0, 0, 0, 0xff);
	text->setHeight( 20);
	text->setFlags(SWFTEXTFIELD_NOEDIT);
	text->addString("The quick brown fox jumps over the lazy dog. 1234567890");
	m->add(text);
	m->nextFrame();
	m->save("test01.swf");

	return 0;
}
	
