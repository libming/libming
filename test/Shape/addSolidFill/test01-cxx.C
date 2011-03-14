#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(8);
		SWFShape *shape1 = new SWFShape();
		shape1->addSolidFill(32, 64, 128, 223);
		shape1->addSolidFill(64, 128, 32, 24);
		m->add(shape1);
		m->save("test01.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
