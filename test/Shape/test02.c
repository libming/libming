#include <libming.h>
#include <stdlib.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFShape s = newSWFShape();
	SWFFill f = SWFShape_addSolidFill(s, 255,0,0,255);
	SWFShape_setRightFill(s, f);
	Ming_setCubicThreshold(10);
	SWFShape_movePenTo(s, 167.000000, 69.875040);
	SWFShape_drawCubicTo(s, 198.796531,69.875040,224.624960,95.703469,224.624960,127.500000);
	SWFShape_drawCubicTo(s, 224.624960,159.296531,198.796531,185.124960,167.000000,185.124960);
	SWFShape_drawCubicTo(s, 135.203469,185.124960,109.375040,159.296531,109.375040,127.500000);
	SWFShape_drawCubicTo(s, 109.375040,95.703469,135.203469,69.875040,167.000000,69.875040);
	SWFMovie_add(m, s);
	SWFMovie_save(m, "test02.swf");
	return 0;
}
