#include <mingpp.h>

int main()
{
	SWFMovie *m = new SWFMovie(8);
	SWFBitmap *bmp = new SWFBitmap("../Media/image01.jpeg");


	SWFFillStyle *fill =  SWFFillStyle::BitmapFillStyle(bmp, SWFFILL_CLIPPED_BITMAP);
	SWFShape *shape = new SWFShape();
	shape->setRightFillStyle(fill);
	
	shape->setLineStyle(1, 0,0,0,255);
	shape->drawLine(100, 0);
	shape->drawLine(0, 100);
	shape->drawLine(-100, 0);
	shape->drawLine(0, -100);

	m->add(shape);
	m->save("test03.swf");
	return 0;

}
