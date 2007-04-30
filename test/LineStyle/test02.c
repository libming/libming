#include <ming.h>


int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);

	SWFShape shape = newSWFShape();
	
	SWFShape_setLine2(shape, 40, 25, 0, 0, 128, 
		SWF_LINESTYLE_FLAG_HINTING | SWF_LINESTYLE_JOIN_BEVEL | SWF_LINESTYLE_FLAG_ENDCAP_SQUARE, 0);
	SWFShape_movePenTo(shape, 5, 5);
        SWFShape_drawLineTo(shape, 50, 100);
	SWFShape_drawLineTo(shape, 100, 100);

	SWFMovie_add(m, (SWFBlock)shape);
	SWFMovie_save(m,"test02.swf");

	return 0;
}
