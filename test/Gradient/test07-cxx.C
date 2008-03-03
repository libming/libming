#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(8);
		m->setBackground(0,0,0);
		m->setDimension(500,450);
		SWFGradient *g = new SWFGradient();

		g->addEntry(0.0, 255, 255, 255, 255);   
		g->addEntry(1.0, 255, 255, 255, 0);     

		SWFFillStyle *fillstyle =  SWFFillStyle::GradientFillStyle(g, SWFFILL_RADIAL_GRADIENT);
		SWFShape *shape = new SWFShape();
		shape->setRightFillStyle(fillstyle);

		shape->drawCircle(55);

		SWFDisplayItem *i = m->add(shape);
		i->moveTo(100,100);
		m->save("test07.swf");
	}
	catch (SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;	
	}
	return 0;
}
