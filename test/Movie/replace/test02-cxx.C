#include <mingpp.h>
#include <cstdlib>


int main()
{
	int i;
	try {
		SWFMovie *m = new SWFMovie(8);

		SWFShape *shape1 = new SWFShape();
	
		shape1->setLine2(1, 25, 0, 0, 128, SWF_LINESTYLE_FLAG_HINTING, 0);
		shape1->movePenTo(5, 5);
		shape1->drawLineTo(50, 30);

		SWFDisplayItem *item = m->add(shape1);
		m->nextFrame();
		m->replace(item, shape1);
		m->nextFrame();
	
		m->save("test02.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
