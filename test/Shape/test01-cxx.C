#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(8);
		SWFShape *shape1 = new SWFShape();
		SWFShape *shape2 = new SWFShape();
		shape2->useVersion(SWF_SHAPE4);
		m->add(shape1);
		m->add(shape2);
		m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
