#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFGradient g = newSWFGradient();

	SWFGradient_addEntry(g, 0, 255, 0, 0, 255);	
	SWFGradient_addEntry(g, 1, 0, 255, 255, 255);	

	SWFFillStyle fs = newSWFGradientFillStyle(g, SWFFILL_LINEAR_GRADIENT);
	SWFShape shape = newSWFShape();
	SWFShape_setRightFillStyle(shape, fs);
	
	SWFShape_setLine(shape, 1, 0,0,0,255);
	SWFShape_movePenTo(shape, 50, 50);
	SWFShape_drawLine(shape, 100, 0);
	SWFShape_drawLine(shape, 0, 100);
	SWFShape_drawLine(shape, -100, 0);
	SWFShape_drawLine(shape, 0, -100);

	SWFMovie_add(m, (SWFBlock)shape);
	SWFMovie_save(m, "test08.swf");
	return 0;

}
