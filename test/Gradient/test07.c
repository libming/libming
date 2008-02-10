#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFMovie_setBackground(m, 0, 0, 0);
	SWFMovie_setDimension(m, 500, 450);
	

	SWFGradient g = newSWFGradient();
	SWFGradient_addEntry(g, 0.0, 255, 255, 255, 255);	
	SWFGradient_addEntry(g, 1.0, 255, 255, 255, 0);
	SWFFillStyle fill = newSWFGradientFillStyle(g, SWFFILL_RADIAL_GRADIENT);
	SWFShape shape = newSWFShape();
	SWFShape_setRightFillStyle(shape, fill);
	SWFShape_drawCircle(shape, 55);
	
	SWFDisplayItem i = SWFMovie_add(m, shape);
	SWFDisplayItem_moveTo(i, 100, 100);	
	SWFMovie_save(m, "test07.swf");
	return 0;
}
