#include <mingpp.h>
#include <cstdlib>


int main()
{
	try { 

		SWFMovie* m = new SWFMovie();
		SWFFont *font = new SWFFont(MEDIADIR "/font-kerntest.fdb");

		SWFText *text = new SWFText(1);
		text->setFont( font);
		text->setColor( 0, 0, 0, 0xff);
		text->setHeight(10);
		text->moveTo(10, 100);
		text->addString( "ABC", NULL);

		m->add(text);
		m->nextFrame(); // end of frame 1 
		m->save("test06.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
