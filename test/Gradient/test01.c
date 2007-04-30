#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFGradient g = newSWFGradient();

	SWFGradient_addEntry(g, 0, 0, 0, 0, 255);	
	SWFGradient_addEntry(g, 1, 255, 255, 255, 255);	

	SWFFillStyle fill = newSWFGradientFillStyle(g, SWFFILL_LINEAR_GRADIENT);
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
