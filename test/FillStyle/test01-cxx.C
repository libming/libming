#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(8);

		SWFFillStyle *fill =  SWFFillStyle::SolidFillStyle(255, 128, 0, 255);
		SWFShape *shape = new SWFShape();
		shape->setRightFillStyle(fill);
	
		shape->setLine(1, 0,0,0,255);
		shape->drawLine(100, 0);
		shape->drawLine(0, 100);
		shape->drawLine(-100, 0);
		shape->drawLine(0, -100);

		m->add(shape);
		m->save("test01.swf");
	}
	catch (SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;

}
