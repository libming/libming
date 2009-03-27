#include <cstdlib>
#include <cstdio>
#include <mingpp.h>

int main()
{
	FILE *fontfile;
	
	try {
		SWFMovie *m = new SWFMovie();
		SWFText *text = new SWFText(2);
	
		fontfile = fopen(MEDIADIR "/font01.fdb", "rb");
		if(fontfile == NULL)
		{
			perror(MEDIADIR "/font01.fdb");
			exit(EXIT_FAILURE);
		}
	
		SWFFont *font = new SWFFont(fontfile);
	
		text->setFont( font);
		text->setColor( 0, 0, 0, 0xff);
		text->setHeight( 20);
		text->addString( "abc", NULL);
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
	
