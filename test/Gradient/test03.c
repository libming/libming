#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFGradient g = newSWFGradient();

	SWFGradient_addEntry(g, 0, 0, 0, 0, 255);	
	SWFGradient_addEntry(g, 0.2, 0, 10, 0, 255);	
	SWFGradient_addEntry(g, 0.3, 110, 0, 10, 255);	
	SWFGradient_addEntry(g, 0.4, 20, 0, 0, 255);	
	SWFGradient_addEntry(g, 0.5, 0, 20, 0, 255);	
	SWFGradient_addEntry(g, 0.6, 0, 0, 20, 255);	
	SWFGradient_addEntry(g, 0.7, 30, 0, 0, 255);	

	SWFGradient_addEntry(g, 0.8, 0, 30, 0, 255);	
	SWFGradient_addEntry(g, 0.9, 0, 0, 30, 255);	


	SWFFillStyle fill = newSWFGradientFillStyle(g, SWFFILL_LINEAR_GRADIENT);
	SWFShape shape = newSWFShape();
	SWFShape_setRightFillStyle(shape, fill);
	
	SWFShape_setLine(shape, 1, 0,0,0,255);
	SWFShape_drawLine(shape, 100, 0);
	SWFShape_drawLine(shape, 0, 100);
	SWFShape_drawLine(shape, -100, 0);
	SWFShape_drawLine(shape, 0, -100);

	SWFMovie_add(m, (SWFBlock)shape);
	SWFMovie_save(m, "test03.swf");
	return 0;

}
