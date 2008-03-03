#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie();
		SWFTextField *text = new SWFTextField();
	
		SWFBrowserFont *font = new SWFBrowserFont("_sans");
	
		text->setFont(font);
		text->setColor( 0, 0, 0, 0xff);
		text->setHeight( 20);
		text->addString( "abc");
		m->add(text);
		m->nextFrame();
		m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}

	return 0;
}
	
