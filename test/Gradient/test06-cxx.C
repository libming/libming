#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(8);
		SWFGradient *g = new SWFGradient();

		g->setSpreadMode(SWF_GRADIENT_REPEAT);
		g->setInterpolationMode(SWF_GRADIENT_LINEAR);
		g->setFocalPoint(0.2);

		g->addEntry(0, 0, 0, 0, 255);	
		g->addEntry(0.2, 0, 10, 0, 255);	
		g->addEntry(0.3, 110, 0, 10, 255);	
		g->addEntry(0.4, 20, 0, 0, 255);	
		g->addEntry(0.5, 0, 20, 0, 255);	
		g->addEntry(0.6, 0, 0, 20, 255);	
		g->addEntry(0.7, 30, 0, 0, 255);

		g->addEntry(0.8, 0, 30, 0, 255);	
		g->addEntry(0.9, 0, 0, 30, 255);		

		SWFFillStyle *fill = SWFFillStyle::GradientFillStyle(g, SWFFILL_LINEAR_GRADIENT);
		SWFShape *shape = new SWFShape();
		shape->useVersion(SWF_SHAPE4);
		shape->setRightFillStyle(fill);
	
		shape->setLine(1, 0,0,0,255);
		shape->drawLine(100, 0);
		shape->drawLine(0, 100);
		shape->drawLine(-100, 0);
		shape->drawLine(0, -100);

		m->add(shape);
		m->save("test06.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
