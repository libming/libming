#include <libming.h>


int main()
{
	int i;
	SWFMovie m = newSWFMovieWithVersion(8);

	SWFMorph morph = newSWFMorphShape();

	SWFShape shape1 = SWFMorph_getShape1(morph);
	SWFShape shape2 = SWFMorph_getShape2(morph);
	
	SWFShape_setLine2(shape1, 20, 25, 0, 0, 128, SWF_LINESTYLE_FLAG_HINTING, 0);
	SWFShape_movePenTo(shape1, 5, 5);
        SWFShape_drawLineTo(shape1, 50, 30);

	SWFShape_setLine2(shape2, 20, 25, 100, 100, 255, SWF_LINESTYLE_FLAG_HINTING, 0);
	SWFShape_movePenTo(shape2, 5, 5);
        SWFShape_drawLineTo(shape2, 50, 130);

	SWFDisplayItem item = SWFMovie_add(m, (SWFBlock)morph);
	for(i = 0; i < 10; i++)
	{
		SWFDisplayItem_setRatio(item, 0.1 * i);
		SWFMovie_nextFrame(m);
	} 

	SWFMovie_save(m,"test02.swf");

	return 0;
}
