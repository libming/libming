#include <mingpp.h>
#include <cstdlib>


int main()
{
	SWFMovie *m;
	SWFAction *a;
	SWFInitAction *init;

	try{
		m = new SWFMovie();
		a = new SWFAction("trace('');");
		init = new SWFInitAction(a);
	
		m->add(init);

		m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
