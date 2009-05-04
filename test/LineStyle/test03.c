#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ming.h>


int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);

	SWFShape shape = newSWFShape();
	SWFShape shape2 = newSWFShape();
	SWFDBLBitmapData img = newSWFDBLBitmapData_fromPngFile(
		MEDIADIR"/image01.png");
	if ( ! img )
	{
		fprintf(stderr, "Could not create bitmap from png file "
				MEDIADIR "/image01.png\n");
		return EXIT_FAILURE;
	}

	SWFFillStyle fill = newSWFBitmapFillStyle((SWFCharacter)img,
				                   SWFFILL_TILED_BITMAP);
	if ( ! fill )
	{
		fprintf(stderr, "Could not create bitmap from png file "
		                MEDIADIR "/image01.png\n");
		return EXIT_FAILURE;
	}

	SWFGradient g = newSWFGradient();

	SWFGradient_addEntry(g, 0.0, 0xff, 0xff, 0, 0x88);
	SWFGradient_addEntry(g, 0.3, 0, 0xff, 0, 0x33);
	SWFGradient_addEntry(g, 0.7, 0, 0, 0xff, 0x33);
	SWFGradient_addEntry(g, 1.0, 0xff, 0, 0, 0x88);

	SWFFillStyle fill2 = newSWFGradientFillStyle(g,
	                                      SWFFILL_LINEAR_GRADIENT);

	SWFShape_setLine2Filled(shape, 40, fill,
		SWF_LINESTYLE_FLAG_HINTING | SWF_LINESTYLE_JOIN_BEVEL |
		SWF_LINESTYLE_FLAG_ENDCAP_SQUARE, 0);
	SWFShape_movePenTo(shape, 5, 5);
	SWFShape_drawLineTo(shape, 50, 100);
	SWFShape_drawLineTo(shape, 100, 100);

	SWFShape_setLine2Filled(shape2, 10, fill2,
		SWF_LINESTYLE_FLAG_HINTING | SWF_LINESTYLE_JOIN_ROUND |
		SWF_LINESTYLE_FLAG_ENDCAP_ROUND, 0);
	SWFShape_movePenTo(shape2, 125, 125);
	SWFShape_drawLineTo(shape2, 150, 100);
	SWFShape_drawLineTo(shape2, 200, 200);
	SWFShape_drawLineTo(shape2, 250, 100);

	SWFMovie_add(m, (SWFBlock)shape);
	SWFMovie_add(m, (SWFBlock)shape2);

	SWFMovie_save(m,"test03.swf");

	return EXIT_SUCCESS;
}
