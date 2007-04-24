#include <mingpp.h>


int main()
{
	SWFMovie *m = new SWFMovie(8);

	SWFShape *shape = new SWFShape();
	
        shape->setLineStyle(4, 25, 0, 0, 128);
	shape->movePenTo(5, 5);
        shape->drawLineTo(50, 100);
	shape->drawLineTo(100, 100);

	m->add(shape);
	m->save("test01.swf");

	return 0;
}
