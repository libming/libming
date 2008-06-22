#include <mingpp.h>
#include <cstdlib>

#define SIZE 20
int main()
{
	
	SWFMovie *m;
	SWFShape *shape;
	SWFButton *b;
	SWFDisplayItem *item;
	SWFFilterMatrix *cmf;
	static SWFFilter *f;
	SWFBitmap *bm;
	SWFFillStyle *fs;

	try {	
		m = new SWFMovie(9);
	
		m->setBackground(0xcc, 0xcc, 0xcc);
		shape = new SWFShape();
	
		bm = new SWFBitmap(MEDIADIR"/image01.dbl");
	
		fs = SWFFillStyle::BitmapFillStyle(bm, SWFFILL_TILED_BITMAP);
	
		shape->setRightFillStyle(fs);
		int w = bm->getWidth();
		int h = bm->getHeight();
		shape->drawLine(w, 0);
		shape->drawLine(0, h);
		shape->drawLine(-w, 0);
		shape->drawLine(0, -h);		


		float tab[SIZE] = {0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1};
	
		cmf = new SWFFilterMatrix(5,4, tab);
		f = SWFFilter::ColorMatrixFilter(cmf);

		b = new SWFButton();
		b->addCharacter(shape, SWFBUTTON_UP | SWFBUTTON_HIT | SWFBUTTON_OVER | SWFBUTTON_DOWN);
		item = m->add(b);
		item->addFilter(f);

		m->save("test05.swf");
	}
	catch (SWFException &e)
	{
		std::cerr << "SWFException: " << e.what() << std::endl << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
