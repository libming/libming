#include <cstdlib>
#include <cstdio>
#include <mingpp.h>

int main()
{
	FILE *fontfile;
	
	try {
		SWFMovie *m = new SWFMovie();
		SWFText *text = new SWFText(2);
	
		SWFFont *font = new SWFFont(MEDIADIR "/font01.fdb");
	
		text->setFont( font);
		text->setColor( 0, 0, 0, 0xff);
		text->setHeight( 20);
		text->moveTo(10, 20);
		text->addString( "abc", NULL);
		text->moveTo(0, 40);
		text->addString( "bca", NULL);
		text->moveTo(60, 0);
		text->addString( "cab", NULL);
		m->add(text);
		m->nextFrame();
		m->save("test03.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
	
