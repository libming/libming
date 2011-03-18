#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);

	SWFGradient gradient = newSWFGradient();
	SWFFillStyle fill = newSWFGradientFillStyle(gradient,
		SWFFILL_LINEAR_GRADIENT);
	SWFShape shape = newSWFShape();

	SWFGradient_addEntry(gradient, 0, 255, 0, 0, 255);
	SWFGradient_addEntry(gradient, 0.25, 128, 32, 32, 255);
	SWFGradient_addEntry(gradient, 0.8, 64, 64, 64, 255);
	SWFShape_setRightFillStyle(shape, fill);
	
	SWFShape_setLine(shape, 1, 0,0,0,255);
	SWFShape_drawLine(shape, 100, 0);
	SWFShape_drawLine(shape, 0, 100);
	SWFShape_drawLine(shape, -100, 0);
	SWFShape_drawLine(shape, 0, -100);

	SWFMovie_add(m, (SWFBlock)shape);
	SWFMovie_save(m, "test08.swf");
	return 0;

}
