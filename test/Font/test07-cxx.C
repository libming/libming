#include <mingpp.h>
#include <cstdlib>


int main()
{
	try { 

		SWFMovie* m = new SWFMovie(6);
		SWFText *text = new SWFText(0);
		SWFTextField *textfield = new SWFTextField();
		const char *string = "Euro \xe2\x82\xac - Trademark \xe2\x84\xa2 - Ligatures \xef\xac\x81 and \xef\xac\x82 ";


		SWFFont *font = new SWFFont(MEDIADIR "/font-kerntest.fdb");

		text->setFont(font);
		text->setColor( 0, 0, 0, 0xff);
		text->setHeight(10);
		text->moveTo(0, 30);
		text->addUTF8String(string);
		m->add(text);

		textfield->setFont( font);
		textfield->setFlags(SWFTEXTFIELD_NOEDIT);
		textfield->setColor( 0xff, 0, 0, 0xff);
		textfield->setHeight(10);
		textfield->addUTF8String(string);
		m->add(textfield);

		m->save("test07.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
