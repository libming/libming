#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	SWFDBLBitmapData img = newSWFDBLBitmapData_fromGifFile(MEDIADIR "/image01.gif");
	if ( ! img )
	{
		fprintf(stderr, "Could not create bitmap from gif file "MEDIADIR"/image01.gif\n");
		return EXIT_FAILURE;
	}
	
	SWFFillStyle fill = newSWFBitmapFillStyle((SWFCharacter)img, SWFFILL_CLIPPED_BITMAP);
	if ( ! fill )
	{
		fprintf(stderr, "Could not create bitmap fill style\n");
		return EXIT_FAILURE;
	}
	SWFShape shape = newSWFShape();
	SWFShape_setRightFillStyle(shape, fill);
	
	SWFShape_setLine(shape, 1, 0,0,0,255);
	SWFShape_drawLine(shape, 100, 0);
	SWFShape_drawLine(shape, 0, 100);
	SWFShape_drawLine(shape, -100, 0);
	SWFShape_drawLine(shape, 0, -100);

	SWFMovie_add(m, (SWFBlock)shape);
	SWFMovie_save(m, "test06.swf");
	return 0;

}
