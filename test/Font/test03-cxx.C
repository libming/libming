#include <mingpp.h>

int main()
{
	SWFMovie *m = new SWFMovie();
	SWFText *text = new SWFText(1);
	
	SWFFont *font = new SWFFont(MEDIADIR "/test.ttf");
	if(font == NULL)
		return -1;

        float w = font->getStringWidth("The quick brown fox jumps over the lazy dog. 1234567890");
	
	text->setFont( font);
	text->setColor(0, 0, 0, 0xff);
	text->setHeight(20);
	text->moveTo(w, 0);
	text->addString("|", NULL);
	m->add(text);
	m->nextFrame();
	m->save("test03.swf");

	return 0;
}
	
