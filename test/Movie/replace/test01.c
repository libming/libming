#include <ming.h>


int main()
{
	int i;
	SWFMovie m = newSWFMovieWithVersion(8);

	SWFShape shape1 = newSWFShape();
	SWFShape shape2 = newSWFShape();
	
	SWFShape_setLine2(shape1, 1, 25, 0, 0, 128, SWF_LINESTYLE_FLAG_HINTING, 0);
	SWFShape_movePenTo(shape1, 5, 5);
        SWFShape_drawLineTo(shape1, 50, 30);

	SWFShape_setLine2(shape2, 1, 25, 100, 100, 255, SWF_LINESTYLE_FLAG_HINTING, 0);
	SWFShape_movePenTo(shape2, 5, 5);
        SWFShape_drawLineTo(shape2, 50, 130);

	SWFDisplayItem item = SWFMovie_add(m, shape1);
	SWFMovie_nextFrame(m);
	SWFMovie_replace(m, item, shape2);
	SWFMovie_nextFrame(m);

	SWFMovie_save(m,"test01.swf");

	return 0;
}
