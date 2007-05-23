#include <mingpp.h>


int main()
{
	int i;
	SWFMovie *m = new SWFMovie(8);

	SWFShape *shape1 = new SWFShape();
	
	shape1->setLine2(1, 25, 0, 0, 128, SWF_LINESTYLE_FLAG_HINTING, 0);
	shape1->movePenTo(5, 5);
        shape1->drawLineTo(50, 30);

	SWFDisplayItem *item = m->add(shape1);
	m->nextFrame();
	m->replace(item, shape1);
	m->nextFrame();

	m->save("test02.swf");

	return 0;
}
