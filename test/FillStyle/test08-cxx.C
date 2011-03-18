#include <mingpp.h>
#include <cstdlib>

main(){

	try {
		SWFMovie* m = new SWFMovie(8);

		SWFShape* shape = new SWFShape();
		SWFGradient* gradient = new SWFGradient();
		gradient->addEntry(0,0xff,0x00,0x00,0xff);
		gradient->addEntry(0.25,0x80,0x20,0x20,0xff);
		gradient->addEntry(0.8,0x40,0x40,0x40,0xff);
		SWFFill* fill = shape->addGradientFill(gradient,
			SWFFILL_LINEAR_GRADIENT);
		shape->setRightFill(fill);

		shape->setLine(1,0,0,0,255);
		shape->drawLine(100, 0);
		shape->drawLine(0, 100);
		shape->drawLine(-100, 0);
		shape->drawLine(0, -100);

		m->add(shape);
		m->save("test08.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
