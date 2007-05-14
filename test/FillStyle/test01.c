#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);

	SWFFillStyle fill = newSWFSolidFillStyle(255, 128, 0, 255);
	SWFShape shape = newSWFShape();
	SWFShape_setRightFillStyle(shape, fill);
	
	SWFShape_setLine(shape, 1, 0,0,0,255);
	SWFShape_drawLine(shape, 100, 0);
	SWFShape_drawLine(shape, 0, 100);
	SWFShape_drawLine(shape, -100, 0);
	SWFShape_drawLine(shape, 0, -100);

	SWFMovie_add(m, (SWFBlock)shape);
	SWFMovie_save(m, "test01.swf");
	return 0;

}
