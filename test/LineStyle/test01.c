#include <libming.h>


int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);

	SWFShape shape = newSWFShape();
	
        SWFShape_setLine(shape, 4, 25, 0, 0, 128);
	SWFShape_movePenTo(shape, 5, 5);
        SWFShape_drawLineTo(shape, 50, 100);
	SWFShape_drawLineTo(shape, 100, 100);

	SWFMovie_add(m, (SWFBlock)shape);
	SWFMovie_save(m, "test01.swf");

	return 0;
}
