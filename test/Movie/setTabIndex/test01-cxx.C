#include <mingpp.h>
#include <cstdlib>


int main()
{
	try {
		SWFMovie *m = new SWFMovie(7);

		m->setTabIndex(1, 2);
		m->setTabIndex(2, 2);

		 m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}

