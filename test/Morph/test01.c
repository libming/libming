#include <libming.h>


int main()
{
	int i;
	SWFMovie m = newSWFMovieWithVersion(8);

	SWFMorph morph = newSWFMorphShape();

	SWFShape shape1 = SWFMorph_getShape1(morph);
	SWFShape shape2 = SWFMorph_getShape2(morph);
	
        SWFShape_setLine(shape1, 4, 25, 0, 0, 128);
	SWFShape_movePenTo(shape1, 5, 5);
        SWFShape_drawLineTo(shape1, 50, 30);

        SWFShape_setLine(shape2, 4, 25, 0, 0, 128);
	SWFShape_movePenTo(shape2, 5, 5);
        SWFShape_drawLineTo(shape2, 50, 130);

	SWFDisplayItem item = SWFMovie_add(m, (SWFBlock)morph);
	for(i = 0; i < 10; i++)
	{
		SWFDisplayItem_setRatio(item, 0.1 * i);
		SWFMovie_nextFrame(m);
	} 

	SWFMovie_save(m,"test01.swf");

	return 0;
}
