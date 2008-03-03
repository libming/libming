#include <mingpp.h>
#include <cstdlib>


int main()
{
	SWFMovie *m;
	SWFPrebuiltClip *pbc;

	try {
		m = new SWFMovie();
		m->save("empty.swf");
	
		m = new SWFMovie();
		pbc = new SWFPrebuiltClip("empty.swf");
		m->add(pbc);

		pbc = new SWFPrebuiltClip("empty.swf");
		m->add(pbc);

		m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
