#include <mingpp.h>
#include <cstdlib>

// static char *test = "test03";

int main()
{
	SWFMovie *m;
	SWFShape *shape;
	SWFButton *b;
	SWFDisplayItem *item;
	SWFBlur *blur;
	SWFFilter *f;
	SWFColor c;

	try {
		Ming_init();
		m = new SWFMovie(7);
		shape = new SWFShape();

		shape->setLine(4, 25, 0, 0, 128);
		shape->movePenTo(5, 5);
		shape->drawLineTo( 0, 10);
	
		blur = new SWFBlur(5,5,2);
		c.red = 0;
		c.green = 0;
		c.blue = 0;
		c.alpha = 0xff;

		f = SWFFilter::GlowFilter(c, blur, 1.0, FILTER_MODE_INNER | FILTER_MODE_KO);
	
		b = new SWFButton();
		b->addShape(shape, SWFBUTTON_UP | SWFBUTTON_HIT | SWFBUTTON_OVER | SWFBUTTON_DOWN);
		item = m->add(b);
	
		item->addFilter(f);
		m->save("test03.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
