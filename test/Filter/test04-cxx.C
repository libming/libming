#include <mingpp.h>
#include <cstdlib>

// static char *test = "test04";

int main()
{
	SWFMovie *m;
	SWFShape *shape;
	SWFButton *b;
	SWFDisplayItem *item;
	SWFBlur *blur;
	SWFFilter *f;
	SWFColor c1, c2;
	SWFShadow *shadow;

	try {
		Ming_init();
		m = new SWFMovie(7);
	
		shape = new SWFShape();

		shape->setLine(4, 25, 0, 0, 128);
		shape->movePenTo(5, 5);
		shape->drawLineTo( 0, 10);
	
		shadow = new SWFShadow(0.79, 5, 1.0);
		blur = new SWFBlur(5,5,2);
		c1.red = 0;
		c1.green = 0;
		c1.blue = 0;
		c1.alpha = 0xff;

		c2.red = 0xff;
		c2.green = 0xff;
		c2.blue = 0xff;
		c2.alpha = 0xff;

		f = SWFFilter::BevelFilter(c1, c2, blur, shadow, FILTER_MODE_INNER | FILTER_MODE_KO);
	
		b = new SWFButton();
		b->addShape(shape, SWFBUTTON_UP | SWFBUTTON_HIT | SWFBUTTON_OVER | SWFBUTTON_DOWN);
		item = m->add(b);
	
		item->addFilter(f);
		m->save("test04.swf");
	}
	catch(SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
