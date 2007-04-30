#include <mingpp.h>


int main()
{
	SWFMovie *m = new SWFMovie(8);

	SWFShape *shape = new SWFShape();
	
        shape->setLine2(40, 25, 0, 0, 128, SWF_LINESTYLE_FLAG_HINTING | SWF_LINESTYLE_JOIN_BEVEL | SWF_LINESTYLE_FLAG_ENDCAP_SQUARE, 0);
	shape->movePenTo(5, 5);
        shape->drawLineTo(50, 100);
	shape->drawLineTo(100, 100);

	m->add(shape);
	m->save("test02.swf");

	return 0;
}
