#include <mingpp.h>
#include <cstdlib>


int main()
{
	try { 

		SWFMovie* m = new SWFMovie();
		SWFFont *font = new SWFFont(MEDIADIR "/font01.fdb");

		SWFText *text = new SWFText(1);
		text->setFont( font);
		text->setColor( 0, 0, 0xff, 0xff);
		text->setHeight(20);
		text->moveTo(100, 100);
		text->addString( "1234567890", NULL);

		SWFTextField *textfield = new SWFTextField();
		textfield->setFont( font);
		textfield->setColor( 0xff, 0, 0, 0xff);
		textfield->setHeight(20);
		textfield->addString( "1234567890");

		m->add(text);
		SWFDisplayItem* i = m->add(textfield);
		i->moveTo(100, 120);

		m->nextFrame(); // end of frame 1 

		m->save("test04.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
