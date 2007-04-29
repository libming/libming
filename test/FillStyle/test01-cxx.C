#include <mingpp.h>

int main()
{
	SWFMovie *m = new SWFMovie(8);

	SWFFillStyle *fill =  SWFFillStyle::SolidFillStyle(255, 128, 0, 255);
	SWFShape *shape = new SWFShape();
	shape->setRightFillStyle(fill);
	
	shape->setLineStyle(1, 0,0,0,255);
	shape->drawLine(100, 0);
	shape->drawLine(0, 100);
	shape->drawLine(-100, 0);
	shape->drawLine(0, -100);

	m->add(shape);
	m->save("test01.swf");
	return 0;

}
