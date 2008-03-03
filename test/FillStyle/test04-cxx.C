#include <mingpp.h>
#include <cstdlib>

int main()
{
	try {
		SWFMovie *m = new SWFMovie(8);
		SWFBitmap *bmp = new SWFBitmap(MEDIADIR "/image01.dbl");

		SWFFillStyle *fill =  SWFFillStyle::BitmapFillStyle(bmp, SWFFILL_CLIPPED_BITMAP);
		SWFShape *shape = new SWFShape();
		shape->setRightFillStyle(fill);
	
		shape->setLine(1, 0,0,0,255);
		shape->drawLine(100, 0);
		shape->drawLine(0, 100);
		shape->drawLine(-100, 0);
		shape->drawLine(0, -100);

		m->add(shape);
		m->save("test04.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;

}
