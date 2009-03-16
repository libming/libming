#include <mingpp.h>
#include <cstdlib>


main(){

	try {
		SWFMovie* m = new SWFMovie(8);

		SWFShape* shape = new SWFShape();
		// TODO: check when we should use
		//       SWFFillStyle::BitmapFillStyle
		// rather than
		//       addBitmapFill
		// See test06-cxx.C ...
		//
		SWFFill* fill = shape->addBitmapFill(0, SWFFILL_CLIPPED_BITMAP);
		shape->setRightFill(fill);

		shape->setLine(1, 0,0,0,255);
		shape->drawLine(100, 0);
		shape->drawLine(0, 100);
		shape->drawLine(-100, 0);
		shape->drawLine(0, -100);

		m->add(shape);
		m->save("test07.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
	
}
