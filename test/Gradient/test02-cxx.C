#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(8);
		SWFGradient *g = new SWFGradient();

		g->addEntry(0, 0, 0, 0, 255);	
		g->addEntry(0.2, 0, 10, 0, 255);	
		g->addEntry(0.3, 110, 0, 10, 255);	
		g->addEntry(0.4, 20, 0, 0, 255);	
		g->addEntry(0.5, 0, 20, 0, 255);	
		g->addEntry(0.6, 0, 0, 20, 255);	
		g->addEntry(0.7, 30, 0, 0, 255);	

		SWFFillStyle *fill = SWFFillStyle::GradientFillStyle(g, SWFFILL_LINEAR_GRADIENT);
		SWFShape *shape = new SWFShape();
		shape->setRightFillStyle(fill);
	
		shape->setLine(1, 0,0,0,255);
		shape->drawLine(100, 0);
		shape->drawLine(0, 100);
		shape->drawLine(-100, 0);
		shape->drawLine(0, -100);

		m->add(shape);
		m->save("test02.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;

}
