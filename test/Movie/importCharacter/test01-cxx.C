#include <mingpp.h>
#include <cstdlib>


int main()
{
	try {
		SWFMovie *m = new SWFMovie();
		SWFCharacter *c = m->importCharacter("test.swf", "test");
		m->add(c);
		m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
