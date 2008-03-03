#include <mingpp.h>
#include <cstdlib>

int main()
{
	int i;
	try {
		SWFMovie *m = new SWFMovie(9);
	
	
		for(i = 0; i < 10; i++)
			m->nextFrame();

		m->defineScene(0, "test0");
		m->defineScene(5, "test1");
		m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
	
