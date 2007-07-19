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
	m->add(text);
	m->nextFrame();
	m->save("test02.swf");

	return 0;
}
	
