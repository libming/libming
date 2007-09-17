#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <libming.h>

int main()
{
	SWFMovie m = newSWFMovieWithVersion(8);
	FILE *file = fopen(MEDIADIR "/image01.jpeg", "rb");
	if(file == NULL)
	{
		perror(MEDIADIR "/image01.jpeg");
		exit(EXIT_FAILURE);
	}

	SWFJpegBitmap img = newSWFJpegBitmap(file);
	SWFFillStyle fill = newSWFBitmapFillStyle((SWFCharacter)img, SWFFILL_CLIPPED_BITMAP);
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
